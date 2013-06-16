function [images] = simple_spiral_recon(ismrmrdfile)
%
%   [images] = simple_spiral_recon(ismrmrdfile)
%
%   Simple example of how to reconstruct a 2D spiral dataset stored
%   in ISMRMRD dataformat
%
%   Michael S. Hansen (michael.hansen@nih.gov), 2012
%

header = ismrmrd_header2struct(h5read(ismrmrdfile,'/dataset/xml'));

%Is this a spiral acquisition
if (~strcmp(header.ismrmrdHeader.encoding.trajectory.Text,'spiral')),
   error('This is not a spiral dataset'); 
end

%Let's get the matrix size
matrix_size = [str2num(header.ismrmrdHeader.encoding.encodedSpace.matrixSize.x.Text), ...
               str2num(header.ismrmrdHeader.encoding.encodedSpace.matrixSize.y.Text)];

%Let's load the data
raw_data = h5read(ismrmrdfile,'/dataset/data');

interleaves = max(raw_data.head.idx.kspace_encode_step_1)+1;
repetitions = max(raw_data.head.idx.repetition)+1;
samples = max(raw_data.head.number_of_samples);
samples_to_skip_end = max(raw_data.head.discard_post);
samples_to_skip_start = max(raw_data.head.discard_pre);
net_samples = samples-samples_to_skip_end-samples_to_skip_start;
channels = max(raw_data.head.active_channels);

trajectory = zeros(2,net_samples,interleaves);
data = zeros(net_samples,interleaves,channels);

images = [];

counter = 0;
for p=1:length(raw_data.head.flags),

   if (bitget(uint64(raw_data.head.flags(p)),19)), %if this is noise, we will skip it
      continue; 
   end
   
   d = reshape(complex(raw_data.data{p}(1:2:end), raw_data.data{p}(2:2:end)), samples, channels);
   t = reshape(raw_data.traj{p}, raw_data.head.trajectory_dimensions(p), samples);
   current_interleave = raw_data.head.idx.kspace_encode_step_1(p)+1;
   start_sample = samples_to_skip_start+1;
   end_sample = samples-samples_to_skip_end;

   data(:,current_interleave,:) = reshape(d(start_sample:end_sample,:), net_samples, 1, channels);
   trajectory(:,:,current_interleave) = t(:,start_sample:end_sample);
   
   if (bitget(uint64(raw_data.head.flags(p)),8)), %Is this the last in slice? We should make an image
      fprintf('Reconstructing image %d....', counter+1); 
      co = permute(trajectory(:,:),[2 1]);

      %Some simple density compensation
      k = complex(co(:,1),co(:,2));
      g = complex(diff(co(:,1)),diff(co(:,2)));
      g(end+1) = g(end);
      weights = abs(g(:)) .* abs(sin(angle(g(:))-angle(k(:)))); %Estimating weights from Meyer et al. Magn Reson Med. 1992 Dec;28(2):202-13.
      
      %Loop over coils and grid images
      for c=1:size(data,3), 
           img(:,:,c) = simple_gridder(co,data(:,:,c),weights, matrix_size); 
      end;
      images(:,:,counter+1) = sqrt(sum(abs(img).^2,3)); %RMS coil combination
      counter = counter + 1;
      fprintf('done\n'); 
   end
   
end

%Let's just display the first image
imagesc(images(:,:,1));colormap(gray);axis image;

return

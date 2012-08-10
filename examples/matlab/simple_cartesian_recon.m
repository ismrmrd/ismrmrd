function [images] = simple_cartesian_recon(ismrmrdfile)
%
%   [images] = simple_cartesian_recon(ismrmrdfile)
%
%   Simple example of how to reconstruct a 2D/3D cartesian dataset stored
%   in ISMRMRD dataformat. Reconstruction handles partial fourier,
%   oversampling, etc. 
%
%   Michael S. Hansen (michael.hansen@nih.gov), 2012
%


%read the header
header = ismrmrd_header2struct(h5read(ismrmrdfile,'/dataset/xml'));


%Is this a spiral acquisition
if (~strcmp(header.ismrmrdHeader.encoding.trajectory.Text,'cartesian')),
   error('This is not a cartesian dataset'); 
end

%if (str2num(header.ismrmrdHeader.encoding.encodedSpace.matrixSize.z.Text) <= 1),
%    error('This is not a 3D dataset'); 
%end
    
%Let's get the matrix size
matrix_size = [str2num(header.ismrmrdHeader.encoding.encodedSpace.matrixSize.x.Text), ...
               str2num(header.ismrmrdHeader.encoding.encodedSpace.matrixSize.y.Text), ...
               str2num(header.ismrmrdHeader.encoding.encodedSpace.matrixSize.z.Text)];

           
recon_size = [str2num(header.ismrmrdHeader.encoding.reconSpace.matrixSize.x.Text), ...
              str2num(header.ismrmrdHeader.encoding.reconSpace.matrixSize.y.Text), ...
              str2num(header.ismrmrdHeader.encoding.reconSpace.matrixSize.z.Text)];


%Let's load the data
raw_data = h5read(ismrmrdfile,'/dataset/data');

channels = max(raw_data.head.active_channels);
samples = max(raw_data.head.number_of_samples);
center_line = str2num(header.ismrmrdHeader.encoding.encodingLimits.kspace_encoding_step_1.center.Text);
center_partition = str2num(header.ismrmrdHeader.encoding.encodingLimits.kspace_encoding_step_2.center.Text);

if (samples ~= matrix_size(1)),
    error('Mismatch between number of samples and matrix size');
end

buffer = zeros(matrix_size(1),matrix_size(2),matrix_size(3),channels);           
counter = 0;
for p=1:length(raw_data.head.flags),
   if (bitget(uint64(raw_data.head.flags(p)),19)), %if this is noise, we will skip it
      continue; 
   end
   line_number      = (int32(raw_data.head.idx.kspace_encode_step_1(p))-center_line)+bitshift(matrix_size(2),-1);  
   partition_number = (int32(raw_data.head.idx.kspace_encode_step_2(p))-center_partition)+bitshift(matrix_size(3),-1);  
   buffer(:,line_number+1,partition_number+1,:) = reshape(complex(raw_data.data{p}.real, raw_data.data{p}.imag), samples, 1, 1, channels);
   
   if (bitget(uint64(raw_data.head.flags(p)),8)), %Is this the last in slice? We should make an image
      fprintf('Reconstructing image %d....', counter+1); 

      img = fftshift(ifft(ifftshift(buffer),[],1)) ./ sqrt(size(buffer,1));
      img = fftshift(ifft(ifftshift(img),[],2)) ./ sqrt(size(img,2));
      if (matrix_size(3) > 1), 
          img = fftshift(ifft(ifftshift(img),[],3)) ./ sqrt(size(img,3));
      end
      
      img = img((1:recon_size(1))+bitshift(matrix_size(1)-recon_size(1),-1), ...
                (1:recon_size(2))+bitshift(matrix_size(2)-recon_size(2),-1), ...
                (1:recon_size(3))+bitshift(matrix_size(3)-recon_size(3),-1), :);
      
      images(:,:,:,counter+1) = sqrt(sum(abs(img).^2,4)); %RMS coil combination
      counter = counter + 1;
      fprintf('done\n'); 
      buffer = zeros(matrix_size(1),matrix_size(2),matrix_size(3), channels);           
   end  
end

%Let's just show the middle slice image
imagesc(images(:,:,bitshift(size(images,3),-1)+1, 1));colormap(gray);axis image;



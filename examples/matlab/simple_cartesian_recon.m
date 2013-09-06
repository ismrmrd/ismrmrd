function [images] = simple_cartesian_recon(ismrmrdfile, dataset)
%
%   [images] = simple_cartesian_recon(ismrmrdfile, dataset)
%
%   Simple example of how to reconstruct a 2D/3D cartesian dataset stored
%   in ISMRMRD dataformat. Reconstruction handles partial fourier,
%   oversampling, etc. 
%
%   Michael S. Hansen (michael.hansen@nih.gov), 2012
%   Souheil J. Inati  (souheil.inati@nih.gov),  2013
%
%   Usage notes:
%     ismrmrdfile: string, required.
%     dataset:     string, optional, default='dataset'
%

switch nargin
    case 1
        dset = 'dataset';
    case 2
        dset = dataset;
    otherwise
        error('Wrong number of input arguments.')
end

% Open the dataset
f = ismrmrd.IsmrmrdDataset(ismrmrdfile, dset);

% Is this a cartesian acquisition
traj = f.xmlhdr.getEncoding.get(0).getTrajectory();
if (~traj.equals(traj.CARTESIAN))
   error('This is not a cartesian dataset'); 
end

% Get the matrix sizes and the number of slices and channels
matrix_size = [f.xmlhdr.getEncoding.get(0).getEncodedSpace.getMatrixSize.getX, ...
               f.xmlhdr.getEncoding.get(0).getEncodedSpace.getMatrixSize.getY, ...
               f.xmlhdr.getEncoding.get(0).getEncodedSpace.getMatrixSize.getZ];

recon_size = [f.xmlhdr.getEncoding.get(0).getReconSpace.getMatrixSize.getX, ...
              f.xmlhdr.getEncoding.get(0).getReconSpace.getMatrixSize.getY, ...
              f.xmlhdr.getEncoding.get(0).getReconSpace.getMatrixSize.getZ];

if ~isempty(f.xmlhdr.getEncoding.get(0).getEncodingLimits.getSlice)
    slices = f.xmlhdr.getEncoding.get(0).getEncodingLimits.getSlice;
else
    slices = 1;
end

channels = f.xmlhdr.getAcquisitionSystemInformation.getReceiverChannels.doubleValue;


% Get the encoding centers
center_line = f.xmlhdr.getEncoding.get(0).getEncodingLimits.getKspaceEncodingStep1.getCenter;
if (matrix_size(3) > 1)
    % 3D
    center_partition = f.xmlhdr.getEncoding.get(0).getEncodingLimits.getKspaceEncodingStep2.getCenter;
else
    % 2D
    center_partition = 0;
end

% Allocate a buffer for storing 1 repetition's worth of data
buffer = zeros([matrix_size(1),matrix_size(2),matrix_size(3),slices,channels],'single');

% Initalize the output
images = [];

% read all the acquisitions
acq = f.readAcquisition();

% Loop over all the acquisitions
counter = 0;
for p = 1:acq.getNumber()
    
    % ignore the noise scans
    if acq.head.flagIsSet(acq.head.FLAGS.ACQ_IS_NOISE_MEASUREMENT,p)
        continue
    end

    % stuff the data in the buffer
    line_number      = int32(acq.head.idx.kspace_encode_step_1(p)) - center_line + matrix_size(2)/2 + 1;
    if matrix_size(3) > 1
        partition_number = int32(acq.head.idx.kspace_encode_step_2(p)) - center_partition + matrix_size(3)/2 + 1;
    else
        partition_number = 1;
    end
    slice_number     = acq.head.idx.slice(p) + 1;
    buffer(:,line_number,partition_number,slice_number,:) = acq.data{p};
    
    % Is this the last in slice? We should make an image
    if acq.head.flagIsSet(acq.head.FLAGS.ACQ_LAST_IN_SLICE,p)
        counter = counter + 1;
        fprintf('Reconstructing image %d\n', counter); 
        fprintf('   slice: %d\n', slice_number); 
        fprintf('   rep:   %d\n', acq.head.idx.repetition(p)+1); 

        % FFT
        img = fftshift(ifft(ifftshift(buffer(:,:,:,slice_number,:)),[],1)) ./ sqrt(size(buffer,1));
        img = fftshift(ifft(ifftshift(img),[],2)) ./ sqrt(size(img,2));
        if (matrix_size(3) > 1), 
            img = fftshift(ifft(ifftshift(img),[],3)) ./ sqrt(size(img,3));
        end

        % Handle over-sampling
        if matrix_size(3) > 1
            img = img((1:recon_size(1)) + (matrix_size(1)-recon_size(1))/2, ...
                      (1:recon_size(2)) + (matrix_size(2)-recon_size(2))/2, ...
                      (1:recon_size(3)) + (matrix_size(3)-recon_size(3))/2, :);
        else
            img = img((1:recon_size(1)) + (matrix_size(1)-recon_size(1))/2, ...
                      (1:recon_size(2)) + (matrix_size(2)-recon_size(2))/2, ...
                      1, :);
        end

        % RMS coil combination
        img = sqrt(sum(abs(img).^2,4));
        
        % Append to the output buffer
        images(:,:,:,counter) = img;
    end

end
fprintf('done\n'); 

% Display the middle slice image
% imagesc(images(:,:,bitshift(size(images,3),-1)+1, 1));colormap(gray);axis image;

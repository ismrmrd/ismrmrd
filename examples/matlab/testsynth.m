% Read an existing file
f = ismrmrd.file('../examples/data/testdata.h5');

% Output file
synthfile = 'test_mat_synth.h5';


% Get its header
xmlhdr = f.readxml();

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Generate a Synthetic Data Set %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Create a fake signal
nx = 256; ny = 256;
rho = zeros(nx,ny,'single');
rho(nx/4+1:3*nx/4,ny/4+1:3*ny/4) = 1;
kdat = fftshift(fft2(fftshift(rho)));

% Create a new ISMRMRD file
g = ismrmrd.file(synthfile);

% Write the xml header
g.writexml(xmlhdr);

% Initialize the acquisition object
acq = ismrmrd.Acquisition;
acq.head.available_channels = 1;
acq.head.active_channels = 1;
acq.head.number_of_samples = nx;
acq.head.center_sample = nx/2;

% Loop over the acquisitions, set the looping variables, 
% stuff the data, and append.  For this example, read ky from top to bottom.
for p = ny:-1:1
    acq.head.idx.kspace_encode_step_1 = p-1;
    acq.data = kdat(:,p);
    g.appendAcquisition(acq);
end

% close
g.close();


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Read it in and reconstruct it %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Open the ISMRMRD file
g = ismrmrd.file(synthfile);

% Read the XML header
xml = g.readxml();

% Read the number of acquisitions
nky = g.getNumberOfAcquisitions();
% Read the first acquisition
acq1 = g.readAcquisition(1);
nx = acq1.head.number_of_samples;

% Loop over the acquisitions and read them in
% stuffing into the correct part of the k-space data array
kdata = zeros(nx,nky);
for p = 1:nky
    acq = f.readAcquisition(p);
    kdata(:,acq.head.idx.kspace_encode_step_1+1) = acq.data;
end

% Simple reconstruction
rhohat = fftshift(ifft2(fftshift(kdata)));

imagesc(abs(rhohat)); axis image; axis off; colorbar
title('Reconstructed image');

% close
g.close();

% close the original dataset
f.close();

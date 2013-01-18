%% ISMRM Raw Data Matlab Interface
% Tutorial introduction to the MATLAB interface of the ISMRM Raw Data
% file format.

%% Overview
% For a detailed description of the ISMRM Raw Data file format, please
% consult the documentation on the project home page:
% <http://ismrmrd.sourceforge.com>
%
% The ISMRM Raw Data Matlab interface is distributed as a package folder
% (+ismrmrd) containing a set of Matlab classes and functions and a Java
% interface to the XML header.
%
% Install the package folder and add it to your Matlab path.
% Change the path below to match your install location.
%addpath('/usr/local/ismrmd/matlab');

%% Creating an ISMRMRD file with synthetic data
%

% A simulated object: a square in the middle of an image with value 1.
nx = 256; ny = 256;
rho = zeros(nx,ny,'single');
rho(nx/4+1:3*nx/4,ny/4+1:3*ny/4) = 1;

% The synthetic k-space signal from a conventional evenly sampled MR experiment
kdata = fftshift(fft2(fftshift(rho)));

% Create a new ISMRMRD file
g = ismrmrd.file('synthetic.h5');

% Construct the XML header

% Write the XML header
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
    acq.data = kdata(:,p);
    f.appendAcquisition(acq);
end

% close
f.close();

%% Reconstruct an image from an existing ISMRMRD file
%
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

% close the original dataset
f.close();

%% Generating a simple ISMRMRD data set

% This is an example of how to construct a dataset from synthetic data
% simulating a fully sampled acquisition on a cartesian grid.
% data from 4 coils from a single slice object that looks like a square

% File Name
filename = 'testdata.h5';

% Create an empty ismrmrd dataset
if exist(filename,'file')
    error(['File ' filename ' already exists.  Please remove first'])
end
dset = ismrmrd.Dataset(filename);

% Synthesize the object
nX = 256;
nY = 256;
rho = zeros(nX,nY);
indxstart = floor(nX/4)+1;
indxend   = floor(3*nX/4);
indystart = floor(nY/4)+1;
indyend   = floor(3*nY/4);
rho(indxstart:indxend,indystart:indyend) = 1;

% Synthesize some coil sensitivities
[X,Y] = ndgrid((0:nX-1)/nX/2.0 - 0.5, (0:nY-1)/nY/2.0 - 0.5);
C = zeros(nX,nY,4);
C(:,:,1) = exp(-((X-.5).^2 + (Y).^2)    + 1i*(X-.5));
C(:,:,2) = exp(-((X+.5).^2 + (Y).^2)    - 1i*(X+.5));
C(:,:,3) = exp(-((X).^2    + (Y-.5).^2) + 1i*(Y-.5));
C(:,:,4) = exp(-((X).^2    + (Y+.5).^2) - 1i*(Y+.5));
nCoils = size(C,3);

% Synthesize the k-space data
nReps = 5;
noiselevel = 0.05;
K = zeros(nX, nY, nCoils, nReps);
for rep = 1:nReps
    for coil = 1:nCoils
        noise = noiselevel * (randn(nX,nY)+1j*randn(nX,nY));
        K(:,:,coil,rep) = fftshift(fft2(fftshift( C(:,:,coil).*rho + noise)));
    end
end

% It is very slow to append one acquisition at a time, so we're going
% to append a block of acquisitions at a time.
% In this case, we'll do it one repetition at a time to show off this
% feature.  Each block has nY acquisitions
acqblock = ismrmrd.Acquisition(nY);

% Set the header elements that don't change
acqblock.head.version(:) = 1;
acqblock.head.number_of_samples(:) = nX;
acqblock.head.center_sample(:) = floor(nX/2);
acqblock.head.active_channels(:) = nCoils;
acqblock.head.read_dir  = repmat([1 0 0]',[1 nY]);
acqblock.head.phase_dir = repmat([0 1 0]',[1 nY]);
acqblock.head.slice_dir = repmat([0 0 1]',[1 nY]);

% Loop over the acquisitions, set the header, set the data and append
for rep = 1:nReps
    for acqno = 1:nY
        
        % Set the header elements that change from acquisition to the next
        % c-style counting
        acqblock.head.scan_counter(acqno) = (rep-1)*nY + acqno-1;
        acqblock.head.idx.kspace_encode_step_1(acqno) = acqno-1; 
        acqblock.head.idx.repetition(acqno) = rep - 1;
        
        % Set the flags
        acqblock.head.flagClearAll(acqno);
        if acqno == 1
            acqblock.head.flagSet('ACQ_FIRST_IN_ENCODE_STEP1', acqno);
            acqblock.head.flagSet('ACQ_FIRST_IN_SLICE', acqno);
            acqblock.head.flagSet('ACQ_FIRST_IN_REPETITION', acqno);
        elseif acqno==size(K,2)
            acqblock.head.flagSet('ACQ_LAST_IN_ENCODE_STEP1', acqno);
            acqblock.head.flagSet('ACQ_LAST_IN_SLICE', acqno);
            acqblock.head.flagSet('ACQ_LAST_IN_REPETITION', acqno);
        end
        
        % fill the data
        acqblock.data{acqno} = squeeze(K(:,acqno,:,rep));
    end

    % Append the acquisition block
    dset.appendAcquisition(acqblock);
        
end % rep loop


%%%%%%%%%%%%%%%%%%%%%%%%
%% Fill the xml header %
%%%%%%%%%%%%%%%%%%%%%%%%
% We create a matlab struct and then serialize it to xml.
% Look at the xml schema to see what the field names should be

header = [];

% Experimental Conditions (Required)
header.experimentalConditions.H1resonanceFrequency_Hz = 128000000; % 3T

% Acquisition System Information (Optional)
header.acquisitionSystemInformation.systemVendor = 'ISMRMRD Labs';
header.acquisitionSystemInformation.systemModel = 'Virtual Scanner';
header.acquisitionSystemInformation.receiverChannels = nCoils;

% The Encoding (Required)
header.encoding.trajectory = 'cartesian';
header.encoding.encodedSpace.fieldOfView_mm.x = 256;
header.encoding.encodedSpace.fieldOfView_mm.y = 256;
header.encoding.encodedSpace.fieldOfView_mm.z = 5;
header.encoding.encodedSpace.matrixSize.x = size(K,1);
header.encoding.encodedSpace.matrixSize.y = size(K,2);
header.encoding.encodedSpace.matrixSize.z = 1;
% Recon Space
% (in this case same as encoding space)
header.encoding.reconSpace = header.encoding.encodedSpace;
% Encoding Limits
header.encoding.encodingLimits.kspace_encoding_step_0.minimum = 0;
header.encoding.encodingLimits.kspace_encoding_step_0.maximum = size(K,1)-1;
header.encoding.encodingLimits.kspace_encoding_step_0.center = floor(size(K,1)/2);
header.encoding.encodingLimits.kspace_encoding_step_1.minimum = 0;
header.encoding.encodingLimits.kspace_encoding_step_1.maximum = size(K,2)-1;
header.encoding.encodingLimits.kspace_encoding_step_1.center = floor(size(K,2)/2);
header.encoding.encodingLimits.repetition.minimum = 0;
header.encoding.encodingLimits.repetition.maximum = nReps-1;
header.encoding.encodingLimits.repetition.center = 0;

%% Serialize and write to the data set
xmlstring = ismrmrd.xml.serialize(header);
dset.writexml(xmlstring);

%% Write the dataset
dset.close();

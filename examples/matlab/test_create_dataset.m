%% Generating a simple ISMRMRD data set

% This is an example of how to construct a datset from synthetic data
% simulating a fully sampled acquisition on a cartesian grid.
% data from 4 coils from a single slice object that looks like a square

% File Name
filename = 'testdata.h5';

% Create an empty ismrmrd dataset
if exist(filename,'file')
    error(['File ' filename ' already exists.  Please remove first'])
end
dset = ismrmrd.IsmrmrdDataset(filename);

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
% feature.  Each block has nY aquisitions
acqblock = ismrmrd.Acquisition(nY);

% Set the header elements that don't change
acqblock.head.version(:) = 1.0;
acqblock.head.number_of_samples(:) = nX;
acqblock.head.center_sample(:) = floor(nX/2);
acqblock.head.active_channels(:) = nCoils;
acqblock.head.read_dir  = repmat(single([1 0 0]'),[1 nY]);
acqblock.head.phase_dir = repmat(single([0 1 0]'),[1 nY]);
acqblock.head.slice_dir = repmat(single([0 0 1]'),[1 nY]);

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
            acqblock.head.flagSet(acqblock.head.FLAGS.ACQ_FIRST_IN_ENCODE_STEP1, acqno);
            acqblock.head.flagSet(acqblock.head.FLAGS.ACQ_FIRST_IN_SLICE, acqno);
            acqblock.head.flagSet(acqblock.head.FLAGS.ACQ_FIRST_IN_REPETITION, acqno);
        elseif acqno==size(K,2)
            acqblock.head.flagSet(acqblock.head.FLAGS.ACQ_LAST_IN_ENCODE_STEP1, acqno);
            acqblock.head.flagSet(acqblock.head.FLAGS.ACQ_LAST_IN_SLICE, acqno);
            acqblock.head.flagSet(acqblock.head.FLAGS.ACQ_LAST_IN_REPETITION, acqno);
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

%% Experimental Conditions
expcond = org.ismrm.ismrmrd.ExperimentalConditionsType;
expcond.setH1ResonanceFrequencyHz(128000000); % 3T
dset.xmlhdr.setExperimentalConditions(expcond);

%% Acquisition System Information
acqsysinfo = org.ismrm.ismrmrd.AcquisitionSystemInformationType;
acqsysinfo.setReceiverChannels(java.lang.Integer(nCoils));
dset.xmlhdr.setAcquisitionSystemInformation(acqsysinfo);

%% The Encoding
encoding = org.ismrm.ismrmrd.Encoding;
encoding.setTrajectory(org.ismrm.ismrmrd.TrajectoryType.CARTESIAN);

% Encoded Space
fov = org.ismrm.ismrmrd.FieldOfViewMm;
fov.setX(256);
fov.setX(256);
fov.setZ(5);
matrix = org.ismrm.ismrmrd.MatrixSize;
matrix.setX(size(K,1));
matrix.setY(size(K,2));
matrix.setZ(1);
encodedspace = org.ismrm.ismrmrd.EncodingSpaceType;
encodedspace.setMatrixSize(matrix);
encodedspace.setFieldOfViewMm(fov);
encoding.setEncodedSpace(encodedspace);

% Recon Space
% (same as encoding space)
encoding.setReconSpace(encodedspace);

% Encoding Limits
% Be careful!!! All of the XML Header stuff is  JAVA so it is
% being done with objects.  Make sure that you
% create one object per thing you care about
encodinglimits = org.ismrm.ismrmrd.EncodingLimitsType;

limitsa = org.ismrm.ismrmrd.LimitType;
limitsa.setMinimum(0);
limitsa.setCenter(floor(size(K,1)/2)); 
limitsa.setMaximum(size(K,1)-1); 
encodinglimits.setKspaceEncodingStep0(limitsa);

limitsb = org.ismrm.ismrmrd.LimitType;
limitsb.setMinimum(0);
limitsb.setCenter(floor(size(K,2)/2)); 
limitsb.setMaximum(size(K,2)-1); 
encodinglimits.setKspaceEncodingStep1(limitsb);

limitsc = org.ismrm.ismrmrd.LimitType;
limitsc.setMinimum(0);
limitsc.setCenter(floor(nReps/2)); 
limitsc.setMaximum(nReps-1);
encodinglimits.setRepetition(limitsc);

% All the rest are zero and we can use the same one for all of them
limits = org.ismrm.ismrmrd.LimitType;
limits.setMinimum(0);
limits.setCenter(0); 
limits.setMaximum(0);
encodinglimits.setAverage(limits);
encodinglimits.setContrast(limits);
encodinglimits.setKspaceEncodingStep2(limits);
encodinglimits.setPhase(limits);
encodinglimits.setSegment(limits);
encodinglimits.setSet(limits);
encodinglimits.setSlice(limits);
% Stuff
encoding.setEncodingLimits(encodinglimits);
dset.xmlhdr.getEncoding.add(encoding);

%% Write the dataset
dset.close();

%% Generating a simple ISMRMRD data set to simulate GRAPPA undersampling
% Based on test_create_dataset
%
% This is an example of how to construct a dataset from synthetic data
% simulating an under sampled acquisition on a cartesian grid with a 
% central fully sampled (ACS) region.
% Data from 4 coils from a single slice object with 6 repetitions.
%
% Reconstruction using the gadgetron configuration file 
% Generic_Cartesian_Grappa.xml may require changes to cope with the small 
% number of coils:
%  <property><name>upstream_coil_compression_thres</name><value>0.00001</value></property>
%  <property><name>downstream_coil_compression_thres</name><value>0.0001</value></property>
%  <property><name>use_constant_scalingFactor</name><value>false</value></property>

% Output file Name
def_filename = 'testusdatamri.h5';
FilterSpec = '*.h5' ;
DialogTitle = 'Output h5 filename' ;

[FileName,PathName,FilterIndex] = uiputfile(FilterSpec,DialogTitle,def_filename);
filename = fullfile(PathName,FileName) ;

dset = ismrmrd.Dataset(filename);

% Synthesize the object
% nY here corresponds to fully sampled data (256), nYsamp is number of actually
% sampled lines (128 + additional central ACS lines)
nX = 256;
nY = 256;
rho = zeros(nX,nY);
indxstart = floor(nX/4)+1;
indxend   = floor(3*nX/4);
indystart = floor(nY/4)+1;
indyend   = floor(3*nY/4);
% put an MR image in the centre
load mri % loads D - an example 128 128 1 27 dataset
rho(indxstart:indxend,indystart:indyend) = squeeze(double(D(:,:,1,12))) ;


% Synthesize some coil sensitivities
[X,Y] = ndgrid((0:nX-1)/nX/2.0 - 0.5, (0:nY-1)/nY/2.0 - 0.5);
C = zeros(nX,nY,4);
C(:,:,1) = exp(-((X-.5).^2 + (Y).^2)    + 1i*(X-.5));
C(:,:,2) = exp(-((X+.5).^2 + (Y).^2)    - 1i*(X+.5));
C(:,:,3) = exp(-((X).^2    + (Y-.5).^2) + 1i*(Y-.5));
C(:,:,4) = exp(-((X).^2    + (Y+.5).^2) - 1i*(Y+.5));
nCoils = size(C,3);
if exist('eshow','file')
    eshow(C)  % displays coil sensitivities
end

% set ACS lines for GRAPPA simulation (fully sampled central k-space
% region)
ACShw = 14 ; % GRAPPA ACS half width i.e. here 28 lines are ACS
Ysamp_u = [1:2:nY] ; % undersampling by every alternate line
Ysamp_ACS = [nY/2-ACShw+1 : nY/2+ACShw] ; % GRAPPA autocalibration lines
Ysamp = union(Ysamp_u, Ysamp_ACS) ; % actually sampled lines
nYsamp = length(Ysamp) ; % number of actually sampled

% Ysamp indexes the actually sampled lines to the encoded k-space line number. 
% For example, if there were just regular factor 2 undersampling 
% (with no ACS lines), Ysamp would have length 128 and be [1 3 5 ... 255].
% With ACS lines, the elements of Ysamp are separated by 2 near the k-space
% edges, and by 1 in the central ACS region.


% Synthesize the k-space data
nReps = 6  % increased from 4 to 6 to avoid confusion with number of coils.
noiselevel = 0.05;
K = zeros(nX, nYsamp, nCoils, nReps);
for rep = 1:nReps
    for coil = 1:nCoils
        noise = noiselevel * (randn(nX,nY)+1j*randn(nX,nY));
        ksp = fftshift(fft2(fftshift( C(:,:,coil).*rho + noise))); 
        K(:,:,coil,rep) = ksp(:,Ysamp);
    end
end

% It is very slow to append one acquisition at a time, so we're going
% to append a block of acquisitions at a time.
% In this case, we'll do it one repetition at a time to show off this
% feature.  Each block has nYsamp acquisitions
acqblock = ismrmrd.Acquisition(nYsamp);

% Set the header elements that don't change
acqblock.head.version(:) = 1;
acqblock.head.number_of_samples(:) = nX;
acqblock.head.center_sample(:) = floor(nX/2);
acqblock.head.active_channels(:) = nCoils;
acqblock.head.read_dir  = repmat([1 0 0]',[1 nYsamp]);
acqblock.head.phase_dir = repmat([0 1 0]',[1 nYsamp]);
acqblock.head.slice_dir = repmat([0 0 1]',[1 nYsamp]);

% Loop over the acquisitions, set the header, set the data and append
for rep = 1:nReps
    for acqno = 1:nYsamp
        
        % Set the header elements that change from acquisition to the next
        % c-style counting
        acqblock.head.scan_counter(acqno) = (rep-1)*nYsamp + acqno-1;
        % Note next entry is k-space encoded line number (not acqno which
        % is just the sequential acquisition number)
        acqblock.head.idx.kspace_encode_step_1(acqno) = Ysamp(acqno)-1; 
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
        
        if ismember(Ysamp(acqno),Ysamp_ACS)
            if ismember(Ysamp(acqno),Ysamp_u)
                % both calibration and part of the undersampled pattern
                acqblock.head.flagSet('ACQ_IS_PARALLEL_CALIBRATION_AND_IMAGING', acqno)
            else
                % in ACS block but not part of the regular undersampling
                % pattern Ysamp_u
                acqblock.head.flagSet('ACQ_IS_PARALLEL_CALIBRATION', acqno) ;
            end
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
header.encoding.encodedSpace.matrixSize.y = nY;
header.encoding.encodedSpace.matrixSize.z = 1;
% Recon Space
% (in this case same as encoding space)
header.encoding.reconSpace = header.encoding.encodedSpace;
% Encoding Limits
header.encoding.encodingLimits.kspace_encoding_step_0.minimum = 0;
header.encoding.encodingLimits.kspace_encoding_step_0.maximum = size(K,1)-1;
header.encoding.encodingLimits.kspace_encoding_step_0.center = floor(size(K,1)/2);
header.encoding.encodingLimits.kspace_encoding_step_1.minimum = 0;
header.encoding.encodingLimits.kspace_encoding_step_1.maximum = nY-1;
header.encoding.encodingLimits.kspace_encoding_step_1.center = floor(nY/2);
header.encoding.encodingLimits.repetition.minimum = 0;
header.encoding.encodingLimits.repetition.maximum = nReps-1;
header.encoding.encodingLimits.repetition.center = 0;

header.encoding.parallelImaging.accelerationFactor.kspace_encoding_step_1 = 2 ;
header.encoding.parallelImaging.accelerationFactor.kspace_encoding_step_2 = 1 ;
header.encoding.parallelImaging.calibrationMode = 'embedded' ;

% Commented code below appears not necessary - saw this parameter after converting
% a scanner file using siemens_to_ismrmrd
% header.userParameters.userParameterLong.name = 'EmbeddedRefLinesE1' ;
% header.userParameters.userParameterLong.value = ACShw *2  ;

%% Serialize and write to the data set
xmlstring = ismrmrd.xml.serialize(header);
dset.writexml(xmlstring);

%% Write the dataset
dset.close();

function hdr = ImageHeader(imtype)
% Return an initialized ISMRMRD ImageHeader
% 
% hdr = ImageHeader(imtype)
%
% imtype is one of the types defined in ismrmrd.ImageType
%
% example usage:
%   hdr_c = ImageHeader(ismrmrd.ImageType.TYPE_COMPLEX)
%

    % Check for a valid type
    fnames = fieldnames(ismrmrd.ImageType);
    b = false;
    for p = 1:length(fnames)
        if uint16(imtype) == ismrmrd.ImageType.(fnames{p})
            b = true;
            break;
        end
    end
    if ~b
        error('%d is an unknown ISMRMRD image type.',imtype);
    end
    
    hdr.version = uint16(0);                          % First unsigned int indicates the version %
    hdr.flags = uint64(0);                            % bit field with flags %
    hdr.measurement_uid = uint32(0);                  % Unique ID for the measurement %
    hdr.matrix_size = zeros(3,1,'uint16');            % Pixels in the 3 spatial dimensions
    hdr.field_of_view = zeros(3,1,'single');          % Size (in mm) of the 3 spatial dimensions %
    hdr.channels = uint16(0);                         % Number of receive channels %
    hdr.position = zeros(3,1,'single');               % Three-dimensional spatial offsets from isocenter %
    hdr.read_dir = zeros(3,1,'single');               % Directional cosines of the readout/frequency encoding %
    hdr.phase_dir = zeros(3,1,'single');              % Directional cosines of the phase encoding %
    hdr.slice_dir = zeros(3,1,'single');              % Directional cosines of the slice %
    hdr.patient_table_position = zeros(3,1,'single'); % Patient table off-center %
    hdr.average = uint16(0);                          % e.g. signal average number %
    hdr.slice = uint16(0);                            % e.g. imaging slice number %
    hdr.contrast = uint16(0);                         % e.g. echo number in multi-echo %
    hdr.phase = uint16(0);                            % e.g. cardiac phase number %
    hdr.repetition = uint16(0);                       % e.g. dynamic number for dynamic scanning %
    hdr.set = uint16(0);                              % e.g. flow encodning set %
    hdr.acquisition_time_stamp = uint32(0);           % Acquisition clock %
    hdr.physiology_time_stamp = zeros(8,1,'uint32');  % Physiology time stamps, e.g. ecg, breating, etc. %
    hdr.image_data_type = uint16(0);                  % e.g. unsigned short, float, complex float, etc. %
    hdr.image_type = uint16(imtype);                   % e.g. magnitude, phase, complex, real, imag, etc. %
    hdr.image_index = uint16(0);					  % e.g. image number in series of images  %
    hdr.image_series_index = uint16(0);               % e.g. series number %
    hdr.user_int = zeros(8,1,'int32');                % Free user parameters %
    hdr.user_float = zeros(8,1,'single');             % Free user parameters %

end
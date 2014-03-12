classdef AcquisitionHeader < handle
    
    properties
        
        version = uint16([]);                          % First unsigned int indicates the version %
        flags = uint64([]);                            % bit field with flags %
        measurement_uid = uint32([]);                  % Unique ID for the measurement %
        scan_counter = uint32([]);                     % Current acquisition number in the measurement %
        acquisition_time_stamp = uint32([]);           % Acquisition clock %
        physiology_time_stamp = uint32([]);            % Physiology time stamps, e.g. ecg, breating, etc. %
        number_of_samples = uint16([]);                % Number of samples acquired %
        available_channels = uint16([]);               % Available coils %
        active_channels = uint16([]);                  % Active coils on current acquisiton %
        channel_mask = uint64([]);                     % Mask to indicate which channels are active. Support for 1024 channels %
        discard_pre = uint16([]);                      % Samples to be discarded at the beginning of acquisition %
        discard_post = uint16([]);                     % Samples to be discarded at the end of acquisition %
        center_sample = uint16([]);                    % Sample at the center of k-space %
        encoding_space_ref = uint16([]);               % Reference to an encoding space, typically only one per acquisition %
        trajectory_dimensions = uint16([]);            % Indicates the dimensionality of the trajectory vector (0 means no trajectory) %
        sample_time_us = single([]);                   % Time between samples in micro seconds, sampling BW %
        position = single([]);                         % Three-dimensional spatial offsets from isocenter %
        read_dir = single([]);                         % Directional cosines of the readout/frequency encoding %
        phase_dir = single([]);                        % Directional cosines of the phase encoding %
        slice_dir = single([]);                        % Directional cosines of the slice %
        patient_table_position = single([]);           % Patient table off-center %
        idx = struct(  ...                             % Encoding loop counters, see above %
            'kspace_encode_step_1', uint16([]), ...   
            'kspace_encode_step_2', uint16([]), ...
            'average', uint16([]), ...
            'slice', uint16([]), ...
            'contrast', uint16([]), ...
            'phase', uint16([]), ...
            'repetition', uint16([]), ...
            'set', uint16([]), ...
            'segment', uint16([]), ...
            'user', uint16([]));
        user_int = int32([]);                 % Free user parameters %
        user_float = single([]);              % Free user parameters %
        
    end
    
    properties(Constant)
        FLAGS = struct( ...
            'ACQ_FIRST_IN_ENCODE_STEP1',                1, ...
            'ACQ_LAST_IN_ENCODE_STEP1',                 2, ...
            'ACQ_FIRST_IN_ENCODE_STEP2',                3, ...
            'ACQ_LAST_IN_ENCODE_STEP2',                 4, ...
            'ACQ_FIRST_IN_AVERAGE',                     5, ...
            'ACQ_LAST_IN_AVERAGE',                      6, ...
            'ACQ_FIRST_IN_SLICE',                       7, ...
            'ACQ_LAST_IN_SLICE',                        8, ...
            'ACQ_FIRST_IN_CONTRAST',                    9, ...
            'ACQ_LAST_IN_CONTRAST',                    10, ...
            'ACQ_FIRST_IN_PHASE',                      11, ...
            'ACQ_LAST_IN_PHASE',                       12, ...
            'ACQ_FIRST_IN_REPETITION',                 13, ...
            'ACQ_LAST_IN_REPETITION',                  14, ...
            'ACQ_FIRST_IN_SET',                        15, ...
            'ACQ_LAST_IN_SET',                         16, ...
            'ACQ_FIRST_IN_SEGMENT',                    17, ...
            'ACQ_LAST_IN_SEGMENT',                     18, ...
            'ACQ_IS_NOISE_MEASUREMENT',                19, ...
            'ACQ_IS_PARALLEL_CALIBRATION',             20, ...
            'ACQ_IS_PARALLEL_CALIBRATION_AND_IMAGING', 21, ...
            'ACQ_IS_REVERSE',                          22, ...
            'ACQ_IS_NAVIGATION_DATA',                  23, ...
            'ACQ_USER1',                               57, ...
            'ACQ_USER2',                               58, ...
            'ACQ_USER3',                               59, ...
            'ACQ_USER4',                               60, ...
            'ACQ_USER5',                               61, ...
            'ACQ_USER6',                               62, ...
            'ACQ_USER7',                               63, ...
            'ACQ_USER8',                               64);
    end
    
    methods
        
        function obj = AcquisitionHeader(arg)
            % Constructor
            
            switch nargin
                case 0
                    % No argument constructor
                    % initialize to a single acquisition header
                    extend(obj,1);
                    
                case 1
                    % One argument constructor
                    if isstruct(arg)
                        % plain struct
                        fromStruct(obj,arg);
                    elseif (length(arg)==1 && ismrmrd.util.isInt(arg)) == 1
                        % number
                        extend(obj,arg);
                    elseif isa(arg,'uint8')
                        % Byte array
                        fromBytes(obj,arg);
                    else
                        % Unknown type
                        error('Unknown argument type.')
                    end
                    
                otherwise
                    error('Wrong number of arguments.')
                    
            end
        end
        
        function nacq = getNumber(obj)
            % Return the number of headers
            
            nacq = length(obj.version);
            
        end

        function hdr = select(obj, range)
            % Return a copy of a range of acquisition headers
            
            % create an empty acquisition header
            M = length(range);
            hdr = ismrmrd.AcquisitionHeader(M);
            
            % Fill
            hdr.version = obj.version(range);
            hdr.flags = obj.flags(range);
            hdr.measurement_uid = obj.measurement_uid(range);
            hdr.scan_counter = obj.scan_counter(range);
            hdr.acquisition_time_stamp = obj.acquisition_time_stamp(range);
            hdr.physiology_time_stamp = obj.physiology_time_stamp(:,range);
            hdr.number_of_samples = obj.number_of_samples(range);
            hdr.available_channels = obj.available_channels(range);
            hdr.active_channels = obj.active_channels(range);
            hdr.channel_mask = obj.channel_mask(:,range);
            hdr.discard_pre = obj.discard_pre(range);
            hdr.discard_post = obj.discard_post(range);
            hdr.center_sample = obj.center_sample(range);
            hdr.encoding_space_ref = obj.encoding_space_ref(range);
            hdr.trajectory_dimensions = obj.trajectory_dimensions(range);
            hdr.sample_time_us = obj.sample_time_us(range);
            hdr.position = obj.position(:,range);
            hdr.read_dir = obj.read_dir(:,range);
            hdr.phase_dir = obj.phase_dir(:,range);
            hdr.slice_dir = obj.slice_dir(:,range);
            hdr.patient_table_position = obj.patient_table_position(:,range);
            hdr.idx.kspace_encode_step_1 = obj.idx.kspace_encode_step_1(range);
            hdr.idx.kspace_encode_step_2 = obj.idx.kspace_encode_step_2(range);
            hdr.idx.average = obj.idx.average(range);
            hdr.idx.slice = obj.idx.slice(range);
            hdr.idx.contrast = obj.idx.contrast(range);
            hdr.idx.phase = obj.idx.phase(range);
            hdr.idx.repetition = obj.idx.repetition(range);
            hdr.idx.set = obj.idx.set(range);
            hdr.idx.segment = obj.idx.segment(range);
            hdr.idx.user = obj.idx.user(:,range);
            hdr.user_int = obj.user_int(:,range);
            hdr.user_float = obj.user_float(:,range);
            
        end        
        
        function extend(obj,N)
            % Extend with blank header
            
            range = obj.getNumber + (1:N);
            obj.version(1,range)                  = zeros(1,N,'uint16');
            obj.flags(1,range)                    = zeros(1,N,'uint64');
            obj.measurement_uid(1,range)          = zeros(1,N,'uint32');
            obj.scan_counter(1,range)             = zeros(1,N,'uint32');
            obj.acquisition_time_stamp(1,range)   = zeros(1,N,'uint32');
            obj.physiology_time_stamp(1:3,range)  = zeros(3,N,'uint32');
            obj.number_of_samples(1,range)        = zeros(1,N,'uint16');
            obj.available_channels(1,range)       = zeros(1,N,'uint16');
            obj.active_channels(1,range)          = zeros(1,N,'uint16');
            obj.channel_mask(1:16,range)          = zeros(16,N,'uint64');
            obj.discard_pre(1,range)              = zeros(1,N,'uint16');
            obj.discard_post(1,range)             = zeros(1,N,'uint16');
            obj.center_sample(1,range)            = zeros(1,N,'uint16');
            obj.encoding_space_ref(1,range)       = zeros(1,N,'uint16');
            obj.trajectory_dimensions(1,range)    = zeros(1,N,'uint16');
            obj.sample_time_us(1,range)           = zeros(1,N,'uint16');
            obj.position(1:3,range)               = zeros(3,N,'single');
            obj.read_dir(1:3,range)               = zeros(3,N,'single');
            obj.phase_dir(1:3,range)              = zeros(3,N,'single');
            obj.slice_dir(1:3,range)              = zeros(3,N,'single');
            obj.patient_table_position(1:3,range) = zeros(3,N,'single');
            obj.idx.kspace_encode_step_1(1,range) = zeros(1,N,'uint16');
            obj.idx.kspace_encode_step_2(1,range) = zeros(1,N,'uint16');
            obj.idx.average(1,range)              = zeros(1,N,'uint16');
            obj.idx.slice(1,range)                = zeros(1,N,'uint16');
            obj.idx.contrast(1,range)             = zeros(1,N,'uint16');
            obj.idx.phase(1,range)                = zeros(1,N,'uint16');
            obj.idx.repetition(1,range)           = zeros(1,N,'uint16');
            obj.idx.set(1,range)                  = zeros(1,N,'uint16');
            obj.idx.segment(1,range)              = zeros(1,N,'uint16');
            obj.idx.user(1:8,range)               = zeros(8,N,'uint16');
            obj.user_int(1:8,range)               = zeros(8,N,'int32');
            obj.user_float(1:8,range)             = zeros(8,N,'single');
        end
        
        function append(obj, head)
            % Append a header
            
            Nstart = obj.getNumber + 1;
            Nend   = obj.getNumber + length(head.version);
            Nrange = Nstart:Nend;
            obj.version(Nrange) = hdr.version;
            obj.flags(Nrange) = hdr.flags;
            obj.measurement_uid(Nrange) = hdr.measurement_uid;
            obj.scan_counter(Nrange) = hdr.scan_counter;
            obj.acquisition_time_stamp(Nrange) = hdr.acquisition_time_stamp;
            obj.physiology_time_stamp(:,Nrange) = hdr.physiology_time_stamp;
            obj.number_of_samples(Nrange) = hdr.number_of_samples;
            obj.available_channels(Nrange) = hdr.available_channels;
            obj.active_channels(Nrange) = hdr.active_channels;
            obj.channel_mask(:,Nrange) = hdr.channel_mask;
            obj.discard_pre(Nrange) = hdr.discard_pre;
            obj.discard_post(Nrange) = hdr.discard_post;
            obj.center_sample(Nrange) = hdr.center_sample;
            obj.encoding_space_ref(Nrange) = hdr.encoding_space_ref;
            obj.trajectory_dimensions(Nrange) = hdr.trajectory_dimensions;
            obj.sample_time_us(Nrange) = hdr.sample_time_us;
            obj.position(:,Nrange) = hdr.position;
            obj.read_dir(:,Nrange) = hdr.read_dir;
            obj.phase_dir(:,Nrange) = hdr.phase_dir;
            obj.slice_dir(:,Nrange) = hdr.slice_dir;
            obj.patient_table_position(:,Nrange) = hdr.patient_table_position;
            obj.idx.kspace_encode_step_1(Nrange) = hdr.idx.kspace_encode_step_1;
            obj.idx.kspace_encode_step_2(Nrange) = hdr.idx.kspace_encode_step_2;
            obj.idx.average(Nrange) = hdr.idx.average;
            obj.idx.slice(Nrange) = hdr.idx.slice;
            obj.idx.contrast(Nrange) = hdr.idx.contrast;
            obj.idx.phase(Nrange) = hdr.idx.phase;
            obj.idx.repetition(Nrange) = hdr.idx.repetition;
            obj.idx.set(Nrange) = hdr.idx.set;
            obj.idx.segment(Nrange) = hdr.idx.segment;
            obj.idx.user(:,Nrange) = hdr.idx.user;
            obj.user_int(:,Nrange) = hdr.user_int;
            obj.user_float(:,Nrange) = hdr.user_float;            
        end

        function fromStruct(obj, hdr)
            % Convert a struct to the object
            
            %warning! no error checking
            obj.version = hdr.version;
            obj.flags = hdr.flags;
            obj.measurement_uid = hdr.measurement_uid;
            obj.scan_counter = hdr.scan_counter;
            obj.acquisition_time_stamp = hdr.acquisition_time_stamp;
            obj.physiology_time_stamp = hdr.physiology_time_stamp;
            obj.number_of_samples = hdr.number_of_samples;
            obj.available_channels = hdr.available_channels;
            obj.active_channels = hdr.active_channels;
            obj.channel_mask = hdr.channel_mask;
            obj.discard_pre = hdr.discard_pre;
            obj.discard_post = hdr.discard_post;
            obj.center_sample = hdr.center_sample;
            obj.encoding_space_ref = hdr.encoding_space_ref;
            obj.trajectory_dimensions = hdr.trajectory_dimensions;
            obj.sample_time_us = hdr.sample_time_us;
            obj.position = hdr.position;
            obj.read_dir = hdr.read_dir;
            obj.phase_dir = hdr.phase_dir;
            obj.slice_dir = hdr.slice_dir;
            obj.patient_table_position = hdr.patient_table_position;
            obj.idx.kspace_encode_step_1 = hdr.idx.kspace_encode_step_1;
            obj.idx.kspace_encode_step_2 = hdr.idx.kspace_encode_step_2;
            obj.idx.average = hdr.idx.average;
            obj.idx.slice = hdr.idx.slice;
            obj.idx.contrast = hdr.idx.contrast;
            obj.idx.phase = hdr.idx.phase;
            obj.idx.repetition = hdr.idx.repetition;
            obj.idx.set = hdr.idx.set;
            obj.idx.segment = hdr.idx.segment;
            obj.idx.user = hdr.idx.user;
            obj.user_int = hdr.user_int;
            obj.user_float = hdr.user_float;            
        end

        function hdr = toStruct(obj)
            % Convert the object to a plain struct
            
            %warning! no error checking
            hdr = struct();
            hdr.version = obj.version;
            hdr.flags = obj.flags;
            hdr.measurement_uid = obj.measurement_uid;
            hdr.scan_counter = obj.scan_counter;
            hdr.acquisition_time_stamp = obj.acquisition_time_stamp;
            hdr.physiology_time_stamp = obj.physiology_time_stamp;
            hdr.number_of_samples = obj.number_of_samples;
            hdr.available_channels = obj.available_channels;
            hdr.active_channels = obj.active_channels;
            hdr.channel_mask = obj.channel_mask;
            hdr.discard_pre = obj.discard_pre;
            hdr.discard_post = obj.discard_post;
            hdr.center_sample = obj.center_sample;
            hdr.encoding_space_ref = obj.encoding_space_ref;
            hdr.trajectory_dimensions = obj.trajectory_dimensions;
            hdr.sample_time_us = obj.sample_time_us;
            hdr.position = obj.position;
            hdr.read_dir = obj.read_dir;
            hdr.phase_dir = obj.phase_dir;
            hdr.slice_dir = obj.slice_dir;
            hdr.patient_table_position = obj.patient_table_position;
            hdr.idx.kspace_encode_step_1 = obj.idx.kspace_encode_step_1;
            hdr.idx.kspace_encode_step_2 = obj.idx.kspace_encode_step_2;
            hdr.idx.average = obj.idx.average;
            hdr.idx.slice = obj.idx.slice;
            hdr.idx.contrast = obj.idx.contrast;
            hdr.idx.phase = obj.idx.phase;
            hdr.idx.repetition = obj.idx.repetition;
            hdr.idx.set = obj.idx.set;
            hdr.idx.segment = obj.idx.segment;
            hdr.idx.user = obj.idx.user;
            hdr.user_int = obj.user_int;
            hdr.user_float = obj.user_float;            
        end
        
        function fromBytes(obj, bytearray)
            % Convert from a byte array to an ISMRMRD AcquisitionHeader
            % This conforms to the memory layout of the C-struct

            if size(bytearray,1) ~= 360
                error('Wrong number of bytes for AcquisitionHeader.')
            end
            N = size(bytearray,2);
            for p = 1:N
                obj.version(p) =                  typecast(bytearray(  1:  2,p), 'uint16'); ... % First unsigned int indicates the version %
                obj.flags(p) =                    typecast(bytearray(  3: 10,p), 'uint64'); ... % bit field with flags %
                obj.measurement_uid(p) =          typecast(bytearray( 11: 14,p), 'uint32'); ... % Unique ID for the measurement %
                obj.scan_counter(p) =             typecast(bytearray( 15: 18,p), 'uint32'); ... % Current acquisition number in the measurement %
                obj.acquisition_time_stamp(p) =   typecast(bytearray( 19: 22,p), 'uint32'); ... % Acquisition clock %
                obj.physiology_time_stamp(:,p) =  typecast(bytearray( 23: 34,p), 'uint32'); ... % Physiology time stamps, e.g. ecg, breating, etc. %
                                                                                            ... % C-Struct padding
                obj.number_of_samples(p) =        typecast(bytearray( 55: 56,p), 'uint16'); ... % Number of samples acquired %
                obj.available_channels(p) =       typecast(bytearray( 57: 58,p), 'uint16'); ... % Available coils %
                obj.active_channels(p) =          typecast(bytearray( 59: 60,p), 'uint16'); ... % Active coils on current acquisiton %
                obj.channel_mask(:,p) =           typecast(bytearray( 61:188,p), 'uint64'); ... % Mask to indicate which channels are active. Support for 1024 channels %
                obj.discard_pre(p) =              typecast(bytearray(189:190,p), 'uint16'); ... % Samples to be discarded at the beginning of acquisition %
                obj.discard_post(p) =             typecast(bytearray(191:192,p), 'uint16'); ... % Samples to be discarded at the end of acquisition %
                obj.center_sample(p) =            typecast(bytearray(193:194,p), 'uint16'); ... % Sample at the center of k-space %
                obj.encoding_space_ref(p) =       typecast(bytearray(195:196,p), 'uint16'); ... % Reference to an encoding space, typically only one per acquisition %
                obj.trajectory_dimensions(p) =    typecast(bytearray(197:198,p), 'uint16'); ... % Indicates the dimensionality of the trajectory vector (0 means no trajectory) %
                obj.sample_time_us(p) =           typecast(bytearray(199:202,p), 'single'); ... % Time between samples in micro seconds, sampling BW %
                obj.position(:,p) =               typecast(bytearray(203:214,p), 'single'); ... % Three-dimensional spatial offsets from isocenter %
                obj.read_dir(:,p) =               typecast(bytearray(215:226,p), 'single'); ... % Directional cosines of the readout/frequency encoding %
                obj.phase_dir(:,p) =              typecast(bytearray(227:238,p), 'single'); ... % Directional cosines of the phase encoding %
                obj.slice_dir(:,p) =              typecast(bytearray(239:250,p), 'single'); ... % Directional cosines of the slice %
                obj.patient_table_position(:,p) = typecast(bytearray(251:262,p), 'single'); ... % Patient table off-center %
                obj.idx.kspace_encode_step_1(p) = typecast(bytearray(263:264,p), 'uint16'); ... % phase encoding line number %
                obj.idx.kspace_encode_step_2(p) = typecast(bytearray(265:266,p), 'uint16'); ... % partition encodning number %
                obj.idx.average(p) =              typecast(bytearray(267:268,p), 'uint16'); ... % signal average number %
                obj.idx.slice(p) =                typecast(bytearray(269:270,p), 'uint16'); ... % imaging slice number %
                obj.idx.contrast(p) =             typecast(bytearray(271:272,p), 'uint16'); ... % echo number in multi-echo %
                obj.idx.phase(p) =                typecast(bytearray(273:274,p), 'uint16'); ... % cardiac phase number %
                obj.idx.repetition(p) =           typecast(bytearray(275:276,p), 'uint16'); ... % dynamic number for dynamic scanning %
                obj.idx.set(p) =                  typecast(bytearray(277:278,p), 'uint16'); ... % flow encoding set %
                obj.idx.segment(p) =              typecast(bytearray(279:280,p), 'uint16'); ... % segment number for segmented acquisition %
                obj.idx.user(:,p) =               typecast(bytearray(281:296,p), 'uint16'); ... % Free user parameters %
                obj.user_int(:,p) =               typecast(bytearray(297:328,p), 'int32');  ... % Free user parameters %
                obj.user_float(:,p) =             typecast(bytearray(329:360,p), 'single'); ... % Free user parameters %
            end              
        end
        
        function bytes = toBytes(obj)
            % Convert to an ISMRMRD AcquisitionHeader to a byte array
            % This conforms to the memory layout of the C-struct

            N = obj.getNumber;
            bytes = zeros(360,N,'uint8');
            for p = 1:N
                off = 1;
                bytes(off:off+1,p)   = typecast(obj.version(p)               ,'uint8'); off=off+2;
                bytes(off:off+7,p)   = typecast(obj.flags(p)                 ,'uint8'); off=off+8;
                bytes(off:off+3,p)   = typecast(obj.measurement_uid(p)       ,'uint8'); off=off+4;
                bytes(off:off+3,p)   = typecast(obj.scan_counter(p)          ,'uint8'); off=off+4;
                bytes(off:off+3,p)   = typecast(obj.acquisition_time_stamp(p),'uint8'); off=off+4;
                % The C struct has padding because of the 5 unused physio time stamps
                bytes(off:off+11,p)  = typecast(obj.physiology_time_stamp(:,p) ,'uint8'); off=off+12;
                off = off+20; % Discard 5*uint32;
                bytes(off:off+1,p)   = typecast(obj.number_of_samples(p)     ,'uint8'); off=off+2;
                bytes(off:off+1,p)   = typecast(obj.available_channels(p)    ,'uint8'); off=off+2;
                bytes(off:off+1,p)   = typecast(obj.active_channels(p)       ,'uint8'); off=off+2;
                bytes(off:off+127,p) = typecast(obj.channel_mask(:,p)        ,'uint8'); off=off+128;
                bytes(off:off+1,p)   = typecast(obj.discard_pre(p)           ,'uint8'); off=off+2;
                bytes(off:off+1,p)   = typecast(obj.discard_post(p)          ,'uint8'); off=off+2;
                bytes(off:off+1,p)   = typecast(obj.center_sample(p)         ,'uint8'); off=off+2;
                bytes(off:off+1,p)   = typecast(obj.encoding_space_ref(p)    ,'uint8'); off=off+2;
                bytes(off:off+1,p)   = typecast(obj.trajectory_dimensions(p) ,'uint8'); off=off+2;
                bytes(off:off+3,p)   = typecast(obj.sample_time_us(p)        ,'uint8'); off=off+4;
                bytes(off:off+11,p)  = typecast(obj.position(:,p)            ,'uint8'); off=off+12;
                bytes(off:off+11,p)  = typecast(obj.read_dir(:,p)            ,'uint8'); off=off+12;
                bytes(off:off+11,p)  = typecast(obj.phase_dir(:,p)           ,'uint8'); off=off+12;
                bytes(off:off+11,p)  = typecast(obj.slice_dir(:,p)           ,'uint8'); off=off+12;
                bytes(off:off+11,p)  = typecast(obj.patient_table_position(:,p),'uint8'); off=off+12;
                bytes(off:off+1,p)   = typecast(obj.idx.kspace_encode_step_1(p),'uint8'); off=off+2;
                bytes(off:off+1,p)   = typecast(obj.idx.kspace_encode_step_2(p),'uint8'); off=off+2;
                bytes(off:off+1,p)   = typecast(obj.idx.average(p)           ,'uint8'); off=off+2;
                bytes(off:off+1,p)   = typecast(obj.idx.slice(p)             ,'uint8'); off=off+2;
                bytes(off:off+1,p)   = typecast(obj.idx.contrast(p)          ,'uint8'); off=off+2;
                bytes(off:off+1,p)   = typecast(obj.idx.phase(p)             ,'uint8'); off=off+2;
                bytes(off:off+1,p)   = typecast(obj.idx.repetition(p)        ,'uint8'); off=off+2;
                bytes(off:off+1,p)   = typecast(obj.idx.set(p)               ,'uint8'); off=off+2;
                bytes(off:off+1,p)   = typecast(obj.idx.segment(p)           ,'uint8'); off=off+2;
                bytes(off:off+15,p)  = typecast(obj.idx.user(:,p)            ,'uint8'); off=off+16;
                bytes(off:off+31,p)  = typecast(obj.user_int(:,p)            ,'uint8'); off=off+32;
                bytes(off:off+31,p)  = typecast(obj.user_float(:,p)          ,'uint8');
            end
        end
        
        function obj = check(obj)
            % Check and fix the obj types
            
            % Check the number of elements for each entry
            N = obj.getNumber();
            if (size(obj.flags) ~= N)
                error('Size of flags is not correct.');
            end
            if ((size(obj.measurement_uid,1) ~= 1) || ...
                (size(obj.measurement_uid,2) ~= N))
                error('Size of measurement_uid is not correct.');
            end
            if ((size(obj.scan_counter,1) ~= 1) || ...
                (size(obj.scan_counter,2) ~= N))
                error('Size of scan_counter is not correct.');
            end
            if ((size(obj.acquisition_time_stamp,1) ~= 1) || ...
                (size(obj.acquisition_time_stamp,2) ~= N))
                error('Size of acquisition_time_stamp is not correct.');
            end
            if ((size(obj.physiology_time_stamp,1) ~= 3) || ...
                (size(obj.physiology_time_stamp,2) ~= N))
                error('Size of physiology_time_stamp is not correct.');
            end
            if ((size(obj.number_of_samples,1) ~= 1) || ...
                (size(obj.number_of_samples,2) ~= N))
                error('Size of number_of_samples is not correct.');
            end
            
            if ((size(obj.available_channels,1) ~= 1) || ...
                (size(obj.available_channels,2) ~= N))
                error('Size of available_channels is not correct.');
            end
            if ((size(obj.active_channels,1) ~= 1) || ...
                (size(obj.active_channels,2) ~= N))
                error('Size of active_channels is not correct.');
            end
            if ((size(obj.channel_mask,1) ~= 16) || ...
                (size(obj.channel_mask,2) ~= N))    
                error('Size of channel_mask is not correct.');
            end
            if ((size(obj.discard_pre,1) ~= 1) || ...
                (size(obj.discard_pre,2) ~= N))
                error('Size of discard_pre is not correct.');
            end
            if ((size(obj.discard_post,1) ~= 1) || ...
                (size(obj.discard_post,2) ~= N))    
                error('Size of discard_post is not correct.');
            end
            if ((size(obj.center_sample,1) ~= 1) || ...
                (size(obj.center_sample,2) ~= N))
                error('Size of center_sample is not correct.');
            end
            if ((size(obj.encoding_space_ref,1) ~= 1) || ...
                (size(obj.encoding_space_ref,2) ~= N))    
                error('Size of encoding_space_ref is not correct.');
            end
            if ((size(obj.trajectory_dimensions,1) ~= 1) || ...
                (size(obj.trajectory_dimensions,2) ~= N))
                error('Size of trajectory_dimensions is not correct.');
            end
            if ((size(obj.sample_time_us,1) ~= 1) || ...
                (size(obj.sample_time_us,2) ~= N))    
                error('Size of sample_time_us is not correct.');
            end
            if ((size(obj.position,1) ~= 3) || ...
                (size(obj.position,2) ~= N))    
                error('Size of position is not correct.');
            end
            if ((size(obj.read_dir,1) ~= 3) || ...
                (size(obj.read_dir,2) ~= N))    
                error('Size of read_dir is not correct.');
            end
            if ((size(obj.phase_dir,1) ~= 3) || ...
                (size(obj.phase_dir,2) ~= N))    
                error('Size of phase_dir is not correct.');
            end
            if ((size(obj.slice_dir,1) ~= 3) || ...
                (size(obj.slice_dir,2) ~= N))    
                error('Size of slice_dir is not correct.');
            end
            if ((size(obj.patient_table_position,1) ~= 3) || ...
                (size(obj.patient_table_position,2) ~= N))    
                error('Size of patient_table_position is not correct.');
            end
            if ((size(obj.idx.kspace_encode_step_1,1) ~= 1) || ...
                (size(obj.idx.kspace_encode_step_1,2) ~= N))
                error('Size of kspace_encode_step_1 is not correct.');
            end
            if ((size(obj.idx.kspace_encode_step_2,1) ~= 1) || ...
                (size(obj.idx.kspace_encode_step_2,2) ~= N))     
                error('Size of kspace_encode_step_2 is not correct.');
            end
            if ((size(obj.idx.average,1) ~= 1) || ...
                (size(obj.idx.average,2) ~= N))    
                error('Size of idx.average is not correct.');
            end
            if ((size(obj.idx.slice,1) ~= 1) || ...
                (size(obj.idx.slice,2) ~= N))    
                error('Size of idx.slice is not correct.');
            end
            if ((size(obj.idx.contrast,1) ~= 1) || ...
                (size(obj.idx.contrast,2) ~= N))    
                error('Size of idx.contrast is not correct.');
            end
            if ((size(obj.idx.phase,1) ~= 1) || ...
                (size(obj.idx.phase,2) ~= N))    
                error('Size of idx.phase is not correct.');
            end
            if ((size(obj.idx.repetition,1) ~= 1) || ...
                (size(obj.idx.repetition,2) ~= N))    
                error('Size of idx.repetition is not correct.');
            end
            if ((size(obj.idx.set,1) ~= 1) || ...
                (size(obj.idx.set,2) ~= N))    
                error('Size of idx.set is not correct.');
            end
            if ((size(obj.idx.segment,1) ~= 1) || ...
                (size(obj.idx.segment,2) ~= N))    
                error('Size of idx.segment is not correct.');
            end
            if ((size(obj.idx.user,1) ~= 8) || ...
                (size(obj.idx.user,2) ~= N))    
                error('Size of idx.user is not correct.');
            end
            if ((size(obj.user_int,1) ~= 8) || ...
                (size(obj.user_int,2) ~= N))    
                error('Size of user_int is not correct.');
            end
            if ((size(obj.user_float,1) ~= 8) || ...
                (size(obj.user_float,2) ~= N))    
                error('Size of user_float is not correct.');
            end
            
            % Fix the type of all the elements
            obj.version = uint16(obj.version);
            obj.flags = uint64(obj.flags);
            obj.measurement_uid = uint32(obj.measurement_uid);
            obj.scan_counter = uint32(obj.scan_counter);
            obj.acquisition_time_stamp = uint32(obj.acquisition_time_stamp);
            obj.physiology_time_stamp = uint32(obj.physiology_time_stamp);
            obj.number_of_samples = uint16(obj.number_of_samples);
            obj.available_channels = uint16(obj.available_channels);
            obj.active_channels = uint16(obj.active_channels);
            obj.channel_mask = uint64(obj.channel_mask);
            obj.discard_pre = uint16(obj.discard_pre);
            obj.discard_post = uint16(obj.discard_post);
            obj.center_sample = uint16(obj.center_sample);
            obj.encoding_space_ref = uint16(obj.encoding_space_ref);
            obj.trajectory_dimensions = uint16(obj.trajectory_dimensions);
            obj.sample_time_us = single(obj.sample_time_us);
            obj.position = single(obj.position);
            obj.read_dir = single(obj.read_dir);
            obj.phase_dir = single(obj.phase_dir);
            obj.slice_dir = single(obj.slice_dir);
            obj.patient_table_position = single(obj.patient_table_position);
            obj.idx.kspace_encode_step_1 = uint16(obj.idx.kspace_encode_step_1);
            obj.idx.kspace_encode_step_2 = uint16(obj.idx.kspace_encode_step_2);
            obj.idx.average = uint16(obj.idx.average);
            obj.idx.slice = uint16(obj.idx.slice);
            obj.idx.contrast = uint16(obj.idx.contrast);
            obj.idx.phase = uint16(obj.idx.phase);
            obj.idx.repetition = uint16(obj.idx.repetition);
            obj.idx.set = uint16(obj.idx.set);
            obj.idx.segment = uint16(obj.idx.segment);
            obj.idx.user = uint16(obj.idx.user);
            obj.user_int = int32(obj.user_int);
            obj.user_float = single(obj.user_float);
 
        end
        
        function ret = flagIsSet(obj, flag, range)
            % bool = obj.flagIsSet(flag, range)
            
            if nargin < 3
                range = 1:obj.getNumber;
            end
            if isa(flag, 'char')
                b = obj.FLAGS.(flag);
            elseif (flag>0)
                b = uint64(flag);
            else
                error('Flag is of the wrong type.'); 
            end
            bitmask = bitshift(uint64(1),(b-1));
            ret = zeros(size(range));
            for p = 1:length(range)
                ret(p) = (bitand(obj.flags(range(p)), bitmask)>0);
            end
        end
        
        function flagSet(obj, flag, range)
            if nargin < 3
                range = 1:obj.getNumber;
            end
            if isa(flag, 'char')
                b = obj.FLAGS.(flag);
            elseif (flag>0)
                b = uint64(flag);
            else
                error('Flag is of the wrong type.'); 
            end
            bitmask = bitshift(uint64(1),(b-1));

            alreadyset = obj.flagIsSet(flag,range);
            for p = 1:length(range)
                if ~alreadyset(p)
                    obj.flags(range(p)) = obj.flags(range(p)) + bitmask;
                end
            end
        end
        
        function flagClear(obj, flag, range)
            if nargin < 3
                range = 1:obj.getNumber;
            end
            
            if isa(flag, 'char')
                b = obj.FLAGS.(flag);
            elseif (flag>0)
                b = uint64(flag);
            else
                error('Flag is of the wrong type.'); 
            end
            bitmask = bitshift(uint64(1),(b-1));
            
            alreadyset = obj.flagIsSet(flag,range);
            for p = 1:length(range)
                if alreadyset(p)
                    obj.flags(range(p)) = obj.flags(range(p)) - bitmask;
                end
            end
        end
        
        function flagClearAll(obj, range)
            if nargin < 2
                range = 1:obj.getNumber;
            end
            obj.flags(range) = zeros(1,length(range),'uint64');
        end
        
    end
    
end

classdef AcquisitionHeader
    
    properties
        version = uint16(0);                           % First unsigned int indicates the version %
        flags = uint64(0);                             % bit field with flags %
        measurement_uid = uint32(0);                   % Unique ID for the measurement %
        scan_counter = uint32(0);                      % Current acquisition number in the measurement %
        acquisition_time_stamp = uint32(0);            % Acquisition clock %
	% TODO: physiology_time_stamp should be 3.
        %physiology_time_stamp = zeros(3,1,'uint32');   % Physiology time stamps, e.g. ecg, breating, etc. %
        physiology_time_stamp = zeros(8,1,'uint32');   % Physiology time stamps, e.g. ecg, breating, etc. %
        number_of_samples = uint16(0);                 % Number of samples acquired %
        available_channels = uint16(0);                % Available coils %
        active_channels = uint16(0);                   % Active coils on current acquisiton %
        channel_mask = zeros(16,1,'uint64');           % Mask to indicate which channels are active. Support for 1024 channels %
        discard_pre = uint16(0);                       % Samples to be discarded at the beginning of acquisition %
        discard_post = uint16(0);                      % Samples to be discarded at the end of acquisition %
        center_sample = uint16(0);                     % Sample at the center of k-space %
        encoding_space_ref = uint16(0);                % Reference to an encoding space, typically only one per acquisition %
        trajectory_dimensions = uint16(0);             % Indicates the dimensionality of the trajectory vector (0 means no trajectory) %
        sample_time_us = single(0);                    % Time between samples in micro seconds, sampling BW %
        position = zeros(3,1,'single');                % Three-dimensional spatial offsets from isocenter %
        read_dir = zeros(3,1,'single');                % Directional cosines of the readout/frequency encoding %
        phase_dir = zeros(3,1,'single');               % Directional cosines of the phase encoding %
        slice_dir = zeros(3,1,'single');               % Directional cosines of the slice %
        patient_table_position = zeros(3,1, 'single'); % Patient table off-center %
        idx = ismrmrd.EncodingCounters;                % Encoding loop counters, see above %
        user_int = zeros(8,1,'int32');                 % Free user parameters %
        user_float = zeros(8,1,'single');              % Free user parameters %
    end
    
    methods
        
        % Constructor
        function obj = AcquisitionHeader(bytes)
            if (nargin == 1)
                off = 1;
                obj.version                = typecast(bytes(off:off+1),  'uint16'); off=off+2;
                obj.flags                  = typecast(bytes(off:off+7),  'uint64'); off=off+8;
                obj.measurement_uid        = typecast(bytes(off:off+3),  'uint32'); off=off+4;
                obj.scan_counter           = typecast(bytes(off:off+3),  'uint32'); off=off+4;
                obj.acquisition_time_stamp = typecast(bytes(off:off+3),  'uint32'); off=off+4;
                % TODO: physiology_time_stamp should be 3.
                %obj.physiology_time_stamp  = typecast(bytes(off:off+11), 'uint32'); off=off+12;
                obj.physiology_time_stamp  = typecast(bytes(off:off+31), 'uint32'); off=off+32;
                obj.number_of_samples      = typecast(bytes(off:off+1),  'uint16'); off=off+2;
                obj.available_channels     = typecast(bytes(off:off+1),  'uint16'); off=off+2;
                obj.active_channels        = typecast(bytes(off:off+1),  'uint16'); off=off+2;
                obj.channel_mask           = typecast(bytes(off:off+127),'uint64'); off=off+128;
                obj.discard_pre            = typecast(bytes(off:off+1),  'uint16'); off=off+2;
                obj.discard_post           = typecast(bytes(off:off+1),  'uint16'); off=off+2;
                obj.center_sample          = typecast(bytes(off:off+1),  'uint16'); off=off+2;
                obj.encoding_space_ref     = typecast(bytes(off:off+1),  'uint16'); off=off+2;
                obj.trajectory_dimensions  = typecast(bytes(off:off+1),  'uint16'); off=off+2;
                obj.sample_time_us         = typecast(bytes(off:off+3),  'single'); off=off+4;
                obj.position               = typecast(bytes(off:off+11), 'single'); off=off+12;
                obj.read_dir               = typecast(bytes(off:off+11), 'single'); off=off+12;
                obj.phase_dir              = typecast(bytes(off:off+11), 'single'); off=off+12;
                obj.slice_dir              = typecast(bytes(off:off+11), 'single'); off=off+12;
                obj.patient_table_position = typecast(bytes(off:off+11), 'single'); off=off+12;
                obj.idx              = ismrmrd.EncodingCounters(bytes(off:off+33)); off=off+34;
                obj.user_int               = typecast(bytes(off:off+31), 'uint32'); off=off+32;
                obj.user_float             = typecast(bytes(off:off+31), 'single');
            end
        end
        
        % Convert to Bytes
        function bytes = toBytes(obj)
            % TODO: physiology_time_stamp should be 3.
            %bytes = zeros(340,1,'int8');
            bytes = zeros(360,1,'int8');
            off = 1;
            bytes(off:off+1)   = typecast(obj.version               ,'int8'); off=off+2;
            bytes(off:off+7)   = typecast(obj.flags                 ,'int8'); off=off+8;
            bytes(off:off+3)   = typecast(obj.measurement_uid       ,'int8'); off=off+4;
            bytes(off:off+3)   = typecast(obj.scan_counter          ,'int8'); off=off+4;
            bytes(off:off+3)   = typecast(obj.acquisition_time_stamp,'int8'); off=off+4;
            % TODO: physiology_time_stamp should be 3.
            %bytes(off:off+11)  = typecast(obj.physiology_time_stamp ,'int8'); off=off+12;
            bytes(off:off+31)  = typecast(obj.physiology_time_stamp ,'int8'); off=off+32;
            bytes(off:off+1)   = typecast(obj.number_of_samples     ,'int8'); off=off+2;
            bytes(off:off+1)   = typecast(obj.available_channels    ,'int8'); off=off+2;
            bytes(off:off+1)   = typecast(obj.active_channels       ,'int8'); off=off+2;
            bytes(off:off+127) = typecast(obj.channel_mask          ,'int8'); off=off+128;
            bytes(off:off+1)   = typecast(obj.discard_pre           ,'int8'); off=off+2;
            bytes(off:off+1)   = typecast(obj.discard_post          ,'int8'); off=off+2;
            bytes(off:off+1)   = typecast(obj.center_sample         ,'int8'); off=off+2;
            bytes(off:off+1)   = typecast(obj.encoding_space_ref    ,'int8'); off=off+2;
            bytes(off:off+1)   = typecast(obj.trajectory_dimensions ,'int8'); off=off+2;
            bytes(off:off+3)   = typecast(obj.sample_time_us        ,'int8'); off=off+4;
            bytes(off:off+11)  = typecast(obj.position              ,'int8'); off=off+12;
            bytes(off:off+11)  = typecast(obj.read_dir              ,'int8'); off=off+12;
            bytes(off:off+11)  = typecast(obj.phase_dir             ,'int8'); off=off+12;
            bytes(off:off+11)  = typecast(obj.slice_dir             ,'int8'); off=off+12;
            bytes(off:off+11)  = typecast(obj.patient_table_position,'int8'); off=off+12;
            bytes(off:off+33)  = obj.idx.toBytes();                           off=off+34;
            bytes(off:off+31)  = typecast(obj.user_int              ,'int8'); off=off+32;
            bytes(off:off+31)  = typecast(obj.user_float            ,'int8');
        end
        
        % The flag methods
        function val = isFlagSet(obj,flagstring)
            flagbit = ismrmrd.FlagBit(getfield(ismrmrd.AcquisitionFlags,flagstring));
            val = flagbit.isSet(obj.flags);
        end
        
        function obj = setFlag(obj,flagstring)
            flagbit = ismrmrd.FlagBit(getfield(ismrmrd.AcquisitionFlags,flagstring));
            obj.flags = bitor(obj.flags, flagbit.bitmask);
        end
        
        % Set methods
        function obj = set.version(obj,v)
            obj.version = uint16(v);
        end
        
        function obj = set.flags(obj,v)
            obj.flags = uint64(v);
        end
        
        function obj = set.measurement_uid(obj,v)
            obj.measurement_uid = uint32(v);
        end
        
        function obj = set.scan_counter(obj,v)
            obj.scan_counter = uint32(v);
        end
        
        function obj = set.acquisition_time_stamp(obj,v)
            obj.acquisition_time_stamp = uint32(v);
        end
        
        function obj = set.physiology_time_stamp(obj,v)
            % TODO: physiology_time_stamp should be 3.
            %if (length(v)~=3)
            %    error('AcquisitionHeader.physiology_time_stamp must have 3 elements')
            %end
            %obj.physiology_time_stamp = uint32(v);
            obj.physiology_time_stamp(1:3) = uint32(v(1:3));
            obj.physiology_time_stamp(4:8) = uint32(0);
        end
        
        function obj = set.number_of_samples(obj,v)
            obj.number_of_samples = uint16(v);
        end
        
        function obj = set.available_channels(obj,v)
            obj.available_channels = uint16(v);
        end
        
        function obj = set.active_channels(obj,v)
            obj.active_channels = uint16(v);
        end
        
        function obj = set.channel_mask(obj,v)
            if (length(v)~=16)
                error('AcquisitionHeader.channel_mask must have 16 elements')
            end
            obj.channel_mask = uint64(v);
        end
        
        function obj = set.discard_pre(obj,v)
            obj.discard_pre = uint16(v);
        end
        
        function obj = set.discard_post(obj,v)
            obj.discard_post = uint16(v);
        end
        
        function obj = set.center_sample(obj,v)
            obj.center_sample = uint16(v);
        end
        
        function obj = set.encoding_space_ref(obj,v)
            obj.encoding_space_ref = uint16(v);
        end
        
        function obj = set.trajectory_dimensions(obj,v)
            obj.trajectory_dimensions = uint16(v);
        end
        
        function obj = set.sample_time_us(obj,v)
            obj.sample_time_us = single(v);
        end
        
        function obj = set.position(obj,v)
            if (length(v)~=3)
                error('AcquisitionHeader.position must have 3 elements')
            end
            obj.position = single(v);
        end
        
        function obj = set.read_dir(obj,v)
            if (length(v)~=3)
                error('AcquisitionHeader.read_dir must have 3 elements')
            end
            obj.read_dir = single(v);
        end
        
        function obj = set.phase_dir(obj,v)
            if (length(v)~=3)
                error('AcquisitionHeader.phase_dir must have 3 elements')
            end
            obj.phase_dir = single(v);
        end
        
        function obj = set.slice_dir(obj,v)
            if (length(v)~=3)
                error('AcquisitionHeader.slice_dir must have 3 elements')
            end
            obj.slice_dir = single(v);
        end
        
        function obj = set.patient_table_position(obj,v)
            if (length(v)~=3)
                error('AcquisitionHeader.patient_table_position must have 3 elements')
            end
            obj.patient_table_position = single(v);
        end
        
        function obj = set.idx(obj,v)
            if isa(v,'ismrmrd.EncodingCounters')
                obj.idx = v;
            else
                % not of the correct type, hope it's a struct
                % and try to copy one element at a time
                u = fieldnames(obj.idx);
                for p = 1:length(u)
                    obj.idx = setfield(obj.idx,u{p},getfield(v,u{p}));
                end
            end
        end
        
        function obj = set.user_int(obj,v)
            if (length(v)~=8)
                error('AcquisitionHeader.user_int must have 8 elements')
            end
            obj.user_int = int32(v);
        end
        
        function obj = set.user_float(obj,v)
            if (length(v)~=8)
                error('AcquisitionHeader.user_float must have 8 elements')
            end
            obj.user_float = single(v);
        end
        
    end
    
end

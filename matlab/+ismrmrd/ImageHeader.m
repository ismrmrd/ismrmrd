classdef ImageHeader
    
    properties
        version = uint16(0);                          % First unsigned int indicates the version %
        flags = uint64(0);                            % bit field with flags %
        measurement_uid = uint32(0);                  % Unique ID for the measurement %
        matrix_size = zeros(3,1,'uint16');            % Pixels in the 3 spatial dimensions
        field_of_view = zeros(3,1,'single');          % Size (in mm) of the 3 spatial dimensions %
        channels = uint16(0);                         % Number of receive channels %
        position = zeros(3,1,'single');               % Three-dimensional spatial offsets from isocenter %
        read_dir = zeros(3,1,'single');               % Directional cosines of the readout/frequency encoding %
        phase_dir = zeros(3,1,'single');              % Directional cosines of the phase encoding %
        slice_dir = zeros(3,1,'single');              % Directional cosines of the slice %
        patient_table_position = zeros(3,1,'single'); % Patient table off-center %
        average = uint16(0);                          % e.g. signal average number %
        slice = uint16(0);                            % e.g. imaging slice number %
        contrast = uint16(0);                         % e.g. echo number in multi-echo %
        phase = uint16(0);                            % e.g. cardiac phase number %
        repetition = uint16(0);                       % e.g. dynamic number for dynamic scanning %
        set = uint16(0);                              % e.g. flow encodning set %
        acquisition_time_stamp = uint32(0);           % Acquisition clock %
        physiology_time_stamp = zeros(8,1,'uint32');  % Physiology time stamps, e.g. ecg, breating, etc. %
        image_data_type = uint16(0);                  % e.g. unsigned short, float, complex float, etc. %
        image_type = uint16(0);                       % e.g. magnitude, phase, complex, real, imag, etc. %
        image_index = uint16(0);					  % e.g. image number in series of images  %
        image_series_index = uint16(0);               % e.g. series number %
        user_int = zeros(8,1,'int32');                % Free user parameters %
        user_float = zeros(8,1,'single');             % Free user parameters %
    end
    
    methods
        
        % Constructor
        function obj = ImageHeader(bytes)
            if (nargin == 1)
                off = 1;
                obj.version                = typecast(bytes(off:off+1),  'uint16'); off=off+2;
                obj.flags                  = typecast(bytes(off:off+7),  'uint64'); off=off+8;
                obj.measurement_uid        = typecast(bytes(off:off+3),  'uint32'); off=off+4;
                obj.matrix_size            = typecast(bytes(off:off+5),  'uint16'); off=off+6;
                obj.field_of_view          = typecast(bytes(off:off+11), 'single'); off=off+12;
                obj.channels               = typecast(bytes(off:off+1),  'uint16'); off=off+2;
                obj.position               = typecast(bytes(off:off+11), 'single'); off=off+12;
                obj.read_dir               = typecast(bytes(off:off+11), 'single'); off=off+12;
                obj.phase_dir              = typecast(bytes(off:off+11), 'single'); off=off+12;
                obj.slice_dir              = typecast(bytes(off:off+11), 'single'); off=off+12;
                obj.patient_table_position = typecast(bytes(off:off+11), 'single'); off=off+12;
                obj.average                = typecast(bytes(off:off+1),  'uint16'); off=off+2;
                obj.slice                  = typecast(bytes(off:off+1),  'uint16'); off=off+2;
                obj.contrast               = typecast(bytes(off:off+1),  'uint16'); off=off+2;
                obj.phase                  = typecast(bytes(off:off+1),  'uint16'); off=off+2;
                obj.repetition             = typecast(bytes(off:off+1),  'uint16'); off=off+2;
                obj.set                    = typecast(bytes(off:off+1),  'uint16'); off=off+2;
                obj.acquisition_time_stamp = typecast(bytes(off:off+3),  'uint32'); off=off+4;
                obj.physiology_time_stamp  = typecast(bytes(off:off+31), 'uint32'); off=off+32;
                obj.image_data_type        = typecast(bytes(off:off+1),  'uint16'); off=off+2;
                obj.image_type             = typecast(bytes(off:off+1),  'uint16'); off=off+2;
                obj.image_index            = typecast(bytes(off:off+1),  'uint16'); off=off+2;
                obj.image_series_index     = typecast(bytes(off:off+1),  'uint16'); off=off+2;
                obj.user_int               = typecast(bytes(off:off+31), 'uint32'); off=off+32;
                obj.user_float             = typecast(bytes(off:off+31), 'single');
            end
        end
        
        function bytes = toBytes(obj)
            bytes = zeros(214,1,'int8');
            off = 1;
            bytes(off:off+1)   = typecast(obj.version               ,'int8'); off=off+2;
            bytes(off:off+7)   = typecast(obj.flags                 ,'int8'); off=off+8;
            bytes(off:off+3)   = typecast(obj.measurement_uid       ,'int8'); off=off+4;
            bytes(off:off+5)   = typecast(obj.matrix_size           ,'int8'); off=off+6;
            bytes(off:off+11)  = typecast(obj.field_of_view         ,'int8'); off=off+12;
            bytes(off:off+1)   = typecast(obj.channels              ,'int8'); off=off+2;
            bytes(off:off+11)  = typecast(obj.position              ,'int8'); off=off+12;
            bytes(off:off+11)  = typecast(obj.read_dir              ,'int8'); off=off+12;
            bytes(off:off+11)  = typecast(obj.phase_dir             ,'int8'); off=off+12;
            bytes(off:off+11)  = typecast(obj.slice_dir             ,'int8'); off=off+12;
            bytes(off:off+11)  = typecast(obj.patient_table_position,'int8'); off=off+12;
            bytes(off:off+1)   = typecast(obj.average               ,'int8'); off=off+2;
            bytes(off:off+1)   = typecast(obj.slice                 ,'int8'); off=off+2;
            bytes(off:off+1)   = typecast(obj.contrast              ,'int8'); off=off+2;
            bytes(off:off+1)   = typecast(obj.phase                 ,'int8'); off=off+2;
            bytes(off:off+1)   = typecast(obj.repetition            ,'int8'); off=off+2;
            bytes(off:off+1)   = typecast(obj.set                   ,'int8'); off=off+2;
            bytes(off:off+3)   = typecast(obj.acquisition_time_stamp,'int8'); off=off+4;
            bytes(off:off+31)  = typecast(obj.physiology_time_stamp ,'int8'); off=off+32;
            bytes(off:off+1)   = typecast(obj.image_data_type       ,'int8'); off=off+2;
            bytes(off:off+1)   = typecast(obj.image_type            ,'int8'); off=off+2;
            bytes(off:off+1)   = typecast(obj.image_index           ,'int8'); off=off+2;
            bytes(off:off+1)   = typecast(obj.image_series_index    ,'int8'); off=off+2;
            bytes(off:off+31)  = typecast(obj.user_int              ,'int8'); off=off+32;
            bytes(off:off+31)  = typecast(obj.user_float            ,'int8');
        end
        
        % The flag methods
        function val = isFlagSet(obj,flagstring)
            flagbit = ismrmrd.FlagBit(getfield(ismrmrd.ImageFlags,flagstring));
            val = flagbit.isSet(obj.flags);
        end
        
        function obj = setFlag(obj,flagstring)
            flagbit = ismrmrd.FlagBit(getfield(ismrmrd.ImageFlags,flagstring));
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
        
        function obj = set.matrix_size(obj,v)
            if (length(v)~=3)
                error('ImageHeader.matrix_size must have 3 elements')
            end
            obj.matrix_size = uint16(v);
        end
        
        function obj = set.field_of_view(obj,v)
            if (length(v)~=3)
                error('ImageHeader.field_of_view must have 3 elements')
            end
            obj.field_of_view = single(v);
        end
        
        function obj = set.channels(obj,v)
            obj.channels = uint16(v);
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
        
        function obj = set.average(obj,v)
            obj.average = uint16(v);
        end
        
        function obj = set.slice(obj,v)
            obj.slice = uint16(v);
        end
        
        function obj = set.contrast(obj,v)
            obj.contrast = uint16(v);
        end
        
        function obj = set.phase(obj,v)
            obj.phase = uint16(v);
        end
        
        function obj = set.repetition(obj,v)
            obj.repetition = uint16(v);
        end
        
        function obj = set.set(obj,v)
            obj.set = uint16(v);
        end
        
        function obj = set.acquisition_time_stamp(obj,v)
            obj.acquisition_time_stamp = uint32(v);
        end
        
        function obj = set.physiology_time_stamp(obj,v)
            if (length(v)~=8)
                error('AcquisitionHeader.physiology_time_stamp must have 8 elements')
            end
            obj.physiology_time_stamp = uint32(v);
        end
        
        function obj = set.image_data_type(obj,v)
            obj.image_data_type = uint16(v);
        end
        
        function obj = set.image_type(obj,v)
            obj.image_type = uint16(v);
        end
        
        function obj = set.image_index(obj,v)
            obj.image_index = uint16(v);
        end
        function obj = set.image_series_index(obj,v)
            obj.image_series_index = uint16(v);
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

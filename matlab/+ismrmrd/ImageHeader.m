classdef ImageHeader < handle
    
    properties
        
        version = uint16([]);                % First unsigned int indicates the version %
        data_type = uint16([]);              % e.g. unsigned short, float, complex float, etc. */
        flags = uint64([]);                  % bit field with flags %
        measurement_uid = uint32([]);        % Unique ID for the measurement %
        matrix_size = uint16([]);            % Pixels in the 3 spatial dimensions
        field_of_view = single([]);          % Size (in mm) of the 3 spatial dimensions %
        channels = uint16([]);               % Number of receive channels %
        position = single([]);               % Three-dimensional spatial offsets from isocenter %
        read_dir = single([]);               % Directional cosines of the readout/frequency encoding %
        phase_dir = single([]);              % Directional cosines of the phase encoding %
        slice_dir = single([]);              % Directional cosines of the slice %
        patient_table_position = single([]); % Patient table off-center %
        average = uint16([]);                % e.g. signal average number %
        slice = uint16([]);                  % e.g. imaging slice number %
        contrast = uint16([]);               % e.g. echo number in multi-echo %
        phase = uint16([]);                  % e.g. cardiac phase number %
        repetition = uint16([]);             % e.g. dynamic number for dynamic scanning %
        set = uint16([]);                    % e.g. flow encodning set %
        acquisition_time_stamp = uint32([]); % Acquisition clock %
        physiology_time_stamp = uint32([]);  % Physiology time stamps, e.g. ecg, breating, etc. %
        image_type = uint16([]);             % e.g. magnitude, phase, complex, real, imag, etc. %
        image_index = uint16([]);			 % e.g. image number in series of images  %
        image_series_index = uint16([]);     % e.g. series number %
        user_int = int32([]);                % Free user parameters %
        user_float = single([]);             % Free user parameters %
        attribute_string_len = uint32([]);
        
    end

    properties(Constant)
        FLAGS = struct( ...
            'IMAGE_IS_NAVIGATION_DATA',  1, ...
            'IMAGE_USER1',              57, ...
            'IMAGE_USER2',              58, ...
            'IMAGE_USER3',              59, ...
            'IMAGE_USER4',              60, ...
            'IMAGE_USER5',              61, ...
            'IMAGE_USER6',              62, ...
            'IMAGE_USER7',              63, ...
            'IMAGE_USER8',              64);
        
        DATA_TYPE = struct( ...
            'USHORT',   uint16(1), ...
            'SHORT',    uint16(2), ...
            'UINT',     uint16(3), ...
            'INT',      uint16(4), ...
            'FLOAT',    uint16(5), ...
            'DOUBLE',   uint16(6), ...
            'CXFLOAT',  uint16(7), ...
            'CXDOUBLE', uint16(8));
        
        IMAGE_TYPE = struct( ...
            'MAGNITUDE', uint16(1), ...
            'PHASE',     uint16(2), ...
            'REAL',      uint16(3), ...
            'IMAG',      uint16(4), ...
            'COMPLEX',   uint16(5));

    end
    
    methods
        
        function obj = ImageHeader(arg)
            switch nargin
                case 0
                    % No argument constructor
                    % initialize to a single image header
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
            nacq = length(obj.version);
        end
        
        function hdr = select(obj, range)
            % Return a copy of a range of image headers
            
            % create an empty image header
            M = length(range);
            hdr = ismrmrd.ImageHeader(M);
            
            % Fill
            hdr.version = obj.version(range);
            hdr.flags = obj.flags(range);
            hdr.measurement_uid = obj.measurement_uid(range);
            hdr.matrix_size = obj.matrix_size(:,range);
            hdr.field_of_view = obj.field_of_view(:,range);
            hdr.channels = obj.channels(:,range);
            hdr.position = obj.position(:,range);
            hdr.read_dir = obj.read_dir(:,range);
            hdr.phase_dir = obj.phase_dir(:,range);
            hdr.slice_dir = obj.slice_dir(:,range);
            hdr.patient_table_position = obj.patient_table_position(:,range);
            hdr.average = obj.average(range);
            hdr.slice = obj.slice(range);
            hdr.contrast = obj.contrast(range);
            hdr.phase = obj.phase(range);
            hdr.repetition = obj.repetition(range);
            hdr.set = obj.set(range);
            hdr.acquisition_time_stamp = obj.acquisition_time_stamp(range);
            hdr.physiology_time_stamp = obj.physiology_time_stamp(:,range);
            hdr.data_type = obj.data_type(range);
            hdr.image_type = obj.image_type(range);
            hdr.image_index = obj.image_index(range);
            hdr.image_series_index = obj.image_series_index(range);
            hdr.user_int = obj.user_int(:,range);
            hdr.user_float = obj.user_float(:,range);
            hdr.attribute_string_len = obj.attribute_string_len(:,range);

        end        
        
        function extend(obj,N)
            % Extend with blank header

            range = obj.getNumber + (1:N);            
            obj.version(1,range)                  = zeros(1,N,'uint16');
            obj.flags(1,range)                    = zeros(1,N,'uint64');
            obj.measurement_uid(1,range)          = zeros(1,N,'uint32');
            obj.matrix_size(1:3,range)            = zeros(3,N,'uint16');
            obj.field_of_view(1:3,range)          = zeros(3,N,'single');
            obj.channels(1,range)                 = zeros(1,N,'uint16');
            obj.position(1:3,range)               = zeros(3,N,'single');
            obj.read_dir(1:3,range)               = zeros(3,N,'single');
            obj.phase_dir(1:3,range)              = zeros(3,N,'single');
            obj.slice_dir(1:3,range)              = zeros(3,N,'single');
            obj.patient_table_position(1:3,range) = zeros(3,N,'single');
            obj.average(1,range)                  = zeros(1,N,'uint16');
            obj.slice(1,range)                    = zeros(1,N,'uint16');
            obj.contrast(1,range)                 = zeros(1,N,'uint16');
            obj.phase(1,range)                    = zeros(1,N,'uint16');
            obj.repetition(1,range)               = zeros(1,N,'uint16');
            obj.set(1,range)                      = zeros(1,N,'uint16');
            obj.acquisition_time_stamp(1,range)   = zeros(1,N,'uint32');
            obj.physiology_time_stamp(1:3,range)  = zeros(3,N,'uint32');
            obj.data_type(1,range)                = zeros(1,N,'uint16');
            obj.image_type(1,range)               = zeros(1,N,'uint16');
            obj.image_index(1,range)              = zeros(1,N,'uint16');
            obj.image_series_index(1,range)       = zeros(1,N,'uint16');
            obj.user_int(1:8,range)               = zeros(8,N,'int32');
            obj.user_float(1:8,range)             = zeros(8,N,'single');
            obj.attribute_string_len              = zeros(1,N,'uint32');
        end
        
        function append(obj, head)
            Nstart = obj.getNumber + 1;
            Nend   = obj.getNumber + length(head.version);
            Nrange = Nstart:Nend;
            obj.version(Nrange) = hdr.version;
            obj.flags(Nrange) = hdr.flags;
            obj.measurement_uid(Nrange) = hdr.measurement_uid;
            obj.matrix_size(:,Nrange) = hdr.matrix_size;
            obj.field_of_view(:,Nrange) = hdr.field_of_view;
            obj.channels(Nrange) = hdr.channels;
            obj.position(:,Nrange) = hdr.position;
            obj.read_dir(:,Nrange) = hdr.read_dir;
            obj.phase_dir(:,Nrange) = hdr.phase_dir;
            obj.slice_dir(:,Nrange) = hdr.slice_dir;
            obj.patient_table_position(:,Nrange) = hdr.patient_table_position;
            obj.average(Nrange) = hdr.average;
            obj.slice(Nrange) = hdr.slice;
            obj.contrast(Nrange) = hdr.contrast;
            obj.phase(Nrange) = hdr.phase;
            obj.repetition(Nrange) = hdr.repetition;
            obj.set(Nrange) = hdr.set;
            obj.acquisition_time_stamp(Nrange) = hdr.acquisition_time_stamp;
            obj.physiology_time_stamp(:,Nrange) = hdr.physiology_time_stamp;
            obj.data_type(Nrange) = hdr.data_type;
            obj.image_type(Nrange) = hdr.image_type;
            obj.image_index(Nrange) = hdr.image_index;
            obj.image_series_index(Nrange) = hdr.image_series_index;
            obj.user_int(:,Nrange) = hdr.user_int;
            obj.user_float(:,Nrange) = hdr.user_float;
            obj.attribute_string_len(Nrange) = hdr.attribute_string_len;
            
        end

        function fromStruct(obj, hdr)
            %warning! no error checking
            obj.version = hdr.version;
            obj.flags = hdr.flags;
            obj.measurement_uid = hdr.measurement_uid;
            obj.matrix_size = hdr.matrix_size;
            obj.field_of_view = hdr.field_of_view;
            obj.channels = hdr.channels;
            obj.position = hdr.position;
            obj.read_dir = hdr.read_dir;
            obj.phase_dir = hdr.phase_dir;
            obj.slice_dir = hdr.slice_dir;
            obj.patient_table_position = hdr.patient_table_position;
            obj.average = hdr.average;
            obj.slice = hdr.slice;
            obj.contrast = hdr.contrast;
            obj.phase = hdr.phase;
            obj.repetition = hdr.repetition;
            obj.set = hdr.set;
            obj.acquisition_time_stamp = hdr.acquisition_time_stamp;
            obj.physiology_time_stamp = hdr.physiology_time_stamp;
            obj.data_type = hdr.data_type;
            obj.image_type = hdr.image_type;
            obj.image_index = hdr.image_index;
            obj.image_series_index = hdr.image_series_index;
            obj.user_int = hdr.user_int;
            obj.user_float = hdr.user_float;
            obj.attribute_string_len = hdr.attribute_string_len;
        end

        function hdr = toStruct(obj)
            %warning! no error checking
            hdr = struct();
            hdr.version = obj.version;
            hdr.flags = obj.flags;
            hdr.measurement_uid = obj.measurement_uid;
            hdr.matrix_size = obj.matrix_size;
            hdr.field_of_view = obj.field_of_view;
            hdr.channels = obj.channels;
            hdr.position = obj.position;
            hdr.read_dir = obj.read_dir;
            hdr.phase_dir = obj.phase_dir;
            hdr.slice_dir = obj.slice_dir;
            hdr.patient_table_position = obj.patient_table_position;
            hdr.average = obj.average;
            hdr.slice = obj.slice;
            hdr.contrast = obj.contrast;
            hdr.phase = obj.phase;
            hdr.repetition = obj.repetition;
            hdr.set = obj.set;
            hdr.acquisition_time_stamp = obj.acquisition_time_stamp;
            hdr.physiology_time_stamp = obj.physiology_time_stamp;
            hdr.data_type = obj.data_type;
            hdr.image_type = obj.image_type;
            hdr.image_index = obj.image_index;
            hdr.image_series_index = obj.image_series_index;
            hdr.user_int = obj.user_int;
            hdr.user_float = obj.user_float;       
            hdr.attribute_string_len = obj.attribute_string_len;
        end
        
        function fromBytes(obj, bytearray)
            % Convert from a byte array to an ISMRMRD ImageHeader
	    % This conforms to the memory layout of the C-struct
            if size(bytearray,1) ~= 198
                error('Wrong number of bytes for ImageHeader.')
            end
            N = size(bytearray,2);
            for p = 1:N
                obj.version(p)                  = typecast(bytearray(1:2,p),     'uint16');
                obj.data_type(p)                = typecast(bytearray(3:4,p), 'uint16');
                obj.flags(p)                    = typecast(bytearray(5:12,p),    'uint64');
                obj.measurement_uid(p)          = typecast(bytearray(13:16,p),   'uint32');
                obj.matrix_size(:,p)            = typecast(bytearray(17:22,p),   'uint16');
                obj.field_of_view(:,p)          = typecast(bytearray(23:34,p),   'single');
                obj.channels(p)                 = typecast(bytearray(35:36,p),   'uint16');
                obj.position(:,p)               = typecast(bytearray(37:48,p),   'single');
                obj.read_dir(:,p)               = typecast(bytearray(49:60,p),   'single');
                obj.phase_dir(:,p)              = typecast(bytearray(61:72,p),   'single');
                obj.slice_dir(:,p)              = typecast(bytearray(73:84,p),   'single');
                obj.patient_table_position(:,p) = typecast(bytearray(85:96,p),   'single');
                obj.average(p)                  = typecast(bytearray(97:98,p),   'uint16');
                obj.slice(p)                    = typecast(bytearray(99:100,p),   'uint16');
                obj.contrast(p)                 = typecast(bytearray(101:102,p),  'uint16');
                obj.phase(p)                    = typecast(bytearray(103:104,p), 'uint16');
                obj.repetition(p)               = typecast(bytearray(105:106,p), 'uint16');
                obj.set(p)                      = typecast(bytearray(107:108,p), 'uint16');
                obj.acquisition_time_stamp(p)   = typecast(bytearray(109:112,p), 'uint32');
                obj.physiology_time_stamp(:,p)  = typecast(bytearray(113:124,p), 'uint32');                                                          
                obj.image_type(p)               = typecast(bytearray(125:126,p), 'uint16');
                obj.image_index(p)              = typecast(bytearray(127:128,p), 'uint16');
                obj.image_series_index(p)       = typecast(bytearray(129:130,p), 'uint16');
                obj.user_int(:,p)               = typecast(bytearray(131:162,p), 'uint32');
                obj.user_float(:,p)             = typecast(bytearray(163:194,p), 'single');
                obj.attribute_string_len        = typecast(bytearray(195:198,p), 'uint32');
                
            end              
        end
        
        function bytes = toBytes(obj)
            % Convert an ISMRMRD AcquisitionHeader to a byte array
	    % This conforms to the memory layout of the C-struct

            N = obj.getNumber;
            bytes = zeros(198,N,'uint8');
            for p = 1:N
                off = 1;
                bytes(off:off+1,p)   = typecast(obj.version(p)                 ,'uint8'); off=off+2;
                bytes(off:off+1,p)   = typecast(obj.data_type(p)               ,'uint8'); off=off+2;
                bytes(off:off+7,p)   = typecast(obj.flags(p)                   ,'uint8'); off=off+8;
                bytes(off:off+3,p)   = typecast(obj.measurement_uid(p)         ,'uint8'); off=off+4;
                bytes(off:off+5,p)   = typecast(obj.matrix_size(:,p)           ,'uint8'); off=off+6;
                bytes(off:off+11,p)  = typecast(obj.field_of_view(:,p)         ,'uint8'); off=off+12;
                bytes(off:off+1,p)   = typecast(obj.channels(p)                ,'uint8'); off=off+2;
                bytes(off:off+11,p)  = typecast(obj.position(:,p)              ,'uint8'); off=off+12;
                bytes(off:off+11,p)  = typecast(obj.read_dir(:,p)              ,'uint8'); off=off+12;
                bytes(off:off+11,p)  = typecast(obj.phase_dir(:,p)             ,'uint8'); off=off+12;
                bytes(off:off+11,p)  = typecast(obj.slice_dir(:,p)             ,'uint8'); off=off+12;
                bytes(off:off+11,p)  = typecast(obj.patient_table_position(:,p),'uint8'); off=off+12;
                bytes(off:off+1,p)   = typecast(obj.average(p)                 ,'uint8'); off=off+2;
                bytes(off:off+1,p)   = typecast(obj.slice(p)                   ,'uint8'); off=off+2;
                bytes(off:off+1,p)   = typecast(obj.contrast(p)                ,'uint8'); off=off+2;
                bytes(off:off+1,p)   = typecast(obj.phase(p)                   ,'uint8'); off=off+2;
                bytes(off:off+1,p)   = typecast(obj.repetition(p)              ,'uint8'); off=off+2;
                bytes(off:off+1,p)   = typecast(obj.set(p)                     ,'uint8'); off=off+2;
                bytes(off:off+3,p)   = typecast(obj.acquisition_time_stamp(p)  ,'uint8'); off=off+4;
                
                bytes(off:off+11,p)  = typecast(obj.physiology_time_stamp(:,p) ,'uint8'); off=off+12;
                
                bytes(off:off+1,p)   = typecast(obj.image_type(p)              ,'uint8'); off=off+2;
                bytes(off:off+1,p)   = typecast(obj.image_index(p)             ,'uint8'); off=off+2;
                bytes(off:off+1,p)   = typecast(obj.image_series_index(p)      ,'uint8'); off=off+2;
                bytes(off:off+31,p)  = typecast(obj.user_int(:,p)              ,'uint8'); off=off+32;
                bytes(off:off+31,p)  = typecast(obj.user_float(:,p)            ,'uint8'); off=off+32;
                bytes(off:off+3,p)  = typecast(obj.attribute_string_len(:,p)   ,'uint8'); 
                
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
            if ((size(obj.matrix_size,1) ~= 3) || ...
                (size(obj.matrix_size,2) ~= N))
                error('Size of matrix_size is not correct.');
            end
            if ((size(obj.field_of_view,1) ~= 3) || ...
                (size(obj.field_of_view,2) ~= N))
                error('Size of field_of_view is not correct.');
            end
            if ((size(obj.channels,1) ~= 1) || ...
                (size(obj.channels,2) ~= N))
                error('Size of field_of_view is not correct.');
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
            if ((size(obj.average,1) ~= 1) || ...
                (size(obj.average,2) ~= N))    
                error('Size of average is not correct.');
            end
            if ((size(obj.slice,1) ~= 1) || ...
                (size(obj.slice,2) ~= N))    
                error('Size of slice is not correct.');
            end
            if ((size(obj.contrast,1) ~= 1) || ...
                (size(obj.contrast,2) ~= N))    
                error('Size of contrast is not correct.');
            end
            if ((size(obj.phase,1) ~= 1) || ...
                (size(obj.phase,2) ~= N))    
                error('Size of phase is not correct.');
            end
            if ((size(obj.repetition,1) ~= 1) || ...
                (size(obj.repetition,2) ~= N))    
                error('Size of repetition is not correct.');
            end
            if ((size(obj.set,1) ~= 1) || ...
                (size(obj.set,2) ~= N))    
                error('Size of set is not correct.');
            end            
            if ((size(obj.acquisition_time_stamp,1) ~= 1) || ...
                (size(obj.acquisition_time_stamp,2) ~= N))
                error('Size of acquisition_time_stamp is not correct.');
            end
            if ((size(obj.physiology_time_stamp,1) ~= 3) || ...
                (size(obj.physiology_time_stamp,2) ~= N))
                error('Size of physiology_time_stamp is not correct.');
            end
            if ((size(obj.data_type,1) ~= 1) || ...
                (size(obj.data_type,2) ~= N))
                error('Size of image_data_type is not correct.');
            end
            if ((size(obj.image_type,1) ~= 1) || ...
                (size(obj.image_type,2) ~= N))
                error('Size of image_type is not correct.');
            end
            if ((size(obj.image_index,1) ~= 1) || ...
                (size(obj.image_index,2) ~= N))
                error('Size of image_index is not correct.');
            end
            if ((size(obj.image_series_index,1) ~= 1) || ...
                (size(obj.image_series_index,2) ~= N))
                error('Size of image_series_index is not correct.');
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
            obj.matrix_size = uint16(obj.matrix_size);
            obj.field_of_view = single(obj.field_of_view);
            obj.channels = uint16(obj.channels);
            obj.position = single(obj.position);
            obj.read_dir = single(obj.read_dir);
            obj.phase_dir = single(obj.phase_dir);
            obj.slice_dir = single(obj.slice_dir);
            obj.patient_table_position = single(obj.patient_table_position);
            obj.average = uint16(obj.average);
            obj.slice = uint16(obj.slice);
            obj.contrast = uint16(obj.contrast);
            obj.phase = uint16(obj.phase);
            obj.repetition = uint16(obj.repetition);
            obj.set = uint16(obj.set);
            obj.acquisition_time_stamp = uint32(obj.acquisition_time_stamp);
            obj.physiology_time_stamp = uint32(obj.physiology_time_stamp);            
            obj.data_type = uint16(obj.data_type);
            obj.image_type = uint16(obj.image_type);
            obj.image_index = uint16(obj.image_index);
            obj.image_series_index = uint16(obj.image_series_index);
            obj.user_int = int32(obj.user_int);
            obj.user_float = single(obj.user_float);
            obj.attribute_string_len = uint32(obj.attribute_string_len);
 
        end
        
        function ret = flagIsSet(obj, flag, range)
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
        
    end
    
end

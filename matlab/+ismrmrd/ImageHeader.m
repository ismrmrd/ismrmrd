classdef ImageHeader

    properties
        version = uint16(0);                          % First unsigned int indicates the version %
        flags = uint64(0);                             % bit field with flags %
        measurement_uid = uint32(0);                  % Unique ID for the measurement %
        matrix_size = zeros(3,1,'uint16');            % Pixels in the 3 spatial dimensions
        field_of_view = zeros(3,1,'single');          % Size (in mm) of the 3 spatial dimensions %
        channels = uint16(0);                         % Number of receive channels %
        position = zeros(3,1,'single');               % Three-dimensional spatial offsets from isocenter %
        read_dir = zeros(3,1,'single');         % Directional cosines of the readout/frequency encoding %
        phase_dir = zeros(3,1,'single');           % Directional cosines of the phase encoding %
        slice_dir = zeros(3,1,'single');         % Directional cosines of the slice %
        patient_table_position = zeros(3,1,'single'); % Patient table off-center %
        average = uint16(0);                          % e.g. signal average number %
        slice = uint16(0);                            % e.g. imaging slice number %
        contrast = uint16(0);                         % e.g. echo number in multi-echo %
        phase = uint16(0);                            % e.g. cardiac phase number %
        repetition = uint16(0);                       % e.g. dynamic number for dynamic scanning %
        set = uint16(0);                              % e.g. flow encodning set %
        acquisition_time_stamp = uint32(0);           % Acquisition clock %
        physiology_time_stamp = zeros(3,1,'uint32');  % Physiology time stamps, e.g. ecg, breating, etc. %
        image_data_type = uint16(0);                  % e.g. unsigned short, float, complex float, etc. %
        image_type = uint16(0);                       % e.g. magnitude, phase, complex, real, imag, etc. %
        image_index = uint16(0);					  % e.g. image number in series of images  %
        image_series_index = uint16(0);               % e.g. series number %
        user_int = zeros(8,1,'int32');                % Free user parameters %
        user_float = zeros(8,1,'single');             % Free user parameters %
    end
    
    methods
        
        % Constructor
        function obj = ImageHeader(s)
            if (nargin == 1)
                obj.version = s.version;
                obj.flags = s.flags;
                obj.measurement_uid = s.measurement_uid;
                obj.matrix_size = s.matrix_size;
                obj.field_of_view = s.field_of_view;
                obj.channels = s.channels;
                obj.position = s.position;
                obj.read_dir = s.read_dir;
                obj.phase_dir = s.phase_dir;
                obj_slice_dir = s.slice_dir;
                obj.patient_table_position = s.patient_table_position;
                obj.average = s.average;
                obj.slice = s.slice;
                obj.contrast = s.contrast;
                obj.phase = s.phase;
                obj.repetition = s.repetition;
                obj.set = s.set;
                obj.acquisition_time_stamp = s.acquisition_time_stamp;
                obj.physiology_time_stamp = s.physiology_time_stamp;
                obj.image_data_type = s.image_data_type;
                obj.image_type = s.image_type;
                obj.image_index = s.image_index;
                obj.image_series_index = s.image_series_index;
                obj.user_int = s.user_int;
                obj.user_float = s.user_float;
            end
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
            if (length(v)~=3)
                error('AcquisitionHeader.physiology_time_stamp must have 3 elements')
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

classdef ImageHeader

    properties
        version = uint16(0);                          % First unsigned int indicates the version %
        flag = uint64(0);                             % bit field with flags %
        measurement_uid = uint32(0);                  % Unique ID for the measurement %
        matrix_size = zeros(3,1,'uint16');            % Pixels in the 3 spatial dimensions
        field_of_view = zeros(3,1,'single');          % Size (in mm) of the 3 spatial dimensions %
        channels = uint16(0);                         % Number of receive channels %
        position = zeros(3,1,'single');               % Three-dimensional spatial offsets from isocenter %
        readout_cosines = zeros(3,1,'single');         % Directional cosines of the readout/frequency encoding %
        phase_cosines = zeros(3,1,'single');           % Directional cosines of the phase encoding %
        slice_cosines = zeros(3,1,'single');         % Directional cosines of the slice %
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
        
        function obj = set.version(obj,v)
            obj.version = uint16(v);
        end

        function obj = set.flag(obj,v)
            obj.flag = uint64(v);
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
        
        function obj = set.readout_cosines(obj,v)
            if (length(v)~=3)
                error('AcquisitionHeader.readout_cosines must have 3 elements')
            end            
            obj.readout_cosines = single(v);
        end
        
        function obj = set.phase_cosines(obj,v)
            if (length(v)~=3)
                error('AcquisitionHeader.phase_cosines must have 3 elements')
            end            
            obj.phase_cosines = single(v);
        end
        
        function obj = set.slice_cosines(obj,v)
            if (length(v)~=3)
                error('AcquisitionHeader.slice_cosines must have 3 elements')
            end            
            obj.slice_cosines = single(v);
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

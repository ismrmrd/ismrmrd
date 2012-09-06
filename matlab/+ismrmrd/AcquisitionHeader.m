classdef AcquisitionHeader
    
    properties
        version = uint16(0);                           % First unsigned int indicates the version %
        flag = uint64(0);                              % bit field with flags %
        measurement_uid = uint32(0);                   % Unique ID for the measurement %
        scan_counter = uint32(0);                      % Current acquisition number in the measurement %
        acquisition_time_stamp = uint32(0);            % Acquisition clock %
        physiology_time_stamp = zeros(1,3,'uint32');   % Physiology time stamps, e.g. ecg, breating, etc. %
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
        quaternion = zeros(4,1,'single');              % Angulation of acquisition %
        patient_table_position = zeros(3,1, 'single'); % Patient table off-center %
        idx = ismrmrd.EncodingCounters;                % Encoding loop counters, see above %
        user_int = zeros(8,1,'int32');                 % Free user parameters %
        user_float = zeros(8,1,'single');              % Free user parameters %
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
        
        function obj = set.scan_counter(obj,v)
            obj.scan_counter = uint32(v);
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
            obj.channel_mask = uint16(v);
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
        
        function obj = set.quaternion(obj,v)
            if (length(v)~=4)
                error('AcquisitionHeader.quaternion must have 4 elements')
            end            
            obj.quaternion = single(v);
        end
        
        function obj = set.patient_table_position(obj,v)
            if (length(v)~=3)
                error('AcquisitionHeader.patient_table_position must have 3 elements')
            end
            obj.patient_table_position = single(v);
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
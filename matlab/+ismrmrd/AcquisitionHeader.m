classdef AcquisitionHeader
    
    properties
        version = uint16(0);                           % First unsigned int indicates the version %
        flags = uint64(0);                             % bit field with flags %
        measurement_uid = uint32(0);                   % Unique ID for the measurement %
        scan_counter = uint32(0);                      % Current acquisition number in the measurement %
        acquisition_time_stamp = uint32(0);            % Acquisition clock %
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
        read_dir = zeros(3,1,'single');         % Directional cosines of the readout/frequency encoding %
        phase_dir = zeros(3,1,'single');           % Directional cosines of the phase encoding %
        slice_dir = zeros(3,1,'single');         % Directional cosines of the slice %
        patient_table_position = zeros(3,1, 'single'); % Patient table off-center %
        idx = ismrmrd.EncodingCounters;                % Encoding loop counters, see above %
        user_int = zeros(8,1,'int32');                 % Free user parameters %
        user_float = zeros(8,1,'single');              % Free user parameters %
    end
    
    methods
       
        % Constructor
        function obj = AcquisitionHeader(s)
            if (nargin == 1)
                obj.version = s.version;
                obj.flags = s.flags;
                obj.measurement_uid = s.measurement_uid;
                obj.scan_counter = s.scan_counter;
                obj.acquisition_time_stamp = s.acquisition_time_stamp;
                obj.physiology_time_stamp = s.physiology_time_stamp;
                obj.number_of_samples = s.number_of_samples;
                obj.available_channels = s.available_channels;
                obj.active_channels = s.active_channels;
                obj.channel_mask = s.channel_mask;
                obj.discard_pre = s.discard_pre;
                obj.discard_post = s.discard_post;
                obj.center_sample = s.center_sample;
                obj.encoding_space_ref = s.encoding_space_ref;
                obj.trajectory_dimensions = s.trajectory_dimensions;
                obj.sample_time_us = s.sample_time_us;
                obj.position = s.position;
                obj.read_dir = s.read_dir;
                obj.phase_dir = s.phase_dir;
                obj_slice_dir = s.slice_dir;
                obj.patient_table_position = s.patient_table_position;
                obj.idx = ismrmrd.EncodingCounters(s.idx);
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
        
        function obj = set.scan_counter(obj,v)
            obj.scan_counter = uint32(v);
        end
        
        function obj = set.acquisition_time_stamp(obj,v)
            obj.acquisition_time_stamp = uint32(v);
        end
        
        function obj = set.physiology_time_stamp(obj,v)
            if (length(v)~=8)
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

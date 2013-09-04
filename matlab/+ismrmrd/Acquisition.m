classdef Acquisition < handle

    properties
        head = [];
        traj = {};
        data = {};
    end

    methods

        function obj = Acquisition(arg1, traj, data)
            switch nargin
                case 0
                    % No argument constructor
                    % initialize to a single acquisition
                    extend(obj,1);
                    
                case 1
                    % One argument constructor
                    if isfield(arg1, 'version')
                        % First argument is a header
                        M = length(arg1.version);
                        obj.head = arg1;
                        obj.traj{M} = [];
                        obj.data{M} = [];
                    else
                        % First argument is a number
                        M = arg1;
                        extend(obj,M);
                    end
                    
                case 3
                    % Three argument constructor
                    obj.head = arg1;
                    M = length(arg1.version);
                    if isempty(traj)
                        obj.traj{M} = [];
                    else
                        obj.traj = traj;
                    end
                    if isempty(data)
                        obj.data{M} = [];
                    else
                        if isreal(data{1})
                            dataFromFloat(obj,data);
                        else
                            obj.data = data;
                        end
                    end
                    
            otherwise
                    error('ismrmrd.Acquistion constructor, wrong number of arguments.');
            end
        end
        
        function nacq = getNumberOfAcquisitions(obj)
            if isfield(obj.head,'version')
                nacq = length(obj.head.version);
            else
                nacq = 0;
            end
        end
        
        function acq = select(obj, range)
            % Return a copy of a range of acquisitions
            
            % create an empty acquisition
            M = length(range);
            acq = ismrmrd.Acquisition(M);
            % Fill the header
            acq.head.version = obj.head.version(range);
            acq.head.flags = obj.head.flags(range);
            acq.head.measurement_uid = obj.head.measurement_uid(range);
            acq.head.scan_counter = obj.head.scan_counter(range);
            acq.head.acquisition_time_stamp = obj.head.acquisition_time_stamp(range);
            acq.head.physiology_time_stamp = obj.head.physiology_time_stamp(:,range);
            acq.head.number_of_samples = obj.head.number_of_samples(range);
            acq.head.available_channels = obj.head.available_channels(range);
            acq.head.active_channels = obj.head.active_channels(range);
            acq.head.channel_mask = obj.head.channel_mask(:,range);
            acq.head.discard_pre = obj.head.discard_pre(range);
            acq.head.discard_post = obj.head.discard_post(range);
            acq.head.center_sample = obj.head.center_sample(range);
            acq.head.encoding_space_ref = obj.head.encoding_space_ref(range);
            acq.head.trajectory_dimensions = obj.head.trajectory_dimensions(range);
            acq.head.sample_time_us = obj.head.sample_time_us(range);
            acq.head.position = obj.head.position(:,range);
            acq.head.read_dir = obj.head.read_dir(:,range);
            acq.head.phase_dir = obj.head.phase_dir(:,range);
            acq.head.slice_dir = obj.head.slice_dir(:,range);
            acq.head.patient_table_position = obj.head.patient_table_position(:,range);
            acq.head.idx.kspace_encode_step_1 = obj.head.idx.kspace_encode_step_1(range);
            acq.head.idx.kspace_encode_step_2 = obj.head.idx.kspace_encode_step_2(range);
            acq.head.idx.average = obj.head.idx.average(range);
            acq.head.idx.slice = obj.head.idx.slice(range);
            acq.head.idx.contrast = obj.head.idx.contrast(range);
            acq.head.idx.phase = obj.head.idx.phase(range);
            acq.head.idx.repetition = obj.head.idx.repetition(range);
            acq.head.idx.set = obj.head.idx.set(range);
            acq.head.idx.segment = obj.head.idx.segment(range);
            acq.head.idx.user = obj.head.idx.user(:,range);
            acq.head.user_int = obj.head.user_int(:,range);
            acq.head.user_float = obj.head.user_float(:,range);
            
            % Fill the trajectory and the data
            for p = 1:M
                acq.traj{p} = obj.traj{range(p)};
                acq.data{p} = obj.data{range(p)};
            end    
        end
        
        function append(obj, head, traj, data)
            Nstart = obj.getNumberOfAcquisitions + 1;
            Nend   = obj.getNumberOfAcquisitions + length(head.version);
            Nrange = Nstart:Nend;
            obj.head.version(Nrange) = hdr.version;
            obj.head.flags(Nrange) = hdr.flags;
            obj.head.measurement_uid(Nrange) = hdr.measurement_uid;
            obj.head.scan_counter(Nrange) = hdr.scan_counter;
            obj.head.acquisition_time_stamp(Nrange) = hdr.acquisition_time_stamp;
            obj.head.physiology_time_stamp(:,Nrange) = hdr.physiology_time_stamp;
            obj.head.number_of_samples(Nrange) = hdr.number_of_samples;
            obj.head.available_channels(Nrange) = hdr.available_channels;
            obj.head.active_channels(Nrange) = hdr.active_channels;
            obj.head.channel_mask(:,Nrange) = hdr.channel_mask;
            obj.head.discard_pre(Nrange) = hdr.discard_pre;
            obj.head.discard_post(Nrange) = hdr.discard_post;
            obj.head.center_sample(Nrange) = hdr.center_sample;
            obj.head.encoding_space_ref(Nrange) = hdr.encoding_space_ref;
            obj.head.trajectory_dimensions(Nrange) = hdr.trajectory_dimensions;
            obj.head.sample_time_us(Nrange) = hdr.sample_time_us;
            obj.head.position(:,Nrange) = hdr.position;
            obj.head.read_dir(:,Nrange) = hdr.read_dir;
            obj.head.phase_dir(:,Nrange) = hdr.phase_dir;
            obj.head.slice_dir(:,Nrange) = hdr.slice_dir;
            obj.head.patient_table_position(:,Nrange) = hdr.patient_table_position;
            obj.head.idx.kspace_encode_step_1(Nrange) = hdr.idx.kspace_encode_step_1;
            obj.head.idx.kspace_encode_step_2(Nrange) = hdr.idx.kspace_encode_step_2;
            obj.head.idx.average(Nrange) = hdr.idx.average;
            obj.head.idx.slice(Nrange) = hdr.idx.slice;
            obj.head.idx.contrast(Nrange) = hdr.idx.contrast;
            obj.head.idx.phase(Nrange) = hdr.idx.phase;
            obj.head.idx.repetition(Nrange) = hdr.idx.repetition;
            obj.head.idx.set(Nrange) = hdr.idx.set;
            obj.head.idx.segment(Nrange) = hdr.idx.segment;
            obj.head.idx.user(:,Nrange) = hdr.idx.user;
            obj.head.user_int(:,Nrange) = hdr.user_int;
            obj.head.user_float(:,Nrange) = hdr.user_float;
            if isempty(traj) > 0
                obj.traj{Nrange} = traj;
            end
            if isempty(data) > 0
                obj.data{Nrange} = data;
            end
            
        end
        
        function extend(obj,N)
            % Extend with blank head and empty traj and data.
            M = N+obj.getNumberOfAcquisitions();
            hdr = ismrmrd.AcquisitionHeader();
            obj.head.version(M) = hdr.version;
            obj.head.flags(M) = hdr.flags;
            obj.head.measurement_uid(M) = hdr.measurement_uid;
            obj.head.scan_counter(M) = hdr.scan_counter;
            obj.head.acquisition_time_stamp(M) = hdr.acquisition_time_stamp;
            obj.head.physiology_time_stamp(:,M) = hdr.physiology_time_stamp;
            obj.head.number_of_samples(M) = hdr.number_of_samples;
            obj.head.available_channels(M) = hdr.available_channels;
            obj.head.active_channels(M) = hdr.active_channels;
            obj.head.channel_mask(:,M) = hdr.channel_mask;
            obj.head.discard_pre(M) = hdr.discard_pre;
            obj.head.discard_post(M) = hdr.discard_post;
            obj.head.center_sample(M) = hdr.center_sample;
            obj.head.encoding_space_ref(M) = hdr.encoding_space_ref;
            obj.head.trajectory_dimensions(M) = hdr.trajectory_dimensions;
            obj.head.sample_time_us(M) = hdr.sample_time_us;
            obj.head.position(:,M) = hdr.position;
            obj.head.read_dir(:,M) = hdr.read_dir;
            obj.head.phase_dir(:,M) = hdr.phase_dir;
            obj.head.slice_dir(:,M) = hdr.slice_dir;
            obj.head.patient_table_position(:,M) = hdr.patient_table_position;
            obj.head.idx.kspace_encode_step_1(M) = hdr.idx.kspace_encode_step_1;
            obj.head.idx.kspace_encode_step_2(M) = hdr.idx.kspace_encode_step_2;
            obj.head.idx.average(M) = hdr.idx.average;
            obj.head.idx.slice(M) = hdr.idx.slice;
            obj.head.idx.contrast(M) = hdr.idx.contrast;
            obj.head.idx.phase(M) = hdr.idx.phase;
            obj.head.idx.repetition(M) = hdr.idx.repetition;
            obj.head.idx.set(M) = hdr.idx.set;
            obj.head.idx.segment(M) = hdr.idx.segment;
            obj.head.idx.user(:,M) = hdr.idx.user;
            obj.head.user_int(:,M) = hdr.user_int;
            obj.head.user_float(:,M) = hdr.user_float;
            obj.traj{M} = [];
            obj.data{M} = []; 
        end
        
        function obj = setHead(obj,p,hdr)
            obj.head.version(p) = hdr.version;
            obj.head.flags(p) = hdr.flags;
            obj.head.measurement_uid(p) = hdr.measurement_uid;
            obj.head.scan_counter(p) = hdr.scan_counter;
            obj.head.acquisition_time_stamp(p) = hdr.acquisition_time_stamp; 
            obj.head.physiology_time_stamp(:,p) = hdr.physiology_time_stamp;
            obj.head.number_of_samples(p) = hdr.number_of_samples;
            obj.head.available_channels(p) = hdr.available_channels;
            obj.head.active_channels(p) = hdr.active_channels;
            obj.head.channel_mask(:,p) = hdr.channel_mask;
            obj.head.discard_pre(p) = hdr.discard_pre;
            obj.head.discard_post(p) = hdr.discard_post;
            obj.head.center_sample(p) = hdr.center_sample;
            obj.head.encoding_space_ref(p) = hdr.encoding_space_ref;
            obj.head.trajectory_dimensions(p) = hdr.trajectory_dimensions;
            obj.head.sample_time_us(p) = hdr.sample_time_us;
            obj.head.position(:,p) = hdr.position;
            obj.head.read_dir(:,p) = hdr.read_dir;
            obj.head.phase_dir(:,p) = hdr.phase_dir;
            obj.head.slice_dir(:,p) = hdr.slice_dir;
            obj.head.patient_table_position(:,p) = hdr.patient_table_position;
            obj.head.idx.kspace_encode_step_1(p) = hdr.idx.kspace_encode_step_1;
            obj.head.idx.kspace_encode_step_2(p) = hdr.idx.kspace_encode_step_2;
            obj.head.idx.average(p) = hdr.idx.average;
            obj.head.idx.slice(p) = hdr.idx.slice;
            obj.head.idx.contrast(p) = hdr.idx.contrast;
            obj.head.idx.phase(p) = hdr.idx.phase;
            obj.head.idx.repetition(p) = hdr.idx.repetition;
            obj.head.idx.set(p) = hdr.idx.set;
            obj.head.idx.segment(p) = hdr.idx.segment;
            obj.head.idx.user(:,p) = hdr.idx.user;
            obj.head.user_int(:,p) = hdr.user_int;
            obj.head.user_float(:,p) = hdr.user_float;
        end
        
        function dataFromFloat(obj,v)
            if (isempty(obj.head) || (length(v) ~= length(obj.head.version)))
                error('Mismatch between size of head and data.  Please set head first.');
            end
            obj.data = cell(1,length(v));
            for p = 1:length(v)
                dims = [obj.head.number_of_samples(p), ...
                        obj.head.active_channels(p)];
                buff = v{p};
                obj.data{p} = reshape(buff(1:2:end) + 1j*buff(2:2:end), dims);
            end
        end

        function v = dataToFloat(obj)
            v = cell(1,length(obj.data));
            for p = 1:length(obj.data)
                dim = size(obj.data{p});
                buff = zeros([2*prod(dim),1],'single');
                buff(1:2:end) = real(reshape(obj.data{p},prod(dim),1));
                buff(2:2:end) = imag(reshape(obj.data{p},prod(dim),1));
                v{p} = buff;
            end
        end
        
    end

end
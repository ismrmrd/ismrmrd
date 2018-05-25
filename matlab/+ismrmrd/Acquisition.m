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
                    if ismrmrd.util.isInt(arg1)
                        % First argument is a number
                        M = arg1;
                        extend(obj,M);
                    elseif isa(arg1,'ismrmrd.AcquisitionHeader')
                        obj.head = arg1;
                        obj.initializeData();
                    else                        
                        % First argument is a header (hopefully)
                        M = length(arg1.version);
                        obj.head = ismrmrd.AcquisitionHeader(arg1);
                        obj.initializeData();
                    end
                    
                case 3
                    % Three argument constructor
                    obj.head = ismrmrd.AcquisitionHeader(arg1);
                    M = length(arg1.version);
                    if isempty(traj)
                        obj.traj{M} = [];
                    else
                        trajFromFloat(obj,traj);
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
        
        function nacq = getNumber(obj)
            nacq = obj.head.getNumber();
        end
        
        function acq = select(obj, range)
            % Return a copy of a range of acquisitions
            
            % create an empty acquisition
            acq = ismrmrd.Acquisition();
            % Fill the header
            acq.head = obj.head.select(range);
            % Fill the trajectory and the data
            for p = 1:length(range)
                acq.traj{p} = obj.traj{range(p)};
                acq.data{p} = obj.data{range(p)};
            end    
        end
        
        function append(obj, head, traj, data)
            Nstart = obj.getNumber + 1;
            Nend   = obj.getNumber + length(head.version);
            Nrange = Nstart:Nend;
            obj.head.append(head);
            if isempty(traj) > 0
                obj.traj{Nrange} = traj;
            end
            if isempty(data) > 0
                obj.data{Nrange} = data;
            end
            
        end
        
        function extend(obj,N)
            % Extend with blank head and empty traj and data.
            if isempty(obj.head)
                M = N;
                obj.head = ismrmrd.AcquisitionHeader(N);
            else
                M = N+obj.getNumber();
                obj.head.extend(N);
            end
            obj.traj{M} = [];
            obj.data{M} = []; 
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
        
        function trajFromFloat(obj,v)
            if (isempty(obj.head) || (length(v) ~= length(obj.head.version)))
                error('Mismatch between size of head and trajectory.  Please set head first.');
            end
            obj.traj = cell(1,length(v));
            for p = 1:length(v)
                dims = [obj.head.trajectory_dimensions(p), ...
                        obj.head.number_of_samples(p)];
                obj.traj{p} = reshape(v{p}, dims);
            end
        end

        function v = trajToFloat(obj)
            v = cell(1,length(obj.traj));
            for p = 1:length(obj.traj)
                v{p} = single(obj.traj{p});
            end
        end               
    end
    
    methods(Access = private)
        function initializeData(obj)
            for k = 1:length(obj.head.version)
                obj.data{k} = complex(zeros(obj.head.number_of_samples(k),obj.head.active_channels(k),'single'));
                obj.traj{k} = zeros(obj.head.trajectory_dimensions(k),obj.head.number_of_samples(k),'single');
            end
            
        end
    end

end
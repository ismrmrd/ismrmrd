classdef Waveform < handle

    properties
        head = [];
        data = {};
    end

    methods

        function obj = Waveform(arg1, data)
            switch nargin
                case 0
                    % No argument constructor
                    % initialize to a single wavuisition
                    extend(obj,1);
                    
                case 1
                    % One argument constructor
                    if ismrmrd.util.isInt(arg1)
                        % First argument is a number
                        M = arg1;
                        extend(obj,M);
                    else                        
                        % First argument is a header (hopefully)
                        M = length(arg1.version);
                        obj.head = ismrmrd.WaveformHeader(arg1);
                        obj.data{M} = [];
                    end
                    
                case 2
                    % Two argument constructor
                    obj.head = ismrmrd.WaveformHeader(arg1);
                    M = length(arg1.version);
                    if isempty(data)
                        obj.data{M} = [];
                    else
                        obj.data = data;
                    end
                    
            otherwise
                    error('ismrmrd.Acquistion constructor, wrong number of arguments.');
            end
        end
        
        function nwav = getNumber(obj)
            nwav = obj.head.getNumber();
        end
        
        function wav = select(obj, range)
            % Return a copy of a range of waveforms
            
            % create an empty waveform
            wav = ismrmrd.Waveform();
            % Fill the header
            wav.head = obj.head.select(range);
            % Fill the data
            for p = 1:length(range)
                wav.data{p} = obj.data{range(p)};
            end    
        end
        
        function append(obj, head, traj, data)
            Nstart = obj.getNumber + 1;
            Nend   = obj.getNumber + length(head.version);
            Nrange = Nstart:Nend;
            obj.head.append(head);
            if isempty(data) > 0
                obj.data{Nrange} = data;
            end
            
        end
        
        function extend(obj,N)
            % Extend with blank head and empty data.
            if isempty(obj.head)
                M = N;
                obj.head = ismrmrd.WaveformHeader(N);
            else
                M = N+obj.getNumber();
                obj.head.extend(N);
            end
            obj.data{M} = []; 
        end
            
    end

end

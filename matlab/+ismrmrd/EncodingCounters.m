classdef EncodingCounters
    
    properties
        kspace_encode_step_1 = uint16(0); % e.g. phase encoding line number %
        kspace_encode_step_2 = uint16(0); % e.g. partition encodning number %
        average              = uint16(0); % e.g. signal average number %
        slice                = uint16(0); % e.g. imaging slice number %
        contrast             = uint16(0); % e.g. echo number in multi-echo %
        phase                = uint16(0); % e.g. cardiac phase number %
        repetition           = uint16(0); % e.g. dynamic number for dynamic scanning %
        set                  = uint16(0); % e.g. flow encodning set %
        segment              = uint16(0); % e.g. segment number for segmented acquisition %
        user                 = zeros(1,8,'uint16'); % Free user parameters %
    end
    
    methods

        % Constructor
        function obj = EncodingCounters(s)
            if (nargin == 1)
                obj.kspace_encode_step_1 = s.kspace_encode_step_1;
                obj.kspace_encode_step_2 = s.kspace_encode_step_2; 
                obj.average = s.average;
                obj.slice = s.slice;
                obj.contrast = s.contrast;
                obj.phase = s.phase;
                obj.repetition = s.repetition;
                obj.set = s.set;
                obj.segment = s.segment;
                obj.user = s.user;
            end
        end

        % Set methods
        function obj = set.kspace_encode_step_1(obj,v)
            obj.kspace_encode_step_1 = uint16(v);
        end
        
        function obj = set.kspace_encode_step_2(obj,v)
            obj.kspace_encode_step_2 = uint16(v);
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
        
        function obj = set.segment(obj,v)
            obj.segment = uint16(v);
        end
        
        function obj = set.user(obj,v)
            if (length(v)~=8)
                error('EncodingCounters.user must have 8 elements')
            end
            obj.user = uint16(v);
        end
        
    end
   
end % EncodingCounters

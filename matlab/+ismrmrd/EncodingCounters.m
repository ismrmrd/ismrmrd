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
        function obj = EncodingCounters(bytes)
            if (nargin == 1)
                obj.kspace_encode_step_1 = typecast(bytes( 1: 2),'uint16');
                obj.kspace_encode_step_2 = typecast(bytes( 3: 4),'uint16');
                obj.average              = typecast(bytes( 5: 6),'uint16');
                obj.slice                = typecast(bytes( 7: 8),'uint16');
                obj.contrast             = typecast(bytes( 9:10),'uint16');
                obj.phase                = typecast(bytes(11:12),'uint16');
                obj.repetition           = typecast(bytes(13:14),'uint16');
                obj.set                  = typecast(bytes(15:16),'uint16');
                obj.segment              = typecast(bytes(17:18),'uint16');
                obj.user                 = typecast(bytes(19:34),'uint16');
            end
        end

        function bytes = toBytes(obj)
            bytes = zeros(34,1,'int8');
            bytes( 1: 2) = typecast(obj.kspace_encode_step_1,'int8');
            bytes( 3: 4) = typecast(obj.kspace_encode_step_2,'int8');
            bytes( 5: 6) = typecast(obj.average             ,'int8');
            bytes( 7: 8) = typecast(obj.slice               ,'int8');
            bytes( 9:10) = typecast(obj.contrast            ,'int8');
            bytes(11:12) = typecast(obj.phase               ,'int8');
            bytes(13:14) = typecast(obj.repetition          ,'int8');
            bytes(15:16) = typecast(obj.set                 ,'int8');
            bytes(17:18) = typecast(obj.segment             ,'int8');
            bytes(19:34) = typecast(obj.user                ,'int8');
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

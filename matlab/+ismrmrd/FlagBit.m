classdef FlagBit
    
    properties
        bitmask = uint64(0);
    end
    
    methods
        function obj = FlagBit(b)
            if ~(b>0)
                error('b must be positive');
            else
                obj.bitmask = bitshift(uint64(1),(b-1));
            end
        end % FlagBit
    end
    
    methods
        
        function obj = set.bitmask(obj,b)
            obj.bitmask = uint64(b);
        end % bitmask set function
        
        function b = isSet(obj,m)
            if (bitand(obj.bitmask, uint64(m))>0)
                b = true;
            else
                b = false;
            end
        end % isSet
    end
    
end % classdef

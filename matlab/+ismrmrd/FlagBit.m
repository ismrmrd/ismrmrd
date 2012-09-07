classdef FlagBit
    properties
        bitmask_ = uint16(0);
    end
    
    methods
        function fb = FlagBit(b)
            if ~(b>0)
                error('b must be positive');
            else
                fb.bitmask_ = bitshift(uint16(1),(b-1));
            end
        end % FlagBit
    end
    
    methods
        
        function obj = set.bitmask_(obj,b)
            obj.bitmask_ = uint16(b);
        end % bitmask_ set function
        
        function b = isSet(obj,m)
            if (bitand(obj.bitmask_, uint16(m))>0)
               b = true;
            else
                b = false;
            end
        end % isSet
    end
    
end % classdef
classdef Flags
   
    methods
        
        function ret = isSet(obj, flagbits, flag)
            if isa(flag, 'char')
                b = obj.(flag);
            elseif (flag>0)
                b = uint64(flag);
            else
                error('Flag is of the wrong type.'); 
            end
            bitmask = bitshift(uint64(1),(b-1));
            
            ret = (bitand(flagbits, bitmask)>0);

        end
        
        function ret = set(obj, flagbits, flag)

            if isa(flag, 'char')
                b = obj.(flag);
            elseif (flag>0)
                b = uint64(flag);
            else
                error('Flag is of the wrong type.'); 
            end
            bitmask = bitshift(uint64(1),(b-1));

            if (obj.isSet(flagbits, flag))
                % Already set
                ret = flagbits;
            else
                ret = flagbits + bitmask;
            end
            
        end
        
        function ret =  clear(obj, flagbits, flag)
            if (nargin < 3)
                ret = uint64(0);
                return
            end
            
            if isa(flag, 'char')
                b = obj.(flag);
            elseif (flag>0)
                b = uint64(flag);
            else
                error('Flag is of the wrong type.'); 
            end
            bitmask = bitshift(uint64(1),(b-1));
            
            if ~(obj.isSet(flagbits, flag))
                % Already cleared
                ret = flagbits;
            else
                ret = flagbits - bitmask;
            end

        end

    end
  
end % classdef
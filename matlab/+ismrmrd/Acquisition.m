% Acquisition
classdef Acquisition
    
    % Properties
    properties

        head = ismrmrd.AcquisitionHeader;
        traj = [];
        data = [];

    end % Properties
    
    % Methods
    methods
        
        function obj = set.head(obj,v)
            obj.head = v;
        end
        
        function obj = set.traj(obj,v)
            obj.traj = single(v);
        end
        
        function obj = set.data(obj,v)
            if isreal(v)
                obj.data = single(v(1:2:end) + 1j*v(2:2:end));
            else
                obj.data = single(v);
            end
        end

        function b = isFlagSet(obj,flag)
            bitflag = ismrmrd.FlagBit(flag);
            b = bitflag.isSet(obj.head.flag);
        end
        
        function obj = setFlag(obj,flag)
            bitflag = ismrmrd.FlagBit(flag);            
            obj.head.flag = bitor(obj.head.flag, bitflag.bitmask);
        end
        
    end % Methods
    
end
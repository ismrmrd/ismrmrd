% Acquisition
classdef Acquisition
    
    % Properties
    properties

        head_ = ismrmrd.AcquisitionHeader;
        traj_ = [];
        data_ = [];

    end % Properties
    
    % Methods
    methods

        function obj = set.head_(obj,v)
            obj.head_ = v;
        end
        
        function obj = set.traj_(obj,v)
            obj.traj_ = single(v);
        end
        
        function obj = set.data_(obj,v)
            obj.data_ = single(complex(v));
        end

        function b = isFlagSet(obj,flag)
            bitflag = ismrmrd.FlagBit(flag);
            b = bitflag.isSet(obj.head_.flag);
        end
        
        function obj = setFlag(obj,flag)
            bitflag = ismrmrd.FlagBit(flag);            
            obj.head_.flag = bitor(obj.head_.flag, bitflag.bitmask_);
        end
        
    end % Methods
    
end
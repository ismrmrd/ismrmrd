% Acquisition
classdef Acquisition
    
    % Properties
    properties

        head = ismrmrd.AcquisitionHeader;
        traj = single([]);
        data = single([]);

    end % Properties
    
    % Methods
    methods
        
        function obj = set.head(obj,v)
            if isa(v,'ismrmrd.AcquisitionHeader')
                obj.head = v;
            else
                % not of the correct type, hope it's a struct 
                % and try to copy one element at a time
                u = fieldnames(obj.head);
                for p = 1:length(u)
                    obj.head = setfield(obj.head,u{p},getfield(v,u{p}));
                end
            end
        end
        
        function obj = set.traj(obj,v)
            obj.traj = single(v);
        end
        
        function obj = set.data(obj,v)
            obj.data = single(v);        
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
% Acquisition
classdef file

    properties
        fid = -1;
        filename = '';
        xml = '';  % remove this and replace with a get method
        datapath = '';
        xmlpath = '';
    end
    
    methods
        
        function obj = file(filename,groupname)
            
            % If the file exists, open it for read/write
            % otherwise, create it
            if exist(filename,'file')
                obj.fid = H5F.open(filename,'H5F_ACC_RDWR','H5P_DEFAULT');
            else
                fcpl = H5P.create('H5P_FILE_CREATE');
                obj.fid = H5F.create(filename,'H5F_ACC_TRUNC',fcpl,'H5P_DEFAULT');
                H5P.close(fcpl);
            end
            
            % Set the group name
            %   default is dataset
            if nargin == 1
                groupname = 'dataset';
            end
            % Set the paths
            grouppath = ['/' groupname];
            obj.xmlpath   = ['/' groupname '/xml'];
            obj.datapath  = ['/' groupname '/data'];
            
            % Check if the group exists
            %   if it does not exist, create it
            lapl_id=H5P.create('H5P_LINK_ACCESS');
            if (H5L.exists(obj.fid,grouppath,lapl_id) == 0)
                group_id = H5G.create(obj.fid, grouppath, 0);
                H5G.close(group_id);
            end
            H5P.close(lapl_id);
            
            % Read the XML
            obj.readxml();

            % Check if the Data exists
            %   if it does not exist, create it
            lapl_id=H5P.create('H5P_LINK_ACCESS');
            if (H5L.exists(obj.fid, obj.datapath, lapl_id) == 0)
                % Data does not exist
                %   create with rank 2, unlimited, and set the chunk size
                dims    = [1 1];
                maxdims = [H5ML.get_constant_value('H5S_UNLIMITED') 1];
                chunk = [1 1];
                space = H5S.create_simple(2, dims, maxdims);
                dcpl = H5P.create('H5P_DATASET_CREATE');
                H5P.set_chunk (dcpl, chunk);
                data_id = H5D.create(obj.fid, obj.datapath, ...
                                     ismrmrd.hdf5_datatypes.getType_Acquisition, ...
                                     space, dcpl);
                H5D.close(data_id);
            end
            H5P.close(lapl_id);
        
        end

        % destructor
        % is this necessary?
        %function delete(obj)
        %   obj.close();
        %end
        
        function obj = close(obj)
            H5F.close(obj.fid);
        end
        
        function obj = readxml(obj)
            
            % Set variable length string type
            xml_dtype = H5T.copy('H5T_C_S1');
            H5T.set_size(xml_dtype,'H5T_VARIABLE');

            % Check if the XML header exists
            %   if it does not exist, create it
            % TODO: set it's value to the default
            lapl_id=H5P.create('H5P_LINK_ACCESS');
            if (H5L.exists(obj.fid,obj.xmlpath,lapl_id) == 0)
                xml_dtype = H5T.copy('H5T_C_S1');
                H5T.set_size(xml_dtype,'H5T_VARIABLE');
                xml_space_id = H5S.create_simple (1, 1, []);
                xml_id = H5D.create (obj.fid, obj.xmlpath, xml_dtype, xml_space_id, 'H5P_DEFAULT');
                H5D.close(xml_id);
            end
            H5P.close(lapl_id);

            % Open
            xml_id = H5D.open(obj.fid, obj.xmlpath);

            % Read the data
            hdr = H5D.read(xml_id, xml_dtype, 'H5S_ALL', 'H5S_ALL', 'H5P_DEFAULT');
            obj.xml = hdr{1};

            % Close the XML                
            H5D.close (xml_id); 
        end
        
        function obj = writexml(obj,xmlstring)
            % No validation is performed.  You're on your own.
            
            % Set variable length string type
            xml_dtype = H5T.copy('H5T_C_S1');
            H5T.set_size(xml_dtype,'H5T_VARIABLE');

            % Check if the XML header exists
            %   if it does not exist, create it
            lapl_id=H5P.create('H5P_LINK_ACCESS');
            if (H5L.exists(obj.fid,obj.xmlpath,lapl_id) == 0)
                xml_space_id = H5S.create_simple (1, 1, []);
                xml_id = H5D.create (obj.fid, obj.xmlpath, xml_dtype, xml_space_id, 'H5P_DEFAULT');
                H5S.close(xml_space_id);
                H5D.close(xml_id);
            end
            H5P.close(lapl_id);

            % Open the data set and get the space
            xml_id = H5D.open(obj.fid, obj.xmlpath);
            xml_space_id = H5D.get_space(xml_id);
            %H5S.select_all(space);
            
            % Write the data
            H5D.write(xml_id, xml_dtype, 'H5S_ALL', xml_space_id, 'H5P_DEFAULT', xmlstring);
            
            % Close the XML
            H5S.close(xml_space_id);
            H5D.close(xml_id);
            
        end
    end
    
end

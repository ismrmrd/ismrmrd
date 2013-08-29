classdef IsmrmrdDataset

    properties
        fid = -1;
        filename = '';
        datapath = '';
        xmlpath = '';
        xmlhdr = [];
        htypes = [];
    end

    methods

        function obj = IsmrmrdDataset(filename,groupname)
            % add the ismrmrd jar to the javaclasspath
            ismrmrd.includejar();

            % Set the hdf types
            obj.htypes = ismrmrd.hdf5_datatypes;
                      
            % If the file exists, open it for read/write
            % otherwise, create it
            if exist(filename,'file')
                obj.fid = H5F.open(filename,'H5F_ACC_RDWR','H5P_DEFAULT');
            else
                fcpl = H5P.create('H5P_FILE_CREATE');
                obj.fid = H5F.create(filename,'H5F_ACC_TRUNC',fcpl,'H5P_DEFAULT');
                H5P.close(fcpl);
            end

            % Set the filename
            obj.filename = filename;

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
            lapl_id=H5P.create('H5P_LINK_ACCESS');
            if (H5L.exists(obj.fid,grouppath,lapl_id) == 0)
                % group does not exist, create it
                group_id = H5G.create(obj.fid, grouppath, 0);
                H5G.close(group_id);
                % create a default xml header object
                %obj.xmlhdr = ismrmrd.XMLHeader();
                obj.xmlhdr = org.ismrm.ismrmrd.IsmrmrdHeader();
            else
                % group exists, read the xml header
                %obj.xmlhdr = ismrmrd.XMLHeader().stringToHeader(obj.readxml());
                obj.xmlhdr = org.ismrm.ismrmrd.XMLString.StringToIsmrmrdHeader(obj.readxml());
            end
            H5P.close(lapl_id);

        end

        function obj = close(obj)
            % synchronize the xml header
            obj.writexml(obj.xmlstring());
            % close the file
            H5F.close(obj.fid);
        end

        function xmlstring = xmlstring(obj)
            % convert xmlhdr to a string
            %xmlstring = ismrmrd.XMLHeader.headerToString(obj.xmlhdr);
            xmlstring = org.ismrm.ismrmrd.XMLString.IsmrmrdHeaderToString(obj.xmlhdr);
        end
        
        function xmlstring = readxml(obj)
            % Check if the XML header exists
            % TODO: set it's value to the default
            lapl_id=H5P.create('H5P_LINK_ACCESS');
            if (H5L.exists(obj.fid,obj.xmlpath,lapl_id) == 0)
                error('No XML header found.');
            end
            H5P.close(lapl_id);

            % Open
            xml_id = H5D.open(obj.fid, obj.xmlpath);

            % Get the type
            xml_dtype = H5D.get_type(xml_id);

            % Read the data
            hdr = H5D.read(xml_id, xml_dtype, 'H5S_ALL', 'H5S_ALL', 'H5P_DEFAULT');

            % Output depends on whether or not the stored string was variale length
            if (H5T.is_variable_str(xml_dtype))
                xmlstring = hdr{1};
            else
                xmlstring = hdr';
            end

            % Close the XML
            H5T.close(xml_dtype);
            H5D.close (xml_id);
        end

        function writexml(obj,xmlstring)
            % No validation is performed.  You're on your own.
            % make sure it's a char
            xmlstring = char(xmlstring);
            
            % TODO: add error checking on the write and return a status
            % TODO: if the matlab variable length string bug is resolved
            % then we should change this logic to just modify the length
            % and overwrite.

            % Check if the XML header exists
            %   if it does not exist, create it
            %   if it exists, modify the size appropriately
            lapl_id=H5P.create('H5P_LINK_ACCESS');
            if (H5L.exists(obj.fid,obj.xmlpath,lapl_id) == 1)
                % Delete it
                H5L.delete(obj.fid, obj.xmlpath,'H5P_DEFAULT');
            end
            H5P.close(lapl_id);

            % Set variable length string type
            xml_dtype = H5T.copy('H5T_C_S1');
            % Matlab is having trouble writing variable length strings
            % that are longer that 512 characters.  Switched to fixed
            % length.
            % H5T.set_size(xml_dtype,'H5T_VARIABLE');
            H5T.set_size(xml_dtype, length(xmlstring));
            xml_space_id = H5S.create_simple (1, 1, []);
            xml_id = H5D.create (obj.fid, obj.xmlpath, xml_dtype, ....
                                 xml_space_id, 'H5P_DEFAULT');
            H5S.close(xml_space_id);

            % Write the data
            H5D.write(xml_id, xml_dtype, ...
                      'H5S_ALL', 'H5S_ALL', 'H5P_DEFAULT', xmlstring);

            % Close the XML
            H5D.close(xml_id);
        end

        function nacq = getNumberOfAcquisitions(obj)

            % Check if the Data exists
            lapl_id=H5P.create('H5P_LINK_ACCESS');
            if (H5L.exists(obj.fid, obj.datapath, lapl_id) == 0)
                error([obj.datapath ' does not exist in the HDF5 dataset.']);
            end
            dset = H5D.open(obj.fid, obj.datapath);
            space = H5D.get_space(dset);
            H5S.get_simple_extent_dims(space);
            [~,dims,~] = H5S.get_simple_extent_dims(space);
            nacq = dims(1);
            H5S.close(space);
            H5D.close(dset);

        end

        function acq = readAcquisition(obj, nacq)

            % Check if the Data exists
            lapl_id=H5P.create('H5P_LINK_ACCESS');
            if (H5L.exists(obj.fid, obj.datapath, lapl_id) == 0)
                error([obj.datapath ' does not exist in the HDF5 dataset.']);
            end

            % Open the data
            dset_id = H5D.open(obj.fid, obj.datapath);

            % Open the data space
            file_space_id = H5D.get_space(dset_id);

            % Initialize the return object
            acq = ismrmrd.Acquisition;

            % Create a mem_space for reading
            dims = [1 1];
            mem_space_id = H5S.create_simple(2,dims,[]);

            % Read the desired acquisition
            offset = [nacq-1 0];
            H5S.select_hyperslab(file_space_id,'H5S_SELECT_SET',offset,[1 1],[1 1],[1 1]);
            d = H5D.read(dset_id, obj.htypes.T_Acquisition, ...
                            mem_space_id, file_space_id, 'H5P_DEFAULT');

            % Set the structure bits
            acq.head = d.head(1);
            acq.traj = d.traj{1};
            t = d.data{1};
            acq.data = t(1:2:end) + 1j*t(2:2:end);

            % Clean up
            H5S.close(mem_space_id);
            H5S.close(file_space_id);
            H5D.close(dset_id);
        end

        function appendAcquisition(obj, acq)
            % Append an acquisition

            % TODO: Check the type of the input

            % Check if the Data exists
            %   if it does not exist, create it
            %   if it does exist increase it's size by one
            lapl_id=H5P.create('H5P_LINK_ACCESS');
            if (H5L.exists(obj.fid, obj.datapath, lapl_id) == 0)
                % Data does not exist
                %   create with rank 2, unlimited, and set the chunk size
                dims    = [1 1];
                maxdims = [H5ML.get_constant_value('H5S_UNLIMITED') 1];
                file_space_id = H5S.create_simple(2, dims, maxdims);

                dcpl = H5P.create('H5P_DATASET_CREATE');
                chunk = [1 1];
                H5P.set_chunk (dcpl, chunk);
                data_id = H5D.create(obj.fid, obj.datapath, ...
                                     obj.htypes.T_Acquisition, ...
                                     file_space_id, dcpl);
                H5P.close(dcpl);
                H5S.close(file_space_id);

            else
                % Open the data
                data_id = H5D.open(obj.fid, obj.datapath);

                % Open the data space
                file_space_id = H5D.get_space(data_id);

                % Get the size, increment by one
                H5S.get_simple_extent_dims(file_space_id);
                [~,dims,~] = H5S.get_simple_extent_dims(file_space_id);
                dims = [dims(1)+1, 1];
                H5D.set_extent (data_id, dims);
                H5S.close(file_space_id);

            end
            H5P.close(lapl_id);

            % Get the file space (room for one more acq)
            file_space_id = H5D.get_space(data_id);
            [~,dims,~] = H5S.get_simple_extent_dims(file_space_id);

            % Select the last block
            offset = [dims(1)-1 0];
            H5S.select_hyperslab(file_space_id,'H5S_SELECT_SET',offset,[],[],[]);

            % Mem space
            mem_space_id = H5S.create_simple(2,[1 1],[]);

            % Pack the acquisition into the correct struct for writing
            swarn = warning('query','MATLAB:structOnObject');
            warning('off', 'MATLAB:structOnObject')
            d.head(1) = struct(acq.head);
            d.head(1).idx = struct(acq.head.idx);
            warning(swarn.state, 'MATLAB:structOnObject')
            d.traj{1} = acq.traj;
            t = zeros(2*length(acq.data),1,'single');
            t(1:2:end) = real(acq.data);
            t(2:2:end) = imag(acq.data);
            d.data{1} = t;

            % Write
            H5D.write(data_id, obj.htypes.T_Acquisition, ...
                      mem_space_id, file_space_id, 'H5P_DEFAULT', d);

            % Clean up
            H5S.close(mem_space_id);
            H5S.close(file_space_id);
            H5D.close(data_id);
        end

    end

end

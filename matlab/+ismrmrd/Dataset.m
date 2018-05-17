classdef Dataset

    properties
        fid = -1;
        filename = '';
        datapath = '';
        waveformpath = '';
        xmlpath = '';
        htypes = [];
    end

    methods

        function obj = Dataset(filename,groupname)

            % Set the hdf types
            obj.htypes = ismrmrd.util.hdf5_datatypes;
                      
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
            obj.waveformpath= ['/' groupname '/waveforms'];

            % Check if the group exists
            lapl_id=H5P.create('H5P_LINK_ACCESS');
            if (H5L.exists(obj.fid,grouppath,lapl_id) == 0)
                % group does not exist, create it
                group_id = H5G.create(obj.fid, grouppath, 0);
                H5G.close(group_id);
            end
            H5P.close(lapl_id);

        end

        function obj = close(obj)
            % close the file
            H5F.close(obj.fid);
        end
        
        function xmlstring = readxml(obj)
            % Check if the XML header exists
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
            H5T.set_size(xml_dtype,'H5T_VARIABLE');
            %H5T.set_size(xml_dtype, length(xmlstring));
            xml_space_id = H5S.create_simple (1, 1, []);
            xml_id = H5D.create(obj.fid, obj.xmlpath, xml_dtype, ....
                                 xml_space_id, 'H5P_DEFAULT');
            H5S.close(xml_space_id);

            % Write the data
            H5D.write(xml_id, xml_dtype, ...
                      'H5S_ALL', 'H5S_ALL', 'H5P_DEFAULT', {xmlstring});

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

        function block = readAcquisition(obj, start, stop)
            if nargin == 1
                % Read all the acquisitions
                start = 1;
                stop = -1;
            elseif nargin == 2
                % Read a single acquisition
                stop = start;
            end

            % Check if the Data exists
            lapl=H5P.create('H5P_LINK_ACCESS');
            if (H5L.exists(obj.fid, obj.datapath, lapl) == 0)
                error([obj.datapath ' does not exist in the HDF5 dataset.']);
            end

            % Open the data
            dset = H5D.open(obj.fid, obj.datapath);

            % Open the data space
            space = H5D.get_space(dset);
            
            % Get the size
            [~,dims,~] = H5S.get_simple_extent_dims(space);
            nacq = dims(1);

            % Create a mem_space for reading
            if (stop >= start)
                offset = [start-1];
                dims = [stop-start+1];
                mem_space = H5S.create_simple(1,dims,[]);
            else
                offset = [0];
                dims = [nacq];
                mem_space = H5S.create_simple(1,dims,[]);
            end

            % Read the desired acquisitions            
            H5S.select_hyperslab(space,'H5S_SELECT_SET',offset,[1],[1],dims);
            d = H5D.read(dset, obj.htypes.T_Acquisition, ...
                         mem_space, space, 'H5P_DEFAULT');
                     
            % Pack'em
            block = ismrmrd.Acquisition(d.head, d.traj, d.data);

            % Clean up
            H5S.close(mem_space);
            H5S.close(space);
            H5D.close(dset);
        end

        function appendAcquisition(obj, acq)
            % Append an acquisition

            % TODO: Check the type of the input
            
            % The number of acquisitions that we are going to append
            N = acq.getNumber();
            
            % Check if the Data exists
            %   if it does not exist, create it
            %   if it does exist increase it's size
            lapl_id=H5P.create('H5P_LINK_ACCESS');
            if (H5L.exists(obj.fid, obj.datapath, lapl_id) == 0)
                % Data does not exist
                %   create with rank 1, unlimited, and set the chunk size
                dims    = [N];
                maxdims = [H5ML.get_constant_value('H5S_UNLIMITED')];
                file_space_id = H5S.create_simple(1, dims, maxdims);

                dcpl = H5P.create('H5P_DATASET_CREATE');
                chunk = [1];
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

                % Get the size, increment by N
                H5S.get_simple_extent_dims(file_space_id);
                [~,dims,~] = H5S.get_simple_extent_dims(file_space_id);
                dims = [dims(1)+N];
                H5D.set_extent (data_id, dims);
                H5S.close(file_space_id);

            end
            H5P.close(lapl_id);

            % Get the file space
            file_space_id = H5D.get_space(data_id);
            [~,dims,~] = H5S.get_simple_extent_dims(file_space_id);

            % Select the last N block
            offset = [dims(1)-N];
            H5S.select_hyperslab(file_space_id,'H5S_SELECT_SET',offset,[1],[1],[N]);

            % Mem space
            mem_space_id = H5S.create_simple(1,[N],[]);

            % Check and fix the acquisition header types
            acq.head.check();
            % TODO: Error checking on the sizes of the data and trajectories.
            
            % Pack the acquisition into the correct struct for writing
            d = struct();
            d.head = acq.head.toStruct();
            d.traj = acq.trajToFloat();
            d.data = acq.dataToFloat();
            
            % Write
            H5D.write(data_id, obj.htypes.T_Acquisition, ...
                      mem_space_id, file_space_id, 'H5P_DEFAULT', d);

            % Clean up
            H5S.close(mem_space_id);
            H5S.close(file_space_id);
            H5D.close(data_id);
        end


        function nacq = getNumberOfWaveforms(obj)

            % Check if the Data exists
            lapl_id=H5P.create('H5P_LINK_ACCESS');
            if (H5L.exists(obj.fid, obj.waveformpath, lapl_id) == 0)
                error([obj.datapath ' does not exist in the HDF5 dataset.']);
            end
            dset = H5D.open(obj.fid, obj.waveformpath);
            space = H5D.get_space(dset);
            H5S.get_simple_extent_dims(space);
            [~,dims,~] = H5S.get_simple_extent_dims(space);
            nacq = dims(1);
            H5S.close(space);
            H5D.close(dset);

        end

        function block = readWaveform(obj, start, stop)
            if nargin == 1
                % Read all the acquisitions
                start = 1;
                stop = -1;
            elseif nargin == 2
                % Read a single acquisition
                stop = start;
            end

            % Check if the Data exists
            lapl=H5P.create('H5P_LINK_ACCESS');
            if (H5L.exists(obj.fid, obj.waveformpath, lapl) == 0)
                error([obj.waveformpath ' does not exist in the HDF5 dataset.']);
            end

            % Open the data
            dset = H5D.open(obj.fid, obj.waveformpath);

            % Open the data space
            space = H5D.get_space(dset);
            
            % Get the size
            [~,dims,~] = H5S.get_simple_extent_dims(space);
            nacq = dims(1);

            % Create a mem_space for reading
            if (stop >= start)
                offset = [start-1];
                dims = [stop-start+1];
                mem_space = H5S.create_simple(1,dims,[]);
            else
                offset = [0];
                dims = [nacq];
                mem_space = H5S.create_simple(1,dims,[]);
            end

            % Read the desired acquisitions            
            H5S.select_hyperslab(space,'H5S_SELECT_SET',offset,[1],[1],dims);
            d = H5D.read(dset, obj.htypes.T_Waveform, ...
                         mem_space, space, 'H5P_DEFAULT');
                     
            % Pack'em
            block = ismrmrd.Waveform(d.head, d.data);

            % Clean up
            H5S.close(mem_space);
            H5S.close(space);
            H5D.close(dset);
        end

        function appendWaveform(obj, wav)
            % Append an acquisition

            % TODO: Check the type of the input
            
            % The number of acquisitions that we are going to append
            N = wav.getNumber();
            
            % Check if the Data exists
            %   if it does not exist, create it
            %   if it does exist increase it's size
            lapl_id=H5P.create('H5P_LINK_ACCESS');
            if (H5L.exists(obj.fid, obj.waveformpath, lapl_id) == 0)
                % Data does not exist
                %   create with rank 1, unlimited, and set the chunk size
                dims    = [N];
                maxdims = [H5ML.get_constant_value('H5S_UNLIMITED')];
                file_space_id = H5S.create_simple(1, dims, maxdims);

                dcpl = H5P.create('H5P_DATASET_CREATE');
                chunk = [1];
                H5P.set_chunk (dcpl, chunk);
                data_id = H5D.create(obj.fid, obj.waveformpath, ...
                                     obj.htypes.T_Waveform, ...
                                     file_space_id, dcpl);
                H5P.close(dcpl);
                H5S.close(file_space_id);

            else
                % Open the data
                data_id = H5D.open(obj.fid, obj.waveformpath);

                % Open the data space
                file_space_id = H5D.get_space(data_id);

                % Get the size, increment by N
                H5S.get_simple_extent_dims(file_space_id);
                [~,dims,~] = H5S.get_simple_extent_dims(file_space_id);
                dims = [dims(1)+N];
                H5D.set_extent (data_id, dims);
                H5S.close(file_space_id);

            end
            H5P.close(lapl_id);

            % Get the file space
            file_space_id = H5D.get_space(data_id);
            [~,dims,~] = H5S.get_simple_extent_dims(file_space_id);

            % Select the last N block
            offset = [dims(1)-N];
            H5S.select_hyperslab(file_space_id,'H5S_SELECT_SET',offset,[1],[1],[N]);

            % Mem space
            mem_space_id = H5S.create_simple(1,[N],[]);

            % Check and fix the acquisition header types

            % TODO: Error checking on the sizes of the data and trajectories.
            
            % Pack the acquisition into the correct struct for writing
            d = struct();
            d.head = wav.head.toStruct();
            d.data = cellfun(@uint32,wav.data,'UniformOutput',false);
            
            
            % Write
            H5D.write(data_id, obj.htypes.T_Waveform, ...
                      mem_space_id, file_space_id, 'H5P_DEFAULT', d);

            % Clean up
            H5S.close(mem_space_id);
            H5S.close(file_space_id);
            H5D.close(data_id);
        end
        
        
        function delete(obj)
            H5F.close(obj.fid);
        end


    end

end

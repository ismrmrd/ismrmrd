function [xmlhdr data nacq] = testread(filename)

    % Open the HDF5 File
    file = H5F.open(filename, 'H5F_ACC_RDONLY', 'H5P_DEFAULT');

    % Open the xml
    dset_id = H5D.open(file, '/dataset/xml');

    % Get the dataspace
    file_space_id = H5D.get_space(dset_id);

    % Set variable length string type
    ismrm.datatypes.xmlhead = H5T.copy('H5T_C_S1');
    H5T.set_size(ismrm.datatypes.xmlhead,'H5T_VARIABLE');

    % Read the data
    hdr = H5D.read(dset_id, ismrm.datatypes.xmlhead, 'H5S_ALL', 'H5S_ALL', 'H5P_DEFAULT');
    xmlhdr = hdr{1};

    % Close the XML Header
    H5D.close (dset_id);
    H5S.close (file_space_id);

    % Open the data
    dset_id = H5D.open(file, '/dataset/data');

    % Get the number of acquisitions
    file_space_id = H5D.get_space(dset_id);
    H5S.get_simple_extent_dims(file_space_id);
    [~,dims,~] = H5S.get_simple_extent_dims(file_space_id);
    nacq = dims(1);

    % Initialize the return array of structs
    data(nacq) = ismrmrd.Acquisition;
        
    % Loop over the acquisitions and read them in
    dims = [1 1];
    mem_space_id = H5S.create_simple(2,dims,[]);
 
    for p = 1:nacq
        
        offset = [p-1 0];
        H5S.select_hyperslab(file_space_id,'H5S_SELECT_SET',offset,[1 1],[1 1],[1 1]);
        d = H5D.read(dset_id, ismrmrd.hdf5_datatypes.getType_Acquisition, ...
                        mem_space_id, file_space_id, 'H5P_DEFAULT');

    	data(p).head = d.head(1);
    	data(p).traj = d.traj{1};
    	data(p).data = d.data{1};

    end
    
    % Close the data
    H5S.close (file_space_id);
    H5D.close (dset_id);

    % Close the file
    H5F.close (file)

end

    % Read all of the acquisitions at once
    %data = H5D.read(dset_id, ismrmrd.hdf5_datatypes.getType_Acquisition, 'H5S_ALL','H5S_ALL','H5P_DEFAULT');


    
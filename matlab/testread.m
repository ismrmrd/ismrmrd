%filename = '~/temp/testdata.h5';
filename = '~/gadgetron/ismrmrd_data/testdata.h5';

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
xmldata = H5D.read(dset_id, ismrm.datatypes.xmlhead, 'H5S_ALL', 'H5S_ALL', 'H5P_DEFAULT');

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

% Read the first acquisition
dims = [1 1];
mem_space_id = H5S.create_simple(2,dims,[]);
offset = [0 0];
block = [1 1];
H5S.select_hyperslab(file_space_id,'H5S_SELECT_SET',offset,[],[],block);
data = H5D.read(dset_id, ismrmrd.hdf5_datatypes.getType_Acquisition, ...
                mem_space_id, file_space_id, 'H5P_DEFAULT');
acq1 = ismrmrd.Acquisition;
acq1.head_ = data.head(1);
acq1.traj_ = data.traj{1};
acq1.data_ = data.data{1};

% Read the 70th acquisition
offset = [69 0];
H5S.select_hyperslab(file_space_id,'H5S_SELECT_SET',offset,[],[],block);
data = H5D.read(dset_id, ismrmrd.hdf5_datatypes.getType_Acquisition, ...
                mem_space_id, file_space_id, 'H5P_DEFAULT');
acq70 = ismrmrd.Acquisition;
acq70.head_ = data.head(1);
acq70.traj_ = data.traj{1};
acq70.data_ = data.data{1};

% Read all of the acquisitions at once
%data = H5D.read(dset_id, ismrmrd.hdf5_datatypes.getType_Acquisition, 'H5S_ALL','H5S_ALL','H5P_DEFAULT');



% Close the data
%H5D.close (dset);
%H5S.close (space);

% Close the file
%H5F.close (file)

%H5T.close (memtype);

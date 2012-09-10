filename = '~/temp/testdata.h5';

% Open the HDF5 File
file = H5F.open(filename, 'H5F_ACC_RDONLY', 'H5P_DEFAULT');

% Open the xml
dset = H5D.open(file, '/dataset/xml');

% Get the dataspace
space = H5D.get_space(dset);

% Set variable length string type
ismrm.datatypes.xmlhead = H5T.copy('H5T_C_S1');
H5T.set_size(ismrm.datatypes.xmlhead,'H5T_VARIABLE');

% Read the data
xmldata = H5D.read(dset, ismrm.datatypes.xmlhead, 'H5S_ALL', 'H5S_ALL', 'H5P_DEFAULT');

% Close the XML Header
H5D.close (dset);
H5S.close (space);

% Open the data
dset = H5D.open(file, '/dataset/data');
space = H5D.get_space(dset);
H5S.get_simple_extent_dims(space);
[~,dims,~] = H5S.get_simple_extent_dims(space);
nacq = dims(1);
% Select the first acquisition
%data = H5D.read(dset, 'H5ML_DEFAULT', 'H5S_ALL','H5S_ALL','H5P_DEFAULT');
data = H5D.read(dset, ismrmrd.hdf5_datatypes.getType_Acquisition, 'H5S_ALL','H5S_ALL','H5P_DEFAULT');

% Close the data
%H5D.close (dset);
%H5S.close (space);

% Close the file
%H5F.close (file)

%H5T.close (memtype);

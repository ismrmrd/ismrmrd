function example()

filename = 'test.h5';

if (exist(filename, 'file'))
    delete(filename) ;
end

% Create the dataset
i = ismrmrd.IsmrmrdDataset(filename, 'dataset');

ah = ismrmrd.AcquisitionHeader();
ah.center_sample = 42;
ah.active_channels = 32;
ah.version = 2;
ah.position = [3.14, 3.14, 3.14]

a = ismrmrd.Acquisition();
a.head = ah;
a.data = (1:512);
a.traj = (1:3);

i.appendAcquisition(a);

% Write the dataset
i.close();

% Open the dataset
i = ismrmrd.IsmrmrdDataset(filename, 'dataset')

% Read the dataset
nacq = i.getNumberOfAcquisitions();
a = i.readAcquisition(1);

h = a.head;

h.position

i.close();

end

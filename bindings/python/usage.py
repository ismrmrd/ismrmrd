import h5py

HEADER_PATH = 'xml'
DATA_PATH = 'data'

class IsmrmrdDataset(object):
    def __init__(self, filename, dataset):
        self.filename = filename
        self.dsetname = dataset
        self.h5file = None
        self.h5dset = None

    def load(self):
        self.h5file = h5py.File(self.filename)
        self.h5dset = self.filename[self.dsetname]

        a = self.h5dset[HEADER_PATH].value
        self.header = str(a[0])

    def append_array(self, array, name):
        self.h5dset[name] = array

    def read_array(self, name, index):
        return self.h5dset[name]

    def append_acquisition(Acquisition)

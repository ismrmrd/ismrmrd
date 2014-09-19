cimport cismrmrd
cimport numpy as np
from libc.stdlib cimport calloc, free
from libc.string cimport memcpy

cdef AcquisitionHeader_from_struct(cismrmrd.ISMRMRD_AcquisitionHeader *other):
    head = AcquisitionHeader()
    memcpy(head.this, other, sizeof(cismrmrd.ISMRMRD_AcquisitionHeader))
    return head

cdef class AcquisitionHeader:
    cdef cismrmrd.ISMRMRD_AcquisitionHeader *this
    def __cinit__(self):
        self.this = <cismrmrd.ISMRMRD_AcquisitionHeader*>calloc(1, sizeof(cismrmrd.ISMRMRD_AcquisitionHeader))
    def __dealloc__(self):
        free(self.this)

    property version:
        def __get__(self): return self.this.version
        def __set__(self, val): self.this.version = val


cdef class Acquisition:
    cdef cismrmrd.ISMRMRD_Acquisition *this
    def __cinit__(self):
        self.this = <cismrmrd.ISMRMRD_Acquisition*>cismrmrd.ismrmrd_create_acquisition()
    def __dealloc__(self):
        cismrmrd.ismrmrd_free_acquisition(self.this)

    property head:
        def __get__(self): return AcquisitionHeader_from_struct(&self.this.head)
    property data:
        def __get__(self):
            size = cismrmrd.ismrmrd_size_of_acquisition_data(self.this)
            # arr = np.zeros(size, dtype=np.cfloat)
            cdef complex float [:] arrview = self.this.data
            return arrview

cdef class Dataset:
    cdef cismrmrd.ISMRMRD_Dataset *this
    def __cinit__(self, const char *filename, const char *groupname, bint create_if_needed):
        self.this = <cismrmrd.ISMRMRD_Dataset*>calloc(1, sizeof(cismrmrd.ISMRMRD_Dataset))
        cismrmrd.ismrmrd_init_dataset(self.this, filename, groupname)
        cismrmrd.ismrmrd_open_dataset(self.this, create_if_needed)
    def __dealloc__(self):
        cismrmrd.ismrmrd_close_dataset(self.this)
        free(self.this)

    property filename:
        def __get__(self): return self.this.filename
    property groupname:
        def __get__(self): return self.this.filename
    property fileid:
        def __get__(self): return self.this.fileid

    def write_header(self, xmlstring):
        cismrmrd.ismrmrd_write_header(self.this, xmlstring)
    def read_header(self):
        return cismrmrd.ismrmrd_read_header(self.this)

cimport cismrmrd
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

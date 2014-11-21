cimport cismrmrd
from libc.stdlib cimport calloc, free
from libc.string cimport memcpy

# required for PyArray* functions and dtypes
import numpy
cimport numpy
numpy.import_array()

# conversion table between ISMRMRD and Numpy dtypes
cdef dict ismrmrd_to_numpy_typenums_dict = {
    cismrmrd.ISMRMRD_USHORT:    numpy.NPY_UINT16,
    cismrmrd.ISMRMRD_SHORT:     numpy.NPY_INT16,
    cismrmrd.ISMRMRD_UINT:      numpy.NPY_UINT32,
    cismrmrd.ISMRMRD_INT:       numpy.NPY_INT32,
    cismrmrd.ISMRMRD_FLOAT:     numpy.NPY_FLOAT32,
    cismrmrd.ISMRMRD_DOUBLE:    numpy.NPY_FLOAT64,
    cismrmrd.ISMRMRD_CXFLOAT:   numpy.NPY_COMPLEX64,
    cismrmrd.ISMRMRD_CXDOUBLE:  numpy.NPY_COMPLEX128,
}

cdef dict numpy_dtype_to_ismrmrd_typenum = {
    numpy.uint16:       cismrmrd.ISMRMRD_USHORT,
    numpy.int16:        cismrmrd.ISMRMRD_SHORT,
    numpy.uint32:       cismrmrd.ISMRMRD_UINT,
    numpy.int32:        cismrmrd.ISMRMRD_INT,
    numpy.float32:      cismrmrd.ISMRMRD_FLOAT,
    numpy.float64:      cismrmrd.ISMRMRD_DOUBLE,
    numpy.complex64:    cismrmrd.ISMRMRD_CXFLOAT,
    numpy.complex128:   cismrmrd.ISMRMRD_CXDOUBLE,
}

cdef dict ismrmrd_typenum_to_numpy_dtype = {
    cismrmrd.ISMRMRD_USHORT:    numpy.uint16,
    cismrmrd.ISMRMRD_SHORT:     numpy.int16,
    cismrmrd.ISMRMRD_UINT:      numpy.uint32,
    cismrmrd.ISMRMRD_INT:       numpy.int32,
    cismrmrd.ISMRMRD_FLOAT:     numpy.float32,
    cismrmrd.ISMRMRD_DOUBLE:    numpy.float64,
    cismrmrd.ISMRMRD_CXFLOAT:   numpy.complex64,
    cismrmrd.ISMRMRD_CXDOUBLE:  numpy.complex128,
}

# Expose ISMRMRD constants to python
VERSION_MAJOR = cismrmrd.ISMRMRD_VERSION_MAJOR
VERSION_MINOR = cismrmrd.ISMRMRD_VERSION_MINOR
VERSION_PATCH = cismrmrd.ISMRMRD_VERSION_PATCH
XMLHDR_VERSION = cismrmrd.ISMRMRD_XMLHDR_VERSION

USER_INTS = cismrmrd.ISMRMRD_USER_INTS
USER_FLOATS = cismrmrd.ISMRMRD_USER_FLOATS
PHYS_STAMPS = cismrmrd.ISMRMRD_PHYS_STAMPS
CHANNEL_MASKS = cismrmrd.ISMRMRD_CHANNEL_MASKS
NDARRAY_MAXDIM = cismrmrd.ISMRMRD_NDARRAY_MAXDIM
POSITION_LENGTH = cismrmrd.ISMRMRD_POSITION_LENGTH
DIRECTION_LENGTH = cismrmrd.ISMRMRD_DIRECTION_LENGTH

# Error codes
BEGINERROR = cismrmrd.ISMRMRD_BEGINERROR
NOERROR = cismrmrd.ISMRMRD_NOERROR
MEMORYERROR = cismrmrd.ISMRMRD_MEMORYERROR
FILEERROR = cismrmrd.ISMRMRD_FILEERROR
TYPEERROR = cismrmrd.ISMRMRD_TYPEERROR
RUNTIMEERROR = cismrmrd.ISMRMRD_RUNTIMEERROR
HDF5ERROR = cismrmrd.ISMRMRD_HDF5ERROR
ENDERROR = cismrmrd.ISMRMRD_ENDERROR

# Data types
USHORT = cismrmrd.ISMRMRD_USHORT
SHORT = cismrmrd.ISMRMRD_SHORT
UINT = cismrmrd.ISMRMRD_UINT
INT = cismrmrd.ISMRMRD_INT
FLOAT = cismrmrd.ISMRMRD_FLOAT
DOUBLE = cismrmrd.ISMRMRD_DOUBLE
CXFLOAT = cismrmrd.ISMRMRD_CXFLOAT
CXDOUBLE = cismrmrd.ISMRMRD_CXDOUBLE

# Image types
IMTYPE_MAGNITUDE = cismrmrd.ISMRMRD_IMTYPE_MAGNITUDE
IMTYPE_PHASE = cismrmrd.ISMRMRD_IMTYPE_PHASE
IMTYPE_REAL = cismrmrd.ISMRMRD_IMTYPE_REAL
IMTYPE_IMAG = cismrmrd.ISMRMRD_IMTYPE_IMAG
IMTYPE_COMPLEX = cismrmrd.ISMRMRD_IMTYPE_COMPLEX

# Acquisition flags
# TODO: encapsulate that to a class and let set_flag / clear_flag be the
# only interface
ACQ_FIRST_IN_ENCODE_STEP1 = cismrmrd.ISMRMRD_ACQ_FIRST_IN_ENCODE_STEP1
ACQ_LAST_IN_ENCODE_STEP1 = cismrmrd.ISMRMRD_ACQ_LAST_IN_ENCODE_STEP1
ACQ_FIRST_IN_ENCODE_STEP2 = cismrmrd.ISMRMRD_ACQ_FIRST_IN_ENCODE_STEP2
ACQ_LAST_IN_ENCODE_STEP2 = cismrmrd.ISMRMRD_ACQ_LAST_IN_ENCODE_STEP2
ACQ_FIRST_IN_AVERAGE = cismrmrd.ISMRMRD_ACQ_FIRST_IN_AVERAGE
ACQ_LAST_IN_AVERAGE = cismrmrd.ISMRMRD_ACQ_LAST_IN_AVERAGE
ACQ_FIRST_IN_SLICE = cismrmrd.ISMRMRD_ACQ_FIRST_IN_SLICE
ACQ_LAST_IN_SLICE = cismrmrd.ISMRMRD_ACQ_LAST_IN_SLICE
ACQ_FIRST_IN_CONTRAST = cismrmrd.ISMRMRD_ACQ_FIRST_IN_CONTRAST
ACQ_LAST_IN_CONTRAST = cismrmrd.ISMRMRD_ACQ_LAST_IN_CONTRAST
ACQ_FIRST_IN_PHASE = cismrmrd.ISMRMRD_ACQ_FIRST_IN_PHASE
ACQ_LAST_IN_PHASE = cismrmrd.ISMRMRD_ACQ_LAST_IN_PHASE
ACQ_FIRST_IN_REPETITION = cismrmrd.ISMRMRD_ACQ_FIRST_IN_REPETITION
ACQ_LAST_IN_REPETITION = cismrmrd.ISMRMRD_ACQ_LAST_IN_REPETITION
ACQ_FIRST_IN_SET = cismrmrd.ISMRMRD_ACQ_FIRST_IN_SET
ACQ_LAST_IN_SET = cismrmrd.ISMRMRD_ACQ_LAST_IN_SET
ACQ_FIRST_IN_SEGMENT = cismrmrd.ISMRMRD_ACQ_FIRST_IN_SEGMENT
ACQ_LAST_IN_SEGMENT = cismrmrd.ISMRMRD_ACQ_LAST_IN_SEGMENT
ACQ_IS_NOISE_MEASUREMENT = cismrmrd.ISMRMRD_ACQ_IS_NOISE_MEASUREMENT
ACQ_IS_PARALLEL_CALIBRATION = cismrmrd.ISMRMRD_ACQ_IS_PARALLEL_CALIBRATION
ACQ_IS_PARALLEL_CALIBRATION_AND_IMAGING = cismrmrd.ISMRMRD_ACQ_IS_PARALLEL_CALIBRATION_AND_IMAGING
ACQ_IS_REVERSE = cismrmrd.ISMRMRD_ACQ_IS_REVERSE
ACQ_IS_NAVIGATION_DATA = cismrmrd.ISMRMRD_ACQ_IS_NAVIGATION_DATA
ACQ_IS_PHASECORR_DATA = cismrmrd.ISMRMRD_ACQ_IS_PHASECORR_DATA
ACQ_LAST_IN_MEASUREMENT = cismrmrd.ISMRMRD_ACQ_LAST_IN_MEASUREMENT
ACQ_IS_HPFEEDBACK_DATA = cismrmrd.ISMRMRD_ACQ_IS_HPFEEDBACK_DATA
ACQ_IS_DUMMYSCAN_DATA = cismrmrd.ISMRMRD_ACQ_IS_DUMMYSCAN_DATA
ACQ_IS_RTFEEDBACK_DATA = cismrmrd.ISMRMRD_ACQ_IS_RTFEEDBACK_DATA
ACQ_IS_SURFACECOILCORRECTIONSCAN_DATA = cismrmrd.ISMRMRD_ACQ_IS_SURFACECOILCORRECTIONSCAN_DATA
ACQ_USER1 = cismrmrd.ISMRMRD_ACQ_USER1
ACQ_USER2 = cismrmrd.ISMRMRD_ACQ_USER2
ACQ_USER3 = cismrmrd.ISMRMRD_ACQ_USER3
ACQ_USER4 = cismrmrd.ISMRMRD_ACQ_USER4
ACQ_USER5 = cismrmrd.ISMRMRD_ACQ_USER5
ACQ_USER6 = cismrmrd.ISMRMRD_ACQ_USER6
ACQ_USER7 = cismrmrd.ISMRMRD_ACQ_USER7
ACQ_USER8 = cismrmrd.ISMRMRD_ACQ_USER8

# Image flags
IMAGE_IS_NAVIGATION_DATA = cismrmrd.ISMRMRD_IMAGE_IS_NAVIGATION_DATA
IMAGE_USER1 = cismrmrd.ISMRMRD_IMAGE_USER1
IMAGE_USER2 = cismrmrd.ISMRMRD_IMAGE_USER2
IMAGE_USER3 = cismrmrd.ISMRMRD_IMAGE_USER3
IMAGE_USER4 = cismrmrd.ISMRMRD_IMAGE_USER4
IMAGE_USER5 = cismrmrd.ISMRMRD_IMAGE_USER5
IMAGE_USER6 = cismrmrd.ISMRMRD_IMAGE_USER6
IMAGE_USER7 = cismrmrd.ISMRMRD_IMAGE_USER7
IMAGE_USER8 = cismrmrd.ISMRMRD_IMAGE_USER8


cdef bytes build_exception_string():
    cdef char *pfile = NULL
    cdef char *pfunc = NULL
    cdef char *pmsg  = NULL
    cdef int line=0, code=0
    cdef bytes err_string
    if(cismrmrd.ismrmrd_pop_error(&pfile, &line, &pfunc, &code, &pmsg)):
        err_string = "ISMRMRD {0} in {1} ({2}:{3})".format(
            <bytes> cismrmrd.ismrmrd_strerror(code),
            <bytes> pfunc,
            <bytes> pfile,
            line,
            <bytes> pmsg,
            )
    return err_string

def sizeof_data_type(tp):
    return cismrmrd.ismrmrd_sizeof_data_type(tp)

cdef class EncodingCounters:

    cdef cismrmrd.ISMRMRD_EncodingCounters *thisptr

    def __cinit__(self, other = None):
        self.thisptr = <cismrmrd.ISMRMRD_EncodingCounters*>calloc(1, sizeof(cismrmrd.ISMRMRD_EncodingCounters))

    def __dealloc__(self):
        free(self.thisptr)

    def __copy__(self):
        cdef EncodingCounters acopy = EncodingCounters()
        acopy.copy_from(self.thisptr)
        return acopy

    cdef copy_from(self, cismrmrd.ISMRMRD_EncodingCounters *ptr):
        memcpy(self.thisptr, ptr, sizeof(cismrmrd.ISMRMRD_EncodingCounters))

    cdef copy_to(self, cismrmrd.ISMRMRD_EncodingCounters *ptr):
        memcpy(ptr, self.thisptr, sizeof(cismrmrd.ISMRMRD_EncodingCounters))

    property kspace_encode_step_1:
        def __get__(self): return self.thisptr.kspace_encode_step_1
        def __set__(self, val): self.thisptr.kspace_encode_step_1 = val

    property kspace_encode_step_2:
        def __get__(self): return self.thisptr.kspace_encode_step_2
        def __set__(self, val): self.thisptr.kspace_encode_step_2 = val

    property average:
        def __get__(self): return self.thisptr.average
        def __set__(self, val): self.thisptr.average = val

    property slice:
        def __get__(self): return self.thisptr.slice
        def __set__(self, val): self.thisptr.slice = val

    property contrast:
        def __get__(self): return self.thisptr.contrast
        def __set__(self, val): self.thisptr.contrast = val

    property phase:
        def __get__(self): return self.thisptr.phase
        def __set__(self, val): self.thisptr.phase = val

    property repetition:
        def __get__(self): return self.thisptr.repetition
        def __set__(self, val): self.thisptr.repetition = val

    property set:
        def __get__(self): return self.thisptr.set
        def __set__(self, val): self.thisptr.set = val

    property segment:
        def __get__(self): return self.thisptr.segment
        def __set__(self, val): self.thisptr.segment = val

    property user:
        def __get__(self):
            l = [0 for i in range(cismrmrd.ISMRMRD_USER_INTS)]
            for i in range(cismrmrd.ISMRMRD_USER_INTS):
                l[i] = self.thisptr.user[i]
            return l
        def __set__(self, vals):
            for i in range(cismrmrd.ISMRMRD_USER_INTS):
                self.thisptr.user[i] = vals[i]


cdef class AcquisitionHeader:

    cdef cismrmrd.ISMRMRD_AcquisitionHeader *thisptr

    def __cinit__(self):
        self.thisptr = <cismrmrd.ISMRMRD_AcquisitionHeader*>calloc(1, sizeof(cismrmrd.ISMRMRD_AcquisitionHeader))
        errno = cismrmrd.ismrmrd_init_acquisition_header(self.thisptr)
        if errno != cismrmrd.ISMRMRD_NOERROR:
            raise RuntimeError(build_exception_string())

    def __dealloc__(self):
        free(self.thisptr)

    def __copy__(self):
        cdef AcquisitionHeader acopy = AcquisitionHeader()
        acopy.copy_from(self.thisptr)
        return acopy

    cdef copy_from(self, cismrmrd.ISMRMRD_AcquisitionHeader *ptr):
        memcpy(self.thisptr, ptr, sizeof(cismrmrd.ISMRMRD_AcquisitionHeader))

    cdef copy_to(self, cismrmrd.ISMRMRD_AcquisitionHeader *ptr):
        memcpy(ptr, self.thisptr, sizeof(cismrmrd.ISMRMRD_AcquisitionHeader))

    property version:
        def __get__(self): return self.thisptr.version
        def __set__(self, val): self.thisptr.version = val

    property flags:
        def __get__(self): return self.thisptr.flags
        def __set__(self, val): self.thisptr.flags = val

    property measurement_uid:
        def __get__(self): return self.thisptr.measurement_uid
        def __set__(self, val): self.thisptr.measurement_uid = val

    property scan_counter:
        def __get__(self): return self.thisptr.scan_counter
        def __set__(self, val): self.thisptr.scan_counter = val

    property acquisition_time_stamp:
        def __get__(self): return self.thisptr.acquisition_time_stamp
        def __set__(self, val): self.thisptr.acquisition_time_stamp = val

    property physiology_time_stamp:
        def __get__(self):
            return [self.thisptr.physiology_time_stamp[i] for i in
                    range(cismrmrd.ISMRMRD_PHYS_STAMPS)]
        def __set__(self, val):
            for i in range(cismrmrd.ISMRMRD_PHYS_STAMPS):
                self.thisptr.physiology_time_stamp[i] = val[i]

    property number_of_samples:
        def __get__(self): return self.thisptr.number_of_samples
        def __set__(self, val): self.thisptr.number_of_samples = val

    property available_channels:
        def __get__(self): return self.thisptr.available_channels
        def __set__(self, val): self.thisptr.available_channels = val

    property active_channels:
        def __get__(self): return self.thisptr.active_channels
        def __set__(self, val): self.thisptr.active_channels = val

    property channel_mask:
        def __get__(self):
            return [self.thisptr.channel_mask[i] for i in
                    range(cismrmrd.ISMRMRD_CHANNEL_MASKS)]
        def __set__(self, val):
            for i in range(cismrmrd.ISMRMRD_CHANNEL_MASKS):
                self.thisptr.channel_mask[i] = val[i]

    property discard_pre:
        def __get__(self): return self.thisptr.discard_pre
        def __set__(self, val): self.thisptr.discard_pre = val

    property discard_post:
        def __get__(self): return self.thisptr.discard_post
        def __set__(self, val): self.thisptr.discard_post = val

    property center_sample:
        def __get__(self): return self.thisptr.center_sample
        def __set__(self, val): self.thisptr.center_sample = val

    property encoding_space_ref:
        def __get__(self): return self.thisptr.encoding_space_ref
        def __set__(self, val): self.thisptr.encoding_space_ref = val

    property trajectory_dimensions:
        def __get__(self): return self.thisptr.trajectory_dimensions
        def __set__(self, val): self.thisptr.trajectory_dimensions = val

    property sample_time_us:
        def __get__(self): return self.thisptr.sample_time_us
        def __set__(self, val): self.thisptr.sample_time_us = val

    property position:
        def __get__(self):
            return [self.thisptr.position[i] for i in
                    range(cismrmrd.ISMRMRD_POSITION_LENGTH)]
        def __set__(self, val):
            for i in range(cismrmrd.ISMRMRD_POSITION_LENGTH):
                self.thisptr.position[i] = val[i]

    property read_dir:
        def __get__(self):
            return [self.thisptr.read_dir[i] for i in
                    range(cismrmrd.ISMRMRD_DIRECTION_LENGTH)]
        def __set__(self, val):
            for i in range(cismrmrd.ISMRMRD_DIRECTION_LENGTH):
                self.thisptr.read_dir[i] = val[i]

    property phase_dir:
        def __get__(self):
            return [self.thisptr.phase_dir[i] for i in
                    range(cismrmrd.ISMRMRD_DIRECTION_LENGTH)]
        def __set__(self, val):
            for i in range(cismrmrd.ISMRMRD_DIRECTION_LENGTH):
                self.thisptr.phase_dir[i] = val[i]

    property slice_dir:
        def __get__(self):
            return [self.thisptr.slice_dir[i] for i in
                    range(cismrmrd.ISMRMRD_DIRECTION_LENGTH)]
        def __set__(self, val):
            for i in range(cismrmrd.ISMRMRD_DIRECTION_LENGTH):
                self.thisptr.slice_dir[i] = val[i]

    property patient_table_position:
        def __get__(self):
            return [self.thisptr.patient_table_position[i] for i in
                    range(cismrmrd.ISMRMRD_POSITION_LENGTH)]
        def __set__(self, val):
            for i in range(cismrmrd.ISMRMRD_POSITION_LENGTH):
                self.thisptr.patient_table_position[i] = val[i]

    property idx:
        def __get__(self):
            cdef EncodingCounters idx = EncodingCounters()
            idx.copy_from(&self.thisptr.idx)
            return idx
        def __set__(self, EncodingCounters val):
            val.copy_to(&self.thisptr.idx)

    property user_int:
        def __get__(self):
            return [self.thisptr.user_int[i] for i in
                    range(cismrmrd.ISMRMRD_USER_INTS)]
        def __set__(self, val):
            for i in range(cismrmrd.ISMRMRD_USER_INTS):
                self.thisptr.user_int[i] = val[i]

    property user_float:
        def __get__(self):
            return [self.thisptr.user_float[i] for i in
                    range(cismrmrd.ISMRMRD_USER_FLOATS)]
        def __set__(self, val):
            for i in range(cismrmrd.ISMRMRD_USER_FLOATS):
                self.thisptr.user_float[i] = val[i]


cdef class Acquisition:

    cdef cismrmrd.ISMRMRD_Acquisition *thisptr

    def __cinit__(self, AcquisitionHeader head=None):
        self.thisptr = <cismrmrd.ISMRMRD_Acquisition*>calloc(1, sizeof(cismrmrd.ISMRMRD_Acquisition))
        errno = cismrmrd.ismrmrd_init_acquisition(self.thisptr)
        if errno != cismrmrd.ISMRMRD_NOERROR:
            raise RuntimeError(build_exception_string())
        if head is not None:
            self.head = head 

    def __dealloc__(self):
        errno = cismrmrd.ismrmrd_cleanup_acquisition(self.thisptr)
        if errno != cismrmrd.ISMRMRD_NOERROR:
            raise RuntimeError(build_exception_string())
        free(self.thisptr)

    def __copy__(self):
        cdef Acquisition acopy = Acquisition()
        errno = cismrmrd.ismrmrd_copy_acquisition(acopy.thisptr, self.thisptr)
        if errno != cismrmrd.ISMRMRD_NOERROR:
            raise RuntimeError(build_exception_string())        
        return acopy

    property head:
        def __get__(self):
            cdef AcquisitionHeader head = AcquisitionHeader()
            head.copy_from(&self.thisptr.head)
            return head
        def __set__(self, AcquisitionHeader head):
            head.copy_to(&self.thisptr.head)
            errno = cismrmrd.ismrmrd_make_consistent_acquisition(self.thisptr)
            if errno != cismrmrd.ISMRMRD_NOERROR:
                raise RuntimeError(build_exception_string())

    property data:
        def __get__(self):
            cdef numpy.npy_intp shape_data[2]
            shape_data[0] = self.head.active_channels
            shape_data[1] = self.head.number_of_samples
            # careful here, thisptr is a R-W view
            return numpy.PyArray_SimpleNewFromData(2, shape_data,
                    numpy.NPY_COMPLEX64, <void *>(self.thisptr.data))
        def __set__(self, val):
            self.data.ravel()[:] = numpy.asarray(val).ravel()[:]

    property traj:
        def __get__(self):
            cdef numpy.npy_intp shape_traj[2]
            shape_traj[0] = self.head.number_of_samples
            shape_traj[1] = self.head.trajectory_dimensions
            # careful here, thisptr is a R-W view
            # if traj ptr is empty, then will return an empty array
            # which is arguably better than returning a NoneType.
            return numpy.PyArray_SimpleNewFromData(2, shape_traj,
                    numpy.NPY_FLOAT32, <void *>(self.thisptr.traj))
        def __set__(self, val):
            self.data.ravel()[:] = numpy.asarray(val).ravel()[:]


cdef class ImageHeader:

    cdef cismrmrd.ISMRMRD_ImageHeader *thisptr

    def __cinit__(self):
        self.thisptr = <cismrmrd.ISMRMRD_ImageHeader*>calloc(1, sizeof(cismrmrd.ISMRMRD_ImageHeader))
        errno = cismrmrd.ismrmrd_init_image_header(self.thisptr)
        if errno != cismrmrd.ISMRMRD_NOERROR:
            raise RuntimeError(build_exception_string())

    def __dealloc__(self):
        free(self.thisptr)

    def __copy__(self):
        cdef ImageHeader acopy = ImageHeader()
        acopy.copy_from(self.thisptr)
        return acopy

    cdef copy_from(self, cismrmrd.ISMRMRD_ImageHeader *ptr):
        memcpy(self.thisptr, ptr, sizeof(cismrmrd.ISMRMRD_ImageHeader))

    cdef copy_to(self, cismrmrd.ISMRMRD_ImageHeader *ptr):
        memcpy(ptr, self.thisptr, sizeof(cismrmrd.ISMRMRD_ImageHeader))

    property version:
        def __get__(self): return self.thisptr.version
        def __set__(self, val): self.thisptr.version = val

    property data_type:
        def __get__(self): return self.thisptr.data_type
        def __set__(self, val): self.thisptr.data_type = val

    property flags:
        def __get__(self): return self.thisptr.flags
        def __set__(self, val): self.thisptr.flags = val

    property measurement_uid:
        def __get__(self): return self.thisptr.measurement_uid
        def __set__(self, val): self.thisptr.measurement_uid = val

    property matrix_size:
        def __get__(self):
            return [self.thisptr.matrix_size[i] for i in range(3)]
        def __set__(self, val):
            for i in range(3):
                self.thisptr.matrix_size[i] = val[i]

    property field_of_view:
        def __get__(self):
            return [self.thisptr.field_of_view[i] for i in range(3)]
        def __set__(self, val):
            for i in range(3):
                self.thisptr.field_of_view[i] = val[i]

    property channels:
        def __get__(self): return self.thisptr.channels
        def __set__(self, val): self.thisptr.channels = val

    property position:
        def __get__(self):
            return [self.thisptr.position[i] for i in
                    range(cismrmrd.ISMRMRD_POSITION_LENGTH)]
        def __set__(self, val):
            for i in range(cismrmrd.ISMRMRD_POSITION_LENGTH):
                self.thisptr.position[i] = val[i]

    property read_dir:
        def __get__(self):
            return [self.thisptr.read_dir[i] for i in
                    range(cismrmrd.ISMRMRD_DIRECTION_LENGTH)]
        def __set__(self, val):
            for i in range(cismrmrd.ISMRMRD_DIRECTION_LENGTH):
                self.thisptr.read_dir[i] = val[i]

    property phase_dir:
        def __get__(self):
            return [self.thisptr.phase_dir[i] for i in
                    range(cismrmrd.ISMRMRD_DIRECTION_LENGTH)]
        def __set__(self, val):
            for i in range(cismrmrd.ISMRMRD_DIRECTION_LENGTH):
                self.thisptr.phase_dir[i] = val[i]

    property slice_dir:
        def __get__(self):
            return [self.thisptr.slice_dir[i] for i in
                    range(cismrmrd.ISMRMRD_DIRECTION_LENGTH)]
        def __set__(self, val):
            for i in range(cismrmrd.ISMRMRD_DIRECTION_LENGTH):
                self.thisptr.slice_dir[i] = val[i]

    property patient_table_position:
        def __get__(self):
            return [self.thisptr.patient_table_position[i] for i in
                    range(cismrmrd.ISMRMRD_POSITION_LENGTH)]
        def __set__(self, val):
            for i in range(cismrmrd.ISMRMRD_POSITION_LENGTH):
                self.thisptr.patient_table_position[i] = val[i]

    property average:
        def __get__(self): return self.thisptr.average
        def __set__(self, val): self.thisptr.average = val

    property slice:
        def __get__(self): return self.thisptr.slice
        def __set__(self, val): self.thisptr.slice = val

    property contrast:
        def __get__(self): return self.thisptr.contrast
        def __set__(self, val): self.thisptr.contrast = val

    property phase:
        def __get__(self): return self.thisptr.phase
        def __set__(self, val): self.thisptr.phase = val

    property repetition:
        def __get__(self): return self.thisptr.repetition
        def __set__(self, val): self.thisptr.repetition = val

    property set:
        def __get__(self): return self.thisptr.set
        def __set__(self, val): self.thisptr.set = val

    property acquisition_time_stamp:
        def __get__(self): return self.thisptr.acquisition_time_stamp
        def __set__(self, val): self.thisptr.acquisition_time_stamp = val

    property physiology_time_stamp:
        def __get__(self):
            return [self.thisptr.physiology_time_stamp[i] for i in
                    range(cismrmrd.ISMRMRD_PHYS_STAMPS)]
        def __set__(self, val):
            for i in range(cismrmrd.ISMRMRD_PHYS_STAMPS):
                self.thisptr.physiology_time_stamp[i] = val[i]

    property image_type:
        def __get__(self): return self.thisptr.image_type
        def __set__(self, val): self.thisptr.image_type = val

    property image_index:
        def __get__(self): return self.thisptr.image_index
        def __set__(self, val): self.thisptr.image_index = val

    property image_series_index:
        def __get__(self): return self.thisptr.image_series_index
        def __set__(self, val): self.thisptr.image_series_index = val

    property user_int:
        def __get__(self):
            return [self.thisptr.user_int[i] for i in
                    range(cismrmrd.ISMRMRD_USER_INTS)]
        def __set__(self, val):
            for i in range(cismrmrd.ISMRMRD_USER_INTS):
                self.thisptr.user_int[i] = val[i]

    property user_float:
        def __get__(self):
            return [self.thisptr.user_float[i] for i in
                    range(cismrmrd.ISMRMRD_USER_FLOATS)]
        def __set__(self, val):
            for i in range(cismrmrd.ISMRMRD_USER_FLOATS):
                self.thisptr.user_int[i] = val[i]

    property attribute_string_len:
        def __get__(self): return self.thisptr.attribute_string_len
        def __set__(self, val): self.thisptr.attribute_string_len = val


cdef class Image:

    cdef cismrmrd.ISMRMRD_Image *thisptr

    def __cinit__(self, ImageHeader head=None):
        self.thisptr = <cismrmrd.ISMRMRD_Image*>calloc(1, sizeof(cismrmrd.ISMRMRD_Image))
        errno = cismrmrd.ismrmrd_init_image(self.thisptr)
        if errno != cismrmrd.ISMRMRD_NOERROR:
            raise RuntimeError(build_exception_string())
        if head is not None:
            self.head = head

    def __dealloc__(self):
        errno = cismrmrd.ismrmrd_cleanup_image(self.thisptr)
        if errno != cismrmrd.ISMRMRD_NOERROR:
            raise RuntimeError(build_exception_string())
        free(self.thisptr)

    def __copy__(self):
        cdef Image acopy = Image()
        errno = cismrmrd.ismrmrd_copy_image(acopy.thisptr, self.thisptr)
        if errno != cismrmrd.ISMRMRD_NOERROR:
            raise RuntimeError(build_exception_string())
        return acopy

    property head:
        def __get__(self):
            cdef ImageHeader head = ImageHeader()
            head.copy_from(&self.thisptr.head)
            return head
        def __set__(self, ImageHeader head):
            head.copy_to(&self.thisptr.head)
            errno = cismrmrd.ismrmrd_make_consistent_image(self.thisptr)
            if errno != cismrmrd.ISMRMRD_NOERROR:
                raise RuntimeError(build_exception_string())
        
    property attribute_string:
        def __get__(self): return self.thisptr.attribute_string
        
    property data:
        def __get__(self):
            cdef numpy.npy_intp shape_data[3]
            for idim in range(3):
                shape_data[idim] = self.head.matrix_size[idim]            
            cdef int typenum = ismrmrd_to_numpy_typenums_dict[self.head.data_type]
            return numpy.PyArray_SimpleNewFromData(3, shape_data,
                    typenum, <void *>(self.thisptr.data))
        def __set__(self, val):
            self.data.ravel()[:] = numpy.asarray(val).ravel()[:]


cdef class NDArray:
    
    cdef cismrmrd.ISMRMRD_NDArray *thisptr

    def __cinit__(self, shape=None, dtype=None):
        self.thisptr = <cismrmrd.ISMRMRD_NDArray*>calloc(1, sizeof(cismrmrd.ISMRMRD_NDArray))
        errno = cismrmrd.ismrmrd_init_ndarray(self.thisptr)
        if errno != cismrmrd.ISMRMRD_NOERROR:
            raise RuntimeError(build_exception_string())
        # default dtype is numpy.dtype(None), normally float64
        self.dtype = dtype
        # shape remain optional
        if shape is not None:
            self.shape = shape

    def __dealloc__(self):
        errno = cismrmrd.ismrmrd_cleanup_ndarray(self.thisptr)
        if errno != cismrmrd.ISMRMRD_NOERROR:
            raise RuntimeError(build_exception_string())
        free(self.thisptr)

    def __copy__(self):
        cdef NDArray acopy = NDArray()
        errno = cismrmrd.ismrmrd_copy_ndarray(acopy.thisptr, self.thisptr)
        if errno != cismrmrd.ISMRMRD_NOERROR:
            raise RuntimeError(build_exception_string())
        return acopy

    property version:
        def __get__(self): return self.thisptr.version 
        
    property dtype:
        def __get__(self):
            return numpy.dtype(ismrmrd_typenum_to_numpy_dtype[self.thisptr.data_type])
        def __set__(self, val):
            numpy_dtype = numpy.dtype(val)
            self.thisptr.data_type = numpy_dtype_to_ismrmrd_typenum[numpy_dtype.type]
            errno = cismrmrd.ismrmrd_make_consistent_ndarray(self.thisptr)
            if errno != cismrmrd.ISMRMRD_NOERROR:
                raise RuntimeError(build_exception_string())

    property ndim:
        def __get__(self): return self.thisptr.ndim
        def __set__(self, val): self.thisptr.ndim = val
        
    property shape:
        def __get__(self):
            return [self.thisptr.dims[i] for i in range(self.ndim)]
        def __set__(self, val):
            self.ndim = len(val)
            for idim in range(self.ndim):
                self.thisptr.dims[idim] = val[idim]
            errno = cismrmrd.ismrmrd_make_consistent_ndarray(self.thisptr)
            if errno != cismrmrd.ISMRMRD_NOERROR:
                raise RuntimeError(build_exception_string())

    property data:
        def __get__(self):
            cdef numpy.npy_intp shape_data[cismrmrd.ISMRMRD_NDARRAY_MAXDIM]
            for idim in range(self.ndim):
                shape_data[idim] = self.shape[idim]
            cdef int typenum = ismrmrd_to_numpy_typenums_dict[self.thisptr.data_type]
            return numpy.PyArray_SimpleNewFromData(self.ndim, shape_data,
                    typenum, <void *>(self.thisptr.data))
        def __set__(self, val):
            self.data.ravel()[:] = numpy.asarray(val).ravel()[:]


cdef class Dataset:

    cdef cismrmrd.ISMRMRD_Dataset *thisptr
    cdef bint is_open

    def __cinit__(self, const char *filename, const char *groupname, create_if_needed=True):
        self.thisptr = <cismrmrd.ISMRMRD_Dataset*>calloc(1, sizeof(cismrmrd.ISMRMRD_Dataset))
        errno = cismrmrd.ismrmrd_init_dataset(self.thisptr, filename, groupname)
        if errno != cismrmrd.ISMRMRD_NOERROR:
            raise RuntimeError(build_exception_string())
        self.open(create_if_needed)

    def __dealloc__(self):
        self.close()
        free(self.thisptr)

    def open(self, create_if_needed=True):
        if not self.is_open:
            errno = cismrmrd.ismrmrd_open_dataset(self.thisptr, create_if_needed)
            if errno != cismrmrd.ISMRMRD_NOERROR:
                raise RuntimeError(build_exception_string())
            self.is_open = True
        
    def close(self):
        if self.is_open:
            errno = cismrmrd.ismrmrd_close_dataset(self.thisptr)
            if errno != cismrmrd.ISMRMRD_NOERROR:
                raise RuntimeError(build_exception_string())
            self.is_open = False

    property filename:
        def __get__(self): return self.thisptr.filename

    property groupname:
        def __get__(self): return self.thisptr.groupname

    property fileid:
        def __get__(self): return self.thisptr.fileid

    def write_header(self, xmlstring):
        errno = cismrmrd.ismrmrd_write_header(self.thisptr, xmlstring)
        if errno != cismrmrd.ISMRMRD_NOERROR:
            raise RuntimeError(build_exception_string())

    def read_header(self):
        return cismrmrd.ismrmrd_read_header(self.thisptr)

    def append_acquisition(self, Acquisition acq):
        errno = cismrmrd.ismrmrd_append_acquisition(self.thisptr, acq.thisptr)
        if errno != cismrmrd.ISMRMRD_NOERROR:
            raise RuntimeError(build_exception_string())

    def read_acquisition(self, index):
        cdef Acquisition acq = Acquisition()
        errno = cismrmrd.ismrmrd_read_acquisition(self.thisptr, index, acq.thisptr)
        if errno != cismrmrd.ISMRMRD_NOERROR:
            raise RuntimeError(build_exception_string())
        return acq

    @property
    def number_of_acquisitions(self):
        return cismrmrd.ismrmrd_get_number_of_acquisitions(self.thisptr)

    def append_image(self, varname, Image img):
        errno = cismrmrd.ismrmrd_append_image(self.thisptr, varname, img.thisptr)
        if errno != cismrmrd.ISMRMRD_NOERROR:
            raise RuntimeError(build_exception_string())

    def read_image(self, varname, index):
        cdef Image img = Image()
        errno = cismrmrd.ismrmrd_read_image(self.thisptr, varname, index, img.thisptr)
        if errno != cismrmrd.ISMRMRD_NOERROR:
            raise RuntimeError(build_exception_string())
        return img

    def number_of_images(self, varname):
        return cismrmrd.ismrmrd_get_number_of_images(self.thisptr, varname)

    def append_array(self, varname, NDArray arr):
        errno = cismrmrd.ismrmrd_append_array(self.thisptr, varname, arr.thisptr)
        if errno != cismrmrd.ISMRMRD_NOERROR:
            raise RuntimeError(build_exception_string().decode('UTF-8'))        

    def read_array(self, varname, index):
        cdef NDArray arr = NDArray()
        errno = cismrmrd.ismrmrd_read_array(self.thisptr, varname, index, arr.thisptr)
        if errno != cismrmrd.ISMRMRD_NOERROR:
            raise RuntimeError(build_exception_string())
        return arr

    def number_of_arrays(self, varname):
        return cismrmrd.ismrmrd_get_number_of_arrays(self.thisptr, varname)

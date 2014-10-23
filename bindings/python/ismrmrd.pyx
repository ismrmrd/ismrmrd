cimport cismrmrd
from libc.stdlib cimport calloc, free
from libc.string cimport memcpy

#### Helper functions

cdef AcquisitionHeader_from_struct(cismrmrd.ISMRMRD_AcquisitionHeader *other):
    head = AcquisitionHeader()
    memcpy(head.this, other, sizeof(cismrmrd.ISMRMRD_AcquisitionHeader))
    return head

cdef ImageHeader_from_struct(cismrmrd.ISMRMRD_ImageHeader *other):
    head = ImageHeader()
    memcpy(head.this, other, sizeof(cismrmrd.ISMRMRD_ImageHeader))
    return head

####

cdef class EncodingCounters:
    cdef cismrmrd.ISMRMRD_EncodingCounters *this
    def __cinit__(self):
        self.this = <cismrmrd.ISMRMRD_EncodingCounters*>calloc(1, sizeof(cismrmrd.ISMRMRD_EncodingCounters))
    def __dealloc__(self):
        free(self.this)

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
    cdef cismrmrd.ISMRMRD_AcquisitionHeader *this
    def __cinit__(self):
        self.this = <cismrmrd.ISMRMRD_AcquisitionHeader*>calloc(1, sizeof(cismrmrd.ISMRMRD_AcquisitionHeader))
    def __dealloc__(self):
        free(self.this)

    property version:
        def __get__(self): return self.this.version
        def __set__(self, val): self.this.version = val

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
        def __get__(self): return self.idx
        def __set__(self, val): pass    # FIXME (does nothing)

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
            return None     # FIXME: return NumPy array or Cython Typed MemoryView

cdef class ImageHeader:
    cdef cismrmrd.ISMRMRD_ImageHeader *this
    def __cinit__(self):
        self.this = <cismrmrd.ISMRMRD_ImageHeader*>calloc(1, sizeof(cismrmrd.ISMRMRD_ImageHeader))
    def __dealloc__(self):
        free(self.this)

    property version:
        def __get__(self): return self.thisptr.version
        def __set__(self, val): self.thisptr.version = val

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

    property image_data_type:
        def __get__(self): return self.thisptr.image_data_type
        def __set__(self, val): self.thisptr.image_data_type = val

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

cdef class Image:
    cdef cismrmrd.ISMRMRD_Image *this
    def __cinit__(self):
        self.this = <cismrmrd.ISMRMRD_Image*>cismrmrd.ismrmrd_create_image()
    def __dealloc__(self):
        cismrmrd.ismrmrd_free_image(self.this)

    property head:
        def __get__(self): return ImageHeader_from_struct(&self.this.head)
    property attribute_string:
        def __get__(self): return self.this.attribute_string
    property data:
        def __get__(self):
            size = cismrmrd.ismrmrd_size_of_image_data(self.this)
            return None     # FIXME: return NumPy array or Cython Typed MemoryView

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

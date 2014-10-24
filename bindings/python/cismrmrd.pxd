from libc.stdint cimport uint16_t, uint32_t, uint64_t, int32_t

cdef extern from "ismrmrd/ismrmrd.h":
    cdef enum:
        ISMRMRD_VERSION = 1
        ISMRMRD_USER_INTS = 8
        ISMRMRD_USER_FLOATS = 8
        ISMRMRD_PHYS_STAMPS = 3
        ISMRMRD_CHANNEL_MASKS = 16
        ISMRMRD_NDARRAY_MAXDIM = 7
        ISMRMRD_POSITION_LENGTH = 3
        ISMRMRD_DIRECTION_LENGTH = 3

    ctypedef enum ISMRMRD_ErrorCodes:
        ISMRMRD_BEGINERROR
        ISMRMRD_NOERROR
        ISMRMRD_MEMORYERROR
        ISMRMRD_FILEERROR
        ISMRMRD_TYPEERROR
        ISMRMRD_RUNTIMEERROR
        ISMRMRD_HDF5ERROR
        ISMRMRD_ENDERROR

    cdef bint ismrmrd_pop_error(char**, int*, char**, int*, char**)
    char* ismrmrd_strerror(int)

    ctypedef struct ISMRMRD_EncodingCounters:
        uint16_t kspace_encode_step_1  # e.g. phase encoding line number
        uint16_t kspace_encode_step_2  # e.g. partition encodning number
        uint16_t average               # e.g. signal average number
        uint16_t slice                 # e.g. imaging slice number
        uint16_t contrast              # e.g. echo number in multi-echo
        uint16_t phase                 # e.g. cardiac phase number
        uint16_t repetition            # e.g. dynamic number for dynamic scanning
        uint16_t set                   # e.g. flow encodning set
        uint16_t segment               # e.g. segment number for segmented acquisition
        uint16_t user[ISMRMRD_USER_INTS]   # Free user parameters

    cdef enum ISMRMRD_AcquisitionFlags:
        ISMRMRD_ACQ_FIRST_IN_ENCODE_STEP1
        ISMRMRD_ACQ_LAST_IN_ENCODE_STEP1
        ISMRMRD_ACQ_FIRST_IN_ENCODE_STEP2
        ISMRMRD_ACQ_LAST_IN_ENCODE_STEP2
        ISMRMRD_ACQ_FIRST_IN_AVERAGE
        ISMRMRD_ACQ_LAST_IN_AVERAGE
        ISMRMRD_ACQ_FIRST_IN_SLICE
        ISMRMRD_ACQ_LAST_IN_SLICE
        ISMRMRD_ACQ_FIRST_IN_CONTRAST
        ISMRMRD_ACQ_LAST_IN_CONTRAST
        ISMRMRD_ACQ_FIRST_IN_PHASE
        ISMRMRD_ACQ_LAST_IN_PHASE
        ISMRMRD_ACQ_FIRST_IN_REPETITION
        ISMRMRD_ACQ_LAST_IN_REPETITION
        ISMRMRD_ACQ_FIRST_IN_SET
        ISMRMRD_ACQ_LAST_IN_SET
        ISMRMRD_ACQ_FIRST_IN_SEGMENT
        ISMRMRD_ACQ_LAST_IN_SEGMENT
        ISMRMRD_ACQ_IS_NOISE_MEASUREMENT
        ISMRMRD_ACQ_IS_PARALLEL_CALIBRATION
        ISMRMRD_ACQ_IS_PARALLEL_CALIBRATION_AND_IMAGING
        ISMRMRD_ACQ_IS_REVERSE
        ISMRMRD_ACQ_IS_NAVIGATION_DATA
        ISMRMRD_ACQ_IS_PHASECORR_DATA
        ISMRMRD_ACQ_LAST_IN_MEASUREMENT
        ISMRMRD_ACQ_IS_HPFEEDBACK_DATA
        ISMRMRD_ACQ_IS_DUMMYSCAN_DATA
        ISMRMRD_ACQ_IS_RTFEEDBACK_DATA
        ISMRMRD_ACQ_IS_SURFACECOILCORRECTIONSCAN_DATA
        ISMRMRD_ACQ_USER1
        ISMRMRD_ACQ_USER2
        ISMRMRD_ACQ_USER3
        ISMRMRD_ACQ_USER4
        ISMRMRD_ACQ_USER5
        ISMRMRD_ACQ_USER6
        ISMRMRD_ACQ_USER7
        ISMRMRD_ACQ_USER8

    ctypedef struct ISMRMRD_AcquisitionHeader:
        uint16_t version   # First unsigned int indicates the version
        uint64_t flags     # bit field with flags
        uint32_t measurement_uid   # Unique ID for the measurement
        uint32_t scan_counter      # Current acquisition number in the measurement
        uint32_t acquisition_time_stamp    # Acquisition clock
        uint32_t physiology_time_stamp[ISMRMRD_PHYS_STAMPS] # Physiology time stamps, e.g. ecg, breating, etc.
        uint16_t number_of_samples # Number of samples acquired
        uint16_t available_channels    # Available coils
        uint16_t active_channels       # Active coils on current acquisiton
        uint64_t channel_mask[ISMRMRD_CHANNEL_MASKS]   # Mask to indicate which channels are active. Support for 1024 channels
        uint16_t discard_pre       # Samples to be discarded at the beginning of acquisition
        uint16_t discard_post      # Samples to be discarded at the end of acquisition
        uint16_t center_sample     # Sample at the center of k-space
        uint16_t encoding_space_ref    # Reference to an encoding space, typically only one per acquisition
        uint16_t trajectory_dimensions # Indicates the dimensionality of the trajectory vector (0 means no trajectory)
        float sample_time_us                                   # Time between samples in micro seconds, sampling BW
        float position[ISMRMRD_POSITION_LENGTH] # Three-dimensional spatial offsets from isocenter
        float read_dir[ISMRMRD_DIRECTION_LENGTH] # Directional cosines of the readout/frequency encoding
        float phase_dir[ISMRMRD_DIRECTION_LENGTH] # Directional cosines of the phase
        float slice_dir[ISMRMRD_DIRECTION_LENGTH] # Directional cosines of the slice direction
        float patient_table_position[ISMRMRD_POSITION_LENGTH]  # Patient table off-center
        ISMRMRD_EncodingCounters idx       # Encoding loop counters, see above
        int32_t user_int[ISMRMRD_USER_INTS]       # Free user parameters
        float user_float[ISMRMRD_USER_FLOATS]     # Free user parameters

    ctypedef struct ISMRMRD_Acquisition:
        ISMRMRD_AcquisitionHeader head
        float *traj
        float complex *data

    cdef int ismrmrd_init_acquisition_header(ISMRMRD_AcquisitionHeader *hdr)
    cdef ISMRMRD_Acquisition * ismrmrd_create_acquisition()
    cdef int ismrmrd_free_acquisition(ISMRMRD_Acquisition *acq)
    cdef int ismrmrd_init_acquisition(ISMRMRD_Acquisition *acq)
    cdef int ismrmrd_cleanup_acquisition(ISMRMRD_Acquisition *acq)
    cdef int ismrmrd_copy_acquisition(ISMRMRD_Acquisition *acqdest, const ISMRMRD_Acquisition *acqsource)
    cdef int ismrmrd_make_consistent_acquisition(ISMRMRD_Acquisition *acq)
    cdef size_t ismrmrd_size_of_acquisition_traj(const ISMRMRD_Acquisition *acq)
    cdef size_t ismrmrd_size_of_acquisition_data(const ISMRMRD_Acquisition *acq)


    cdef enum ISMRMRD_ImageDataTypes:
        ISMRMRD_USHORT = 1 # corresponds to uint16_t
        ISMRMRD_SHORT      # corresponds to int16_t
        ISMRMRD_UINT       # corresponds to uint32_t
        ISMRMRD_INT        # corresponds to int32_t
        ISMRMRD_FLOAT      # corresponds to float
        ISMRMRD_DOUBLE     # corresponds to double
        ISMRMRD_CXFLOAT    # corresponds to complex float
        ISMRMRD_CXDOUBLE   # corresponds to complex double

    cdef enum ISMRMRD_ImageTypes:
        ISMRMRD_IMTYPE_MAGNITUDE = 1
        ISMRMRD_IMTYPE_PHASE
        ISMRMRD_IMTYPE_REAL
        ISMRMRD_IMTYPE_IMAG
        ISMRMRD_IMTYPE_COMPLEX

    # IMAGE FLAGS
    cdef enum ISMRMRD_ImageFlags:
        ISMRMRD_IMAGE_IS_NAVIGATION_DATA = 1    # used to be 23?
        ISMRMRD_IMAGE_USER1 = 57
        ISMRMRD_IMAGE_USER2 = 58
        ISMRMRD_IMAGE_USER3 = 59
        ISMRMRD_IMAGE_USER4 = 60
        ISMRMRD_IMAGE_USER5 = 61
        ISMRMRD_IMAGE_USER6 = 62
        ISMRMRD_IMAGE_USER7 = 63
        ISMRMRD_IMAGE_USER8 = 64

    ctypedef struct ISMRMRD_ImageHeader:
        uint16_t version    # First unsigned int indicates the version
        uint16_t data_type  # e.g. unsigned short, float, complex float, etc.
        uint64_t flags      # bit field with flags
        uint32_t measurement_uid    # Unique ID for the measurement
        uint16_t matrix_size[3]     # Pixels in the 3 spatial dimensions
        float field_of_view[3]      # Size (in mm) of the 3 spatial dimensions
        uint16_t channels           # Number of receive channels
        float position[ISMRMRD_POSITION_LENGTH]     # Three-dimensional spatial offsets from isocenter
        float read_dir[ISMRMRD_DIRECTION_LENGTH]    # Directional cosines of the readout/frequency encoding
        float phase_dir[ISMRMRD_DIRECTION_LENGTH]   # Directional cosines of the phase
        float slice_dir[ISMRMRD_DIRECTION_LENGTH]   # Directional cosines of the slice direction
        float patient_table_position[ISMRMRD_POSITION_LENGTH] # Patient table off-center
        uint16_t average    # e.g. signal average number
        uint16_t slice      # e.g. imaging slice number
        uint16_t contrast   # e.g. echo number in multi-echo
        uint16_t phase      # e.g. cardiac phase number
        uint16_t repetition # e.g. dynamic number for dynamic scanning
        uint16_t set        # e.g. flow encodning set
        uint32_t acquisition_time_stamp # Acquisition clock
        uint32_t physiology_time_stamp[ISMRMRD_PHYS_STAMPS] # Physiology time stamps, e.g. ecg, breating, etc.
        uint16_t image_type     # e.g. magnitude, phase, complex, real, imag, etc.
        uint16_t image_index    # e.g. image number in series of images
        uint16_t image_series_index             # e.g. series number
        int32_t user_int[ISMRMRD_USER_INTS]     # Free user parameters
        float user_float[ISMRMRD_USER_FLOATS]   # Free user parameters

    ctypedef struct ISMRMRD_Image:
        ISMRMRD_ImageHeader head
        char *attribute_string
        void *data

    cdef int ismrmrd_init_image_header(ISMRMRD_ImageHeader *hdr)
    cdef ISMRMRD_Image * ismrmrd_create_image()
    cdef int ismrmrd_free_image(ISMRMRD_Image *im)
    cdef int ismrmrd_init_image(ISMRMRD_Image *im)
    cdef int ismrmrd_cleanup_image(ISMRMRD_Image *im)
    cdef int ismrmrd_copy_image(ISMRMRD_Image *imdest, const ISMRMRD_Image *imsource)
    cdef int ismrmrd_make_consistent_image(ISMRMRD_Image *im)
    cdef size_t ismrmrd_size_of_image_attribute_string(const ISMRMRD_Image *im)
    cdef size_t ismrmrd_size_of_image_data(const ISMRMRD_Image *im)


    ctypedef struct ISMRMRD_NDArray:
        uint16_t version                      # First unsigned int indicates the version
        uint16_t data_type                    # e.g. unsigned short, float, complex float, etc.
        uint16_t ndim                         # Number of dimensions
        uint16_t dims[ISMRMRD_NDARRAY_MAXDIM] # Dimensions
        void *data                            # Pointer to data

    cdef ISMRMRD_NDArray * ismrmrd_create_ndarray()
    cdef int ismrmrd_free_ndarray(ISMRMRD_NDArray *arr)
    cdef int ismrmrd_init_ndarray(ISMRMRD_NDArray *arr)
    cdef int ismrmrd_cleanup_ndarray(ISMRMRD_NDArray *arr)
    cdef int ismrmrd_copy_ndarray(ISMRMRD_NDArray *arrdest, const ISMRMRD_NDArray *arrsource)
    cdef int ismrmrd_make_consistent_ndarray(ISMRMRD_NDArray *arr)
    cdef size_t ismrmrd_size_of_ndarray_data(const ISMRMRD_NDArray *arr)


    cdef bint ismrmrd_is_flag_set(const uint64_t flags, const uint64_t val)
    cdef int ismrmrd_set_flag(uint64_t *flags, const uint64_t val)
    cdef int ismrmrd_clear_flag(uint64_t *flags, const uint64_t val)
    cdef int ismrmrd_clear_all_flags(uint64_t *flags)

    cdef int ismrmrd_sign_of_directions(float read_dir[3],
            float phase_dir[3], float slice_dir[3])
    cdef void ismrmrd_directions_to_quaternion(float read_dir[3],
            float phase_dir[3], float slice_dir[3], float quat[4])
    cdef void ismrmrd_quaternion_to_directions(float quat[4],
            float read_dir[3], float phase_dir[3], float slice_dir[3])

cdef extern from "ismrmrd/dataset.h":
    cdef enum BlockModes:
        BLOCKMODE_ARRAY
        BLOCKMODE_BLOB

    ctypedef struct ISMRMRD_Dataset:
        char *filename
        char *groupname
        int fileid

    cdef int ismrmrd_init_dataset(ISMRMRD_Dataset*, const char*, const char*)
    cdef int ismrmrd_open_dataset(ISMRMRD_Dataset*, const bint)
    cdef int ismrmrd_close_dataset(ISMRMRD_Dataset*)
    cdef char *ismrmrd_read_header(const ISMRMRD_Dataset *)
    cdef int ismrmrd_write_header(ISMRMRD_Dataset *, const char *)
    cdef uint32_t ismrmrd_get_number_of_acquisitions(const ISMRMRD_Dataset*)
    cdef int ismrmrd_append_acquisition(ISMRMRD_Dataset *,
            const ISMRMRD_Acquisition *)
    cdef int ismrmrd_read_acquisition(const ISMRMRD_Dataset *, uint32_t,
            ISMRMRD_Acquisition *)
    cdef uint32_t ismrmrd_get_number_of_images(const ISMRMRD_Dataset *,
            const char *)
    cdef int ismrmrd_append_image(ISMRMRD_Dataset *, const char *,
            const ISMRMRD_Image *)
    cdef int ismrmrd_read_image(const ISMRMRD_Dataset *, const char *,
            const uint32_t, ISMRMRD_Image *)
    cdef uint32_t ismrmrd_get_number_of_arrays(const ISMRMRD_Dataset *,
            const char *)
    cdef int ismrmrd_append_array(ISMRMRD_Dataset *dset, const char *,
            const ISMRMRD_NDArray *)
    cdef int ismrmrd_read_array(const ISMRMRD_Dataset *, const char *,
            const uint32_t, ISMRMRD_NDArray *)

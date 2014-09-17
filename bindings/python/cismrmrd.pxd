from libc.stdint cimport uint16_t, uint32_t, uint64_t, int32_t

cdef extern from "ismrmrd.h":
    cdef enum:
        ISMRMRD_VERSION = 1
        ISMRMRD_USER_INTS = 8
        ISMRMRD_USER_FLOATS = 8
        ISMRMRD_PHYS_STAMPS = 3
        ISMRMRD_CHANNEL_MASKS = 16
        ISMRMRD_NDARRAY_MAXDIM = 7
        ISMRMRD_POSITION_LENGTH = 3
        ISMRMRD_DIRECTION_LENGTH = 3

    ctypedef struct EncodingCounters:
        uint16_t kspace_encode_step_1  # e.g. phase encoding line number */
        uint16_t kspace_encode_step_2  # e.g. partition encodning number */
        uint16_t average               # e.g. signal average number */
        uint16_t slice                 # e.g. imaging slice number */
        uint16_t contrast              # e.g. echo number in multi-echo */
        uint16_t phase                 # e.g. cardiac phase number */
        uint16_t repetition            # e.g. dynamic number for dynamic scanning */
        uint16_t set                   # e.g. flow encodning set */
        uint16_t segment               # e.g. segment number for segmented acquisition */
        uint16_t user[ISMRMRD_USER_INTS]   # Free user parameters */

    ctypedef struct ISMRMRD_AcquisitionHeader:
        uint16_t version   # First unsigned int indicates the version */
        uint64_t flags     # bit field with flags */
        uint32_t measurement_uid   # Unique ID for the measurement */
        uint32_t scan_counter      # Current acquisition number in the measurement */
        uint32_t acquisition_time_stamp    # Acquisition clock */
        uint32_t physiology_time_stamp[ISMRMRD_PHYS_STAMPS] # Physiology time stamps, e.g. ecg, breating, etc. */
        uint16_t number_of_samples # Number of samples acquired */
        uint16_t available_channels    # Available coils */
        uint16_t active_channels       # Active coils on current acquisiton */
        uint64_t channel_mask[ISMRMRD_CHANNEL_MASKS]   # Mask to indicate which channels are active. Support for 1024 channels */
        uint16_t discard_pre       # Samples to be discarded at the beginning of acquisition */
        uint16_t discard_post      # Samples to be discarded at the end of acquisition */
        uint16_t center_sample     # Sample at the center of k-space */
        uint16_t encoding_space_ref    # Reference to an encoding space, typically only one per acquisition */
        uint16_t trajectory_dimensions # Indicates the dimensionality of the trajectory vector (0 means no trajectory) */
        float sample_time_us                                   # Time between samples in micro seconds, sampling BW */
        float position[ISMRMRD_POSITION_LENGTH] # Three-dimensional spatial offsets from isocenter */
        float read_dir[ISMRMRD_DIRECTION_LENGTH] # Directional cosines of the readout/frequency encoding */
        float phase_dir[ISMRMRD_DIRECTION_LENGTH] # Directional cosines of the phase */
        float slice_dir[ISMRMRD_DIRECTION_LENGTH] # Directional cosines of the slice direction */
        float patient_table_position[ISMRMRD_POSITION_LENGTH]  # Patient table off-center */
        EncodingCounters idx       # Encoding loop counters, see above */
        int32_t user_int[ISMRMRD_USER_INTS]       # Free user parameters */
        float user_float[ISMRMRD_USER_FLOATS]     # Free user parameters */

    ctypedef struct ISMRMRD_Acquisition:
        ISMRMRD_AcquisitionHeader head
        float *traj
        float complex *data

    cdef ISMRMRD_Acquisition ismrmrd_create_acquisition()
    cdef void ismrmrd_free_acquisition(ISMRMRD_Acquisition*)

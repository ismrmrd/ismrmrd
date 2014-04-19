#include "ismrmrd.h"

#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    int i;

    ISMRMRD::EncodingCounters idx;

    idx.kspace_encode_step_1 = 1;
    idx.kspace_encode_step_2 = 2;
    idx.average = 50;
    idx.slice = 14;
    idx.contrast = 5000;
    idx.phase = 42;
    idx.repetition = 2;
    idx.set = 1;
    idx.segment = 1;
    for (i = 0; i < ISMRMRD_USER_INTS; i++) {
        idx.user[i] = i * 42;
    }

    ISMRMRD::AcquisitionHeader acq_head;
    acq_head.version = 1;
    acq_head.flags = -1;
    acq_head.measurement_uid = 12345;
    acq_head.scan_counter = 512;
    acq_head.acquisition_time_stamp = 1363104441;
    for (i = 0; i < ISMRMRD_PHYS_STAMPS; i++) {
        acq_head.physiology_time_stamp[i] = 1363106388;
    }
    acq_head.number_of_samples = 24;
    acq_head.available_channels = 32;
    acq_head.active_channels = 32;
    for (i = 0; i < ISMRMRD_CHANNEL_MASKS; i++) {
        acq_head.channel_mask[i] = -1;
    }
    acq_head.discard_pre = 2;
    acq_head.discard_post = 2;
    acq_head.center_sample = 12;
    acq_head.encoding_space_ref = 1;
    acq_head.trajectory_dimensions = 2;
    acq_head.sample_time_us = 123.321;

    acq_head.position[0] = -12.80f;
    acq_head.position[1] = 0.23f;
    acq_head.position[2] = 12.45f;

    acq_head.read_dir[0] = 1;
    acq_head.read_dir[1] = 0;
    acq_head.read_dir[2] = 0;

    acq_head.phase_dir[0] = 0;
    acq_head.phase_dir[1] = 1;
    acq_head.phase_dir[2] = 0;

    acq_head.slice_dir[0] = 0;
    acq_head.slice_dir[1] = 0;
    acq_head.slice_dir[2] = 1;

    acq_head.patient_table_position[0] = 0.0;
    acq_head.patient_table_position[1] = 0.0;
    acq_head.patient_table_position[2] = 2.13;

    acq_head.idx = idx;
    for (i = 0; i < ISMRMRD_USER_INTS; i++) {
        acq_head.user_int[i] = i;
    }
    for (i = 0; i < ISMRMRD_USER_FLOATS; i++) {
        acq_head.user_float[i] = (float)i;
    }


    ISMRMRD::ImageHeader img_head;
    img_head.version = 1;
    img_head.flags = -1;
    img_head.measurement_uid = 12345;

    img_head.matrix_size[0] = 128;
    img_head.matrix_size[1] = 128;
    img_head.matrix_size[2] = 0;

    img_head.field_of_view[0] = 256.0;
    img_head.field_of_view[1] = 256.0;
    img_head.field_of_view[2] = 12.0;

    img_head.channels = 32;

    img_head.position[0] = -12.80f;
    img_head.position[1] = 0.23f;
    img_head.position[2] = 12.45f;

    img_head.read_dir[0] = 1;
    img_head.read_dir[1] = 0;
    img_head.read_dir[2] = 0;

    img_head.phase_dir[0] = 0;
    img_head.phase_dir[1] = 1;
    img_head.phase_dir[2] = 0;

    img_head.slice_dir[0] = 0;
    img_head.slice_dir[1] = 0;
    img_head.slice_dir[2] = 1;

    img_head.patient_table_position[0] = 0.0;
    img_head.patient_table_position[1] = 0.0;
    img_head.patient_table_position[2] = 2.13f;

    img_head.average = 50;
    img_head.slice = 14;
    img_head.contrast = 5000;
    img_head.phase = 42;
    img_head.repetition = 2;
    img_head.set = 1;

    img_head.acquisition_time_stamp = 1363104441;

    for (i = 0; i < ISMRMRD_PHYS_STAMPS; i++) {
        img_head.physiology_time_stamp[i] = 1363106388;
    }

    img_head.image_data_type = ISMRMRD::DATA_FLOAT;
    img_head.image_type = ISMRMRD::TYPE_MAGNITUDE;
    img_head.image_index = 42;
    img_head.image_series_index = 3;

    for (i = 0; i < ISMRMRD_USER_INTS; i++) {
        img_head.user_int[i] = i;
    }
    for (i = 0; i < ISMRMRD_USER_FLOATS; i++) {
        img_head.user_float[i] = (float)i;
    }

    return EXIT_SUCCESS;
}

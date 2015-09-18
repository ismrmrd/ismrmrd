#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/version.h"
#include <boost/test/unit_test.hpp>

using namespace ISMRMRD;

BOOST_AUTO_TEST_SUITE(AcquisitionsTest)

static void check_header(ISMRMRD_AcquisitionHeader* chead);

BOOST_AUTO_TEST_CASE(test_acquisition_header)
{
    ISMRMRD_AcquisitionHeader chead;

    // Check that header is of expected size
    size_t expected_size = 9 * sizeof(uint16_t) +
            (3 + ISMRMRD_PHYS_STAMPS) * sizeof(uint32_t) +
            ISMRMRD_USER_INTS * sizeof(int32_t) +
            (1 + ISMRMRD_CHANNEL_MASKS) * sizeof(uint64_t) +
            ((2 * ISMRMRD_POSITION_LENGTH) + (3 * ISMRMRD_DIRECTION_LENGTH) +
                    1 + ISMRMRD_USER_FLOATS) * sizeof(float) +
            (9 + ISMRMRD_USER_INTS) * sizeof(uint16_t);
    BOOST_CHECK_EQUAL(sizeof(chead), expected_size);

    // Check that header is initialized properly
    BOOST_CHECK_EQUAL(ismrmrd_init_acquisition_header(NULL), ISMRMRD_RUNTIMEERROR);
    BOOST_CHECK_EQUAL(ismrmrd_init_acquisition_header(&chead), ISMRMRD_NOERROR);
    check_header(&chead);
}

BOOST_AUTO_TEST_CASE(test_acquisition_init_cleanup)
{
    ISMRMRD_Acquisition acq;

    // Check initialization of acquisition
    BOOST_CHECK_EQUAL(ismrmrd_init_acquisition(NULL), ISMRMRD_RUNTIMEERROR);
    BOOST_CHECK_EQUAL(ismrmrd_init_acquisition(&acq), ISMRMRD_NOERROR);
    BOOST_CHECK(!acq.traj);
    BOOST_CHECK(!acq.data);

    // Check cleanup of acquisition
    BOOST_CHECK_EQUAL(ismrmrd_cleanup_acquisition(NULL), ISMRMRD_RUNTIMEERROR);
    BOOST_CHECK_EQUAL(ismrmrd_cleanup_acquisition(&acq), ISMRMRD_NOERROR);
    BOOST_CHECK(!acq.traj);
    BOOST_CHECK(!acq.data);
}

BOOST_AUTO_TEST_CASE(test_acquisition_create_free)
{
    ISMRMRD_Acquisition* cacqp = NULL;

    // Check creation of new acquisition
    BOOST_CHECK(cacqp = ismrmrd_create_acquisition());
    // Check that it's initialized
    check_header(&cacqp->head);
    BOOST_CHECK(!cacqp->traj);
    BOOST_CHECK(!cacqp->data);

    // Check cleanup
    BOOST_CHECK_EQUAL(ismrmrd_free_acquisition(NULL), ISMRMRD_RUNTIMEERROR);
    BOOST_CHECK_EQUAL(ismrmrd_free_acquisition(cacqp), ISMRMRD_NOERROR);
}

BOOST_AUTO_TEST_CASE(test_acquisition_copy)
{
    // Weak check of acquisition copying
    ISMRMRD_Acquisition csrc, cdst;
    BOOST_CHECK_EQUAL(ismrmrd_init_acquisition(&csrc), ISMRMRD_NOERROR);
    // TODO: it is necessary to call init_acquisition on the destination acquisition
    // before copying, in case its traj or data are non-NULL!
    BOOST_CHECK_EQUAL(ismrmrd_init_acquisition(&cdst), ISMRMRD_NOERROR);
    BOOST_CHECK_EQUAL(ismrmrd_copy_acquisition(&cdst, NULL), ISMRMRD_RUNTIMEERROR);
    BOOST_CHECK_EQUAL(ismrmrd_copy_acquisition(NULL, &csrc), ISMRMRD_RUNTIMEERROR);
    BOOST_CHECK_EQUAL(ismrmrd_copy_acquisition(NULL, NULL), ISMRMRD_RUNTIMEERROR);
    BOOST_CHECK_EQUAL(ismrmrd_copy_acquisition(&cdst, &csrc), ISMRMRD_NOERROR);
    check_header(&cdst.head);
    BOOST_CHECK_EQUAL(ismrmrd_size_of_acquisition_data(&cdst), 0);
    BOOST_CHECK(!cdst.traj);
    BOOST_CHECK(!cdst.data);
}

BOOST_AUTO_TEST_CASE(test_acquisition_make_consistent)
{
    ISMRMRD_Acquisition acq;
    BOOST_CHECK_EQUAL(ismrmrd_init_acquisition(&acq), ISMRMRD_NOERROR);

    BOOST_CHECK_EQUAL(ismrmrd_make_consistent_acquisition(NULL), ISMRMRD_RUNTIMEERROR);

    uint16_t nsamples = 512;
    uint16_t nchannels = 8;
    uint16_t ntrajd = 2;
    acq.head.number_of_samples = nsamples;
    acq.head.active_channels = nchannels;
    acq.head.trajectory_dimensions = ntrajd;

    BOOST_CHECK_EQUAL(ismrmrd_size_of_acquisition_traj(&acq), nsamples * ntrajd * sizeof(*acq.traj));
    BOOST_CHECK_EQUAL(ismrmrd_size_of_acquisition_data(&acq), nsamples * nchannels * sizeof(*acq.data));
    BOOST_CHECK_EQUAL(ismrmrd_make_consistent_acquisition(&acq), ISMRMRD_NOERROR);

    BOOST_CHECK_EQUAL(acq.head.available_channels, nchannels);
    // check that traj and data were allocated
    BOOST_CHECK(acq.traj);
    BOOST_CHECK(acq.data);

    ismrmrd_cleanup_acquisition(&acq);
}

static void check_header(ISMRMRD_AcquisitionHeader* chead)
{
    BOOST_CHECK_EQUAL(chead->version, ISMRMRD_VERSION_MAJOR);
    BOOST_CHECK_EQUAL(chead->number_of_samples, 0);
    BOOST_CHECK_EQUAL(chead->available_channels, 1);
    BOOST_CHECK_EQUAL(chead->active_channels, 1);
    BOOST_CHECK_EQUAL(chead->flags, 0);
    BOOST_CHECK_EQUAL(chead->measurement_uid, 0);
    BOOST_CHECK_EQUAL(chead->scan_counter, 0);
    BOOST_CHECK_EQUAL(chead->acquisition_time_stamp, 0);
    for (int idx = 0; idx < ISMRMRD_PHYS_STAMPS; idx++) {
        BOOST_CHECK_EQUAL(chead->physiology_time_stamp[idx], 0);
    }

    for (int idx = 0; idx < ISMRMRD_CHANNEL_MASKS; idx++) {
        BOOST_CHECK_EQUAL(chead->channel_mask[idx], 0);
    }
    BOOST_CHECK_EQUAL(chead->discard_pre, 0);
    BOOST_CHECK_EQUAL(chead->discard_post, 0);
    BOOST_CHECK_EQUAL(chead->center_sample, 0);
    BOOST_CHECK_EQUAL(chead->encoding_space_ref, 0);
    BOOST_CHECK_EQUAL(chead->trajectory_dimensions, 0);
    BOOST_CHECK_EQUAL(chead->sample_time_us, 0);
    for (int idx = 0; idx < ISMRMRD_POSITION_LENGTH; idx++) {
        BOOST_CHECK_EQUAL(chead->position[idx], 0);
    }
    for (int idx = 0; idx < ISMRMRD_DIRECTION_LENGTH; idx++) {
        BOOST_CHECK_EQUAL(chead->read_dir[idx], 0);
    }
    for (int idx = 0; idx < ISMRMRD_DIRECTION_LENGTH; idx++) {
        BOOST_CHECK_EQUAL(chead->phase_dir[idx], 0);
    }
    for (int idx = 0; idx < ISMRMRD_DIRECTION_LENGTH; idx++) {
        BOOST_CHECK_EQUAL(chead->slice_dir[idx], 0);
    }
    for (int idx = 0; idx < ISMRMRD_POSITION_LENGTH; idx++) {
        BOOST_CHECK_EQUAL(chead->patient_table_position[idx], 0);
    }

    // EncodingCounters
    BOOST_CHECK_EQUAL(chead->idx.kspace_encode_step_1, 0);
    BOOST_CHECK_EQUAL(chead->idx.kspace_encode_step_2, 0);
    BOOST_CHECK_EQUAL(chead->idx.average, 0);
    BOOST_CHECK_EQUAL(chead->idx.slice, 0);
    BOOST_CHECK_EQUAL(chead->idx.contrast, 0);
    BOOST_CHECK_EQUAL(chead->idx.phase, 0);
    BOOST_CHECK_EQUAL(chead->idx.repetition, 0);
    BOOST_CHECK_EQUAL(chead->idx.set, 0);
    BOOST_CHECK_EQUAL(chead->idx.segment, 0);

    for (int idx = 0; idx < ISMRMRD_USER_INTS; idx++) {
        BOOST_CHECK_EQUAL(chead->idx.user[idx], 0);
    }

    for (int idx = 0; idx < ISMRMRD_USER_INTS; idx++) {
        BOOST_CHECK_EQUAL(chead->user_int[idx], 0);
    }
    for (int idx = 0; idx < ISMRMRD_USER_FLOATS; idx++) {
        BOOST_CHECK_EQUAL(chead->user_float[idx], 0);
    }
}

BOOST_AUTO_TEST_SUITE_END()

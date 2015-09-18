#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/version.h"
#include <boost/test/unit_test.hpp>

using namespace ISMRMRD;

BOOST_AUTO_TEST_SUITE(ImagesTest)

static void check_header(ISMRMRD_ImageHeader* chead);

BOOST_AUTO_TEST_CASE(test_image_header)
{
    ISMRMRD_ImageHeader chead;

    // Check that header is of expected size
    size_t expected_size = 15 * sizeof(uint16_t) +
            (3 + ISMRMRD_PHYS_STAMPS) * sizeof(uint32_t) +
            ISMRMRD_USER_INTS * sizeof(int32_t) +
            1 * sizeof(uint64_t) +
            ((2 * ISMRMRD_POSITION_LENGTH) + (3 * ISMRMRD_DIRECTION_LENGTH) +
                    3 + ISMRMRD_USER_FLOATS) * sizeof(float);
    BOOST_CHECK_EQUAL(sizeof(chead), expected_size);

    // Check that header is initialized properly
    BOOST_CHECK_EQUAL(ismrmrd_init_image_header(NULL), ISMRMRD_RUNTIMEERROR);
    BOOST_CHECK_EQUAL(ismrmrd_init_image_header(&chead), ISMRMRD_NOERROR);
    check_header(&chead);
}

BOOST_AUTO_TEST_CASE(test_image_init_cleanup)
{
    ISMRMRD_Image cimg;

    // Check initialization of image
    BOOST_CHECK_EQUAL(ismrmrd_init_image(NULL), ISMRMRD_RUNTIMEERROR);
    BOOST_CHECK_EQUAL(ismrmrd_init_image(&cimg), ISMRMRD_NOERROR);
    check_header(&cimg.head);
    BOOST_CHECK(!cimg.attribute_string);
    BOOST_CHECK(!cimg.data);

    // Check cleanup of image
    BOOST_CHECK_EQUAL(ismrmrd_cleanup_image(NULL), ISMRMRD_RUNTIMEERROR);
    BOOST_CHECK_EQUAL(ismrmrd_cleanup_image(&cimg), ISMRMRD_NOERROR);
    BOOST_CHECK(!cimg.attribute_string);
    BOOST_CHECK(!cimg.data);
}

BOOST_AUTO_TEST_CASE(test_image_create_free)
{
    ISMRMRD_Image* cimgp = NULL;

    // Check creation of new image
    BOOST_CHECK(cimgp = ismrmrd_create_image());
    // Check that it's initialized
    check_header(&cimgp->head);
    BOOST_CHECK(!cimgp->attribute_string);
    BOOST_CHECK(!cimgp->data);

    // Check cleanup
    BOOST_CHECK_EQUAL(ismrmrd_free_image(NULL), ISMRMRD_RUNTIMEERROR);
    BOOST_CHECK_EQUAL(ismrmrd_free_image(cimgp), ISMRMRD_NOERROR);
}

BOOST_AUTO_TEST_CASE(test_image_copy)
{
    // Weak check of image copying
    ISMRMRD_Image csrc, cdst;
    BOOST_CHECK_EQUAL(ismrmrd_init_image(&csrc), ISMRMRD_NOERROR);
    // TODO: it is necessary to call init_image on the destination image
    // before copying, in case its attribute_string or data are non-NULL!
    BOOST_CHECK_EQUAL(ismrmrd_init_image(&cdst), ISMRMRD_NOERROR);
    BOOST_CHECK_EQUAL(ismrmrd_copy_image(&cdst, NULL), ISMRMRD_RUNTIMEERROR);
    BOOST_CHECK_EQUAL(ismrmrd_copy_image(NULL, &csrc), ISMRMRD_RUNTIMEERROR);
    BOOST_CHECK_EQUAL(ismrmrd_copy_image(NULL, NULL), ISMRMRD_RUNTIMEERROR);
    BOOST_CHECK_EQUAL(ismrmrd_copy_image(&cdst, &csrc), ISMRMRD_NOERROR);
    check_header(&cdst.head);
    BOOST_CHECK_EQUAL(ismrmrd_size_of_image_attribute_string(&cdst), 0);
    BOOST_CHECK_EQUAL(ismrmrd_size_of_image_data(&cdst), 0);
    BOOST_CHECK(!cdst.attribute_string);
    BOOST_CHECK(!cdst.data);
}

BOOST_AUTO_TEST_CASE(test_image_make_consistent)
{
    ISMRMRD_Image img;
    BOOST_CHECK_EQUAL(ismrmrd_init_image(&img), ISMRMRD_NOERROR);

    BOOST_CHECK_EQUAL(ismrmrd_make_consistent_image(NULL), ISMRMRD_RUNTIMEERROR);

    uint16_t matrix_size[] = {128, 128, 1};
    uint16_t nchannels = 8;
    uint16_t dtype = ISMRMRD_FLOAT;
    uint16_t attrlen = 65;
    img.head.matrix_size[0] = matrix_size[0];
    img.head.matrix_size[1] = matrix_size[1];
    img.head.matrix_size[2] = matrix_size[2];
    img.head.channels = nchannels;
    img.head.data_type = dtype;
    img.head.attribute_string_len = attrlen;

    BOOST_CHECK_EQUAL(ismrmrd_size_of_image_data(&img),
            matrix_size[0] * matrix_size[1] * matrix_size[2] *
            nchannels * ismrmrd_sizeof_data_type(dtype));
    BOOST_CHECK_EQUAL(ismrmrd_size_of_image_attribute_string(&img),
            attrlen * sizeof(*img.attribute_string));
    BOOST_CHECK_EQUAL(ismrmrd_make_consistent_image(&img), ISMRMRD_NOERROR);

    // check that data and attribute_string were allocated
    BOOST_CHECK(img.data);
    BOOST_CHECK(img.attribute_string);

    ismrmrd_cleanup_image(&img);
}

static void check_header(ISMRMRD_ImageHeader* chead)
{
    BOOST_CHECK_EQUAL(chead->version, ISMRMRD_VERSION_MAJOR);
    BOOST_CHECK_EQUAL(chead->matrix_size[0], 0);
    BOOST_CHECK_EQUAL(chead->matrix_size[1], 1);
    BOOST_CHECK_EQUAL(chead->matrix_size[2], 1);
    BOOST_CHECK_EQUAL(chead->channels, 1);

    BOOST_CHECK_EQUAL(chead->data_type, 0);
    BOOST_CHECK_EQUAL(chead->flags, 0);
    BOOST_CHECK_EQUAL(chead->measurement_uid, 0);
    for (int idx = 0; idx < 3; idx++) {
        BOOST_CHECK_EQUAL(chead->field_of_view[idx], 0);
    }
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
    BOOST_CHECK_EQUAL(chead->average, 0);
    BOOST_CHECK_EQUAL(chead->slice, 0);
    BOOST_CHECK_EQUAL(chead->contrast, 0);
    BOOST_CHECK_EQUAL(chead->phase, 0);
    BOOST_CHECK_EQUAL(chead->repetition, 0);
    BOOST_CHECK_EQUAL(chead->set, 0);
    BOOST_CHECK_EQUAL(chead->acquisition_time_stamp, 0);
    for (int idx = 0; idx < ISMRMRD_PHYS_STAMPS; idx++) {
        BOOST_CHECK_EQUAL(chead->physiology_time_stamp[idx], 0);
    }
    BOOST_CHECK_EQUAL(chead->image_type, 0);
    BOOST_CHECK_EQUAL(chead->image_index, 0);
    BOOST_CHECK_EQUAL(chead->image_series_index, 0);

    for (int idx = 0; idx < ISMRMRD_USER_INTS; idx++) {
        BOOST_CHECK_EQUAL(chead->user_int[idx], 0);
    }
    for (int idx = 0; idx < ISMRMRD_USER_FLOATS; idx++) {
        BOOST_CHECK_EQUAL(chead->user_float[idx], 0);
    }
    BOOST_CHECK_EQUAL(chead->attribute_string_len, 0);
}

BOOST_AUTO_TEST_SUITE_END()

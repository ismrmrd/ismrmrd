#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/version.h"
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>

using namespace ISMRMRD;

typedef boost::mpl::list<uint16_t, int16_t, uint32_t, int32_t, float,
                         double, std::complex<float>, std::complex<double> > test_types;

BOOST_AUTO_TEST_SUITE(Images)

static void check_header(const ImageHeader& head);

BOOST_AUTO_TEST_CASE_TEMPLATE(image_create, T, test_types)
{
    Image<T> img;
    ImageHeader head = img.getHead();

    // Check that header is of expected size
    size_t expected_size = 21 * sizeof(uint32_t) +
            ISMRMRD_USER_INTS * sizeof(int32_t) +
            2 * sizeof(uint64_t) +
            ((2 * ISMRMRD_POSITION_LENGTH) + (3 * ISMRMRD_DIRECTION_LENGTH) +
                    3 + ISMRMRD_USER_FLOATS) * sizeof(float);

    BOOST_CHECK_EQUAL(sizeof(head), expected_size);

    // Check that header is initialized properly
    check_header(head);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(image_copy, T, test_types)
{
    Image<T> img1;
    check_header(img1.getHead());
    Image<T> img2(img1);
    check_header(img2.getHead());

    BOOST_CHECK(img1.getHead() == img2.getHead());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(image_getters_setters, T, test_types)
{
    Image<T> img;

    // TODO: implement
}

BOOST_AUTO_TEST_CASE_TEMPLATE(image_resize,T, test_types)
{
    Image<T> img;
    check_header(img.getHead());
    BOOST_CHECK_EQUAL(img.getData().size(), 0);

    img.resize(72, 72, 24, 0);
    BOOST_CHECK_EQUAL(img.getMatrixSizeX(), 72);
    BOOST_CHECK_EQUAL(img.getMatrixSizeY(), 72);
    BOOST_CHECK_EQUAL(img.getMatrixSizeZ(), 24);
    BOOST_CHECK_EQUAL(img.getData().size(), 72*72*24);

    std::vector<T> zeros(72*72*24, 0);
    BOOST_CHECK_EQUAL_COLLECTIONS(zeros.begin(), zeros.end(),
            img.getData().begin(), img.getData().end());
}

static void check_header(const ImageHeader& chead)
{
    BOOST_CHECK_EQUAL(chead.version, ISMRMRD_VERSION_MAJOR);
    BOOST_CHECK_EQUAL(chead.matrix_size[0], 0);
    BOOST_CHECK_EQUAL(chead.matrix_size[1], 1);
    BOOST_CHECK_EQUAL(chead.matrix_size[2], 1);
    BOOST_CHECK_EQUAL(chead.channels, 1);

    //BOOST_CHECK_EQUAL(chead.storage_type, 0);
    BOOST_CHECK_EQUAL(chead.flags, 0);
    for (int idx = 0; idx < 3; idx++) {
        BOOST_CHECK_EQUAL(chead.field_of_view[idx], 0);
    }
    for (int idx = 0; idx < ISMRMRD_POSITION_LENGTH; idx++) {
        BOOST_CHECK_EQUAL(chead.position[idx], 0);
    }
    for (int idx = 0; idx < ISMRMRD_DIRECTION_LENGTH; idx++) {
        BOOST_CHECK_EQUAL(chead.read_dir[idx], 0);
    }
    for (int idx = 0; idx < ISMRMRD_DIRECTION_LENGTH; idx++) {
        BOOST_CHECK_EQUAL(chead.phase_dir[idx], 0);
    }
    for (int idx = 0; idx < ISMRMRD_DIRECTION_LENGTH; idx++) {
        BOOST_CHECK_EQUAL(chead.slice_dir[idx], 0);
    }
    for (int idx = 0; idx < ISMRMRD_POSITION_LENGTH; idx++) {
        BOOST_CHECK_EQUAL(chead.patient_table_position[idx], 0);
    }
    BOOST_CHECK_EQUAL(chead.average, 0);
    BOOST_CHECK_EQUAL(chead.slice, 0);
    BOOST_CHECK_EQUAL(chead.contrast, 0);
    BOOST_CHECK_EQUAL(chead.phase, 0);
    BOOST_CHECK_EQUAL(chead.repetition, 0);
    BOOST_CHECK_EQUAL(chead.set, 0);
    for (int idx = 0; idx < ISMRMRD_PHYS_STAMPS; idx++) {
        BOOST_CHECK_EQUAL(chead.physiology_time_stamp[idx], 0);
    }
    BOOST_CHECK_EQUAL(chead.image_type, 0);
    BOOST_CHECK_EQUAL(chead.image_index, 0);
    BOOST_CHECK_EQUAL(chead.image_series_index, 0);

    for (int idx = 0; idx < ISMRMRD_USER_INTS; idx++) {
        BOOST_CHECK_EQUAL(chead.user_int[idx], 0);
    }
    for (int idx = 0; idx < ISMRMRD_USER_FLOATS; idx++) {
        BOOST_CHECK_EQUAL(chead.user_float[idx], 0);
    }
    BOOST_CHECK_EQUAL(chead.attribute_string_len, 0);
}

BOOST_AUTO_TEST_SUITE_END()

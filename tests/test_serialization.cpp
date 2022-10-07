#include <boost/test/unit_test.hpp>
#include <sstream>

#include "ismrmrd/serialization.h"

using namespace ISMRMRD;

BOOST_AUTO_TEST_SUITE(SerialiationTest)

// Test the serialization of a single acquisition
BOOST_AUTO_TEST_CASE(test_acquisition_serialization) {
    Acquisition acq;
    acq.resize(256, 16, 3);

    // Fill in some data
    for (size_t i = 0; i < acq.getNumberOfDataElements(); i++) {
        acq.getDataPtr()[i] = i;
    }

    // Fill trajectory
    for (size_t i = 0; i < acq.getNumberOfTrajElements(); i++) {
        acq.getTrajPtr()[i] = i;
    }

    // Test serialization and deserialization
    std::stringstream ss;
    ISMRMRD::serialize(acq, ss);
    Acquisition acq2;
    ISMRMRD::deserialize(acq2, ss);

    // Check that the data is the same
    BOOST_ASSERT(acq.getHead() == acq2.getHead());
    BOOST_CHECK_EQUAL_COLLECTIONS(acq.getDataPtr(), acq.getDataPtr() + acq.getNumberOfDataElements(),
                                  acq2.getDataPtr(), acq2.getDataPtr() + acq2.getNumberOfDataElements());
    BOOST_CHECK_EQUAL_COLLECTIONS(acq.getTrajPtr(), acq.getTrajPtr() + acq.getNumberOfTrajElements(),
                                  acq2.getTrajPtr(), acq2.getTrajPtr() + acq2.getNumberOfTrajElements());
}

typedef std::tuple<unsigned short, short, unsigned int, int, float, double, std::complex<float>, std::complex<double> > image_types_w_tuples;

// Test the serialization of a single image
BOOST_AUTO_TEST_CASE_TEMPLATE(test_image_serialization, T, image_types_w_tuples) {
    Image<T> img;
    img.resize(64, 64, 8, 4);

    //  just one header field, we will validate it and
    //  we are also validating matrix sizes, assume rest
    //  is working. Image does not have an == operator
    img.setImageSeriesIndex(10);

    // Fill in some data
    for (size_t i = 0; i < img.getNumberOfDataElements(); i++) {
        img.getDataPtr()[i] = i;
    }

    // set some meta data
    img.setAttributeString("This is my attribute string");

    // Test serialization and deserialization
    std::stringstream ss(std::ios::in | std::ios::out | std::ios::binary);
    ISMRMRD::serialize(img, ss);

    Image<T> img2;
    ISMRMRD::deserialize(img2, ss);

    // Check that the data is the same
    BOOST_CHECK_EQUAL(img.getImageSeriesIndex(), img2.getImageSeriesIndex());
    BOOST_CHECK_EQUAL(img.getAttributeString(), img2.getAttributeString());
    BOOST_CHECK_EQUAL_COLLECTIONS(img.getDataPtr(), img.getDataPtr() + img.getNumberOfDataElements(),
                                  img2.getDataPtr(), img2.getDataPtr() + img2.getNumberOfDataElements());
}

// Test the serialization of a single waveform
BOOST_AUTO_TEST_CASE(test_waveform_serialization) {
    Waveform wf(256, 16);
    wf.head.time_stamp = 1234567890;

    // Fill in some data
    std::fill(wf.begin_data(), wf.end_data(), 1);

    // Test serialization and deserialization
    std::stringstream ss;
    ISMRMRD::serialize(wf, ss);
    Waveform wf2;
    ISMRMRD::deserialize(wf2, ss);

    // Check that the data is the same
    BOOST_ASSERT(wf.head.time_stamp == wf2.head.time_stamp);
    BOOST_CHECK_EQUAL_COLLECTIONS(wf.begin_data(), wf.end_data(),
                                  wf2.begin_data(), wf2.end_data());
}

BOOST_AUTO_TEST_SUITE_END()

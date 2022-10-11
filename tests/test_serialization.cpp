#include "ismrmrd/serialization.h"
#include <boost/test/unit_test.hpp>
#include <fstream>
#include <sstream>

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

BOOST_AUTO_TEST_CASE(test_of_protocol_serialization) {

    // A header
    ISMRMRD::IsmrmrdHeader h;
    h.encoding.push_back(ISMRMRD::Encoding());
    h.encoding[0].encodedSpace.matrixSize.x = 256;
    h.encoding[0].encodedSpace.matrixSize.y = 256;
    h.encoding[0].encodedSpace.matrixSize.z = 1;
    h.encoding[0].reconSpace.matrixSize.x = 256;
    h.encoding[0].reconSpace.matrixSize.y = 256;
    h.encoding[0].reconSpace.matrixSize.z = 1;
    h.encoding[0].trajectory = ISMRMRD::TrajectoryType::CARTESIAN;

    // A few Acquisitions

    Acquisition acq1, acq2;
    acq1.resize(256, 16, 3);
    acq2.resize(256, 16, 3);

    // Fill in some data
    for (size_t i = 0; i < acq1.getNumberOfDataElements(); i++) {
        acq1.getDataPtr()[i] = i;
        acq2.getDataPtr()[i] = 2 * i;
    }

    // Fill trajectory
    for (size_t i = 0; i < acq1.getNumberOfTrajElements(); i++) {
        acq1.getTrajPtr()[i] = i;
        acq2.getDataPtr()[i] = 2 * i;
    }

    // A waveform
    Waveform wf(256, 16);
    wf.head.time_stamp = 1234567890;

    // An image
    Image<float> img;
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

    std::stringstream ss;
    ProtocolSerializer<std::ostream> serializer(ss);
    ProtocolDeserializer<std::istream> deserializer(ss);

    serializer.serialize(h);
    serializer.serialize(acq1);
    serializer.serialize(img);
    serializer.serialize(acq2);
    serializer.serialize(wf);

    // Test deserialization
    ISMRMRD::IsmrmrdHeader h2;
    Acquisition acq3, acq4;
    Waveform wf2;
    Image<float> img2;
    deserializer.deserialize(h2);
    deserializer.deserialize(acq3);
    deserializer.deserialize(img2);
    deserializer.deserialize(acq4);
    deserializer.deserialize(wf2);

    // Check that the header is the same
    BOOST_ASSERT(h.version == h2.version);
    BOOST_ASSERT(h.encoding[0].encodedSpace.matrixSize.x == h2.encoding[0].encodedSpace.matrixSize.x);
    BOOST_ASSERT(h.encoding[0].encodedSpace.matrixSize.y == h2.encoding[0].encodedSpace.matrixSize.y);
    BOOST_ASSERT(h.encoding[0].encodedSpace.matrixSize.z == h2.encoding[0].encodedSpace.matrixSize.z);
    BOOST_ASSERT(h.encoding[0].reconSpace.matrixSize.x == h2.encoding[0].reconSpace.matrixSize.x);
    BOOST_ASSERT(h.encoding[0].reconSpace.matrixSize.y == h2.encoding[0].reconSpace.matrixSize.y);
    BOOST_ASSERT(h.encoding[0].reconSpace.matrixSize.z == h2.encoding[0].reconSpace.matrixSize.z);
    BOOST_ASSERT(h.encoding[0].trajectory == h2.encoding[0].trajectory);

    // Check that the data are the same
    BOOST_ASSERT(acq1.getHead() == acq3.getHead());
    BOOST_CHECK_EQUAL_COLLECTIONS(acq1.getDataPtr(), acq1.getDataPtr() + acq1.getNumberOfDataElements(),
                                  acq3.getDataPtr(), acq3.getDataPtr() + acq3.getNumberOfDataElements());
    BOOST_CHECK_EQUAL_COLLECTIONS(acq1.getTrajPtr(), acq1.getTrajPtr() + acq1.getNumberOfTrajElements(),
                                  acq3.getTrajPtr(), acq3.getTrajPtr() + acq3.getNumberOfTrajElements());

    BOOST_ASSERT(acq2.getHead() == acq4.getHead());
    BOOST_CHECK_EQUAL_COLLECTIONS(acq2.getDataPtr(), acq2.getDataPtr() + acq2.getNumberOfDataElements(),
                                  acq4.getDataPtr(), acq4.getDataPtr() + acq4.getNumberOfDataElements());
    BOOST_CHECK_EQUAL_COLLECTIONS(acq2.getTrajPtr(), acq2.getTrajPtr() + acq2.getNumberOfTrajElements(),
                                  acq4.getTrajPtr(), acq4.getTrajPtr() + acq4.getNumberOfTrajElements());

    BOOST_ASSERT(wf.head.time_stamp == wf2.head.time_stamp);
    BOOST_CHECK_EQUAL_COLLECTIONS(wf.begin_data(), wf.end_data(),
                                  wf2.begin_data(), wf2.end_data());

    // Check that the image is the same
    BOOST_CHECK_EQUAL(img.getImageSeriesIndex(), img2.getImageSeriesIndex());
    BOOST_CHECK_EQUAL(img.getAttributeString(), img2.getAttributeString());
    BOOST_CHECK_EQUAL_COLLECTIONS(img.getDataPtr(), img.getDataPtr() + img.getNumberOfDataElements(),
                                  img2.getDataPtr(), img2.getDataPtr() + img2.getNumberOfDataElements());
}

std::string util_path() {
    // current directory
    std::string path = "./";
    int level = 0;
    while (level < 3) {
        std::string testpath(path + "utilities/ismrmrd_generate_cartesian_shepp_logan");
        std::ifstream f(testpath.c_str());
        if (f.good()) {
            return path;
        } else {
            path += "../";
            level++;
        }
    }
    throw std::runtime_error("Could not find utilities directory");
    return path;
}

BOOST_AUTO_TEST_CASE(test_end_to_end_streaming_reconstruction) {
    std::string tmp_raw_data = std::tmpnam(nullptr);
    std::string tmp_recon_data = std::tmpnam(nullptr);

    std::string simulator_path = util_path() + "utilities/ismrmrd_generate_cartesian_shepp_logan";
    std::system((simulator_path + " -o " + tmp_raw_data).c_str());
}

BOOST_AUTO_TEST_SUITE_END()

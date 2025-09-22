#include <boost/test/unit_test.hpp>
#include <boost/mpl/vector.hpp>
#include <fstream>
#include <sstream>

#include "ismrmrd/serialization.h"
#include "ismrmrd/serialization_iostream.h"

using namespace ISMRMRD;

BOOST_AUTO_TEST_SUITE(SerialiationTest)

// Some helpers
template <typename T>
T value_from_size_t(size_t i);
template <>
uint16_t value_from_size_t<uint16_t>(size_t i) { return (uint16_t)i; }
template <>
uint32_t value_from_size_t<uint32_t>(size_t i) { return (uint32_t)i; }
template <>
int16_t value_from_size_t<int16_t>(size_t i) { return (int16_t)i; }
template <>
int32_t value_from_size_t<int32_t>(size_t i) { return (int32_t)i; }
template <>
float value_from_size_t<float>(size_t i) { return (float)(i * 1.0f); }
template <>
double value_from_size_t<double>(size_t i) { return (double)(i * 1.0f); }
template <>
std::complex<float> value_from_size_t<std::complex<float> >(size_t i) { return std::complex<float>(1.0f * i, 1.0f * i); }
template <>
std::complex<double> value_from_size_t<std::complex<double> >(size_t i) { return std::complex<double>(1.0f * i, 1.0f * i); }

// Test the serialization of a single acquisition
BOOST_AUTO_TEST_CASE(test_acquisition_serialization) {
    Acquisition acq;
    acq.resize(256, 16, 3);

    // Fill in some data
    for (size_t i = 0; i < acq.getNumberOfDataElements(); i++) {
        acq.getDataPtr()[i] = value_from_size_t<std::complex<float> >(i);
    }

    // Fill trajectory
    for (size_t i = 0; i < acq.getNumberOfTrajElements(); i++) {
        acq.getTrajPtr()[i] = value_from_size_t<float>(i);
    }

    // Test serialization and deserialization
    std::stringstream ss(std::ios::in | std::ios::out | std::ios::binary);
    OStreamView ws(ss);
    IStreamView rs(ss);
    ISMRMRD::serialize(acq, ws);
    Acquisition acq2;
    ISMRMRD::deserialize(acq2, rs);

    // Check that the data is the same
    BOOST_ASSERT(acq.getHead() == acq2.getHead());
    BOOST_CHECK_EQUAL_COLLECTIONS(acq.getDataPtr(), acq.getDataPtr() + acq.getNumberOfDataElements(),
                                  acq2.getDataPtr(), acq2.getDataPtr() + acq2.getNumberOfDataElements());
    BOOST_CHECK_EQUAL_COLLECTIONS(acq.getTrajPtr(), acq.getTrajPtr() + acq.getNumberOfTrajElements(),
                                  acq2.getTrajPtr(), acq2.getTrajPtr() + acq2.getNumberOfTrajElements());
}

typedef boost::mpl::vector<unsigned short, short, unsigned int, int, float, double, std::complex<float>, std::complex<double> > image_types_w_tuples;

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
        img.getDataPtr()[i] = value_from_size_t<T>(i);
    }

    // set some meta data
    img.setAttributeString("This is my attribute string");

    // Test serialization and deserialization
    std::stringstream ss(std::ios::in | std::ios::out | std::ios::binary);
    OStreamView ws(ss);
    IStreamView rs(ss);
    ISMRMRD::serialize(img, ws);

    Image<T> img2;
    ISMRMRD::deserialize(img2, rs);

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
    std::stringstream ss(std::ios::in | std::ios::out | std::ios::binary);
    OStreamView ws(ss);
    IStreamView rs(ss);
    ISMRMRD::serialize(wf, ws);
    Waveform wf2;
    ISMRMRD::deserialize(wf2, rs);

    // Check that the data is the same
    BOOST_ASSERT(wf.head.time_stamp == wf2.head.time_stamp);
    BOOST_CHECK_EQUAL_COLLECTIONS(wf.begin_data(), wf.end_data(),
                                  wf2.begin_data(), wf2.end_data());
}

// Test serrialization of const length arrays
BOOST_AUTO_TEST_CASE(test_config_serialization) {
    ConfigFile cfg;

    strcpy(cfg.config, "default.xml");

    std::stringstream ss(std::ios::in | std::ios::out | std::ios::binary);
    OStreamView ws(ss);
    IStreamView rs(ss);

    ISMRMRD::serialize(cfg, ws);

    ConfigFile cfg2;
    ISMRMRD::deserialize(cfg2, rs);
    std::string str2(cfg2.config);
    BOOST_CHECK_EQUAL("default.xml", str2);
}

// Test string serialization
BOOST_AUTO_TEST_CASE(test_string_serialization) {
    std::string str("This can really be any string, including <XML> configuration and unicode characters: 你好");

    std::stringstream ss(std::ios::in | std::ios::out | std::ios::binary);
    OStreamView ws(ss);
    IStreamView rs(ss);

    ISMRMRD::serialize(str, ws);

    std::string str2;
    ISMRMRD::deserialize(str2, rs);
    BOOST_CHECK_EQUAL(str, str2);
}

// Some early iterations used explicit null terminations in strings,
// We are testing here that it doesn't trip us up
BOOST_AUTO_TEST_CASE(test_string_desiralization_with_null_terminators) {
    std::stringstream ss(std::ios::in | std::ios::out | std::ios::binary);

    std::vector<char> buffer;
    buffer.push_back('h');
    buffer.push_back('e');
    buffer.push_back('l');
    buffer.push_back('l');
    buffer.push_back('o');
    buffer.push_back('\0');

    // Write a config text manually
    uint32_t len = static_cast<uint32_t>(buffer.size());
    BOOST_CHECK_EQUAL(len, 6); // 6 characters with the null terminator
    ss.write(reinterpret_cast<char *>(&len), sizeof(uint32_t));
    ss.write(&buffer[0], len);

    IStreamView rs(ss);

    ConfigText cf;
    ISMRMRD::deserialize(cf.config_text, rs);

    BOOST_CHECK_EQUAL(cf.config_text.size(), 5); // String is 1 shorter
    BOOST_CHECK_EQUAL(cf.config_text, "hello");
}

typedef boost::mpl::vector<unsigned short, short, unsigned int, int, float, double, std::complex<float>, std::complex<double> > array_types_w_tuples;

// Test the serialization of a single image
BOOST_AUTO_TEST_CASE_TEMPLATE(test_nd_array_serialization, T, array_types_w_tuples) {

    for (uint16_t ndim=1; ndim<=ISMRMRD_NDARRAY_MAXDIM; ndim++)
    {
        std::vector<size_t> dims(ndim, 1);
        for (size_t i=0; i<ndim; i++) dims[i] = std::rand() % 8 + 1;
        NDArray<T> arr;
        arr.resize(dims);

        size_t N = arr.getNumberOfElements();
        T* p_data = arr.getDataPtr();
        for (size_t i=0; i<N; i++) p_data[i] = (T)(( (float)(std::rand()) / RAND_MAX -0.5f) * 2048.0f);

        // Test serialization and deserialization
        std::stringstream ss(std::ios::in | std::ios::out | std::ios::binary);
        OStreamView ws(ss);
        IStreamView rs(ss);
        ISMRMRD::serialize(arr, ws);

        NDArray<T> arr2;
        ISMRMRD::deserialize(arr2, rs);

        // Check that the data is the same
        BOOST_CHECK_EQUAL(arr.getNDim(), arr2.getNDim());
        BOOST_CHECK_EQUAL(arr.getVersion(), arr2.getVersion());
        BOOST_CHECK_EQUAL(arr.getDataType(), arr2.getDataType());
        BOOST_CHECK_EQUAL(arr.getDataSize(), arr2.getDataSize());
        BOOST_CHECK_EQUAL(arr.getNumberOfElements(), arr2.getNumberOfElements());

        BOOST_CHECK_EQUAL_COLLECTIONS(arr.getDataPtr(), arr.getDataPtr() + arr.getNumberOfElements(),
                                    arr2.getDataPtr(), arr2.getDataPtr() + arr2.getNumberOfElements());
    }
}

BOOST_AUTO_TEST_CASE(test_of_control_plane_protocol_serialization) {
    ConfigFile cfg;
    std::string config_name("test_config.xml");
    strcpy(cfg.config, config_name.c_str());

    ConfigText cfg_txt;
    cfg_txt.config_text = "{{ \"test\": \"test\" }}";

    TextMessage txt_msg;
    txt_msg.message = "This is a test";

    std::stringstream ss(std::ios::in | std::ios::out | std::ios::binary);
    OStreamView ws(ss);
    IStreamView rs(ss);

    ProtocolSerializer serializer(ws);
    ProtocolDeserializer deserializer(rs);

    serializer.serialize(cfg);
    serializer.serialize(cfg_txt);
    serializer.serialize(txt_msg);

    ConfigFile cfg2;
    deserializer.deserialize(cfg2);
    BOOST_CHECK_EQUAL(cfg2.config, config_name.c_str());

    ConfigText cfg_txt2;
    deserializer.deserialize(cfg_txt2);
    BOOST_CHECK_EQUAL(cfg_txt2.config_text, cfg_txt.config_text);

    TextMessage txt_msg2;
    deserializer.deserialize(txt_msg2);
    BOOST_CHECK_EQUAL(txt_msg2.message, txt_msg.message);
}

BOOST_AUTO_TEST_CASE(test_of_protocol_serialization) {

    // A header
    ISMRMRD::IsmrmrdHeader h;
    h.experimentalConditions.H1resonanceFrequency_Hz = 63500000;
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
        acq1.getDataPtr()[i] = value_from_size_t<std::complex<float> >(i);
        acq2.getDataPtr()[i] = value_from_size_t<std::complex<float> >(2 * i);
    }

    // Fill trajectory
    for (size_t i = 0; i < acq1.getNumberOfTrajElements(); i++) {
        acq1.getTrajPtr()[i] = value_from_size_t<float>(i);
        acq2.getTrajPtr()[i] = value_from_size_t<float>(2 * i);
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
        img.getDataPtr()[i] = value_from_size_t<float>(i);
    }

    // set some meta data
    img.setAttributeString("This is my attribute string");

    std::stringstream ss(std::ios::in | std::ios::out | std::ios::binary);

    OStreamView ws(ss);
    IStreamView rs(ss);

    ProtocolSerializer serializer(ws);
    ProtocolDeserializer deserializer(rs);

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
    BOOST_CHECK_EQUAL(h, h2);

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

BOOST_AUTO_TEST_SUITE_END()

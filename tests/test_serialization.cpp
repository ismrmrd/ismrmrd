#include <boost/test/unit_test.hpp>
#include <fstream>
#include <sstream>

#include "ismrmrd/dataset.h"
#include "ismrmrd/serialization.h"

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
    WritableStream ws(ss);
    ReadableStream rs(ss);
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
        img.getDataPtr()[i] = value_from_size_t<T>(i);
    }

    // set some meta data
    img.setAttributeString("This is my attribute string");

    // Test serialization and deserialization
    std::stringstream ss(std::ios::in | std::ios::out | std::ios::binary);
    WritableStream ws(ss);
    ReadableStream rs(ss);
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
    WritableStream ws(ss);
    ReadableStream rs(ss);
    ISMRMRD::serialize(wf, ws);
    Waveform wf2;
    ISMRMRD::deserialize(wf2, rs);

    // Check that the data is the same
    BOOST_ASSERT(wf.head.time_stamp == wf2.head.time_stamp);
    BOOST_CHECK_EQUAL_COLLECTIONS(wf.begin_data(), wf.end_data(),
                                  wf2.begin_data(), wf2.end_data());
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

    std::stringstream ss;
    WritableStream ws(ss);
    ReadableStream rs(ss);

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

std::string executable_name(std::string exename) {
#ifdef _MSC_VER /* MS compiler */
    exename += ".exe";
#endif
    return exename;
}

std::string path_separator() {
#ifdef _MSC_VER /* MS compiler */
    return "\\";
#else
    return "/";
#endif
}

std::string util_path() {
    // current directory
    std::string path = "." + path_separator();
    int level = 0;
    while (level < 3) {
        std::string testpath(path + executable_name("utilities" + path_separator() + "ismrmrd_generate_cartesian_shepp_logan"));
        std::ifstream f(testpath.c_str());
        if (f.good()) {
            return path;
        } else {
            path += ".." + path_separator();
            level++;
        }
    }
    throw std::runtime_error("Could not find utilities directory");
    return path;
}

std::string random_file_name(std::string prefix) {
    std::stringstream ss;
    ss << prefix << "_" << std::rand();
    return ss.str();
}

BOOST_AUTO_TEST_CASE(test_end_to_end_streaming_reconstruction) {
    std::string tmp_raw_data = random_file_name("." + path_separator() + "testdata") + ".h5";
    std::string tmp_raw_data_copy = random_file_name("." + path_separator() + "testdata_copy") + ".h5";
    std::string tmp_stream_recon_data = random_file_name("." + path_separator() + "stream_recon") + ".h5";
    std::string tmp_stream_recon_data_copy = random_file_name("." + path_separator() + "stream_recon_copy") + ".h5";

    std::string simulator_path = util_path() + executable_name("utilities" + path_separator() + "ismrmrd_generate_cartesian_shepp_logan");
    BOOST_CHECK_EQUAL(std::system((simulator_path + " -o " + tmp_raw_data).c_str()), 0);

    std::string recon_path = util_path() + executable_name("utilities" + path_separator() + "ismrmrd_recon_cartesian_2d");
    std::string stream_recon_path = util_path() + executable_name("utilities" + path_separator() + "ismrmrd_stream_recon_cartesian_2d");
    std::string stoh_path = util_path() + executable_name("utilities" + path_separator() + "ismrmrd_stream_to_hdf5");
    std::string htos_path = util_path() + executable_name("utilities" + path_separator() + "ismrmrd_hdf5_to_stream");

    // Use a scope here to make sure we close the files (which will be needed later)
    {
        // Stream raw data to another raw data file
        std::string cmd = htos_path + " -i " + tmp_raw_data + " | " + stoh_path + " -o " + tmp_raw_data_copy;
        BOOST_CHECK_EQUAL(std::system(cmd.c_str()), 0);

        // Open dataset
        Dataset d(tmp_raw_data.c_str(), "dataset", false);
        Dataset d_copy(tmp_raw_data_copy.c_str(), "dataset", false);

        std::string header_str;
        IsmrmrdHeader h;
        d.readHeader(header_str);
        deserialize(header_str.c_str(), h);

        std::string header_str_copy;
        IsmrmrdHeader h_copy;
        d_copy.readHeader(header_str_copy);
        deserialize(header_str_copy.c_str(), h_copy);

        BOOST_CHECK_EQUAL(h, h_copy);
        BOOST_CHECK_EQUAL(d.getNumberOfAcquisitions(), d_copy.getNumberOfAcquisitions());

        for (uint32_t i = 0; i < d.getNumberOfAcquisitions(); i++) {
            Acquisition acq;
            Acquisition acq_copy;
            d.readAcquisition(i, acq);
            d_copy.readAcquisition(i, acq_copy);

            BOOST_ASSERT(acq.getHead() == acq_copy.getHead());
            BOOST_CHECK_EQUAL_COLLECTIONS(acq.getDataPtr(), acq.getDataPtr() + acq.getNumberOfDataElements(),
                                          acq_copy.getDataPtr(), acq_copy.getDataPtr() + acq_copy.getNumberOfDataElements());
            BOOST_CHECK_EQUAL_COLLECTIONS(acq.getTrajPtr(), acq.getTrajPtr() + acq.getNumberOfTrajElements(),
                                          acq_copy.getTrajPtr(), acq_copy.getTrajPtr() + acq_copy.getNumberOfTrajElements());
        }
    }

    std::string stream_recon_cmd = htos_path + " -i " + tmp_raw_data + " | " + stream_recon_path + " | " + stoh_path + " -o " + tmp_stream_recon_data;
    BOOST_CHECK_EQUAL(std::system(stream_recon_cmd.c_str()), 0);

    std::string recon_cmd = recon_path + " " + tmp_raw_data;
    BOOST_CHECK_EQUAL(std::system(recon_cmd.c_str()), 0);

    std::string recon_copy_cmd = htos_path + " -i " + tmp_stream_recon_data + " --image-series image_0 | " + stoh_path + " -o " + tmp_stream_recon_data_copy;
    BOOST_CHECK_EQUAL(std::system(recon_copy_cmd.c_str()), 0);

    Dataset d(tmp_raw_data.c_str(), "dataset", false);
    Dataset d_recon(tmp_stream_recon_data.c_str(), "dataset", false);
    Dataset d_recon_copy(tmp_stream_recon_data_copy.c_str(), "dataset", false);

    Image<float> cpp_recon;
    d.readImage("cpp", 0, cpp_recon);

    Image<float> cpp_stream_recon;
    d_recon.readImage("image_0", 0, cpp_stream_recon);

    Image<float> cpp_stream_recon_copy;
    d_recon_copy.readImage("image_0", 0, cpp_stream_recon_copy);

    // The 3 images should be the same
    BOOST_CHECK_EQUAL_COLLECTIONS(cpp_recon.getDataPtr(), cpp_recon.getDataPtr() + cpp_recon.getNumberOfDataElements(),
                                  cpp_stream_recon.getDataPtr(), cpp_stream_recon.getDataPtr() + cpp_stream_recon.getNumberOfDataElements());
    BOOST_CHECK_EQUAL_COLLECTIONS(cpp_recon.getDataPtr(), cpp_recon.getDataPtr() + cpp_recon.getNumberOfDataElements(),
                                  cpp_stream_recon_copy.getDataPtr(), cpp_stream_recon_copy.getDataPtr() + cpp_stream_recon_copy.getNumberOfDataElements());

    // delete raw data file
    BOOST_CHECK_EQUAL(std::remove(tmp_stream_recon_data.c_str()), 0);
    BOOST_CHECK_EQUAL(std::remove(tmp_stream_recon_data_copy.c_str()), 0);
    BOOST_CHECK_EQUAL(std::remove(tmp_raw_data.c_str()), 0);
    BOOST_CHECK_EQUAL(std::remove(tmp_raw_data_copy.c_str()), 0);
}

BOOST_AUTO_TEST_SUITE_END()

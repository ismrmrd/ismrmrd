#include <fstream>
#include <boost/test/unit_test.hpp>

#include "ismrmrd/dataset.h"
#include "ismrmrd/serialization.h"
#include "ismrmrd/serialization_iostream.h"

using namespace ISMRMRD;

BOOST_AUTO_TEST_SUITE(EndToEndTest)

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

void test_end_to_end_streaming_reconstruction(bool use_binary_files) {
    std::string tmp_raw_data = random_file_name("." + path_separator() + "testdata") + ".h5";
    std::string tmp_raw_data_stream1 = random_file_name("." + path_separator() + "stream_testdata1") + ".bin";
    std::string tmp_raw_data_stream2 = random_file_name("." + path_separator() + "stream_testdata2") + ".bin";
    std::string tmp_recon_data_stream1 = random_file_name("." + path_separator() + "stream_recon1") + ".bin";
    std::string tmp_recon_data_stream2 = random_file_name("." + path_separator() + "stream_recon2") + ".bin";
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
        if (use_binary_files) {
            std::string cmd = htos_path + " -i " + tmp_raw_data + " -o " + tmp_raw_data_stream1;
            BOOST_CHECK_EQUAL(std::system(cmd.c_str()), 0);
            cmd = stoh_path + " -i " + tmp_raw_data_stream1 + " -o " + tmp_raw_data_copy;
            BOOST_CHECK_EQUAL(std::system(cmd.c_str()), 0);
        } else {
            std::string cmd = htos_path + " -i " + tmp_raw_data + " --use-stdout | " + stoh_path + " --use-stdin -o " + tmp_raw_data_copy;
            BOOST_CHECK_EQUAL(std::system(cmd.c_str()), 0);
        }
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

    { // Scope to make sure files get closed.
        if (use_binary_files) {
            std::string cmd = htos_path + " -i " + tmp_raw_data + " -o " + tmp_raw_data_stream2;
            BOOST_CHECK_EQUAL(std::system(cmd.c_str()), 0);
            cmd = stream_recon_path + " --output-magnitude -i " + tmp_raw_data_stream2 + " -o " + tmp_recon_data_stream1;
            BOOST_CHECK_EQUAL(std::system(cmd.c_str()), 0);
            cmd = stoh_path + " -i " + tmp_recon_data_stream1 + " -o " + tmp_stream_recon_data;
            BOOST_CHECK_EQUAL(std::system(cmd.c_str()), 0);
        } else {
            std::string stream_recon_cmd = htos_path + " -i " + tmp_raw_data + " --use-stdout | " + stream_recon_path + " --output-magnitude --use-stdin --use-stdout | " + stoh_path + " --use-stdin -o " + tmp_stream_recon_data;
            BOOST_CHECK_EQUAL(std::system(stream_recon_cmd.c_str()), 0);
        }

        std::string recon_cmd = recon_path + " " + tmp_raw_data;
        BOOST_CHECK_EQUAL(std::system(recon_cmd.c_str()), 0);

        if (use_binary_files) {
            std::string cmd = htos_path + " --image-series image_0  -i " + tmp_stream_recon_data + " -o " + tmp_recon_data_stream2;
            BOOST_CHECK_EQUAL(std::system(cmd.c_str()), 0);
            cmd = stoh_path + " -i " + tmp_recon_data_stream2 + " -o " + tmp_stream_recon_data_copy;
            BOOST_CHECK_EQUAL(std::system(cmd.c_str()), 0);

        } else {
            std::string recon_copy_cmd = htos_path + " -i " + tmp_stream_recon_data + " --image-series image_0 --use-stdout | " + stoh_path + " --use-stdin -o " + tmp_stream_recon_data_copy;
            BOOST_CHECK_EQUAL(std::system(recon_copy_cmd.c_str()), 0);
        }

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
        size_t elements = cpp_recon.getNumberOfDataElements();
        for (size_t i = 0; i < elements; i++) {
            BOOST_CHECK_CLOSE(cpp_recon.getDataPtr()[i], cpp_stream_recon.getDataPtr()[i], 1e-4);
            BOOST_CHECK_CLOSE(cpp_recon.getDataPtr()[i], cpp_stream_recon_copy.getDataPtr()[i], 1e-4);
        }
    }

    // delete temporary files
    if (use_binary_files) {
        BOOST_CHECK_EQUAL(std::remove(tmp_raw_data_stream1.c_str()), 0);
    }
    BOOST_CHECK_EQUAL(std::remove(tmp_stream_recon_data.c_str()), 0);
    BOOST_CHECK_EQUAL(std::remove(tmp_stream_recon_data_copy.c_str()), 0);
    BOOST_CHECK_EQUAL(std::remove(tmp_raw_data.c_str()), 0);
    BOOST_CHECK_EQUAL(std::remove(tmp_raw_data_copy.c_str()), 0);
}

BOOST_AUTO_TEST_CASE(test_end_to_end_streaming_reconstruction_pipes) {
    test_end_to_end_streaming_reconstruction(false);
}

BOOST_AUTO_TEST_CASE(test_end_to_end_streaming_reconstruction_files) {
    test_end_to_end_streaming_reconstruction(true);
}

BOOST_AUTO_TEST_SUITE_END()

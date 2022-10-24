#include "ismrmrd/dataset.h"
#include "ismrmrd/serialization_iostream.h"
#include "ismrmrd_io_utils.h"
#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>

namespace po = boost::program_options;

template <typename T>
std::string create_image_series_name(const ISMRMRD::Image<T> &img) {
    std::stringstream ss;
    ss << "image_" << img.getHead().image_series_index;
    return ss.str();
}

void convert_stream_to_hdf5(std::string output_file, std::string groupname, std::istream &is) {
    ISMRMRD::Dataset d(output_file.c_str(), groupname.c_str(), true);

    ISMRMRD::IStreamView rs(is);
    ISMRMRD::ProtocolDeserializer deserializer(rs);

    // Some reconstructions return the header but it is not required.
    if (deserializer.peek() == ISMRMRD::ISMRMRD_MESSAGE_HEADER) {
        ISMRMRD::IsmrmrdHeader hdr;
        deserializer.deserialize(hdr);

        // We will convert the XML header to a string and write it to the HDF5 file
        std::stringstream xmlstream;
        ISMRMRD::serialize(hdr, xmlstream);
        d.writeHeader(xmlstream.str());
    }

    while (deserializer.peek() != ISMRMRD::ISMRMRD_MESSAGE_CLOSE) {
        if (deserializer.peek() == ISMRMRD::ISMRMRD_MESSAGE_ACQUISITION) {
            ISMRMRD::Acquisition acq;
            deserializer.deserialize(acq);
            d.appendAcquisition(acq);
        } else if (deserializer.peek() == ISMRMRD::ISMRMRD_MESSAGE_IMAGE) {
            if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_USHORT) {
                ISMRMRD::Image<unsigned short> img;
                deserializer.deserialize(img);
                d.appendImage(create_image_series_name(img), img);
            } else if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_SHORT) {
                ISMRMRD::Image<short> img;
                deserializer.deserialize(img);
                d.appendImage(create_image_series_name(img), img);
            } else if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_UINT) {
                ISMRMRD::Image<unsigned int> img;
                deserializer.deserialize(img);
                d.appendImage(create_image_series_name(img), img);
            } else if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_INT) {
                ISMRMRD::Image<int> img;
                deserializer.deserialize(img);
                d.appendImage(create_image_series_name(img), img);
            } else if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_FLOAT) {
                ISMRMRD::Image<float> img;
                deserializer.deserialize(img);
                d.appendImage(create_image_series_name(img), img);
            } else if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_DOUBLE) {
                ISMRMRD::Image<double> img;
                deserializer.deserialize(img);
                d.appendImage(create_image_series_name(img), img);
            } else if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_CXFLOAT) {
                ISMRMRD::Image<std::complex<float> > img;
                deserializer.deserialize(img);
                d.appendImage(create_image_series_name(img), img);
            } else if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_CXDOUBLE) {
                ISMRMRD::Image<std::complex<double> > img;
                deserializer.deserialize(img);
                d.appendImage(create_image_series_name(img), img);
            } else {
                throw std::runtime_error("Unknown image type");
            }
        } else if (deserializer.peek() == ISMRMRD::ISMRMRD_MESSAGE_WAVEFORM) {
            ISMRMRD::Waveform wfm;
            deserializer.deserialize(wfm);
            d.appendWaveform(wfm);
        } else if (deserializer.peek() == ISMRMRD::ISMRMRD_MESSAGE_TEXT) {
            ISMRMRD::TextMessage txt;
            deserializer.deserialize(txt);
            std::cerr << "TEXT MESSAGE: " << txt.message << std::endl;
        } else {
            std::stringstream ss;
            ss << "Unknown message type " << deserializer.peek();
            throw std::runtime_error(ss.str());
        }
    }

    // If we can read any more at this point, it is an error
    if (is.get() != EOF) {
        throw std::runtime_error("Extra data after ISMRMRD_CLOSE");
    }
}

int main(int argc, char **argv) {
    // Arguments
    std::string input_file = "";
    std::string output_file;
    std::string groupname;
    bool use_stdin = false;

    // Parse arguments using boost program options
    po::options_description desc("Allowed options");

    // clang-format off
    desc.add_options()
        ("help,h", "produce help message")
        ("input,i", po::value<std::string>(&input_file),"Binary input file")
        ("output,o", po::value<std::string>(&output_file)->required(),"ISMRMRD HDF5 output file")
        ("use-stdin", po::bool_switch(&use_stdin), "Use stdout for output")
        ("group,g", po::value<std::string>(&groupname)->default_value("dataset"), "group name");
    // clang-format on

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (boost::wrapexcept<po::required_option> &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << desc << std::endl;
        return 1;
    }

    if (vm.count("input") && use_stdin) {
        std::cerr << "Error: Cannot specify both output file and use-stdout" << std::endl;
        return 1;
    }

    if (vm.count("help")) {
        std::cerr << desc << "\n";
        return 1;
    }

    if (vm.count("input")) {
        std::ifstream is(input_file.c_str(), std::ios::binary);
        if (!is) {
            std::cerr << "Error: Could not open input file " << input_file << std::endl;
            return 1;
        }
        convert_stream_to_hdf5(output_file, groupname, is);
    } else if (use_stdin) {
        ISMRMRD::set_binary_io();
        convert_stream_to_hdf5(output_file, groupname, std::cin);
    } else {
        std::cerr << "Error: Must specify either input file or use-stdin" << std::endl;
        return 1;
    }

    return 0;
}

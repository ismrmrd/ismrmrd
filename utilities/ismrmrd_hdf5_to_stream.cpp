#include "ismrmrd/dataset.h"
#include "ismrmrd/serialization.h"
#include "ismrmrd/serialization_iostream.h"
#include "ismrmrd_io_utils.h"

#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>
#include <string>

namespace po = boost::program_options;

void serialize_to_stream(const std::string &input_file, const std::string &groupname, const std::vector<std::string> &image_series, std::ostream &os, std::string config_file, std::string config_text) {
    ISMRMRD::Dataset d(input_file.c_str(), groupname.c_str(), false);
    ISMRMRD::OStreamView ws(os);
    ISMRMRD::ProtocolSerializer serializer(ws);

    if (config_file.size()) {
        ISMRMRD::ConfigFile cfg;
        strcpy(cfg.config, config_file.c_str());
        serializer.serialize(cfg);
    }

    if (config_text.size()) {
        ISMRMRD::ConfigText cfg;
        cfg.config_text = config_text;
        serializer.serialize(cfg);
    }

    try {
        std::string xml;
        d.readHeader(xml);
        ISMRMRD::IsmrmrdHeader hdr;
        ISMRMRD::deserialize(xml.c_str(), hdr);
        serializer.serialize(hdr);
    } catch (std::exception &) {
        if (image_series.empty()) {
            // rethrow exception if image series not specified
            throw;
        }
    }

    // If user has asked for image series, we will only send images,
    // otherwise we will send acquisitions and waveforms.
    if (!image_series.empty()) {
        for (size_t s = 0; s < image_series.size(); s++) {
            unsigned int number_of_images = d.getNumberOfImages(image_series[s].c_str());
            for (unsigned int i = 0; i < number_of_images; i++) {
                // Danger: Here we will take advantage of the fact that the
                // C++ wrapper calls ismrmrd_read_image (C API) without checking
                // that the type is correct. This is a bug in the C++ wrapper.
                ISMRMRD::Image<complex_double_t> img;
                d.readImage(image_series[s].c_str(), i, img);

                if (img.getHead().data_type == ISMRMRD::ISMRMRD_USHORT) {
                    ISMRMRD::Image<unsigned short> *img_ushort = reinterpret_cast<ISMRMRD::Image<unsigned short> *>(&img);
                    serializer.serialize(*img_ushort);
                } else if (img.getHead().data_type == ISMRMRD::ISMRMRD_SHORT) {
                    ISMRMRD::Image<short> *img_short = reinterpret_cast<ISMRMRD::Image<short> *>(&img);
                    serializer.serialize(*img_short);
                } else if (img.getHead().data_type == ISMRMRD::ISMRMRD_UINT) {
                    ISMRMRD::Image<unsigned int> *img_uint = reinterpret_cast<ISMRMRD::Image<unsigned int> *>(&img);
                    serializer.serialize(*img_uint);
                } else if (img.getHead().data_type == ISMRMRD::ISMRMRD_INT) {
                    ISMRMRD::Image<int> *img_int = reinterpret_cast<ISMRMRD::Image<int> *>(&img);
                    serializer.serialize(*img_int);
                } else if (img.getHead().data_type == ISMRMRD::ISMRMRD_FLOAT) {
                    ISMRMRD::Image<float> *img_float = reinterpret_cast<ISMRMRD::Image<float> *>(&img);
                    serializer.serialize(*img_float);
                } else if (img.getHead().data_type == ISMRMRD::ISMRMRD_DOUBLE) {
                    ISMRMRD::Image<double> *img_double = reinterpret_cast<ISMRMRD::Image<double> *>(&img);
                    serializer.serialize(*img_double);
                } else if (img.getHead().data_type == ISMRMRD::ISMRMRD_CXFLOAT) {
                    ISMRMRD::Image<complex_float_t> *img_cxfloat = reinterpret_cast<ISMRMRD::Image<complex_float_t> *>(&img);
                    serializer.serialize(*img_cxfloat);
                } else if (img.getHead().data_type == ISMRMRD::ISMRMRD_CXDOUBLE) {
                    serializer.serialize(img);
                } else {
                    throw std::runtime_error("Unknown data type");
                }
            }
        }
    } else {
        // Here we implement the pattern of merge sorting acquisitions and waveforms based on timestamp.
        // For each dataset of waveforms and acquisitions, we will assume that they are sorted by timestamp.
        unsigned int number_of_acquisitions = d.getNumberOfAcquisitions();
        unsigned int number_of_waveforms = d.getNumberOfWaveforms();
        unsigned int a = 0, a_fetched = number_of_acquisitions, w = 0, w_fetched = number_of_waveforms;
        ISMRMRD::Acquisition acq;
        ISMRMRD::Waveform wfm;
        while (a < number_of_acquisitions || w < number_of_waveforms) {
            if (a < number_of_acquisitions && a_fetched != a) {
                d.readAcquisition(a, acq);
                a_fetched = a;
            }
            if (w < number_of_waveforms && w_fetched != w) {
                d.readWaveform(w, wfm);
                w_fetched = w;
            }

            // If we have both acquisitions and waveforms, advance the one with the
            // smaller timestamp, otherwise advance the one that exists
            if (a < number_of_acquisitions && w < number_of_waveforms) {
                if (acq.getHead().acquisition_time_stamp < wfm.head.time_stamp) {
                    serializer.serialize(acq);
                    a++;
                } else {
                    serializer.serialize(wfm);
                    w++;
                }
            } else if (a < number_of_acquisitions) {
                serializer.serialize(acq);
                a++;
            } else {
                serializer.serialize(wfm);
                w++;
            }
        }
    }
    serializer.close();
}

int main(int argc, char **argv) {

    // Parse arguments using boost program options
    po::options_description desc("Allowed options");

    // Arguments
    std::string config_file = "";
    std::string local_config_file = "";
    std::string config_text = "";
    std::string input_file;
    std::string output_file = "";
    bool use_stdout = false;
    std::vector<std::string> image_series;
    std::string groupname;

    // clang-format off
    desc.add_options()
        ("help,h", "produce help message")
        ("input,i", po::value<std::string>(&input_file)->required(),"input ISMRMRD HDF5 file")
        ("output,o", po::value<std::string>(&output_file),"Binary output file")
        ("use-stdout", po::bool_switch(&use_stdout), "Use stdout for output")
        ("group,g", po::value<std::string>(&groupname)->default_value("dataset"), "group name")
        ("image-series,s", po::value<std::vector<std::string>>(&image_series)->multitoken(), "image series to extract")
        ("config-file,c", po::value<std::string>(&config_file), "Configuration name (aka config file)")
        ("local-config-file,C", po::value<std::string>(&local_config_file), "Configuration text file");
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

    if (vm.count("help")) {
        std::cerr << desc << "\n";
        return 1;
    }

    if (vm.count("output") && use_stdout) {
        std::cerr << "Error: Cannot specify both output file and use-stdout" << std::endl;
        return 1;
    }

    if (vm.count("config-file") && vm.count("local-config-file")) {
        std::cerr << "Error: Cannot specify both config-name and config-text-file" << std::endl;
        return 1;
    }

    // Read config text file into string
    if (vm.count("local-config-file")) {
        std::ifstream f(local_config_file);
        std::stringstream buffer;
        buffer << f.rdbuf();
        config_text = buffer.str();
    }

    if (use_stdout) {
        ISMRMRD::set_binary_io();
        serialize_to_stream(input_file, groupname, image_series, std::cout, config_file, config_text);
    } else if (output_file != "") {
        std::ofstream out(output_file, std::ios::out | std::ios::binary);
        serialize_to_stream(input_file, groupname, image_series, out, config_file, config_text);
    } else {
        std::cerr << "Error: Must specify either output file or use-stdout" << std::endl;
        return 1;
    }

    return 0;
}

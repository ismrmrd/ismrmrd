#include "fftw3.h"
#include "ismrmrd/meta.h"
#include "ismrmrd/serialization_iostream.h"
#include "ismrmrd_io_utils.h"
#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>

namespace po = boost::program_options;

// Helper function for the FFTW library
void circshift(complex_float_t *out, const complex_float_t *in, int xdim, int ydim, int xshift, int yshift) {
    for (int i = 0; i < ydim; i++) {
        int ii = (i + yshift) % ydim;
        for (int j = 0; j < xdim; j++) {
            int jj = (j + xshift) % xdim;
            out[ii * xdim + jj] = in[i * xdim + j];
        }
    }
}

#define fftshift(out, in, x, y) circshift(out, in, x, y, (x / 2), (y / 2))

void reconstruct(std::istream &in, std::ostream &out, bool magnitude = false) {
    ISMRMRD::IStreamView rs(in);
    ISMRMRD::OStreamView ws(out);

    ISMRMRD::ProtocolDeserializer deserializer(rs);
    ISMRMRD::ProtocolSerializer serializer(ws);

    if (deserializer.peek() == ISMRMRD::ISMRMRD_MESSAGE_CONFIG_FILE) {
        ISMRMRD::ConfigFile cfg;
        deserializer.deserialize(cfg);
        std::string config_name(cfg.config);
        std::cerr << "Reconstruction received config file: " << config_name << std::endl;
        std::cerr << "Configuration file is ignored in this sample reconstruction" << std::endl;
    }

    if (deserializer.peek() == ISMRMRD::ISMRMRD_MESSAGE_TEXT) {
        ISMRMRD::TextMessage msg;
        deserializer.deserialize(msg);
        std::cerr << "Reconstruction received text message prior to config: " << std::endl
                  << msg.message << std::endl;
    }

    ISMRMRD::IsmrmrdHeader hdr;
    deserializer.deserialize(hdr);

    if (hdr.encoding.size() != 1) {
        throw std::runtime_error("This simple reconstruction application only supports one encoding space");
    }

    ISMRMRD::EncodingSpace e_space = hdr.encoding[0].encodedSpace;
    ISMRMRD::EncodingSpace r_space = hdr.encoding[0].reconSpace;

    if (e_space.matrixSize.z != 1) {
        throw std::runtime_error("This simple reconstruction application only supports 2D encoding spaces");
    }

    uint16_t nX = e_space.matrixSize.x;
    uint16_t nY = e_space.matrixSize.y;
    uint16_t nCoils = 0;
    ISMRMRD::NDArray<complex_float_t> buffer;
    ISMRMRD::AcquisitionHeader acqhdr;
    while (std::cin) {
        ISMRMRD::Acquisition acq;
        try {
            deserializer.deserialize(acq);
        } catch (ISMRMRD::ProtocolStreamClosed &) {
            break;
        }

        if (!nCoils) {
            nCoils = acq.active_channels();
            acqhdr = acq.getHead();

            // Allocate a buffer for the data
            std::vector<size_t> dims;
            dims.push_back(nX);
            dims.push_back(nY);
            dims.push_back(nCoils);
            buffer = ISMRMRD::NDArray<complex_float_t>(dims);
            std::fill(buffer.begin(), buffer.end(), complex_float_t(0.0f, 0.0f));
        }

        for (uint16_t c = 0; c < nCoils; c++) {
            memcpy(&buffer(0, acq.idx().kspace_encode_step_1, c), &acq.data(0, c), sizeof(complex_float_t) * nX);
        }
    }

    for (uint16_t c = 0; c < nCoils; c++) {
        fftwf_complex *tmp = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex) * (nX * nY));
        if (!tmp) {
            throw std::runtime_error("Error allocating temporary storage for FFTW");
        }
        fftwf_plan p = fftwf_plan_dft_2d(nY, nX, tmp, tmp, FFTW_BACKWARD, FFTW_ESTIMATE);
        fftshift(reinterpret_cast<complex_float_t *>(tmp), &buffer(0, 0, c), nX, nY);
        fftwf_execute(p);
        fftshift(&buffer(0, 0, c), reinterpret_cast<std::complex<float> *>(tmp), nX, nY);
        fftwf_destroy_plan(p);
        fftwf_free(tmp);
    }

    // Allocate an image
    ISMRMRD::Image<std::complex<float> > img_out(r_space.matrixSize.x, r_space.matrixSize.y, 1, 1);
    std::fill(img_out.begin(), img_out.end(), std::complex<float>(0.0f, 0.0f));

    // if there is oversampling in the readout direction remove it
    uint16_t offset = ((e_space.matrixSize.x - r_space.matrixSize.x) >> 1);

    // Take the sqrt of the sum of squares
    for (uint16_t y = 0; y < r_space.matrixSize.y; y++) {
        for (uint16_t x = 0; x < r_space.matrixSize.x; x++) {
            float mag = 0;
            float phase = 0;
            for (uint16_t c = 0; c < nCoils; c++) {
                std::complex<float> val = buffer(x + offset, y, c);
                float w = std::abs(val) * std::abs(val);
                mag += w;
                phase += std::arg(val) * w;
            }
            img_out(x, y) = std::polar<float>(std::sqrt(mag), phase / mag);
        }
    }

    img_out.setImageType(ISMRMRD::ISMRMRD_IMTYPE_COMPLEX);
    img_out.setSlice(0);
    img_out.setFieldOfView(r_space.fieldOfView_mm.x, r_space.fieldOfView_mm.y, r_space.fieldOfView_mm.z);
    img_out.setPosition(acqhdr.position[0], acqhdr.position[1], acqhdr.position[2]);
    img_out.setPatientTablePosition(acqhdr.patient_table_position[0], acqhdr.patient_table_position[1], acqhdr.patient_table_position[2]);
    img_out.setReadDirection(acqhdr.read_dir[0], acqhdr.read_dir[1], acqhdr.read_dir[2]);
    img_out.setPhaseDirection(acqhdr.phase_dir[0], acqhdr.phase_dir[1], acqhdr.phase_dir[2]);
    img_out.setSliceDirection(acqhdr.slice_dir[0], acqhdr.slice_dir[1], acqhdr.slice_dir[2]);

    ISMRMRD::MetaContainer meta;
    meta.append("ImageRowDir", acqhdr.read_dir[0]);
    meta.append("ImageRowDir", acqhdr.read_dir[1]);
    meta.append("ImageRowDir", acqhdr.read_dir[2]);

    meta.append("ImageColumnDir", acqhdr.phase_dir[0]);
    meta.append("ImageColumnDir", acqhdr.phase_dir[1]);
    meta.append("ImageColumnDir", acqhdr.phase_dir[2]);

    std::stringstream meta_string_stream;
    ISMRMRD::serialize(meta, meta_string_stream);
    img_out.setAttributeString(meta_string_stream.str().c_str());

    // If this we want magnitude:
    if (magnitude) {
        ISMRMRD::Image<float> float_img_out(img_out.getMatrixSizeX(), img_out.getMatrixSizeY(), 1, 1);
        auto head = img_out.getHead();
        head.image_type = ISMRMRD::ISMRMRD_IMTYPE_MAGNITUDE;
        head.data_type = ISMRMRD::ISMRMRD_FLOAT;
        float_img_out.setHead(head);
        float_img_out.setAttributeString(meta_string_stream.str().c_str());
        std::transform(img_out.begin(), img_out.end(), float_img_out.begin(), [](std::complex<float> val) { return std::abs(val); });
        serializer.serialize(float_img_out);
    } else {
        serializer.serialize(img_out);
    }

    serializer.close();
}

int main(int argc, char **argv) {
    // Parse arguments using boost program options
    po::options_description desc("Allowed options");

    // Arguments
    std::string input_file = "";
    std::string output_file = "";
    bool use_stdin = false;
    bool use_stdout = false;
    bool output_magnitude = false; // Default output images is complex float
    // clang-format off
    desc.add_options()
        ("help,h", "produce help message")
        ("input,i", po::value<std::string>(&input_file),"input ISMRMRD HDF5 file")
        ("output,o", po::value<std::string>(&output_file),"Binary output file")
        ("use-stdout", po::bool_switch(&use_stdout), "Use stdout for output")
        ("use-stdin", po::bool_switch(&use_stdin), "Use stdout for output")
        ("output-magnitude", po::bool_switch(&output_magnitude), "Output magnitude images");
    // clang-format on

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cerr << desc << "\n";
        return 1;
    }

    if (vm.count("input") && use_stdin) {
        std::cerr << "Error: Cannot specify both input file and use-stdin" << std::endl;
        return 1;
    }

    if (vm.count("output") && use_stdout) {
        std::cerr << "Error: Cannot specify both output file and use-stdout" << std::endl;
        return 1;
    }

    if (use_stdin && use_stdout) {
        ISMRMRD::set_binary_io();
        reconstruct(std::cin, std::cout, output_magnitude);
    } else if (input_file.size() && output_file.size()) {
        std::ifstream input(input_file.c_str(), std::ios::in | std::ios::binary);
        std::ofstream output(output_file.c_str(), std::ios::out | std::ios::binary);
        reconstruct(input, output, output_magnitude);
    } else if (input_file.size() && use_stdout) {
        ISMRMRD::set_binary_io();
        std::ifstream input(input_file.c_str(), std::ios::in | std::ios::binary);
        reconstruct(input, std::cout, output_magnitude);
    } else if (output_file.size() && use_stdin) {
        ISMRMRD::set_binary_io();
        std::ofstream output(output_file.c_str(), std::ios::out | std::ios::binary);
        reconstruct(std::cin, output, output_magnitude);
    } else {
        std::cerr << "Error: Must specify either input file and output file or use-stdin and use-stdout" << std::endl;
        return 1;
    }

    return 0;
}

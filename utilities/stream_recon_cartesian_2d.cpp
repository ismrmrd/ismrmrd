#include "fftw3.h"
#include "ismrmrd/serialization.h"
#include <iostream>

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

int main() {
    ISMRMRD::ProtocolDeserializer deserializer(std::cin);
    ISMRMRD::ProtocolSerializer serializer(std::cout);

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
    while (std::cin) {
        ISMRMRD::Acquisition acq;
        try {
            deserializer.deserialize(acq);
        } catch (ISMRMRD::ProtocolStreamClosed &) {
            break;
        }

        if (!nCoils) {
            nCoils = acq.active_channels();

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
            std::cerr << "Error allocating temporary storage for FFTW" << std::endl;
            return -1;
        }
        fftwf_plan p = fftwf_plan_dft_2d(nY, nX, tmp, tmp, FFTW_BACKWARD, FFTW_ESTIMATE);
        fftshift(reinterpret_cast<complex_float_t *>(tmp), &buffer(0, 0, c), nX, nY);
        fftwf_execute(p);
        fftshift(&buffer(0, 0, c), reinterpret_cast<std::complex<float> *>(tmp), nX, nY);
        fftwf_destroy_plan(p);
        fftwf_free(tmp);
    }

    // Allocate an image
    ISMRMRD::Image<float> img_out(r_space.matrixSize.x, r_space.matrixSize.y, 1, 1);
    std::fill(img_out.begin(), img_out.end(), 0.0f);

    // if there is oversampling in the readout direction remove it
    uint16_t offset = ((e_space.matrixSize.x - r_space.matrixSize.x) >> 1);

    // Take the sqrt of the sum of squares
    for (uint16_t y = 0; y < r_space.matrixSize.y; y++) {
        for (uint16_t x = 0; x < r_space.matrixSize.x; x++) {
            for (uint16_t c = 0; c < nCoils; c++) {
                img_out(x, y) += (std::abs(buffer(x + offset, y, c))) * (std::abs(buffer(x + offset, y, c)));
            }
            img_out(x, y) = std::sqrt(img_out(x, y));
        }
    }

    img_out.setImageType(ISMRMRD::ISMRMRD_IMTYPE_MAGNITUDE);
    img_out.setSlice(0);
    img_out.setFieldOfView(r_space.fieldOfView_mm.x, r_space.fieldOfView_mm.y, r_space.fieldOfView_mm.z);

    serializer.serialize(hdr);
    serializer.serialize(img_out);
    serializer.close();
    return 0;
}

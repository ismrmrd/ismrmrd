/*
 * test_recon_dataset.cpp
 *
 *  Created on: Sep 6, 2012
 *      Author: Michael S. Hansen (michael.hansen@nih.gov)
 *
 */

#include <iostream>
#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/dataset.h"
#include "ismrmrd/xml.h"
#include "fftw3.h"

//Helper function for the FFTW library
void circshift(complex_float_t *out, const complex_float_t *in, int xdim, int ydim, int xshift, int yshift)
{
    for (int i =0; i < ydim; i++) {
        int ii = (i + yshift) % ydim;
        for (int j = 0; j < xdim; j++) {
            int jj = (j + xshift) % xdim;
            out[ii * xdim + jj] = in[i * xdim + j];
        }
    }
}

#define fftshift(out, in, x, y) circshift(out, in, x, y, (x/2), (y/2))

void print_usage(const char* application)
{
    std::cout << "Usage:" << std::endl;
    std::cout << "  - " << application << " <HDF5_FILENAME> " << std::endl;
}

/* MAIN APPLICATION */
int main(int argc, char** argv)
{
    if (argc < 2) {
        print_usage(argv[0]);
        return -1;
    }

    std::string datafile(argv[1]);

    std::cout << "Simple ISMRMRD Reconstruction program" << std::endl;
    std::cout << "   - filename: " << datafile << std::endl;

    //Let's open the existing dataset
    ISMRMRD::Dataset d(datafile.c_str(),"dataset", false);

    std::string xml;
    d.readHeader(xml);
    ISMRMRD::IsmrmrdHeader hdr;
    ISMRMRD::deserialize(xml.c_str(),hdr);

    //Let's print some information from the header
    if (hdr.encoding.size() != 1) {
        std::cout << "Number of encoding spaces: " << hdr.encoding.size() << std::endl;
        std::cout << "This simple reconstruction application only supports one encoding space" << std::endl;
        return -1;
    }

    ISMRMRD::EncodingSpace e_space = hdr.encoding[0].encodedSpace;
    ISMRMRD::EncodingSpace r_space = hdr.encoding[0].reconSpace;

    std::cout << "Encoding Matrix Size        : [" << e_space.matrixSize.x << ", " << e_space.matrixSize.y << ", " << e_space.matrixSize.z << "]" << std::endl;
    std::cout << "Reconstruction Matrix Size  : [" << r_space.matrixSize.x << ", " << r_space.matrixSize.y << ", " << r_space.matrixSize.z << "]" << std::endl;
    std::cout << "Number of acquisitions      : " << d.getNumberOfAcquisitions() << std::endl;

    if (e_space.matrixSize.z != 1) {
        std::cout << "This simple reconstruction application only supports 2D encoding spaces" << std::endl;
        return -1;
    }

    //Allocate a buffer for the data
    std::vector<size_t> dims;
    dims.push_back(e_space.matrixSize.x);
    dims.push_back(e_space.matrixSize.y);
    ISMRMRD::NDArray<complex_float_t> buffer(dims);
    
    //Now loop through and copy data
    unsigned int number_of_acquisitions = d.getNumberOfAcquisitions();
    ISMRMRD::Acquisition acq;
    for (unsigned int i = 0; i < number_of_acquisitions; i++) {
        //Read one acquisition at a time
        d.readAcquisition(i, acq);

        //Copy data, we should probably be more careful here and do more tests....
        //We are not considering multiple channels here.
        unsigned int offset = acq.idx().kspace_encode_step_1*dims[0];
        memcpy(&buffer.getData()[offset], acq.getData(),sizeof(complex_float_t)*dims[0]);
    }

    //Let's FFT the k-space to image
    fftwf_complex* tmp = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex)*buffer.getNumberOfElements());

    if (!tmp) {
        std::cout << "Error allocating temporary storage for FFTW" << std::endl;
        return -1;
    }

    //FFTSHIFT
    fftshift(reinterpret_cast<complex_float_t*>(tmp), buffer.getData(), dims[0], dims[1]);

    //Create the FFTW plan
    fftwf_plan p = fftwf_plan_dft_2d(dims[1], dims[0], tmp ,tmp, FFTW_BACKWARD, FFTW_ESTIMATE);

    //Execute the FFT
    fftwf_execute(p);

    //FFTSHIFT
    fftshift( buffer.getData(), reinterpret_cast<std::complex<float>*>(tmp), dims[0], dims[1]);

    //Clean up.
    fftwf_destroy_plan(p);
    fftwf_free(tmp);

    //Allocate an image
    ISMRMRD::Image img_out;
    img_out.data_type(ISMRMRD::ISMRMRD_FLOAT);
    uint16_t matrix_size[3];
    matrix_size[0] = r_space.matrixSize.x;
    matrix_size[1] = r_space.matrixSize.y;
    matrix_size[2] = 1;
    img_out.matrix_size(matrix_size);
    img_out.channels(1);

    //f there is oversampling in the readout direction remove it
    //Take the magnitude
    size_t offset = ((e_space.matrixSize.x - r_space.matrixSize.x)>>1);
    for (unsigned int y = 0; y < matrix_size[1]; y++) {
        for (unsigned int x = 0; x < matrix_size[0]; x++) {
            static_cast<float*>(img_out.getData())[y*matrix_size[0]+x] =
                    std::abs(buffer.getData()[y*dims[0] + x + offset]);
        }
    }
    
    // The following are extra guidance we can put in the image header
    img_out.image_type() = ISMRMRD::ISMRMRD_IMTYPE_MAGNITUDE;
    img_out.slice() = 0;
    img_out.field_of_view()[0] = r_space.fieldOfView_mm.x;
    img_out.field_of_view()[1] = r_space.fieldOfView_mm.y;
    img_out.field_of_view()[2] = r_space.fieldOfView_mm.z;
    //And so on
    
    //Let's write the reconstructed image into the same data file
    d.appendImage("myimage", ISMRMRD::ISMRMRD_BLOCKMODE_ARRAY, img_out);

    return 0;
}

/*
 * test_recon_dataset.cpp
 *
 *  Created on: Sep 6, 2012
 *      Author: Michael S. Hansen (michael.hansen@nih.gov)
 *
 */

#include <iostream>
#include "ismrmrd.h"
#include "ismrmrd.hxx"
#include "ismrmrd_hdf5.h"
#include "fftw3.h"

//Helper function for the FFTW library
template<typename TI, typename TO> void circshift(TO *out, const TI *in, int xdim, int ydim, int xshift, int yshift)
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
	std::cout << "  - " << application << " <HDF5_FILENAME> " << "<SCHEMA_FILENAME>" << std::endl;
}

/* MAIN APPLICATION */
int main(int argc, char** argv)
{
	if (argc < 3) {
		print_usage(argv[0]);
		return -1;
	}

	std::string datafile(argv[1]);
	std::string schemafile(argv[2]);

	std::cout << "Simple ISMRMRD Reconstruction program" << std::endl;
	std::cout << "   - filename: " << datafile << std::endl;
	std::cout << "   - schema  : " << schemafile << std::endl;

	//Let's open the dataset
	ISMRMRD::IsmrmrdDataset d(datafile.c_str(),"dataset");

	//We will start by getting the header and turning it into a C++ class
	//In order to do true validation of the XML, we will use the XML schema
	xml_schema::properties props;
	props.schema_location ("http://www.ismrm.org/ISMRMRD", schemafile);

	boost::shared_ptr<std::string> xml = d.readHeader();
	std::istringstream str_stream(*xml, std::stringstream::in);

	boost::shared_ptr<ISMRMRD::ismrmrdHeader> cfg;

	try {
		cfg = boost::shared_ptr<ISMRMRD::ismrmrdHeader>(ISMRMRD::ismrmrdHeader_ (str_stream,0,props));
	}  catch (const xml_schema::exception& e) {
		std::cout << "Failed to parse XML Parameters: " << e.what() << std::endl;
	}

	//Let's print some information from the header
	ISMRMRD::ismrmrdHeader::encoding_sequence e_seq = cfg->encoding();
	if (e_seq.size() != 1) {
		std::cout << "Number of encoding spaces: " << e_seq.size() << std::endl;
		std::cout << "This simple reconstruction application only supports one encoding space" << std::endl;
		return -1;
	}

	ISMRMRD::encodingSpaceType e_space = (*e_seq.begin()).encodedSpace();
	ISMRMRD::encodingSpaceType r_space = (*e_seq.begin()).reconSpace();
	ISMRMRD::encodingLimitsType e_limits = (*e_seq.begin()).encodingLimits();

	std::cout << "Encoding Matrix Size        : [" << e_space.matrixSize().x() << ", " << e_space.matrixSize().y() << ", " << e_space.matrixSize().z() << "]" << std::endl;
	std::cout << "Reconstruction Matrix Size  : [" << r_space.matrixSize().x() << ", " << r_space.matrixSize().y() << ", " << r_space.matrixSize().z() << "]" << std::endl;
	std::cout << "Number of acquisitions      : " << d.getNumberOfAcquisitions() << std::endl;

	if (e_space.matrixSize().z() != 1) {
		std::cout << "This simple reconstruction application only supports 2D encoding spaces" << std::endl;
		return -1;
	}

	//Allocate a buffer for the data
	ISMRMRD::NDArrayContainer< std::complex<float> > buffer;
	buffer.dimensions_.push_back(e_space.matrixSize().x());
	buffer.dimensions_.push_back(e_space.matrixSize().y());
	buffer.data_.resize(e_space.matrixSize().x()*e_space.matrixSize().y(), std::complex<float>(0.0,0.0));

	//Now loop through and copy data
	unsigned int number_of_acquisitions = d.getNumberOfAcquisitions();
	for (unsigned int i = 0; i < number_of_acquisitions; i++) {
		//Read one acquisition at a time
		boost::shared_ptr<ISMRMRD::Acquisition> acq = d.readAcquisition(i);

		//Copy data, we should probably be more careful here and do more tests....
		//We are not considering multiple channels here.
		unsigned int offset = acq->head_.idx.kspace_encode_step_1*buffer.dimensions_[0];
		memcpy(&buffer.data_[offset],acq->data_,sizeof(float)*2*buffer.dimensions_[0]);
	}

	//Let's FFT the k-space to image
	fftwf_complex* tmp = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex)*buffer.data_.size());

	if (!tmp) {
		std::cout << "Error allocating temporary storage for FFTW" << std::endl;
		return -1;
	}

	fftshift(reinterpret_cast<std::complex<float>*>(tmp),&buffer.data_[0],buffer.dimensions_[0],buffer.dimensions_[1]);

	//Create the FFTW plan
	fftwf_plan p = fftwf_plan_dft_2d(buffer.dimensions_[1], buffer.dimensions_[0], tmp,tmp, FFTW_BACKWARD, FFTW_ESTIMATE);

	fftwf_execute(p);

	fftshift(&buffer.data_[0],reinterpret_cast<std::complex<float>*>(tmp),buffer.dimensions_[0],buffer.dimensions_[1]);

	//Clean up.
	fftwf_destroy_plan(p);
	fftwf_free(tmp);

	//Now, let's remove the oversampling in the readout and take the magnitude
	//Allocate a buffer for the data
	ISMRMRD::NDArrayContainer< float > img;
	img.dimensions_.push_back(r_space.matrixSize().x());
	img.dimensions_.push_back(r_space.matrixSize().y());
	img.data_.resize(r_space.matrixSize().x()*r_space.matrixSize().y(), 0.0);

	for (unsigned int y = 0; y < img.dimensions_[1]; y++) {
		for (unsigned int x = 0; x < img.dimensions_[0]; x++) {
			img.data_[y*img.dimensions_[0]+x] =
				std::abs(buffer.data_[y*buffer.dimensions_[0] + x + ((e_space.matrixSize().x()-r_space.matrixSize().x())>>1)]);
		}
	}

	//Let's write the reconstructed image straight in the same data file
	ISMRMRD::ImageHeader img_h;
	img_h.channels = 1;
	img_h.image_data_type = ISMRMRD::DATA_COMPLEX_FLOAT; //This is actually just guidance
	img_h.image_type      = ISMRMRD::TYPE_COMPLEX;       //This is actually just guidance
	img_h.slice = 0;
	//And so on

	//Now append, we will append image and header separately (in two different datasets)
	d.appendImageHeader(img_h,"myimage.head");
	d.appendArray(img,"myimage.img");


	return 0;
}

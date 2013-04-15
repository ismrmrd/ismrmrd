/*
 * generate_cartesian_shepp_logan.cpp
 *
 *  Created on: Apr 1, 2013
 *      Author: Michael S. Hansen
 *
 */

#include <iostream>
#include "ismrmrd_phantom.h"
#include "ismrmrd_hdf5.h"
#include "ismrmrd_fftw.h"
#include "ismrmrd.hxx"

#include <boost/program_options.hpp>

using namespace ISMRMRD;
namespace po = boost::program_options;

int main(int argc, char** argv)
{
	/** TODO
	 *
	 *  Noise samples
	 *  Acceleration
	 *  k-space coordinates
	 *
	 */

	unsigned int matrix_size; //Matrix size
	unsigned int ncoils;      //Number of coils
	unsigned int ros;           //Readout ovesampling
	unsigned int repetitions;
	unsigned int acc_factor;
	float noise_level;
	std::string outfile;
	std::string dataset;
	bool store_coordinates;
	bool noise_calibration;

	po::options_description desc("Allowed options");
	desc.add_options()
	    ("help,h", "produce help message")
	    ("matrix,m", po::value<unsigned int>(&matrix_size)->default_value(256), "Matrix Size")
	    ("coils,c", po::value<unsigned int>(&ncoils)->default_value(8), "Number of Coils")
	    ("oversampling,O", po::value<unsigned int>(&ros)->default_value(2), "Readout oversampling")
	    ("repetitions,r", po::value<unsigned int>(&repetitions)->default_value(1), "Repetitions")
	    ("acceleration,a", po::value<unsigned int>(&acc_factor)->default_value(1), "Acceleration factor")
	    ("noise-level,n", po::value<float>(&noise_level)->default_value(0.05f,"0.05"), "Noise Level")
	    ("output,o", po::value<std::string>(&outfile)->default_value("testdata.h5"), "Output File Name")
	    ("dataset,d", po::value<std::string>(&dataset)->default_value("dataset"), "Output Dataset Name")
	    ("noise-calibration,C", po::value<bool>(&noise_calibration)->zero_tokens(), "Add noise calibration")
	    ("k-coordinates,k",  po::value<bool>(&store_coordinates)->zero_tokens(), "Store k-space coordinates")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
	    std::cout << desc << "\n";
	    return 1;
	}

	std::cout << "Generating Cartesian Shepp Logan Phantom" << std::endl;
	std::cout << "Accelleration: " << acc_factor << std::endl;

	boost::shared_ptr<NDArrayContainer<std::complex<float> > > phantom = shepp_logan_phantom(matrix_size);
	boost::shared_ptr<NDArrayContainer<std::complex<float> > > coils = generate_birdcage_sensititivies(matrix_size, ncoils, 1.5);

	std::vector<unsigned int> dims;
	dims.push_back(matrix_size*ros); //oversampling in the readout direction
	dims.push_back(matrix_size);
	dims.push_back(ncoils);

	NDArrayContainer<std::complex<float> > coil_images(dims);
	coil_images.data_ = std::complex<float>(0.0,0.0);

	for (unsigned int c = 0; c < ncoils; c++) {
		for (unsigned int y = 0; y < matrix_size; y++) {
			for (unsigned int x = 0; x < matrix_size; x++) {
				size_t out_index = c*matrix_size*matrix_size*ros + y*matrix_size*ros + ((matrix_size*ros-matrix_size)>>1) + x;
				size_t cindex = c*matrix_size*matrix_size + y*matrix_size + x;
				size_t iindex = y*matrix_size + x;
				coil_images.data_[out_index] = phantom->data_[iindex] * coils->data_[cindex];
			}
		}
	}


	//Let's append the data to the file
	IsmrmrdDataset d(outfile.c_str(),dataset.c_str());
	Acquisition acq;
	acq.setActiveChannels(ncoils);
	acq.setAvailableChannels(ncoils);
	size_t readout = matrix_size*ros;
	acq.setNumberOfSamples(readout);
	acq.setCenterSample(readout>>1);

	if (noise_calibration) {
		acq.setFlag(ISMRMRD::FlagBit(ISMRMRD::ACQ_IS_NOISE_MEASUREMENT));
		acq.setNumberOfSamples(readout);
		for (size_t s = 0; s < acq.getData().size();s++) {
			acq[s] = 0.0;
		}
		add_noise(acq,noise_level);
		acq.setSampleTimeUs(5.0);
		d.appendAcquisition(&acq);
	}

	std::valarray<float> k;
	if (store_coordinates) {
		k.resize(2*readout);
	}

	for (unsigned int r = 0; r < repetitions; r++) {
		for (unsigned int a = 0; a < acc_factor; a++) {
			NDArrayContainer<std::complex<float> > cm = coil_images;
			fft2c(cm);
			add_noise(cm,noise_level);
			for (int i = a; i < matrix_size; i+=acc_factor) {
				acq.setFlags(0);
				acq.setNumberOfSamples(readout);

				//Set some flags
				if (i == a) {
					acq.setFlag(ISMRMRD::FlagBit(ISMRMRD::ACQ_FIRST_IN_SLICE));
				}
				if (i >= (matrix_size-acc_factor)) {
					acq.setFlag(ISMRMRD::FlagBit(ISMRMRD::ACQ_LAST_IN_SLICE));
				}
				acq.getIdx().kspace_encode_step_1 = i;
				acq.getIdx().repetition = r*acc_factor + a;
				acq.setSampleTimeUs(5.0);
				for (unsigned int c = 0; c < ncoils; c++) {
					memcpy(&acq[c*readout*2],&(cm.data_[c*matrix_size*readout + i*readout]),sizeof(float)*readout*2);
				}

				if (store_coordinates) {
					acq.setTrajectoryDimensions(2);
					float ky = (1.0*i-(matrix_size>>1))/(1.0*matrix_size);
					for (int x = 0; x < readout; x++) {
						float kx = (1.0*x-(readout>>1))/(1.0*readout);
						k[x*2  ] = kx;
						k[x*2+1] = ky;
					}
					acq.setTraj(k);
				}
				d.appendAcquisition(&acq);
			}
		}
	}

	//Let's create a header, we will use the C++ class generated by XSD
	ISMRMRD::experimentalConditionsType exp(63500000); //~1.5T
	ISMRMRD::acquisitionSystemInformationType sys;

	sys.institutionName("ISMRM Synthetic Imaging Lab");
	sys.receiverChannels(ncoils);


	ISMRMRD::ismrmrdHeader h(exp);

	h.acquisitionSystemInformation(sys);

	//Create an encoding section
	ISMRMRD::encodingSpaceType es(ISMRMRD::matrixSize(readout,matrix_size,1),ISMRMRD::fieldOfView_mm(600,300,6));
	ISMRMRD::encodingSpaceType rs(ISMRMRD::matrixSize((readout>>1),matrix_size,1),ISMRMRD::fieldOfView_mm(300,300,6));
	ISMRMRD::encodingLimitsType el;
	el.kspace_encoding_step_1(ISMRMRD::limitType(0,matrix_size-1,(matrix_size>>1)));
	el.repetition(ISMRMRD::limitType(0,repetitions*acc_factor,0));
	ISMRMRD::encoding e(es,rs,el,ISMRMRD::trajectoryType::cartesian);

	//Add the encoding section to the header
	h.encoding().push_back(e);

	//Add any additional fields that you may want would go here....

	//e.g. parallel imaging
	if (acc_factor > 1) {
		ISMRMRD::parallelImagingType parallel(ISMRMRD::accelerationFactorType(acc_factor,1));
		parallel.calibrationMode(ISMRMRD::calibrationModeType::interleaved);
		h.parallelImaging(parallel);
	}

	//Serialize the header
	xml_schema::namespace_infomap map;
	map[""].name = "http://www.ismrm.org/ISMRMRD";
	map[""].schema = "ismrmrd.xsd";
	std::stringstream str;
	ISMRMRD::ismrmrdHeader_(str, h, map);
	std::string xml_header = str.str();

	//Write the header to the data file.
	d.writeHeader(xml_header);


	d.appendArray(*phantom,"phantom");
	d.appendArray(*coils,"csm");
	d.appendArray(coil_images,"coil_images");

	return 0;
}





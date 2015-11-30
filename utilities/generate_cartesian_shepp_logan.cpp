
/*
 * generate_cartesian_shepp_logan.cpp
 *
 *  Created on: Apr 1, 2013
 *      Author: Michael S. Hansen
 *
 */

#include <iostream>
#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/xml.h"
#include "ismrmrd/dataset.h"
#include "ismrmrd/version.h"
#include "ismrmrd_phantom.h"
#include "ismrmrd_fftw.h"

#include <boost/program_options.hpp>

using namespace ISMRMRD;
namespace po = boost::program_options;

// MAIN APPLICATION
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
    bool store_coordinates = false;
    bool noise_calibration = false;

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

    std::cout << "Generating Cartesian Shepp Logan Phantom!!!" << std::endl;
    std::cout << "Acceleration: " << acc_factor << std::endl;

    boost::shared_ptr<NDArray<std::complex<float> > > phantom = shepp_logan_phantom(matrix_size);
    boost::shared_ptr<NDArray<std::complex<float> > > coils = generate_birdcage_sensititivies(matrix_size, ncoils, 1.5);

    std::vector<size_t> dims;
    dims.push_back(matrix_size*ros); //oversampling in the readout direction
    dims.push_back(matrix_size);
    dims.push_back(ncoils);

    NDArray<std::complex<float> > coil_images(dims);

    for (unsigned int c = 0; c < ncoils; c++) {
        for (unsigned int y = 0; y < matrix_size; y++) {
            for (unsigned int x = 0; x < matrix_size; x++) {
                uint16_t xout = x + (matrix_size*ros-matrix_size)/2;
                coil_images.at(xout, y, c) = phantom->at(x,y) * coils->at(x,y,c);
            }
        }
    }

    //Let's append the data to the file
    //Create if needed
    Dataset d(outfile.c_str(), dataset.c_str());
    Acquisition<float> acq;
    size_t readout = matrix_size*ros;

    if (noise_calibration)
    {
        acq.resize(readout, ncoils);
        acq.setFlag(ISMRMRD_ACQ_IS_NOISE_MEASUREMENT);
        add_noise(acq,noise_level);
        acq.setDwellTime_ns(5000);
        d.appendAcquisition(acq);
    }

    if (store_coordinates) {
        acq.resize(readout, ncoils, 2);
    } else {
        acq.resize(readout, ncoils);
    }

    acq.setAvailableChannels(ncoils);
    acq.setCenterSample(readout / 2);

    for (unsigned int r = 0; r < repetitions; r++) {
        for (unsigned int a = 0; a < acc_factor; a++) {
            NDArray<std::complex<float> > cm = coil_images;
            fft2c(cm);

            add_noise(cm, noise_level);
            for (size_t i = a; i < matrix_size; i+=acc_factor) {
                acq.clearAllFlags();

                //Set some flags
                if (i == a) {
                    acq.setFlag(ISMRMRD_ACQ_FIRST_IN_SLICE);
                }
                if (i >= (matrix_size-acc_factor)) {
                    acq.setFlag(ISMRMRD_ACQ_LAST_IN_SLICE);
                }
                acq.getEncodingCounters().kspace_encode_step_1 = i;
                acq.getEncodingCounters().repetition = r*acc_factor + a;
                acq.setDwellTime_ns(5000);
                for (size_t c = 0; c < ncoils; c++) {
                    for (size_t s = 0; s < readout; s++) {
                        acq.at(s, c) = cm.at(s, i, c);
                    }
                }

                if (store_coordinates) {
                    float ky = (1.0*i-(matrix_size / 2))/(1.0*matrix_size);
                    for (size_t x = 0; x < readout; x++) {
                        float kx = (1.0*x-(readout / 2))/(1.0*readout);
                        acq.trajAt(0, x) = kx;
                        acq.trajAt(1, x) = ky;
                    }
                }
                d.appendAcquisition(acq);
            }
        }
    }

    //Let's create a header, we will use the C++ classes in ismrmrd/xml.h
    IsmrmrdHeader h;
    h.version = ISMRMRD_XMLHDR_VERSION;
    h.experimentalConditions.H1resonanceFrequency_Hz = 63500000; //~1.5T

    AcquisitionSystemInformation sys;
    sys.institutionName = "ISMRM Synthetic Imaging Lab";
    sys.receiverChannels = ncoils;
    h.acquisitionSystemInformation = sys;

    //Create an encoding section
    Encoding e;
    e.encodedSpace.matrixSize.x = readout;
    e.encodedSpace.matrixSize.y = matrix_size;
    e.encodedSpace.matrixSize.z = 1;
    e.encodedSpace.fieldOfView_mm.x = 600;
    e.encodedSpace.fieldOfView_mm.y = 300;
    e.encodedSpace.fieldOfView_mm.z = 6;
    e.reconSpace.matrixSize.x = readout/2;
    e.reconSpace.matrixSize.y = matrix_size;
    e.reconSpace.matrixSize.z = 1;
    e.reconSpace.fieldOfView_mm.x = 300;
    e.reconSpace.fieldOfView_mm.y = 300;
    e.reconSpace.fieldOfView_mm.z = 6;
    e.trajectory = "cartesian";
    e.encodingLimits.kspace_encoding_step_1 = Limit(0, matrix_size-1,(matrix_size / 2));
    e.encodingLimits.repetition = Limit(0, repetitions*acc_factor - 1,0);

    //e.g. parallel imaging
    if (acc_factor > 1) {
        ParallelImaging parallel;
        parallel.accelerationFactor.kspace_encoding_step_1 = acc_factor;
        parallel.accelerationFactor.kspace_encoding_step_2 = 1;
        parallel.calibrationMode = "interleaved";
        e.parallelImaging = parallel;
    }

    //Add the encoding section to the header
    h.encoding.push_back(e);

    //Add the description of the only acquisition stream to the header
    Stream stream;
    stream.label = "K-Space Data";
    stream.storageType = ISMRMRD_CXFLOAT;
    stream.entityType = ISMRMRD_MRACQUISITION;
    stream.number = 0;

    h.streams.push_back(stream);

    //Add any additional fields that you may want would go here....

    //Serialize the header
    std::stringstream str;
    ISMRMRD::serialize( h, str);
    std::string xml_header = str.str();
    //std::cout << xml_header << std::endl;

    //Write the header to the data file.
    d.writeHeader(xml_header);

    //Write out some arrays for convenience
    d.appendNDArray("phantom", *phantom);
    d.appendNDArray("csm", *coils);
    d.appendNDArray("coil_images", coil_images);

    return 0;
}

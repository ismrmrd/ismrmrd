#include "ismrmrd_dataset.h"

// for memcpy and free in older compilers
#include <string.h>
#include <stdlib.h>

namespace ISMRMRD {
//
// Dataset class implementation
//
// Constructor
Dataset::Dataset(const char* filename, const char* groupname, bool create_file_if_needed)
{
    // TODO error checking and exception throwing
    // Initialize the dataset
    int status;
    status = ismrmrd_init_dataset(&dset_, filename, groupname);
    // Open the file
    status = ismrmrd_open_dataset(&dset_, create_file_if_needed);
    if (status != ISMRMRD_NOERROR) {
      // TODO throw an exception
    }
}

// Destructor
Dataset::~Dataset()
{
    int status = ismrmrd_close_dataset(&dset_);
    if (status != ISMRMRD_NOERROR) {
      // TODO throw an exception
    }
}

// XML Header
int Dataset::writeHeader(const std::string xmlstring)
{
    int status = ismrmrd_write_header(&dset_, xmlstring.c_str());
    return status;
}

int Dataset::readHeader(std::string& xmlstring){
    char * temp = ismrmrd_read_header(&dset_);
    xmlstring = std::string(temp);
    free(temp);
    return ISMRMRD_NOERROR;
}

// Acquisitions
int Dataset::appendAcquisition(const Acquisition acq)
{
    int status = ismrmrd_append_acquisition(&dset_, reinterpret_cast<const ISMRMRD_Acquisition*>(&acq));
    return status;
}

Acquisition * Dataset::readAcquisition(uint32_t index)
{
    Acquisition * acq = new Acquisition();
    int status = ismrmrd_read_acquisition(&dset_, index, reinterpret_cast<ISMRMRD_Acquisition*>(acq));
    if (status != ISMRMRD_NOERROR) {
      //TODO throw an exception
    }
    return acq;
}

int Dataset::readAcquisition(uint32_t index, Acquisition * acqptr) {
    int status = ismrmrd_read_acquisition(&dset_, index, reinterpret_cast<ISMRMRD_Acquisition*>(acqptr));
    if (status != ISMRMRD_NOERROR) {
      //TODO throw an exception
    }
    return status;
}


uint32_t Dataset::getNumberOfAcquisitions()
{
    uint32_t num =  ismrmrd_get_number_of_acquisitions(&dset_);
    return num;
}

// Images
int Dataset::appendImage(const std::string var, const ISMRMRD_BlockModes blockmode, const Image im)
{
    int status = ismrmrd_append_image(&dset_, var.c_str(), blockmode, reinterpret_cast<const ISMRMRD_Image*>(&im));
    if (status != ISMRMRD_NOERROR) {
      //TODO throw an exception
    }
    return status;
}

Image * Dataset::readImage(const std::string var, uint32_t index)
{
    Image * im = new Image();
    int status = ismrmrd_read_image(&dset_, var.c_str(), index, reinterpret_cast<ISMRMRD_Image*>(im));
    if (status != ISMRMRD_NOERROR) {
      //TODO throw an exception
    }
    return im;
}

int Dataset::readImage(const std::string var, uint32_t index, Image * imptr) {
    int status = ismrmrd_read_image(&dset_, var.c_str(), index, reinterpret_cast<ISMRMRD_Image*>(imptr));
    if (status != ISMRMRD_NOERROR) {
      //TODO throw an exception
    }
    return status;
}

uint32_t Dataset::getNumberOfImages(const std::string var)
{
    uint32_t num =  ismrmrd_get_number_of_images(&dset_, var.c_str());
    return num;
}


// NDArrays
int Dataset::appendNDArray(const std::string var, const ISMRMRD_BlockModes blockmode, const NDArray arr)
{
    int status = ismrmrd_append_array(&dset_, var.c_str(), blockmode, reinterpret_cast<const ISMRMRD_NDArray*>(&arr));
    if (status != ISMRMRD_NOERROR) {
      //TODO throw an exception
    }
    return status;
}

NDArray * Dataset::readNDArray(const std::string var, uint32_t index)
{
    NDArray * arr = new NDArray();
    int status = ismrmrd_read_array(&dset_, var.c_str(), index, reinterpret_cast<ISMRMRD_NDArray*>(arr));
    if (status != ISMRMRD_NOERROR) {
      //TODO throw an exception
    }
    return arr;
}

int Dataset::readNDArray(const std::string var, uint32_t index, NDArray *arrptr) {
    int status = ismrmrd_read_array(&dset_, var.c_str(), index, reinterpret_cast<ISMRMRD_NDArray*>(arrptr));
    if (status != ISMRMRD_NOERROR) {
      //TODO throw an exception
    }
    return status;
}

uint32_t Dataset::getNumberOfNDArrays(const std::string var)
{
    uint32_t num =  ismrmrd_get_number_of_arrays(&dset_, var.c_str());
    return num;
}

} // namespace ISMRMRD

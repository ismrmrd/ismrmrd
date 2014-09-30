#include "ismrmrd/dataset.h"

// for memcpy and free in older compilers
#include <string.h>
#include <stdlib.h>
#include <stdexcept>

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
    if (status != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
    // Open the file
    status = ismrmrd_open_dataset(&dset_, create_file_if_needed);
    if (status != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

// Destructor
Dataset::~Dataset()
{
    int status = ismrmrd_close_dataset(&dset_);
    if (status != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

// XML Header
int Dataset::writeHeader(const std::string &xmlstring)
{
    int status = ismrmrd_write_header(&dset_, xmlstring.c_str());
    return status;
}

int Dataset::readHeader(std::string& xmlstring){
    char * temp = ismrmrd_read_header(&dset_);
    if (NULL == temp) {
        throw std::runtime_error(build_exception_string());
    } else {
        xmlstring = std::string(temp);
        free(temp);
    }
    return ISMRMRD_NOERROR;
}

// Acquisitions
int Dataset::appendAcquisition(const Acquisition &acq)
{
    int status = ismrmrd_append_acquisition(&dset_, reinterpret_cast<const ISMRMRD_Acquisition*>(&acq));
    return status;
}

int Dataset::readAcquisition(uint32_t index, Acquisition & acq) {
    int status = ismrmrd_read_acquisition(&dset_, index, reinterpret_cast<ISMRMRD_Acquisition*>(&acq));
    if (status != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
    return status;
}


uint32_t Dataset::getNumberOfAcquisitions()
{
    uint32_t num = ismrmrd_get_number_of_acquisitions(&dset_);
    return num;
}

// Images
int Dataset::appendImage(const std::string &var, const ISMRMRD_BlockModes blockmode, const Image &im)
{
    int status = ismrmrd_append_image(&dset_, var.c_str(), blockmode, reinterpret_cast<const ISMRMRD_Image*>(&im));
    if (status != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
    return status;
}

int Dataset::appendImage(const std::string &var, const ISMRMRD_BlockModes blockmode, const ISMRMRD_Image *im)
{
    int status = ismrmrd_append_image(&dset_, var.c_str(), blockmode, im);
    if (status != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
    return status;
}

int Dataset::readImage(const std::string &var, uint32_t index, Image &im) {
    int status = ismrmrd_read_image(&dset_, var.c_str(), index, reinterpret_cast<ISMRMRD_Image*>(&im));
    if (status != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
    return status;
}

uint32_t Dataset::getNumberOfImages(const std::string &var)
{
    uint32_t num =  ismrmrd_get_number_of_images(&dset_, var.c_str());
    return num;
}


// NDArrays
template <typename T> int Dataset::appendNDArray(const std::string &var, const ISMRMRD_BlockModes blockmode, const NDArray<T> &arr)
{
    int status = ismrmrd_append_array(&dset_, var.c_str(), blockmode, static_cast<const ISMRMRD_NDArray*>(&arr));
    if (status != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
    return status;
}
// Specific instantiations
template EXPORTISMRMRD int Dataset::appendNDArray(const std::string &var, const ISMRMRD_BlockModes blockmode, const NDArray<uint16_t> &arr);
template EXPORTISMRMRD int Dataset::appendNDArray(const std::string &var, const ISMRMRD_BlockModes blockmode, const NDArray<int16_t> &arr);
template EXPORTISMRMRD int Dataset::appendNDArray(const std::string &var, const ISMRMRD_BlockModes blockmode, const NDArray<uint32_t> &arr);
template EXPORTISMRMRD int Dataset::appendNDArray(const std::string &var, const ISMRMRD_BlockModes blockmode, const NDArray<int32_t> &arr);
template EXPORTISMRMRD int Dataset::appendNDArray(const std::string &var, const ISMRMRD_BlockModes blockmode, const NDArray<float> &arr);
template EXPORTISMRMRD int Dataset::appendNDArray(const std::string &var, const ISMRMRD_BlockModes blockmode, const NDArray<double> &arr);
template EXPORTISMRMRD int Dataset::appendNDArray(const std::string &var, const ISMRMRD_BlockModes blockmode, const NDArray<complex_float_t> &arr);
template EXPORTISMRMRD int Dataset::appendNDArray(const std::string &var, const ISMRMRD_BlockModes blockmode, const NDArray<complex_double_t> &arr);

int Dataset::appendNDArray(const std::string &var, const ISMRMRD_BlockModes blockmode, const ISMRMRD_NDArray *arr)
{
    int status = ismrmrd_append_array(&dset_, var.c_str(), blockmode, arr);
    if (status != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
    return status;
}

template <typename T> int Dataset::readNDArray(const std::string &var, uint32_t index, NDArray<T> &arr) {
    int status = ismrmrd_read_array(&dset_, var.c_str(), index, static_cast<ISMRMRD_NDArray*>(&arr));
    if (status != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
    return status;
}

uint32_t Dataset::getNumberOfNDArrays(const std::string &var)
{
    uint32_t num = ismrmrd_get_number_of_arrays(&dset_, var.c_str());
    return num;
}

} // namespace ISMRMRD

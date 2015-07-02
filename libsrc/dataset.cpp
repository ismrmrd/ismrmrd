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
Dataset::Dataset(const char* filename, const char* groupname, bool create_file_if_needed, bool read_only)
{
    // TODO error checking and exception throwing
    // Initialize the dataset
    int status;
    status = ismrmrd_init_dataset(&dset_, filename, groupname);
    if (status != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }

    // Check if file exists. If not, create as necessary
    if (!ismrmrd_dataset_exists(filename)) {
        if (create_file_if_needed) {
            status = ismrmrd_create_dataset(&dset_);
            if (status != ISMRMRD_NOERROR) {
                throw std::runtime_error(build_exception_string());
            }
        } else {
            throw std::runtime_error("File does not exist");
        }
    }

    // Open the file
    status = ismrmrd_open_dataset(&dset_, read_only);
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

bool Dataset::exists(const char* filename)
{
    return ismrmrd_dataset_exists(filename);
}

// XML Header
void Dataset::writeHeader(const std::string &xmlstring)
{
    int status = ismrmrd_write_header(&dset_, xmlstring.c_str());
    if (status != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

void Dataset::readHeader(std::string& xmlstring){
    char * temp = ismrmrd_read_header(&dset_);
    if (NULL == temp) {
        throw std::runtime_error(build_exception_string());
    } else {
        xmlstring = std::string(temp);
        free(temp);
    }
}

// Acquisitions
template <typename T>
void Dataset::appendAcquisition(const Acquisition<T> &acq)
{
    this->appendAcquisition(&acq.acq);
}

void Dataset::appendAcquisition(const ISMRMRD_Acquisition *acq)
{
    int status = ismrmrd_append_acquisition(&dset_, acq);
    if (status != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

template EXPORTISMRMRD void Dataset::appendAcquisition(const Acquisition<uint16_t>& acq);
template EXPORTISMRMRD void Dataset::appendAcquisition(const Acquisition<int16_t>& acq);
template EXPORTISMRMRD void Dataset::appendAcquisition(const Acquisition<uint32_t>& acq);
template EXPORTISMRMRD void Dataset::appendAcquisition(const Acquisition<int32_t>& acq);
template EXPORTISMRMRD void Dataset::appendAcquisition(const Acquisition<float>& acq);
template EXPORTISMRMRD void Dataset::appendAcquisition(const Acquisition<double>& acq);
template EXPORTISMRMRD void Dataset::appendAcquisition(const Acquisition<complex_float_t>& acq);
template EXPORTISMRMRD void Dataset::appendAcquisition(const Acquisition<complex_double_t>& acq);

template <typename T>
void Dataset::readAcquisition(uint16_t stream_number, uint32_t index, Acquisition<T>& acq) {
    int status = ismrmrd_read_acquisition(&dset_, stream_number, index, &acq.acq);//reinterpret_cast<ISMRMRD_Acquisition*>(&acq));
    if (status != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

template EXPORTISMRMRD void Dataset::readAcquisition(uint16_t, uint32_t, Acquisition<uint16_t>& acq);
template EXPORTISMRMRD void Dataset::readAcquisition(uint16_t, uint32_t, Acquisition<int16_t>& acq);
template EXPORTISMRMRD void Dataset::readAcquisition(uint16_t, uint32_t, Acquisition<uint32_t>& acq);
template EXPORTISMRMRD void Dataset::readAcquisition(uint16_t, uint32_t, Acquisition<int32_t>& acq);
template EXPORTISMRMRD void Dataset::readAcquisition(uint16_t, uint32_t, Acquisition<float>& acq);
template EXPORTISMRMRD void Dataset::readAcquisition(uint16_t, uint32_t, Acquisition<double>& acq);
template EXPORTISMRMRD void Dataset::readAcquisition(uint16_t, uint32_t, Acquisition<complex_float_t>& acq);
template EXPORTISMRMRD void Dataset::readAcquisition(uint16_t, uint32_t, Acquisition<complex_double_t>& acq);

uint32_t Dataset::getNumberOfAcquisitions()
{
    uint32_t num = ismrmrd_get_number_of_acquisitions(&dset_);
    return num;
}

// Images
template <typename T> void Dataset::appendImage(const std::string &var, const Image<T> &im)
{
    this->appendImage(var, &im.im);
}

void Dataset::appendImage(const std::string &var, const ISMRMRD_Image *im)
{
    int status = ismrmrd_append_image(&dset_, var.c_str(), im);
    if (status != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}
// Specific instantiations
template EXPORTISMRMRD void Dataset::appendImage(const std::string &var, const Image<uint16_t> &im);
template EXPORTISMRMRD void Dataset::appendImage(const std::string &var, const Image<int16_t> &im);
template EXPORTISMRMRD void Dataset::appendImage(const std::string &var, const Image<uint32_t> &im);
template EXPORTISMRMRD void Dataset::appendImage(const std::string &var, const Image<int32_t> &im);
template EXPORTISMRMRD void Dataset::appendImage(const std::string &var, const Image<float> &im);
template EXPORTISMRMRD void Dataset::appendImage(const std::string &var, const Image<double> &im);
template EXPORTISMRMRD void Dataset::appendImage(const std::string &var, const Image<complex_float_t> &im);
template EXPORTISMRMRD void Dataset::appendImage(const std::string &var, const Image<complex_double_t> &im);


template <typename T> void Dataset::readImage(const std::string &var, uint32_t index, Image<T> &im) {
    int status = ismrmrd_read_image(&dset_, var.c_str(), index, &im.im);
    if (status != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

// Specific instantiations
template EXPORTISMRMRD void Dataset::readImage(const std::string &var, uint32_t index, Image<uint16_t> &im);
template EXPORTISMRMRD void Dataset::readImage(const std::string &var, uint32_t index, Image<int16_t> &im);
template EXPORTISMRMRD void Dataset::readImage(const std::string &var, uint32_t index, Image<uint32_t> &im);
template EXPORTISMRMRD void Dataset::readImage(const std::string &var, uint32_t index, Image<int32_t> &im);
template EXPORTISMRMRD void Dataset::readImage(const std::string &var, uint32_t index, Image<float> &im);
template EXPORTISMRMRD void Dataset::readImage(const std::string &var, uint32_t index, Image<double> &im);
template EXPORTISMRMRD void Dataset::readImage(const std::string &var, uint32_t index, Image<complex_float_t> &im);
template EXPORTISMRMRD void Dataset::readImage(const std::string &var, uint32_t index, Image<complex_double_t> &im);

uint32_t Dataset::getNumberOfImages(const std::string &var)
{
    uint32_t num =  ismrmrd_get_number_of_images(&dset_, var.c_str());
    return num;
}


// NDArrays
template <typename T> void Dataset::appendNDArray(const std::string &var, const NDArray<T> &arr)
{
    appendNDArray(var, &arr.arr);
}

void Dataset::appendNDArray(const std::string &var, const ISMRMRD_NDArray *arr)
{
    int status = ismrmrd_append_array(&dset_, var.c_str(), arr);
    if (status != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

// Specific instantiations
template EXPORTISMRMRD void Dataset::appendNDArray(const std::string &var, const NDArray<uint16_t> &arr);
template EXPORTISMRMRD void Dataset::appendNDArray(const std::string &var, const NDArray<int16_t> &arr);
template EXPORTISMRMRD void Dataset::appendNDArray(const std::string &var, const NDArray<uint32_t> &arr);
template EXPORTISMRMRD void Dataset::appendNDArray(const std::string &var, const NDArray<int32_t> &arr);
template EXPORTISMRMRD void Dataset::appendNDArray(const std::string &var, const NDArray<float> &arr);
template EXPORTISMRMRD void Dataset::appendNDArray(const std::string &var, const NDArray<double> &arr);
template EXPORTISMRMRD void Dataset::appendNDArray(const std::string &var, const NDArray<complex_float_t> &arr);
template EXPORTISMRMRD void Dataset::appendNDArray(const std::string &var, const NDArray<complex_double_t> &arr);

template <typename T> void Dataset::readNDArray(const std::string &var, uint32_t index, NDArray<T> &arr) {
    int status = ismrmrd_read_array(&dset_, var.c_str(), index, &arr.arr);
    if (status != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

// Specific instantiations
template EXPORTISMRMRD void Dataset::readNDArray(const std::string &var, uint32_t index, NDArray<uint16_t> &arr);
template EXPORTISMRMRD void Dataset::readNDArray(const std::string &var, uint32_t index, NDArray<int16_t> &arr);
template EXPORTISMRMRD void Dataset::readNDArray(const std::string &var, uint32_t index, NDArray<uint32_t> &arr);
template EXPORTISMRMRD void Dataset::readNDArray(const std::string &var, uint32_t index, NDArray<int32_t> &arr);
template EXPORTISMRMRD void Dataset::readNDArray(const std::string &var, uint32_t index, NDArray<float> &arr);
template EXPORTISMRMRD void Dataset::readNDArray(const std::string &var, uint32_t index, NDArray<double> &arr);
template EXPORTISMRMRD void Dataset::readNDArray(const std::string &var, uint32_t index, NDArray<complex_float_t> &arr);
template EXPORTISMRMRD void Dataset::readNDArray(const std::string &var, uint32_t index, NDArray<complex_double_t> &arr);

uint32_t Dataset::getNumberOfNDArrays(const std::string &var)
{
    uint32_t num = ismrmrd_get_number_of_arrays(&dset_, var.c_str());
    return num;
}

} // namespace ISMRMRD

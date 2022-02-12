/* ISMRMRD Data Set */

/**
 * @file dataset.h
 */

#pragma once
#ifndef ISMRMRD_DATASET_H
#define ISMRMRD_DATASET_H

#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/waveform.h"
#include <hdf5.h>

#ifdef __cplusplus
#include <string>
namespace ISMRMRD {
extern "C" {
#endif

/**
 *   Interface for accessing an ISMRMRD Data Set stored on disk in HDF5 format.
 *
 *   A given ISMRMRD dataset if assumed to be stored under one group name in the
 *   HDF5 file.  To make the datasets consistent, this library enforces that the
 *   XML configuration is stored in the variable groupname/xml and the
 *   Acquisitions are stored in the variable groupname/data.
 *
 */
typedef struct ISMRMRD_Dataset {
    char *filename;
    char *groupname;
    hid_t fileid;
} ISMRMRD_Dataset;

/**
 * Initializes an ISMRMRD dataset structure
 *
 */
EXPORTISMRMRD int ismrmrd_init_dataset(ISMRMRD_Dataset *dset, const char *filename, const char *groupname);
            
/**
 * Opens an ISMRMRD dataset.
 *
 */
EXPORTISMRMRD int ismrmrd_open_dataset(ISMRMRD_Dataset *dset, const bool create_if_needed);

/**
 * Closes all references to the underlying HDF5 file.
 *
 */
EXPORTISMRMRD int ismrmrd_close_dataset(ISMRMRD_Dataset *dset);

/**
 *  Writes the XML header string to the dataset.
 *
 *  @warning There is no check of whether the string is a valid XML document at this point.
 *
 */
EXPORTISMRMRD int ismrmrd_write_header(const ISMRMRD_Dataset *dset, const char *xmlstring);

/**
 *  Reads the XML configuration header from the dataset.
 *
 *  @warning There is no check of whether the string is a valid XML document at this point.
 *
 */
EXPORTISMRMRD char * ismrmrd_read_header(const ISMRMRD_Dataset *dset);

/**
 *  Appends and NMR/MRI acquisition to the dataset.
 *
 *  Please consult @See ISMRMRD_Acquisition struct for details.
 */
EXPORTISMRMRD int ismrmrd_append_acquisition(const ISMRMRD_Dataset *dset, const ISMRMRD_Acquisition *acq);

/**
 *  Reads the acquisition with the specified index from the dataset.
 */
EXPORTISMRMRD int ismrmrd_read_acquisition(const ISMRMRD_Dataset *dset, uint32_t index, ISMRMRD_Acquisition *acq);

/**
 *  Return the number of acquisitions in the dataset.
 */
EXPORTISMRMRD uint32_t ismrmrd_get_number_of_acquisitions(const ISMRMRD_Dataset *dset);

/**
 *  Appends and waveform data to the dataset.
 *
 *  Please consult @See ISMRMRD_Waveform struct for details.
 */
EXPORTISMRMRD int ismrmrd_append_waveform(const ISMRMRD_Dataset *dset, const ISMRMRD_Waveform *wav);

/**
 *  Reads the  wveformith the specified index from the dataset.
 */
EXPORTISMRMRD int ismrmrd_read_waveform(const ISMRMRD_Dataset *dset, uint32_t index, ISMRMRD_Waveform* wav);

/**
 *  Return the number of waveforms in the dataset.
 */
EXPORTISMRMRD uint32_t ismrmrd_get_number_of_waveforms(const ISMRMRD_Dataset *dset);
/**
 *  Appends an Image to the variable named varname in the dataset.
 *
 *  Please consult @See ISMRMRD_Image struct for details.
 *
 *  Headers and attribute strings are stored separately from the data.
 *  This allows for easy viewing and reading in other applications.
 *
 *  Images of the same size can be appended to "grow" an array.
 *    e.g. 20 images of size (256, 256, 4, 16), i.e. 4 slice and 16 channels, can be appended
 *    one at a time to make a (256, 256, 4, 16, 20) array in the file.
 *
 */
EXPORTISMRMRD int ismrmrd_append_image(const ISMRMRD_Dataset *dset, const char *varname,
                                       const ISMRMRD_Image *im);

/**
 *   Reads an image stored with appendImage.
 *   The index indicates which image to read from the variable named varname.
 */
EXPORTISMRMRD int ismrmrd_read_image(const ISMRMRD_Dataset *dset, const char *varname,
                                     const uint32_t index, ISMRMRD_Image *im);

/**
 *  Return the number of images in the variable varname in the dataset.
 */
EXPORTISMRMRD uint32_t ismrmrd_get_number_of_images(const ISMRMRD_Dataset *dset, const char *varname);

/**
 *  Appends an NDArray to the variable named varname in the dataset.
 *
 *  Please consult @See NDArray struct for details.
 *
 *  Arrays contain simple data elements such as float, double, etc.
 *
 *  Arrays of the same size can be appended to "grow" an array,
 *    e.g. 3D arrays of size (K,L,M) can be appended to grow a 4D array of size (K,L,M,N).
 *
 */
EXPORTISMRMRD int ismrmrd_append_array(const ISMRMRD_Dataset *dset, const char *varname,
                                       const ISMRMRD_NDArray *arr);

/**
 *  Reads an array from the data file.
 */
EXPORTISMRMRD int ismrmrd_read_array(const ISMRMRD_Dataset *dataset, const char *varname,
                                     const uint32_t index, ISMRMRD_NDArray *arr);

/**
 *  Return the number of arrays in the variable varname in the dataset.
 */
EXPORTISMRMRD uint32_t ismrmrd_get_number_of_arrays(const ISMRMRD_Dataset *dset, const char *varname);

    
#ifdef __cplusplus
} /* extern "C" */

//  ISMRMRD Dataset C++ Interface
class EXPORTISMRMRD Dataset {
public:
    // Constructor and destructor
    Dataset(const char* filename, const char* groupname, bool create_file_if_needed = true);
    ~Dataset();
    
    // Methods
    // XML Header
    void writeHeader(const std::string &xmlstring);
    void readHeader(std::string& xmlstring);
    // Acquisitions
    void appendAcquisition(const Acquisition &acq);
    void readAcquisition(uint32_t index, Acquisition &acq);
    uint32_t getNumberOfAcquisitions();
    // Images
    template <typename T> void appendImage(const std::string &var, const Image<T> &im);
    void appendImage(const std::string &var, const ISMRMRD_Image *im);
    template <typename T> void readImage(const std::string &var, uint32_t index, Image<T> &im);
    uint32_t getNumberOfImages(const std::string &var);
    // NDArrays
    template <typename T> void appendNDArray(const std::string &var, const NDArray<T> &arr);
    void appendNDArray(const std::string &var, const ISMRMRD_NDArray *arr);
    template <typename T> void readNDArray(const std::string &var, uint32_t index, NDArray<T> &arr);
    uint32_t getNumberOfNDArrays(const std::string &var);

    //Waveforms
    void appendWaveform(const Waveform &wav);
    void readWaveform(uint32_t index, Waveform & wav);
    uint32_t getNumberOfWaveforms();
protected:
    ISMRMRD_Dataset dset_;
};

} /* ISMRMRD namespace */
#endif

#endif /* ISMRMRD_DATASET_H */

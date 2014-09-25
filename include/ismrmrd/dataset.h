/* ISMRMRD Data Set */

/**
 * @file dataset.h
 */

#pragma once
#ifndef ISMRMRD_DATASET_H
#define ISMRMRD_DATASET_H

#include "ismrmrd.h"

#ifdef __cplusplus
#include <string>
namespace ISMRMRD {
extern "C" {
#endif

/**
 * Image and Array storage block modes
 */
enum ISMRMRD_BlockModes {
    ISMRMRD_BLOCKMODE_ARRAY,
    ISMRMRD_BLOCKMODE_BLOB
};
    
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
    int fileid;
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
EXPORTISMRMRD int ismrmrd_open_dataset(ISMRMRD_Dataset *dset, const bool create_if_neded);

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
 *  Appends an Image to the variable named varname in the dataset.
 *
 *  Please consult @See ISMRMRD_Image struct for details.
 *
 *  Images can be stored in one of two ways.  In either case headers and attribute strings are stored
 *  separatey for each of image.   This allows for easy viewing and reading in other applications.
 *
 *  Method 1: Images of the same size can be appended to "grow" an array.
 *    e.g. 20 images of size (256, 256, 4, 16), i.e. 4 slice and 16 channels, can be appended
 *    one at a time to make a (256, 256, 4, 16, 20) array in the file.
 *
 *  Method 2: Images of different sizes can be appended individually.
 *
 */
EXPORTISMRMRD int ismrmrd_append_image(const ISMRMRD_Dataset *dset, const char *varname,
                                       const int block_mode, const ISMRMRD_Image *im);

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
 *  Arrays can be stored in one of two ways to allow for easy viewing in other applications.
 *
 *  Method 1: Arrays of the same size can be appended to "grow" an array,
 *    e.g. 3D arrays of size (K,L,M) can be appended to grow a 4D array of size (K,L,M,N).
 *
 *  Method 2:Arrays of varying dimensionality and size can be appended as blobs of data.
 *
 */
EXPORTISMRMRD int ismrmrd_append_array(const ISMRMRD_Dataset *dset, const char *varname,
                                       const int block_mode, const ISMRMRD_NDArray *arr);

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

//
//  ISMRMRD Datset C++ Interface
//

class EXPORTISMRMRD Dataset {
public:
    // Constructor and destructor
    Dataset(const char* filename, const char* groupname, bool create_file_if_needed = true);
    ~Dataset();
    
    // Methods
    // XML Header
    int writeHeader(const std::string &xmlstring);
    int readHeader(std::string& xmlstring);
    // Acquisitions
    int appendAcquisition(const Acquisition &acq);
    int readAcquisition(uint32_t index, Acquisition &acq);
    uint32_t getNumberOfAcquisitions();
    // Images
    int appendImage(const std::string &var, const ISMRMRD_BlockModes blockmode, const Image &im);
    int readImage(const std::string &var, uint32_t index, Image &im);
    uint32_t getNumberOfImages(const std::string &var);
    // NDArrays
    int appendNDArray(const std::string &var, const ISMRMRD_BlockModes blockmode, const NDArray &arr);
    int readNDArray(const std::string &var, uint32_t index, NDArray &arr);
    uint32_t getNumberOfNDArrays(const std::string &var);

protected:
    ISMRMRD_Dataset dset_;
};


} /* ISMRMRD namespace */
#endif

#endif /* ISMRMRD_DATASET_H */

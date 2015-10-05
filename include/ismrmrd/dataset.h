/// @file dataset.h
#pragma once
#ifndef ISMRMRD_DATASET_H
#define ISMRMRD_DATASET_H

#include "ismrmrd/ismrmrd.h"

#include <H5Cpp.h>

#ifndef H5_NO_NAMESPACE
using namespace H5;
#endif

namespace ISMRMRD {

//  ISMRMRD Dataset C++ Interface
class EXPORTISMRMRD Dataset {
public:
    // Constructor and destructor
    Dataset(const char* filename, const char* groupname, bool create_file_if_needed=true, bool read_only=false);
    ~Dataset();

    // Helper functions
    static bool exists(const char* filename);

    // XML Header
    void writeHeader(const std::string &xmlstring);
    void readHeader(std::string& xmlstring);

    // Acquisitions
    void appendAcquisition(const Acquisition& acq);
    void readAcquisition(uint16_t stream_number, uint32_t index, Acquisition &acq);
    uint32_t getNumberOfAcquisitions();

    // Images
    template <typename T> void appendImage(const std::string &var, const Image<T> &im);
    template <typename T> void readImage(const std::string &var, uint32_t index, Image<T> &im);
    uint32_t getNumberOfImages(const std::string &var);

    // NDArrays
    template <typename T> void appendNDArray(const std::string &var, const NDArray<T> &arr);
    template <typename T> void readNDArray(const std::string &var, uint32_t index, NDArray<T> &arr);
    uint32_t getNumberOfNDArrays(const std::string &var);

protected:
    std::string filename_;
    std::string groupname_;
    std::string xml_header_path_;
    std::string data_path_;

    bool file_open_;
    bool file_exists_;
    bool create_file_if_needed_;
    bool dataset_open_;

    std::unique_ptr<H5File> file_;
    std::unique_ptr<DataSet> dataset_;
};

} /* namespace ISMRMRD */

#endif /* ISMRMRD_DATASET_H */

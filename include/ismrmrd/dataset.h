/// @file dataset.h
#pragma once
#ifndef ISMRMRD_DATASET_H
#define ISMRMRD_DATASET_H

#include "ismrmrd/ismrmrd.h"

#include <H5Cpp.h>

#include <map>

#ifndef H5_NO_NAMESPACE
using namespace H5;
#endif

namespace ISMRMRD {

//  ISMRMRD Dataset C++ Interface
class EXPORTISMRMRD Dataset {
public:
    // Constructor and destructor
    Dataset(const std::string& filename, const std::string& groupname,
            bool create_file_if_needed=true, bool read_only=false);
    ~Dataset();

    // XML Header
    void writeHeader(const std::string &xmlstring);
    std::string readHeader();

    // Acquisitions
    void appendAcquisition(const Acquisition& acq, int stream_number=-1);
    Acquisition readAcquisition(unsigned long index, int stream_number=-1);
    unsigned long getNumberOfAcquisitions(int stream_number=-1);

    // Images
    template <typename T> void appendImage(const std::string &var, const Image<T> &im);
    template <typename T> Image<T> readImage(const std::string &var, unsigned long index);
    unsigned long getNumberOfImages(const std::string &var);

    // NDArrays
    template <typename T> void appendNDArray(const std::string &var, const NDArray<T> &arr);
    template <typename T> NDArray<T> readNDArray(const std::string &var, unsigned long index);
    unsigned long getNumberOfNDArrays(const std::string &var);

protected:
    bool linkExists(const std::string& path);
    void createGroup(const std::string& path);
    std::string constructDataPath(unsigned int stream_number);
    void makeIndex();
    size_t appendToDataSet(const std::string& path, const DataType& dtype,
            const std::vector<hsize_t>& dims, void* data);
    void readFromDataSet(const std::string& path, const DataType& dtype,
            const std::vector<hsize_t>& entry_dims, size_t index, void* data);


    std::string filename_;
    std::string groupname_;
    std::string xml_header_path_;
    std::string data_path_;
    std::string index_path_;

    std::map<std::string, DataSet> datasets_;

    bool read_only_;
    bool file_open_;
    bool dataset_open_;

    std::unique_ptr<H5File> file_;
    //std::unique_ptr<DataSet> dataset_;
};

} /* namespace ISMRMRD */

#endif /* ISMRMRD_DATASET_H */

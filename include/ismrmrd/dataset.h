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
    // TODO 1. rename writeHeader into writeXmlHeader, and readHeader into readXmlHeader
    void writeHeader(const std::string &xmlstring);
    std::string readHeader();

    // Acquisitions
    // TODO 2. Since we are planning for multiple stream numbers for acquisitions, the concept of default stream
    //         number may be difficult to implement. Hence, propose to remove the -1 option, and require a specific
    //         value instead.
    template <typename T> void appendAcquisition(const Acquisition<T>& acq, int stream_number=-1);
    template <typename T> Acquisition<T> readAcquisition(unsigned long index, int stream_number=-1);
    unsigned long getNumberOfAcquisitions(int stream_number=-1);

    // Images
    // TODO 3. Resolve potential stream number ambiguity. Replace the string type variable with stream number.
    template <typename T> void appendImage(const std::string &var, const Image<T> &im);
    template <typename T> Image<T> readImage(const std::string &var, unsigned long index);
    unsigned long getNumberOfImages(const std::string &var);

    // NDArrays
    // TODO 4. Resolve potential stream number ambiguity. Should this data also be identifiable
    //         by a stream number or should this be a special case?
    template <typename T> void appendNDArray(const std::string &var, const NDArray<T> &arr);
    template <typename T> NDArray<T> readNDArray(const std::string &var, unsigned long index);
    unsigned long getNumberOfNDArrays(const std::string &var);

protected:
    bool linkExists(const std::string& path);
    void createGroup(const std::string& path);
    // TODO 5. Change constructDataPath to accept existing path and an addition string
    std::string constructDataPath(unsigned int stream_number);
    size_t appendToDataSet(const std::string& path, const DataType& dtype,
            const std::vector<hsize_t>& dims, void* data);
    void readFromDataSet(const std::string& path, const DataType& dtype,
            const std::vector<hsize_t>& entry_dims, size_t index, void* data);


    std::string filename_;
    std::string groupname_;
    std::string xml_header_path_;
    // TODO 6. Not sure if data_path_ needs to be a member. If it does, then we might need to change it and add
    //         unique names for paths to acquisitions, waveforms, images, and et cetera
    std::string data_path_;
    std::string index_path_;

    // TODO 7. The word dataset has three meanings in this library: the name of the library, the groupname(s),
    //         and the meaning in the context of the H5Cpp library which refers to a directory within the data
    //         hierarchy. Should we rename datasets_ into directories_? Or nodes_?
    std::map<std::string, DataSet> datasets_;

    bool read_only_;
    bool file_open_;
    // TODO 8. dataset_open_ is not used and probably not needed.
    bool dataset_open_;

    H5File file_;
};

// TODO 9. Add append, read, and getNumberOf for waveform.
// TODO 10. Consider simulteneous access to the same file by multiple applications.
// TODO 11. Consider method "IndexEntry getNextIndexEntry (uint32_t prev_entry);" for playback functionality.

} /* namespace ISMRMRD */

#endif /* ISMRMRD_DATASET_H */

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

    // TODO: Thread safety.

    public:
    // Constructor and destructor
    Dataset(const std::string& filename, const std::string& groupname,
            bool create_file_if_needed=true, bool read_only=false);
    ~Dataset();

    // XML Header
    void writeXmlHeader(const std::string &xmlstring);
    std::string readXmlHeader();

    uint32_t getNumberOfEntities(StreamId stream);

    // Acquisitions
    template <typename T>
    void appendAcquisition(const Acquisition<T>& acq, StreamId stream = STREAM_NONE);
    template <typename T>
    Acquisition<T> readAcquisition(unsigned long index, StreamId stream = STREAM_NONE);
    unsigned long getNumberOfAcquisitions(StreamId stream = STREAM_NONE);

    // Images
    template <typename T> void appendImage(const Image<T> &im, StreamId stream = STREAM_NONE);
    template <typename T> Image<T> readImage(unsigned long index, StreamId stream = STREAM_NONE);
    unsigned long getNumberOfImages(StreamId stream = STREAM_NONE);

    // Waveforms
    template <typename T>
    void appendWaveform(const Waveform<T> &wf, StreamId stream = STREAM_NONE);
    template <typename T>
    Waveform<T> readWaveform(unsigned long index, StreamId stream = STREAM_NONE);
    unsigned long getNumberOfWaveforms(StreamId stream = STREAM_NONE);

    // NDArrays
    template <typename T> void appendNDArray(const NDArray<T> &arr, StreamId stream = STREAM_NONE);
    template <typename T> NDArray<T> readNDArray(unsigned long index, StreamId stream = STREAM_NONE);
    unsigned long getNumberOfNDArrays(StreamId stream = STREAM_NONE);

    bool getIndexEntry (uint32_t entry_number, IndexEntry& entry);

    private:

    bool linkExists(const std::string& path);
    void createGroup(const std::string& path);
    std::string constructPath(std::string base, StreamId stream);
    std::string constructPath(std::string base, std::string addition);
    size_t appendToDataSet(const std::string& path, const DataType& dtype,
            const std::vector<hsize_t>& dims, void* data);
    void readFromDataSet(const std::string& path, const DataType& dtype,
            const std::vector<hsize_t>& entry_dims, size_t index, void* data);
    void updateIndexTable();
    void addIndexEntry(uint32_t index, StreamId stream, uint32_t substream = 0);


    std::string filename_;
    std::string groupname_;
    std::string xml_header_group_path_;
    std::string xml_header_data_path_;
    std::string index_path_;

    std::map<std::string, DataSet> h5_datasets_;
    std::vector<IndexEntry> index_table_;

    bool read_only_;
    bool file_open_;
    bool index_table_updated_;

    H5File file_;
};


} /* namespace ISMRMRD */

#endif /* ISMRMRD_DATASET_H */

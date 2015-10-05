#include "ismrmrd/dataset.h"

#include <boost/algorithm/string.hpp>

#include <fstream>

namespace ISMRMRD {
//
// Dataset class implementation
//
// Constructor
Dataset::Dataset(const char* filename, const char* groupname, bool create_file_if_needed, bool read_only)
  : filename_(filename),
    groupname_(groupname),
    read_only_(read_only),
    file_open_(false),
    dataset_open_(false)
{
    std::ifstream ifile(filename_.c_str());
    bool file_exists = ifile.is_open();

    int open_flag = read_only ? H5F_ACC_RDONLY : H5F_ACC_RDWR;

    if (file_exists) {
        if (!H5File::isHdf5(filename_.c_str())) {
            throw std::runtime_error("Not an HDF file");
        }
        // TODO: use std::make_unique
        file_ = std::unique_ptr<H5File>(new H5File(filename_, open_flag));
    } else if (create_file_if_needed) {
        file_ = std::unique_ptr<H5File>(new H5File(filename_, H5F_ACC_TRUNC));
        // We will close and then immediately open the file again.
        // We need to make sure the file is saved as an HDF5 file in case other processes and functions
        // need to access it immediately. The line above does not cause the file to be marked as and HDF5 file.
        // H5File::isHdf5(filename_.c_str()) will return false at this point.
        file_->close();
        file_ = std::unique_ptr<H5File>(new H5File(filename_, open_flag));
        file_open_ = true;
    } else {
        throw std::runtime_error("file not found");
    }

    if (!linkExists(groupname)) {
        createGroup(groupname);
    }

    xml_header_path_ = groupname_ + "/xml";
    data_path_ = groupname_ + "/data";
}

// Destructor
Dataset::~Dataset()
{ }

bool Dataset::linkExists(const std::string& path)
{
    std::vector<std::string> elements;
    std::string delimiter("/");
    boost::split(elements, path, boost::is_any_of("/"));
    std::string current_path;
    for (auto it = elements.begin(); it != elements.end(); ++it) {
        if (it->size() > 0) {
            current_path += "/" + *it;
            if (!H5Lexists(file_->getId(), current_path.c_str(), H5P_DEFAULT)) {
                return false;
            }
        }
    }
    return true;
}

void Dataset::createGroup(const std::string& path)
{
    std::vector<std::string> elements;
    boost::split(elements, path, boost::is_any_of("/"));
    std::string current_path;
    for (auto it = elements.begin(); it != elements.end(); ++it) {
        if (it->size() > 0) {
            current_path += "/" + *it;
            if (!H5Lexists(file_->getId(), current_path.c_str(), H5P_DEFAULT)) {
                file_->createGroup(current_path.c_str());
            }
        }
    }
}

// XML Header
void Dataset::writeHeader(const std::string& xml)
{
    std::vector<hsize_t> dims(1, 1);
    std::vector<hsize_t> max_dims(1, 1);

    std::unique_ptr<DataSet> xml_dataset;
    auto datatype = std::unique_ptr<DataType>(new StrType(0, H5T_VARIABLE));

    if (!linkExists(xml_header_path_)) {
        DataSpace mspace1(dims.size(), &dims[0], &max_dims[0]);
        xml_dataset = std::unique_ptr<DataSet>(
                new DataSet(file_->createDataSet(xml_header_path_.c_str(), *datatype, mspace1)));
    } else {
        xml_dataset = std::unique_ptr<DataSet>(
                new DataSet(file_->openDataSet(xml_header_path_.c_str())));
        DataType mtype = xml_dataset->getDataType();
        // no `!=` operator for DataType so we use (!(expr)) here
        if (!(mtype == *datatype)) {
            throw std::runtime_error("conflicting HDF5 types for XML header");
        }
    }

    DataSpace mspace1 = xml_dataset->getSpace();
    xml_dataset->write(xml, *datatype, mspace1);
}

std::string Dataset::readHeader()
{
    if (!linkExists(xml_header_path_)) {
        throw std::runtime_error("XML header not found in file");
    }

    std::unique_ptr<DataSet> xml_dataset(new DataSet(file_->openDataSet(xml_header_path_.c_str())));
    auto datatype = std::unique_ptr<DataType>(new StrType(0, H5T_VARIABLE));

    DataType dt = xml_dataset->getDataType();

    std::string xml;
    xml_dataset->read(xml, dt);

    return xml;
}

void Dataset::appendAcquisition(const Acquisition& acq)
{
    /* OLD */
    /* int status = ismrmrd_append_acquisition(&dset_, acq); */
    /* if (status != ISMRMRD_NOERROR) { */
    /*     throw std::runtime_error(build_exception_string()); */
    /* } */
}

Acquisition Dataset::readAcquisition(uint16_t stream_number, uint32_t index) {
    /* OLD */
    /* int status = ismrmrd_read_acquisition(&dset_, stream_number, index, &acq.acq); */
    /* if (status != ISMRMRD_NOERROR) { */
    /*     throw std::runtime_error(build_exception_string()); */
    /* } */
    return std::move(Acquisition());
}

unsigned long Dataset::getNumberOfAcquisitions(uint16_t stream_number)
{
    unsigned long nacq = 0;

    if (!linkExists(data_path_)) {
        throw std::runtime_error("Data path does not exist in dataset");
    }

    // TODO: implement

    return nacq;
}

template <typename T> void Dataset::appendImage(const std::string& var, const Image<T>& im)
{
    // TODO: implement
}

template EXPORTISMRMRD void Dataset::appendImage(const std::string& var, const Image<uint16_t>& im);
template EXPORTISMRMRD void Dataset::appendImage(const std::string& var, const Image<int16_t>& im);
template EXPORTISMRMRD void Dataset::appendImage(const std::string& var, const Image<uint32_t>& im);
template EXPORTISMRMRD void Dataset::appendImage(const std::string& var, const Image<int32_t>& im);
template EXPORTISMRMRD void Dataset::appendImage(const std::string& var, const Image<float>& im);
template EXPORTISMRMRD void Dataset::appendImage(const std::string& var, const Image<double>& im);
template EXPORTISMRMRD void Dataset::appendImage(const std::string& var, const Image<std::complex<float> >& im);
template EXPORTISMRMRD void Dataset::appendImage(const std::string& var, const Image<std::complex<double> >& im);


template <typename T>
Image<T> Dataset::readImage(const std::string& var, uint32_t index)
{
    // TODO: implement
    return std::move(Image<T>());
}

template EXPORTISMRMRD Image<uint16_t> Dataset::readImage(const std::string& var, uint32_t index);
template EXPORTISMRMRD Image<int16_t> Dataset::readImage(const std::string& var, uint32_t index);
template EXPORTISMRMRD Image<uint32_t> Dataset::readImage(const std::string& var, uint32_t index);
template EXPORTISMRMRD Image<int32_t> Dataset::readImage(const std::string& var, uint32_t index);
template EXPORTISMRMRD Image<float> Dataset::readImage(const std::string& var, uint32_t index);
template EXPORTISMRMRD Image<double> Dataset::readImage(const std::string& var, uint32_t index);
template EXPORTISMRMRD Image<std::complex<float> > Dataset::readImage(const std::string& var, uint32_t index);
template EXPORTISMRMRD Image<std::complex<double> > Dataset::readImage(const std::string& var, uint32_t index);

unsigned long Dataset::getNumberOfImages(const std::string& var)
{
    unsigned long nimg = 0;

    // TODO: implement

    return nimg;
}

template <typename T> void Dataset::appendNDArray(const std::string& var, const NDArray<T>& arr)
{
    // TODO: implement
}

template EXPORTISMRMRD void Dataset::appendNDArray(const std::string& var, const NDArray<uint16_t>& arr);
template EXPORTISMRMRD void Dataset::appendNDArray(const std::string& var, const NDArray<int16_t>& arr);
template EXPORTISMRMRD void Dataset::appendNDArray(const std::string& var, const NDArray<uint32_t>& arr);
template EXPORTISMRMRD void Dataset::appendNDArray(const std::string& var, const NDArray<int32_t>& arr);
template EXPORTISMRMRD void Dataset::appendNDArray(const std::string& var, const NDArray<float>& arr);
template EXPORTISMRMRD void Dataset::appendNDArray(const std::string& var, const NDArray<double>& arr);
template EXPORTISMRMRD void Dataset::appendNDArray(const std::string& var, const NDArray<std::complex<float> >& arr);
template EXPORTISMRMRD void Dataset::appendNDArray(const std::string& var, const NDArray<std::complex<double> >& arr);

template <typename T>
NDArray<T> Dataset::readNDArray(const std::string& var, uint32_t index)
{
    // TODO: implement
    return std::move(NDArray<T>());
}

template EXPORTISMRMRD NDArray<uint16_t> Dataset::readNDArray(const std::string& var, uint32_t index);
template EXPORTISMRMRD NDArray<int16_t> Dataset::readNDArray(const std::string& var, uint32_t index);
template EXPORTISMRMRD NDArray<uint32_t> Dataset::readNDArray(const std::string& var, uint32_t index);
template EXPORTISMRMRD NDArray<int32_t> Dataset::readNDArray(const std::string& var, uint32_t index);
template EXPORTISMRMRD NDArray<float> Dataset::readNDArray(const std::string& var, uint32_t index);
template EXPORTISMRMRD NDArray<double> Dataset::readNDArray(const std::string& var, uint32_t index);
template EXPORTISMRMRD NDArray<std::complex<float> > Dataset::readNDArray(const std::string& var, uint32_t index);
template EXPORTISMRMRD NDArray<std::complex<double> > Dataset::readNDArray(const std::string& var, uint32_t index);

unsigned long Dataset::getNumberOfNDArrays(const std::string& var)
{
    unsigned long narr = 0;

    // TODO: implement

    return narr;
}

} // namespace ISMRMRD

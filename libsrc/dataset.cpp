#include "ismrmrd/dataset.h"
#include "ismrmrd/dataset_dtypes.h"

#include <boost/algorithm/string.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

namespace ISMRMRD {


/**********************************************************************************************************************/
Dataset::Dataset(const std::string& filename, const std::string& groupname, bool create_file_if_needed, bool read_only)
  : filename_(filename),
    groupname_(groupname),
    read_only_(read_only),
    file_open_(false),
    index_table_updated_(false)
{
    std::ifstream ifile(filename_.c_str());
    bool file_exists = ifile.is_open();

    int open_flag = read_only ? H5F_ACC_RDONLY : H5F_ACC_RDWR;

    if (file_exists) {
        if (!H5File::isHdf5(filename_.c_str())) {
            throw std::runtime_error("Not an HDF file");
        }
        file_ = H5File(filename_, open_flag);
    } else if (create_file_if_needed) {
        file_ = H5File(filename_, H5F_ACC_TRUNC);
        // We will close and then immediately open the file again.
        // We need to make sure the file is saved as an HDF5 file in case other processes and functions
        // need to access it immediately. The line above does not cause the file to be marked as and HDF5 file.
        // H5File::isHdf5(filename_.c_str()) will return false at this point.
        file_.close();
        file_ = H5File(filename_, open_flag);
        file_open_ = true;
    } else {
        throw std::runtime_error("file not found");
    }

    xml_header_group_path_ = constructPath(groupname_, STREAM_HEADER);
    xml_header_data_path_  = constructPath(xml_header_group_path_, "xml");
    index_path_            = constructPath(groupname_, "index");

    updateIndexTable();
}

/**********************************************************************************************************************/
Dataset::~Dataset()
{
    file_.close();
}

/**********************************************************************************************************************/
bool Dataset::linkExists(const std::string& path)
{
    std::vector<std::string> elements;
    std::string delimiter("/");
    boost::split(elements, path, boost::is_any_of("/"));
    std::string current_path;
    for (std::vector<std::string>::const_iterator it = elements.begin(); it != elements.end(); ++it) {
        if (it->size() > 0) {
            current_path += "/" + *it;
            if (!H5Lexists(file_.getId(), current_path.c_str(), H5P_DEFAULT)) {
                return false;
            }
        }
    }
    return true;
}

/**********************************************************************************************************************/
void Dataset::createGroup(const std::string& path)
{
    std::vector<std::string> elements;
    boost::split(elements, path, boost::is_any_of("/"));
    std::string current_path;
    for (std::vector<std::string>::const_iterator it = elements.begin(); it != elements.end(); ++it) {
        if (it->size() > 0) {
            current_path += "/" + *it;
            if (!H5Lexists(file_.getId(), current_path.c_str(), H5P_DEFAULT)) {
                file_.createGroup(current_path.c_str());
            }
        }
    }
}

/**********************************************************************************************************************/
std::string Dataset::constructPath(std::string base, StreamId stream)
{
    return base + "/" + streamIdToString(stream);
}

/**********************************************************************************************************************/
std::string Dataset::constructPath(std::string base, std::string addition)
{
    return base + "/" + addition;
}

/**********************************************************************************************************************/
// XML Header
void Dataset::writeXmlHeader(const std::string& xml)
{
    std::vector<hsize_t> dims(1, 1);
    std::vector<hsize_t> max_dims(1, 1);

    DataSet xml_dataset;
    DataType datatype = StrType(0, H5T_VARIABLE);

    if (!linkExists(xml_header_group_path_)) {
        createGroup(xml_header_group_path_);
        DataSpace mspace1(dims.size(), &dims[0], &max_dims[0]);
        xml_dataset = DataSet(file_.createDataSet(xml_header_data_path_.c_str(), datatype, mspace1));

    } else {
        xml_dataset = DataSet(file_.openDataSet(xml_header_data_path_.c_str()));
        DataType mtype = xml_dataset.getDataType();
        // no `!=` operator for DataType so we use (!(expr)) here
        if (!(mtype == datatype)) {
            throw std::runtime_error("conflicting HDF5 types for XML header");
        }
    }

    // TODO 4. What happens if some header already exists there?
    //         Should we allow multiple headers? name xml_timestamp?
    DataSpace mspace1 = xml_dataset.getSpace();
    xml_dataset.write(xml, datatype, mspace1);

    addIndexEntry( getNumberOfEntities( STREAM_HEADER), STREAM_HEADER);
}

/**********************************************************************************************************************/
std::string Dataset::readXmlHeader()
{
    if (!linkExists(xml_header_data_path_)) {
        throw std::runtime_error("XML header not found in file");
    }

    // TODO 5. (related to todo #4) What to do if multiple headers?
    DataSet xml_dataset = file_.openDataSet(xml_header_data_path_.c_str());
    DataType datatype = StrType(0, H5T_VARIABLE);

    DataType dt = xml_dataset.getDataType();

    std::string xml;
    xml_dataset.read(xml, dt);

    return xml;
}

/**********************************************************************************************************************/
template <typename T> void Dataset::appendAcquisition(const Acquisition<T>& acq, StreamId stream)
{
    std::string group_path = constructPath(groupname_, stream);
    std::string data_path = constructPath(group_path, "data");
    if (!linkExists(group_path))
    {
      createGroup(group_path);
    }

    std::vector<hsize_t> dims(2, 1);
    DataType dtype = get_hdf5_data_type<AcquisitionHeader_with_data<T> >();

    AcquisitionHeader_with_data<T> obj;
    obj.head = acq.getHead();
    obj.traj.len = acq.getTrajectoryDimensions() * acq.getNumberOfSamples();
    obj.traj.p = const_cast<float*>(&acq.getTraj()[0]);
    obj.data.len = acq.getActiveChannels() * acq.getNumberOfSamples() * 2;

    std::vector<T> fdata;
    std::vector<std::complex<T> > data = acq.getData();
    for (typename std::vector<std::complex<T> >::const_iterator it = data.begin(); it != data.end(); ++it) {
        fdata.push_back(it->real());
        fdata.push_back(it->imag());
    }
    obj.data.p = const_cast<T*>(&fdata[0]);

    // add the acquisition to its correct stream dataset
    // note: subtract 1 to account for zero-indexing
    unsigned int acquisition_number = appendToDataSet(data_path, dtype, dims, &obj) - 1;
    addIndexEntry(acquisition_number, stream);
}

/**********************************************************************************************************************/
//Template instanciations
template Acquisition<int16_t> Dataset::readAcquisition<int16_t>(unsigned long, StreamId);
template Acquisition<int32_t> Dataset::readAcquisition<int32_t>(unsigned long, StreamId);
template Acquisition<float> Dataset::readAcquisition<float>(unsigned long, StreamId);
template Acquisition<double> Dataset::readAcquisition<double>(unsigned long, StreamId);


/**********************************************************************************************************************/
size_t Dataset::appendToDataSet(const std::string& path, const DataType& dtype,
        const std::vector<hsize_t>& dims, void* data)
{
    bool first_entry = false;

    // check if the dataset is already open
    if (h5_datasets_.count(path) == 0) {
        // check if the dataset even exists
        if (linkExists(path)) {
            h5_datasets_[path] = file_.openDataSet(path.c_str());
        } else {
            first_entry = true;

            std::vector<hsize_t> max_dims = dims;
            max_dims[0] = H5S_UNLIMITED;

            DataSpace space(dims.size(), &dims[0], &max_dims[0]);

            DSetCreatPropList cparms;
            cparms.setChunk(dims.size(), &dims[0]);

            h5_datasets_[path] = file_.createDataSet(path.c_str(), dtype, space, cparms);
        }
    }

    DataSet dset = h5_datasets_[path];

    unsigned int rank = dims.size();
    // check rank of existing dataset
    DataSpace mspace = dset.getSpace();
    unsigned int cur_rank = mspace.getSimpleExtentNdims();
    if (cur_rank != rank) {
        throw std::runtime_error("incorrect rank in HDF5 dataset");
    }

    size_t length = 1;
    // extend current dataset and determine offset for new entry
    std::vector<hsize_t> offset(rank, 0);
    if (!first_entry) {
        std::vector<hsize_t> ddims(rank, 1);
        mspace.getSimpleExtentDims(&ddims[0], NULL);
        offset[0] = ddims[0];
        length = ++ddims[0];
        dset.extend(&ddims[0]);
    }

    // select space in file for new entry
    DataSpace fspace = dset.getSpace();
    fspace.selectHyperslab(H5S_SELECT_SET, &dims[0], &offset[0]);

    // append entry to index dataset
    mspace = DataSpace(rank, &dims[0]);
    dset.write(data, dtype, mspace, fspace);

    return length;
}

/**********************************************************************************************************************/
void Dataset::readFromDataSet(const std::string& path, const DataType& dtype,
        const std::vector<hsize_t>& entry_dims, size_t index, void* data)
{
    if (!linkExists(path)) {
        throw std::runtime_error("Data path does not exist in dataset");
    }

    if (h5_datasets_.count(path) == 0) {
        h5_datasets_[path] = file_.openDataSet(path);
    }

    DataSet dset = h5_datasets_[path];

    DataType actual_dtype = dset.getDataType();
    // no `!=` operator for DataType so we use (!(a == b)) here
    if (!(actual_dtype == dtype)) {
        throw std::runtime_error("Attempting to open HDF5 Dataset with incorrect datatype");
    }

    DataSpace dspace = dset.getSpace();
    unsigned int rank = dspace.getSimpleExtentNdims();
    std::vector<hsize_t> dims(rank);
    dspace.getSimpleExtentDims(&dims[0]);

    if (index >= dims[0]) {
        //TODO: remove debug code
        std::cout << "readFromDataSet: data_path = " << path << ", index = " << index
                  << ", num entities = " << dims[0] << "\n";
        throw std::runtime_error("Attempting to access non-existent hyperslice in HDF5 dataset");
    }

    std::vector<hsize_t> offset(rank);
    offset[0] = index;

    dspace.selectHyperslab(H5S_SELECT_SET, &entry_dims[0], &offset[0]);
    DataSpace mspace(rank, &entry_dims[0]);

    dset.read(data, dtype, mspace, dspace, H5P_DEFAULT);
}

/**********************************************************************************************************************/
template <typename T>
Acquisition<T> Dataset::readAcquisition(unsigned long index, StreamId stream)
{
    std::string group_path = constructPath(groupname_, stream);
    std::string data_path  = constructPath(group_path, "data");

    if (!linkExists(data_path))
    {
      throw std::runtime_error("Acquisition path not found");
    }

    AcquisitionHeader_with_data<T> obj;
    DataType dtype = get_hdf5_data_type<AcquisitionHeader_with_data<T> >();
    std::vector<hsize_t> entry_dims(2, 1);
    readFromDataSet(data_path, dtype, entry_dims, index, &obj);

    Acquisition<T> acq;
    acq.setHead(obj.head);

    if (obj.data.len != acq.getData().size() * 2) {
        throw std::runtime_error("Acquisition Header does not match data length in file");
    }
    if (obj.traj.len != acq.getTraj().size()) {
        throw std::runtime_error("Acquisition Header does not match trajectory length in file");
    }

    // TODO: fix this ASAP:
    memcpy(const_cast<std::complex<T>*>(&acq.getData()[0]), obj.data.p, sizeof(T) * obj.data.len);
    memcpy(const_cast<float*>(&acq.getTraj()[0]), obj.traj.p, sizeof(float) * obj.traj.len);

    free(obj.data.p);
    free(obj.traj.p);

    return acq;
}

/**********************************************************************************************************************/
template void Dataset::appendAcquisition(const Acquisition<int16_t>&, StreamId);
template void Dataset::appendAcquisition(const Acquisition<int32_t>&, StreamId);
template void Dataset::appendAcquisition(const Acquisition<float>&, StreamId);
template void Dataset::appendAcquisition(const Acquisition<double>&, StreamId);

/**********************************************************************************************************************/
unsigned long Dataset::getNumberOfAcquisitions(StreamId stream)
{
  return getNumberOfEntities (stream);
}

/**********************************************************************************************************************/
uint32_t Dataset::getNumberOfEntities(StreamId stream)
{
  std::string group_path = constructPath(groupname_, stream);
  std::string data_path = constructPath(group_path, "data");
  if (!linkExists(data_path))
  {
    return 0;
  }

  if (h5_datasets_.count(data_path) == 0)
  {
    h5_datasets_[data_path] = file_.openDataSet(data_path);
  }
  DataSet dataset = h5_datasets_[data_path];

  DataSpace space = dataset.getSpace();
  int rank = space.getSimpleExtentNdims();
  std::vector<hsize_t> dims(rank, 0);
  space.getSimpleExtentDims(&dims[0]);

  return (uint32_t)dims[0];
}

/**********************************************************************************************************************/
void Dataset::addIndexEntry (uint32_t index, StreamId stream, uint32_t substream)
{
  DataType dtype = get_hdf5_data_type<IndexEntry>();
  std::vector<hsize_t> dims = std::vector<hsize_t>(2, 1);
  IndexEntry entry;
  entry.stream = stream;
  entry.substream = substream;
  entry.index = index;
  appendToDataSet(index_path_, dtype, dims, &entry);
  index_table_updated_ = false;

}

/**********************************************************************************************************************/
bool Dataset::getIndexEntry (uint32_t entry_number, IndexEntry& entry)
{
  if (entry_number < index_table_.size())
  {
    entry = index_table_[entry_number];
    return true;
  }

  if (!index_table_updated_)
  {
    updateIndexTable();
    if (entry_number < index_table_.size())
    {
      entry = index_table_[entry_number];
      return true;
    }
  }

  return false;
}

/**********************************************************************************************************************/void Dataset::updateIndexTable()
{
  if (!linkExists(index_path_))
  {
    index_table_updated_ = true;
    return;
  }

  if (h5_datasets_.count(index_path_) == 0)
  {
    h5_datasets_[index_path_] = file_.openDataSet(index_path_);
  }

  DataSet dataset = h5_datasets_[index_path_];

  DataSpace space = dataset.getSpace();
  int rank = space.getSimpleExtentNdims();
  std::vector<hsize_t> dims(rank);
  space.getSimpleExtentDims(&dims[0]);

  index_table_.reserve(dims[0]);
  DataType dtype = get_hdf5_data_type<IndexEntry>();
  dataset.read(&index_table_[0], dtype);

  index_table_updated_ = true;
}

/**********************************************************************************************************************/
template <typename T> void Dataset::appendImage(const Image<T>& im, StreamId stream)
{
    // TODO 8. Replace string type argument with stream number.
    //         Construct path for image.
    //         Write attribute string
    //         Write image header
    //         Write image data
    //         ?? Need to determine index value for an image within a group of images, and add to index table.
}

/**********************************************************************************************************************/
template EXPORTISMRMRD void Dataset::appendImage(const Image<uint16_t>&, StreamId);
template EXPORTISMRMRD void Dataset::appendImage(const Image<int16_t>&, StreamId);
template EXPORTISMRMRD void Dataset::appendImage(const Image<uint32_t>&, StreamId);
template EXPORTISMRMRD void Dataset::appendImage(const Image<int32_t>&, StreamId);
template EXPORTISMRMRD void Dataset::appendImage(const Image<float>&, StreamId);
template EXPORTISMRMRD void Dataset::appendImage(const Image<double>&, StreamId);
template EXPORTISMRMRD void Dataset::appendImage(const Image<std::complex<float> >&, StreamId);
template EXPORTISMRMRD void Dataset::appendImage(const Image<std::complex<double> >&, StreamId);

/**********************************************************************************************************************/
template <typename T>
Image<T> Dataset::readImage (unsigned long index, StreamId stream)
{
  // TODO 9. Need stream number.
  //         Construct path and read the image attribute, header and data.
  //         Initialize the Image<T> with the data read.
  return Image<T>();
}

/**********************************************************************************************************************/
template EXPORTISMRMRD Image<uint16_t> Dataset::readImage(unsigned long, StreamId);
template EXPORTISMRMRD Image<int16_t> Dataset::readImage(unsigned long, StreamId);
template EXPORTISMRMRD Image<uint32_t> Dataset::readImage(unsigned long, StreamId);
template EXPORTISMRMRD Image<int32_t> Dataset::readImage(unsigned long, StreamId);
template EXPORTISMRMRD Image<float> Dataset::readImage(unsigned long, StreamId);
template EXPORTISMRMRD Image<double> Dataset::readImage(unsigned long, StreamId);
template EXPORTISMRMRD Image<std::complex<float> > Dataset::readImage(unsigned long, StreamId);
template EXPORTISMRMRD Image<std::complex<double> > Dataset::readImage(unsigned long, StreamId);

/**********************************************************************************************************************/
unsigned long Dataset::getNumberOfImages(StreamId stream)
{
    unsigned long nimg = 0;

    // TODO 10. Need stream number. Parse index table to find the high index value for the stream.

    return nimg;
}

/**********************************************************************************************************************/
template <typename T> void Dataset::appendNDArray(const NDArray<T>& arr, StreamId stream)
{
    // TODO 11. Need stream number. Otherwise similar to appendImage minus header and attribute.
}

/**********************************************************************************************************************/
template EXPORTISMRMRD void Dataset::appendNDArray(const NDArray<uint16_t>&, StreamId);
template EXPORTISMRMRD void Dataset::appendNDArray(const NDArray<int16_t>&, StreamId);
template EXPORTISMRMRD void Dataset::appendNDArray(const NDArray<uint32_t>&, StreamId);
template EXPORTISMRMRD void Dataset::appendNDArray(const NDArray<int32_t>&, StreamId);
template EXPORTISMRMRD void Dataset::appendNDArray(const NDArray<float>&, StreamId);
template EXPORTISMRMRD void Dataset::appendNDArray(const NDArray<double>&, StreamId);
template EXPORTISMRMRD void Dataset::appendNDArray(const NDArray<std::complex<float> >&, StreamId);
template EXPORTISMRMRD void Dataset::appendNDArray(const NDArray<std::complex<double> >&, StreamId);

/**********************************************************************************************************************/
template <typename T>
NDArray<T> Dataset::readNDArray(unsigned long index, StreamId stream)
{
    // TODO 12. Need stream number. Otherwise similar to readImage minus header and attribute.
    return NDArray<T>();
}

/**********************************************************************************************************************/
template EXPORTISMRMRD NDArray<uint16_t> Dataset::readNDArray(unsigned long, StreamId);
template EXPORTISMRMRD NDArray<int16_t> Dataset::readNDArray(unsigned long, StreamId);
template EXPORTISMRMRD NDArray<uint32_t> Dataset::readNDArray(unsigned long, StreamId);
template EXPORTISMRMRD NDArray<int32_t> Dataset::readNDArray(unsigned long, StreamId);
template EXPORTISMRMRD NDArray<float> Dataset::readNDArray(unsigned long, StreamId);
template EXPORTISMRMRD NDArray<double> Dataset::readNDArray(unsigned long, StreamId);
template EXPORTISMRMRD NDArray<std::complex<float> > Dataset::readNDArray(unsigned long, StreamId);
template EXPORTISMRMRD NDArray<std::complex<double> > Dataset::readNDArray(unsigned long, StreamId);

/**********************************************************************************************************************/
unsigned long Dataset::getNumberOfNDArrays(StreamId stream)
{
    unsigned long narr = 0;

    // TODO 13. Similar to getNumberOfImages. Need stream number.

    return narr;
}

/**********************************************************************************************************************/

} // namespace ISMRMRD

#include "ismrmrd/dataset.h"
#include "ismrmrd/dataset_dtypes.h"

#include <boost/algorithm/string.hpp>

#include <fstream>
#include <sstream>

#include <iostream>

namespace ISMRMRD {


Dataset::Dataset
(
  const std::string& filename,
  const std::string& groupname,
  bool create_file_if_needed,
  bool read_only
)
  : filename_(filename),
    groupname_(groupname),
    read_only_(read_only),
    file_open_(false),
    dataset_open_(false)
{
    std::ifstream ifile (filename_.c_str());
    bool file_exists = ifile.is_open();

    int open_flag = read_only ? H5F_ACC_RDONLY : H5F_ACC_RDWR;

    if (file_exists)
    {
        if (!H5File::isHdf5(filename_.c_str()))
        {
            throw std::runtime_error("Not an HDF file");
        }
        file_ = H5File(filename_, open_flag);
    }
    else if (create_file_if_needed)
    {
        file_ = H5File(filename_, H5F_ACC_TRUNC);
        // We will close and then immediately open the file again.
        // We need to make sure the file is saved as an HDF5 file
        // in case other processes and functions
        // need to access it immediately. The line above does not
        // cause the file to be marked as an HDF5 file.
        // H5File::isHdf5(filename_.c_str()) will return false at this point.
        file_.close();
        file_ = H5File(filename_, open_flag);
        file_open_ = true;
    }
    else
    {
        throw std::runtime_error("file not found");
    }

    xml_header_path_ = groupname_   + "/xml";
    data_path_       = groupname_   + "/data";
    index_path_      = data_path_   + "/index";

    image_path_      = groupname_   + "/image";
    im_head_path_    = image_path_  + "/header";
    im_attr_path_    = image_path_  + "/attributes";
    //im_attr_path_    = image_path_  + "/ias";
    

    // TODO: maybe the following steps should only happen for new files??

    // create groups for /dataset and /dataset/data
    if (!linkExists(image_path_))
    {
        createGroup(image_path_);
    }
    if (!linkExists(data_path_))
    {
        createGroup(data_path_);
    }


}

// Destructor
Dataset::~Dataset()
{
    file_.close();
}

bool Dataset::linkExists(const std::string& path)
{
    std::vector<std::string> elements;
    std::string delimiter("/");
    boost::split(elements, path, boost::is_any_of("/"));
    std::string current_path;

    for (std::vector<std::string>::const_iterator it = elements.begin();
         it != elements.end(); ++it)
    {
        if (it->size() > 0)
        {
            current_path += "/" + *it;
            if (!H5Lexists(file_.getId(), current_path.c_str(), H5P_DEFAULT))
            {
                return false;
            }
        }
    }
    return true;
}

void Dataset::createGroup (const std::string& path)
{
    std::vector<std::string> elements;
    boost::split(elements, path, boost::is_any_of("/"));
    std::string current_path;
    for (std::vector<std::string>::const_iterator it = elements.begin();
         it != elements.end(); ++it)
    {
        if (it->size() > 0)
        {
            current_path += "/" + *it;
            if (!H5Lexists(file_.getId(), current_path.c_str(), H5P_DEFAULT))
            {
                file_.createGroup(current_path.c_str());
//std::cout << __func__ << ": created group = " << current_path << "\n";
            }
        }
    }
}

// This routine can be substituted with the constructPath
std::string Dataset::constructDataPath (unsigned int stream)
{
    // construct group path for this acquisition
    std::stringstream sstr;
    sstr << data_path_ << "/" << stream;
    return sstr.str();
}

std::string Dataset::constructPath (std::string base, unsigned int stream)
{
    // append stream number to the base path
    std::stringstream sstr;
    sstr << base << "/" << stream;
    return sstr.str();
}

/*
// May be needed if appending image with subgroup name rather than stream number
std::string Dataset::constructPath (std::string base, std::string addition)
{
    // construct group path by adding the path_part
    std::stringstream sstr;
    sstr << base << "/" << addition;
    return sstr.str();
}
*/


// XML Header
void Dataset::writeHeader(const std::string& xml)
{
    std::vector<hsize_t> dims(1, 1);
    std::vector<hsize_t> max_dims(1, 1);

    DataSet xml_dataset;
    DataType datatype = StrType(0, H5T_VARIABLE);

    if (!linkExists(xml_header_path_))
    {

        DataSpace mspace1(dims.size(), &dims[0], &max_dims[0]);
        xml_dataset = DataSet (file_.createDataSet(xml_header_path_.c_str(),
                                                   datatype, mspace1));
    }
    else
    {
        xml_dataset = DataSet(file_.openDataSet(xml_header_path_.c_str()));
        DataType mtype = xml_dataset.getDataType();
        // no `!=` operator for DataType so we use (!(expr)) here
        if (!(mtype == datatype))
        {
            throw std::runtime_error("conflicting HDF5 types for XML header");
        }
    }

    DataSpace mspace1 = xml_dataset.getSpace();
    xml_dataset.write(xml, datatype, mspace1);
}

std::string Dataset::readHeader ()
{
    if (!linkExists(xml_header_path_))
    {
        throw std::runtime_error("XML header not found in file");
    }

    DataSet xml_dataset = file_.openDataSet(xml_header_path_.c_str());
    DataType datatype = StrType(0, H5T_VARIABLE);

    DataType dt = xml_dataset.getDataType();

    std::string xml;
    xml_dataset.read(xml, dt);

    return xml;
}

/*******************************************************************************
 ******************************************************************************/
void Dataset::writeImageHeader
(
  void* data
)
{
  std::vector<hsize_t> dims(1, 1);
  std::vector<hsize_t> max_dims(1, 1);

  DataSet im_dataset;
  DataType dtype = get_hdf5_data_type<ImageHeader>();

  if (!linkExists(im_head_path_))
  {
    DataSpace mspace1(dims.size(), &dims[0], &max_dims[0]);
    im_dataset = DataSet (file_.createDataSet(im_head_path_.c_str(), dtype, mspace1));
    //std::cout <<__func__<<": Created dataset for " << im_head_path_ << "\n";
  }
  else
  {
    im_dataset = DataSet(file_.openDataSet(im_head_path_.c_str()));
    DataType mtype = im_dataset.getDataType();
    if (!(mtype == dtype))
    {
      throw std::runtime_error("conflicting HDF5 types for Image header");
    }
  }

  DataSpace mspace1 = im_dataset.getSpace();
  im_dataset.write(data, dtype, mspace1);
}


/*******************************************************************************
 ******************************************************************************/
ImageHeader Dataset::readImageHeader ()
{
  if (!linkExists(im_head_path_))
  {
    throw std::runtime_error("Image header not found in file");
  }

  DataSet im_dataset = file_.openDataSet(im_head_path_.c_str());
  DataType dt = im_dataset.getDataType();

  ImageHeader image_header;
  im_dataset.read((void*)&image_header, dt);

  return image_header;
}

/*******************************************************************************
 ******************************************************************************/
void Dataset::writeImageAttribute(const std::string& attr)
{
    std::vector<hsize_t> dims(1, 1);
    std::vector<hsize_t> max_dims(1, 1);

    DataSet attr_dataset;
    DataType datatype = StrType(0, H5T_VARIABLE);

    if (!linkExists(im_attr_path_))
    {
        DataSpace mspace1 (dims.size(), &dims[0], &max_dims[0]);
        attr_dataset = DataSet (file_.createDataSet(im_attr_path_.c_str(),
                                                   datatype, mspace1));
    }
    else
    {
        attr_dataset = DataSet(file_.openDataSet(im_attr_path_.c_str()));
        DataType mtype = attr_dataset.getDataType();
        if (!(mtype == datatype))
        {
            throw std::runtime_error("conflicting HDF5 types for Image Attribute");
        }
    }

    DataSpace mspace1 = attr_dataset.getSpace();
    attr_dataset.write(attr, datatype, mspace1);
}

/*******************************************************************************
 ******************************************************************************/
std::string Dataset::readImageAttribute ()
{
    if (!linkExists(im_attr_path_))
    {
        throw std::runtime_error("Image Attributes not found in file");
    }

    DataSet attr_dataset = file_.openDataSet(im_attr_path_.c_str());
    DataType dt = attr_dataset.getDataType();

    std::string attr;
    attr_dataset.read(attr, dt);

    return attr;
}

/*******************************************************************************
 ******************************************************************************/
template <typename T>
void Dataset::appendAcquisition
(
  const Acquisition<T>& acq,
  int                   stream
)
{
    if (stream < 0)
    {
        stream = acq.getStream();
    }

    std::string path = constructDataPath (stream);

    std::vector<hsize_t> dims(2, 1);
    DataType dtype = get_hdf5_data_type<AcquisitionHeader_with_data<T> >();

    AcquisitionHeader_with_data<T> obj;
    obj.head = acq.getHead();
    obj.traj.len = acq.getTrajectoryDimensions() * acq.getNumberOfSamples();
    obj.traj.p = const_cast<float*>(&acq.getTraj()[0]);
    obj.data.len = acq.getActiveChannels() * acq.getNumberOfSamples() * 2;

    std::vector<T> fdata;
    std::vector<std::complex<T> > data = acq.getData();
    for (typename std::vector<std::complex<T> >::const_iterator it = data.begin();
         it != data.end(); ++it)
    {
        fdata.push_back (it->real());
        fdata.push_back (it->imag());
    }
    obj.data.p = const_cast<T*>(&fdata[0]);

    // add the acquisition to its correct stream dataset
    // note: subtract 1 to account for zero-indexing
    unsigned int acquisition_number = appendToDataSet(path, dtype, dims, &obj) - 1;

    // now add an entry to the index for this acquisition
    dtype = get_hdf5_data_type<IndexEntry>();
    dims = std::vector<hsize_t>(2, 1);
    IndexEntry entry;
    entry.stream = stream;
    entry.index = acquisition_number;
    appendToDataSet(index_path_, dtype, dims, &entry);
}

/*******************************************************************************
//Template instanciations
 ******************************************************************************/
template Acquisition<int16_t> Dataset::readAcquisition<int16_t>(unsigned long, int);
template Acquisition<int32_t> Dataset::readAcquisition<int32_t>(unsigned long, int);
template Acquisition<float>   Dataset::readAcquisition<float>  (unsigned long, int);
template Acquisition<double>  Dataset::readAcquisition<double> (unsigned long, int);
    
/*******************************************************************************
 ******************************************************************************/
size_t Dataset::appendToDataSet
(
  const std::string& path,
  const DataType& dtype,
  const std::vector<hsize_t>& dims,
  void* data
)
{
    bool first_entry = false;

    // check if the dataset is already open
    if (datasets_.count(path) == 0)
    {
        // check if the dataset even exists
        if (linkExists(path))
        {
            datasets_[path] = file_.openDataSet(path.c_str());
        }
        else
        {
            first_entry = true;

            std::vector<hsize_t> max_dims = dims;
            max_dims[0] = H5S_UNLIMITED;

            DataSpace space(dims.size(), &dims[0], &max_dims[0]);

            DSetCreatPropList cparms;
            cparms.setChunk(dims.size(), &dims[0]);

            datasets_[path] = file_.createDataSet(path.c_str(), dtype, space, cparms);
        }
    }

    DataSet dset = datasets_[path];

    unsigned int rank = dims.size();
    // check rank of existing dataset
    DataSpace mspace = dset.getSpace();
    unsigned int cur_rank = mspace.getSimpleExtentNdims();
    if (cur_rank != rank)
    {
        throw std::runtime_error("incorrect rank in HDF5 dataset");
    }

    size_t length = 1;
    // extend current dataset and determine offset for new entry
    std::vector<hsize_t> offset(rank, 0);
    if (!first_entry)
    {
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

/*******************************************************************************
 ******************************************************************************/
void Dataset::readFromDataSet(const std::string& path, const DataType& dtype,
        const std::vector<hsize_t>& entry_dims, size_t index, void* data)
{
  if (!linkExists(path))
  {
    //std::cout << __func__ << ": path = " << path << "\n";
    throw std::runtime_error("Data path does not exist in dataset");
  }

    if (datasets_.count(path) == 0) {
        datasets_[path] = file_.openDataSet(path);
    }

    DataSet dset = datasets_[path];

    DataType actual_dtype = dset.getDataType();
    // no `!=` operator for DataType so we use (!(a == b)) here
    if (!(actual_dtype == dtype))
    {
      throw std::runtime_error
        ("Attempting to open HDF5 Dataset with incorrect datatype");
    }

    DataSpace dspace = dset.getSpace();
    unsigned int rank = dspace.getSimpleExtentNdims();
    std::vector<hsize_t> dims(rank);
    dspace.getSimpleExtentDims(&dims[0]);

    if (index >= dims[0])
    {
      throw std::runtime_error
        ("Attempting to access non-existent hyperslice in HDF5 dataset");
    }

    std::vector<hsize_t> offset(rank);
    offset[0] = index;

    dspace.selectHyperslab(H5S_SELECT_SET, &entry_dims[0], &offset[0]);
    DataSpace mspace(rank, &entry_dims[0]);

    dset.read(data, dtype, mspace, dspace, H5P_DEFAULT);
}

/*******************************************************************************
 ******************************************************************************/
template <typename T> Acquisition<T> Dataset::readAcquisition(unsigned long index, int stream)
{
    if (stream < 0) {
        IndexEntry entry;
        std::vector<hsize_t> entry_dims(2, 1);
        DataType dtype = get_hdf5_data_type<IndexEntry>();

        readFromDataSet(index_path_, dtype, entry_dims, index, &entry);

        stream = entry.stream;
        index = entry.index;
    }

    std::string path = constructDataPath(stream);

    AcquisitionHeader_with_data<T> obj;
    DataType dtype = get_hdf5_data_type<AcquisitionHeader_with_data<T> >();
    std::vector<hsize_t> entry_dims(2, 1);
    readFromDataSet(path, dtype, entry_dims, index, &obj);

    Acquisition<T> acq;
    acq.setHead(obj.head);

    if (obj.data.len != acq.getData().size() * 2) {
        throw std::runtime_error("Header does not match data length in file");
    }
    if (obj.traj.len != acq.getTraj().size()) {
        throw std::runtime_error("Header does not match trajectory length in file");
    }

    // TODO: fix this ASAP:
    memcpy(const_cast<std::complex<T>*>(&acq.getData()[0]), obj.data.p, sizeof(T) * obj.data.len);
    memcpy(const_cast<float*>(&acq.getTraj()[0]), obj.traj.p, sizeof(float) * obj.traj.len);

    free(obj.data.p);
    free(obj.traj.p);

    return acq;
}

/*******************************************************************************
 ******************************************************************************/
template void Dataset::appendAcquisition(const Acquisition<int16_t>&, int);
template void Dataset::appendAcquisition(const Acquisition<int32_t>&, int);
template void Dataset::appendAcquisition(const Acquisition<float>&, int);
template void Dataset::appendAcquisition(const Acquisition<double>&, int);
    
/*******************************************************************************
 ******************************************************************************/
unsigned long Dataset::getNumberOfAcquisitions(int stream)
{
    std::string path;
    if (stream < 0) {
        path = index_path_;
    } else {
        path = constructDataPath(stream);
    }

    if (!linkExists(path)) {
        throw std::runtime_error("Data path does not exist in dataset (num acq)");
    }

    // check if dataset is open
    if (datasets_.count(path) == 0) {
        datasets_[path] = file_.openDataSet(path);
    }

    DataSet dataset = datasets_[path];

    DataSpace space = dataset.getSpace();
    int rank = space.getSimpleExtentNdims();
    std::vector<hsize_t> dims(rank, 0);
    space.getSimpleExtentDims(&dims[0]);
    unsigned long nacq = dims[0];

    return nacq;
}

/*******************************************************************************
 ******************************************************************************/
template <typename T>
void Dataset::appendImage(const Image<T>& im, int stream)
{
  im_data_path_ = constructPath (image_path_, stream);
  //std::stringstream sstr;
  //sstr << image_path_ << "/" << stream;
  //im_data_path_ = sstr.str();
  
  writeImageAttribute (im.getAttributeString());
  writeImageHeader ((void*)&im.getHead());

  std::vector<hsize_t> dims(4, 1);
  dims[3] = im.getMatrixSizeX();
  dims[2] = im.getMatrixSizeY();
  dims[1] = im.getMatrixSizeZ();
  dims[0] = im.getNumberOfChannels();
  DataType data_type = get_hdf5_data_type<T>();
  writeImageData (im_data_path_, data_type, dims,
                  const_cast<T*>(&im.getData()[0]));
}

/*******************************************************************************
 ******************************************************************************/
void Dataset::writeImageData
(
  const std::string& path,
  const DataType& dtype,
  const std::vector<hsize_t>& dims,
  void* data
)
{
  // check if the dataset is already open
  if (datasets_.count(path) == 0)
  {
    // check if the dataset even exists
    if (linkExists(path))
    {
      createGroup (path);
      datasets_[path] = file_.openDataSet(path.c_str());
    }
    else
    {
      std::vector<hsize_t> max_dims = dims;
      max_dims[0] = dims[0];
      DataSpace space(dims.size(), &dims[0], &max_dims[0]);
      datasets_[path] = file_.createDataSet(path.c_str(), dtype, space);
    }
  }

  DataSet dset = datasets_[path];

  unsigned int rank = dims.size();
  // check rank of existing dataset
  DataSpace mspace = dset.getSpace();
  unsigned int cur_rank = mspace.getSimpleExtentNdims();
  if (cur_rank != rank)
  {
    throw std::runtime_error("incorrect rank in Image dataset");
  }

  DataSpace fspace = dset.getSpace();

  // append entry to index dataset
  mspace = DataSpace(rank, &dims[0]);
  dset.write(data, dtype, mspace, fspace);
}

/*******************************************************************************
 ******************************************************************************/
void readImageData (const std::string& path, const DataType& dtype,
                    const std::vector<hsize_t>& entry_dims, void* data)
{
  return;
}

/*******************************************************************************
 ******************************************************************************/
template EXPORTISMRMRD void Dataset::appendImage
  (const Image<uint16_t>& im, int stream);
template EXPORTISMRMRD void Dataset::appendImage
  (const Image<int16_t>& im, int stream);
template EXPORTISMRMRD void Dataset::appendImage
  (const Image<uint32_t>& im, int stream);
template EXPORTISMRMRD void Dataset::appendImage
  (const Image<int32_t>& im, int stream);
template EXPORTISMRMRD void Dataset::appendImage
  (const Image<float>& im, int stream);
template EXPORTISMRMRD void Dataset::appendImage
  (const Image<double>& im, int stream);
template EXPORTISMRMRD void Dataset::appendImage
  (const Image<std::complex<float> >& im, int stream);
template EXPORTISMRMRD void Dataset::appendImage
  (const Image<std::complex<double> >& im, int stream);
/*
template EXPORTISMRMRD void Dataset::appendImage
  (const std::string& var, const Image<uint16_t>& im);
template EXPORTISMRMRD void Dataset::appendImage
  (const std::string& var, const Image<int16_t>& im);
template EXPORTISMRMRD void Dataset::appendImage
  (const std::string& var, const Image<uint32_t>& im);
template EXPORTISMRMRD void Dataset::appendImage
  (const std::string& var, const Image<int32_t>& im);
template EXPORTISMRMRD void Dataset::appendImage
  (const std::string& var, const Image<float>& im);
template EXPORTISMRMRD void Dataset::appendImage
  (const std::string& var, const Image<double>& im);
template EXPORTISMRMRD void Dataset::appendImage
  (const std::string& var, const Image<std::complex<float> >& im);
template EXPORTISMRMRD void Dataset::appendImage
  (const std::string& var, const Image<std::complex<double> >& im);
*/

/*******************************************************************************
 ******************************************************************************/

template <typename T>
Image<T> Dataset::readImage(unsigned long index, int stream)
{
  im_data_path_ = constructPath (image_path_, stream);

  // TODO: Are we planning on storing multiple images in a file?
  //if (index > getNumberOfImages (stream))
  //{
    //throw std::runtime_error ("Image index exceeds the number of stored images");
  //}

  ImageHeader header = readImageHeader();

  Image<T> im;
  im.setHead(header);
  im.setAttributeString (readImageAttribute());

  DataType dtype;
  switch (im.getStorageType())
  {
    case ISMRMRD_USHORT:
      dtype = get_hdf5_data_type<uint16_t>();
      break;
    case ISMRMRD_SHORT:
      dtype = get_hdf5_data_type<int16_t>();
      break;
    case ISMRMRD_UINT:
      dtype = get_hdf5_data_type<uint32_t>();
      break;
    case ISMRMRD_INT:
      dtype = get_hdf5_data_type<int32_t>();
      break;
    case ISMRMRD_FLOAT:
      dtype = get_hdf5_data_type<float>();
      break;
    case ISMRMRD_DOUBLE:
      dtype = get_hdf5_data_type<double>();
      break;
    case ISMRMRD_CXFLOAT:
      dtype = get_hdf5_data_type<std::complex<float> >();
      break;
    case ISMRMRD_CXDOUBLE:
      dtype = get_hdf5_data_type<std::complex<double> >();
      break;
    default:
      throw std::runtime_error ("Unexpected image storage type");
      break;
  }

  //readImageData (image_data_path_, dtype, &dims, (void*)data);

  /*
  if (obj.data.len != im.getData().size()) {
    throw std::runtime_error
      ("Image Header does not match data length in file");
  }
  if (obj.attr.len != im.getAttributeStringLength()) {
    throw std::runtime_error
      ("Image attribute does not match attribute length in file");
  }
  */

  //im.setAttributeString (obj.attr.p);
  //memcpy (const_cast<char*>(&im.getAttributeString()[0]), obj.attr.p, obj.attr.len);
  //memcpy (const_cast<T*>(&im.getData()[0]), obj.data.p, sizeof(T) * obj.data.len);


  return im;
}

/*******************************************************************************
 ******************************************************************************/
template EXPORTISMRMRD Image<uint16_t> Dataset::readImage
  (unsigned long index, int stream);
template EXPORTISMRMRD Image<int16_t> Dataset::readImage
  (unsigned long index, int stream);
template EXPORTISMRMRD Image<uint32_t> Dataset::readImage
  (unsigned long index, int stream);
template EXPORTISMRMRD Image<int32_t> Dataset::readImage
  (unsigned long index, int stream);
template EXPORTISMRMRD Image<float> Dataset::readImage
  (unsigned long index, int stream);
template EXPORTISMRMRD Image<double> Dataset::readImage
  (unsigned long index, int stream);
template EXPORTISMRMRD Image<std::complex<float> > Dataset::readImage
  (unsigned long index, int stream);
template EXPORTISMRMRD Image<std::complex<double> > Dataset::readImage
  (unsigned long index, int stream);
/*
template EXPORTISMRMRD Image<uint16_t> Dataset::readImage
  (const std::string& var, unsigned long index);
template EXPORTISMRMRD Image<int16_t> Dataset::readImage
  (const std::string& var, unsigned long index);
template EXPORTISMRMRD Image<uint32_t> Dataset::readImage
  (const std::string& var, unsigned long index);
template EXPORTISMRMRD Image<int32_t> Dataset::readImage
  (const std::string& var, unsigned long index);
template EXPORTISMRMRD Image<float> Dataset::readImage
  (const std::string& var, unsigned long index);
template EXPORTISMRMRD Image<double> Dataset::readImage
  (const std::string& var, unsigned long index);
template EXPORTISMRMRD Image<std::complex<float> > Dataset::readImage
  (const std::string& var, unsigned long index);
template EXPORTISMRMRD Image<std::complex<double> > Dataset::readImage
  (const std::string& var, unsigned long index);
*/

/*******************************************************************************
 ******************************************************************************/
unsigned long Dataset::getNumberOfImages(int stream)
{
  // TODO: Are we storing multiple images in a file?
  std::string path = constructPath (image_path_, stream);

  if (!linkExists (path))
  {
    throw std::runtime_error("Image data path does not exist in dataset");
  }

  if (datasets_.count(path) == 0)
  {
    datasets_[path] = file_.openDataSet (path);
  }

  DataSet dataset = datasets_[path];

  DataSpace space = dataset.getSpace();
  int rank = space.getSimpleExtentNdims();
  std::vector<hsize_t> dims(rank, 0);
  space.getSimpleExtentDims(&dims[0]);
  unsigned long nimg = dims[0];

  return nimg;
}

/*******************************************************************************
 ******************************************************************************/
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

/*******************************************************************************
 ******************************************************************************/
template <typename T>
NDArray<T> Dataset::readNDArray(const std::string& var, unsigned long index)
{
    // TODO: implement
    return NDArray<T>();
}

template EXPORTISMRMRD NDArray<uint16_t> Dataset::readNDArray(const std::string& var, unsigned long index);
template EXPORTISMRMRD NDArray<int16_t> Dataset::readNDArray(const std::string& var, unsigned long index);
template EXPORTISMRMRD NDArray<uint32_t> Dataset::readNDArray(const std::string& var, unsigned long index);
template EXPORTISMRMRD NDArray<int32_t> Dataset::readNDArray(const std::string& var, unsigned long index);
template EXPORTISMRMRD NDArray<float> Dataset::readNDArray(const std::string& var, unsigned long index);
template EXPORTISMRMRD NDArray<double> Dataset::readNDArray(const std::string& var, unsigned long index);
template EXPORTISMRMRD NDArray<std::complex<float> > Dataset::readNDArray(const std::string& var, unsigned long index);
template EXPORTISMRMRD NDArray<std::complex<double> > Dataset::readNDArray(const std::string& var, unsigned long index);

/*******************************************************************************
 ******************************************************************************/
unsigned long Dataset::getNumberOfNDArrays(const std::string& var)
{
    unsigned long narr = 0;

    // TODO: implement

    return narr;
}


} // namespace ISMRMRD

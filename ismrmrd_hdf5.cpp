#include "ismrmrd_hdf5.h"
#include "ismrmrd_hdf5_datatypes.h"


#include <iostream>
#include <boost/algorithm/string.hpp>


namespace ISMRMRD
{

int IsmrmrdDataset::openHDF5File()
{

	if (file_exists_) {
		try {
			if (!H5File::isHdf5(filename_.c_str())) {
				std::cerr << "File \"" << filename_ << "\" is not an HDF file file" << std::endl;
				return -1;
			}
			file_ = boost::shared_ptr<H5File>(new H5File(filename_.c_str(), H5F_ACC_RDWR));
		} catch (...) {
                    try {
                        file_ = boost::shared_ptr<H5File>(new H5File(filename_.c_str(), H5F_ACC_RDONLY));
                    } catch (...) {
                        std::cerr << "Failed to open HDF5 file." << std::endl;
                        return -1;
                    }
		}
		file_open_ = true;
	} else if (create_file_if_needed_){
		try {
			file_ = boost::shared_ptr<H5File>(new H5File(filename_, H5F_ACC_TRUNC));

			//We will close and then immediately open the file again.
			//We need to make sure the file is saved as an HDF5 file in case other processes and functions
			//need to access it immediately. The line above does not cause the file to be marked as and HDF5 file.
			//H5File::isHdf5(filename_.c_str()) will return false at this point.
			file_->close();
			file_ = boost::shared_ptr<H5File>(new H5File(filename_, H5F_ACC_RDWR));

		} catch (...) {
			std::cerr << "Failed to create and open HDF5 file." << std::endl;
			return -1;
		}
		file_open_ = true;
		file_exists_ = true;
	} else {
		std::cerr << "Unable to find HDF5 file (" << filename_ << "), and file cannot be created." << std::endl;
		return -1;
	}

	return 0;
}

void IsmrmrdDataset::close(void)
{
    if (file_open_) {
        dataset_->flush(H5F_SCOPE_LOCAL);
        dataset_->close();
        dataset_.reset();
        file_->flush(H5F_SCOPE_LOCAL);
        file_->close();
        file_.reset();
        file_open_ = false;
    }
}



bool IsmrmrdDataset::linkExists(const char* name)
{
	if (!file_open_) {
		std::cerr << "IsmrmrdDataset::linkExists: file not open." << std::endl;
		return false;
	}
	std::vector<std::string> name_elements;
	std::string splitstr("/");
	std::string namestr(name);
	boost::split(name_elements, namestr, boost::is_any_of(splitstr));
	std::string current_path("");
	for (unsigned int i = 0; i < name_elements.size(); i++) {
		if (name_elements[i].size() > 0) {
			current_path = current_path + std::string("/") + name_elements[i];
			if (!H5Lexists(file_->getId(), current_path.c_str(), H5P_DEFAULT )) {
				return false;
			}
		}
	}
	return true;
}

int IsmrmrdDataset::createGroupForDataset(const char* name)
{
	if (!file_open_) {
		std::cerr << "IsmrmrdDataset::linkExists: file not open." << std::endl;
		return -1;
	}

	std::vector<std::string> name_elements;
	std::string splitstr("/");
	std::string namestr(name);
	boost::split(name_elements, namestr, boost::is_any_of(splitstr));
	std::string current_path("");
	for (unsigned int i = 0; i < name_elements.size(); i++) {
		if (name_elements[i].size() > 0) {
			current_path = current_path + std::string("/") + name_elements[i];
			if (!H5Lexists(file_->getId(), current_path.c_str(), H5P_DEFAULT )) {
				file_->createGroup( current_path.c_str());
			}
		}
	}
	return 0;
}

template <typename T> int IsmrmrdDataset::appendArray(NDArrayContainer<T>& a, const char* varname)
{

	std::string data_path = groupname_ + std::string("/") + std::string(varname);

	if (!a.is_consistent()) {
		std::cerr << "IsmrmrdDataset::append(..): Asking to append inconsistent array." << std::endl;
		return -1;
	}

	boost::shared_ptr<DataSet> dataset;
    boost::shared_ptr<DataType> datatype = getIsmrmrdHDF5Type<T>();

	std::vector<hsize_t> dims;
	std::vector<hsize_t> max_dims;
	if (!linkExists(data_path.c_str())) {
		std::vector<hsize_t> dims;
		std::vector<hsize_t> max_dims;
		dims.push_back(1);
		max_dims.push_back(H5S_UNLIMITED);
		for (std::vector<unsigned int>::reverse_iterator it = a.dimensions_.rbegin(); it != a.dimensions_.rend(); it++) {
			dims.push_back(static_cast<unsigned long long int>(*it));
			max_dims.push_back(static_cast<unsigned long long int>(*it));
		}

		try {

			if (createGroupForDataset(groupname_.c_str()) < 0) {
				std::cout << "Failed to create group in HDF 5 file." << std::endl;
				return -1;
			}

			DataSpace mspace1( dims.size(), &dims[0], &max_dims[0]);

			DSetCreatPropList cparms;
			cparms.setChunk( dims.size(), &dims[0] );

			dataset = boost::shared_ptr<DataSet>(new DataSet(file_->createDataSet( data_path.c_str(), *datatype, mspace1, cparms)));
			mspace1 = dataset->getSpace();

			DataSpace mspace2( dims.size(), &dims[0] );

			std::vector<hsize_t> offset(dims.size());
			mspace1.selectHyperslab(H5S_SELECT_SET, &dims[0], &offset[0]);
			dataset->write( &(a.data_[0]), *datatype, mspace2, mspace1 );

		} catch( Exception& e ) {
			std::cerr << "Exception caught while creating HDF5 dataset" << std::endl;
			std::cerr << e.getDetailMsg() << std::endl;
			return -1;
		}
	} else {
		try {  // to determine if the dataset exists in the group
			dataset = boost::shared_ptr<DataSet>(new DataSet(file_->openDataSet(data_path.c_str())));

			DataType mtype = dataset->getDataType();
			if (!(mtype == (*datatype))) {
				std::cout << "Attempting to append data to HDF5 dataset with the wrong type" << std::endl;
				return -1;
			}

			DataSpace mspace1 = dataset->getSpace();
			int rank = mspace1.getSimpleExtentNdims();
			std::vector<hsize_t> ddims(rank,0);
			mspace1.getSimpleExtentDims(&ddims[0], NULL);


			if ((ddims.size()-1) != a.dimensions_.size()) {
				std::cerr << "Dimensions in dataset does not match with existing HDF5 dataset" << std::endl;
			}

			dims.push_back(1);
			for (unsigned int i = 1; i < ddims.size(); i++) {
				if (a.dimensions_[ddims.size()-1-i] != ddims[i]) {
					std::cerr << "Error trying to write array to existing HDF5 file. Variable has wrong size." << std::endl;
					std::cerr << a.dimensions_[ddims.size()-1-i] << ", " << ddims[i];
					std::cerr << std::endl;
					return -1;
				}
				dims.push_back(ddims[i]);
			}

			std::vector<hsize_t> offset(rank, 0);
			offset[0] = ddims[0];

			ddims[0]++;

			dataset->extend(&ddims[0]);

			DataSpace fspace2 = dataset->getSpace();
			fspace2.selectHyperslab( H5S_SELECT_SET, &dims[0], &offset[0] );

			DataSpace mspace2( rank, &dims[0] );

			dataset->write( &(a.data_[0]), *datatype, mspace2, fspace2 );

		 }
		 catch( FileIException& not_found_error)
		 {
			 std::cout << "Dataset is not found. At this point, it should have been created!" << std::endl;
			 return -1;
		 }
	}
	return 0;
}

template EXPORTISMRMRD int IsmrmrdDataset::appendArray(NDArrayContainer< unsigned short int >& a,const char* varname);
template EXPORTISMRMRD int IsmrmrdDataset::appendArray(NDArrayContainer< float >& a,const char* varname);
template EXPORTISMRMRD int IsmrmrdDataset::appendArray(NDArrayContainer< double >& a,const char* varname);
template EXPORTISMRMRD int IsmrmrdDataset::appendArray(NDArrayContainer< std::complex<float> >& a,const char* varname);
template EXPORTISMRMRD int IsmrmrdDataset::appendArray(NDArrayContainer< std::complex<double> >& a,const char* varname);
template EXPORTISMRMRD int IsmrmrdDataset::appendArray(NDArrayContainer< ImageHeader >& a,const char* varname);

template <typename T> boost::shared_ptr< NDArrayContainer<T> > IsmrmrdDataset::readArray(const char* varname, unsigned long int index)
{
	boost::shared_ptr< NDArrayContainer<T> > ret(new NDArrayContainer<T>());
    boost::shared_ptr<DataType> datatype = getIsmrmrdHDF5Type<T>();

    std::string data_path = groupname_ + std::string("/") + std::string(varname);

	try {

		if (!linkExists(data_path.c_str())) {
			std::cout << "Trying to access non-existing variable in HDF5 file." << std::endl;
			return ret;
		}

		DataSet d = file_->openDataSet(H5std_string(data_path.c_str()));
		DataSpace dataspace = d.getSpace();
		DataType dtype = d.getDataType();

		if (!(dtype == *datatype)) {
			std::cout << "HDF5 datatype for selected variable does not match signature of reading function" << std::endl;
			return ret;
		}

		int rank = dataspace.getSimpleExtentNdims();
		std::vector<hsize_t> dims(rank,0);
		dataspace.getSimpleExtentDims(&dims[0]);

		if (dims[0] <= index) {
			std::cout << "Attempting to access non-existing hyperslice" << std::endl;
			return ret;
		}

		std::vector<hsize_t> slice_dims(rank,0);
		std::vector<hsize_t> offset(rank,0);
		ret->dimensions_ = std::vector<unsigned int>(rank-1,0);
		slice_dims[0] = 1;
		offset[0] = index;

		for (unsigned int i = 1; i < rank; i++) {
			slice_dims[i] = dims[i];
			ret->dimensions_[ret->dimensions_.size()-i] = dims[i]; //Flip dimensions for NDArray.
		}

		dataspace.selectHyperslab( H5S_SELECT_SET, &slice_dims[0], &offset[0] );

		DataSpace memspace(rank,&slice_dims[0]);

		ret->data_.resize(ret->elements());

		//OK finally ready, now read the data.
		d.read(reinterpret_cast<void*>(&(ret->data_[0])), *datatype, memspace, dataspace, H5P_DEFAULT);

	} catch (...) {
		std::cout << "Error caught while attempting to read HDF5 file" << std::endl;
		return ret;
	}

	return ret;

}

template EXPORTISMRMRD boost::shared_ptr< NDArrayContainer<unsigned short int> > IsmrmrdDataset::readArray(const char* varname, unsigned long int index);
template EXPORTISMRMRD boost::shared_ptr< NDArrayContainer<float> > IsmrmrdDataset::readArray(const char* varname, unsigned long int index);
template EXPORTISMRMRD boost::shared_ptr< NDArrayContainer<double> > IsmrmrdDataset::readArray(const char* varname, unsigned long int index);
template EXPORTISMRMRD boost::shared_ptr< NDArrayContainer< std::complex<float> > > IsmrmrdDataset::readArray(const char* varname, unsigned long int index);
template EXPORTISMRMRD boost::shared_ptr< NDArrayContainer< std::complex<double> > > IsmrmrdDataset::readArray(const char* varname, unsigned long int index);


int IsmrmrdDataset::appendAcquisition(Acquisition* a)
{
	std::vector<hsize_t> dims(2,1);
	std::vector<hsize_t> max_dims(2,1);max_dims[0] = H5S_UNLIMITED;

	int rank = 2;
	std::vector<hsize_t> ddims = dims;
	std::vector<hsize_t> offset(rank,0);

	bool new_dataset = false;

	if (!dataset_open_) {
		try {
			boost::shared_ptr<DataType> datatype = getIsmrmrdHDF5Type<AcquisitionHeader_with_data>();
			if (!linkExists(data_path_.c_str())) {
				DataSpace mspace1( dims.size(), &dims[0], &max_dims[0]);
				DSetCreatPropList cparms;
				cparms.setChunk( dims.size(), &dims[0] );
				dataset_ = boost::shared_ptr<DataSet>(new DataSet(file_->createDataSet( data_path_.c_str(), *datatype, mspace1, cparms)));
				new_dataset = true;
			} else {
				dataset_ = boost::shared_ptr<DataSet>(new DataSet(file_->openDataSet(data_path_.c_str())));
				DataType mtype = dataset_->getDataType();
				if (!(mtype == (*datatype))) {
					std::cout << "Attempting to append data to HDF5 dataset with the wrong type" << std::endl;
					return -1;
				}

			}
			dataset_open_ = true;
		} catch( Exception& e ) {
			std::cout << "Exception caught while opening (creating) HDF5 dataset" << std::endl;
			std::cout << e.getDetailMsg() << std::endl;
			return -1;
		}
	}

	try {
		boost::shared_ptr<DataType> datatype = getIsmrmrdHDF5Type<AcquisitionHeader_with_data>();
		AcquisitionHeader_with_data tmp;
		tmp.head = a->getHead();
		tmp.traj.len = tmp.head.trajectory_dimensions*tmp.head.number_of_samples;
		tmp.traj.p = const_cast<float*>(&a->getTraj()[0]);

		tmp.data.len = tmp.head.active_channels*tmp.head.number_of_samples*2;
		tmp.data.p = const_cast<float*>(&a->getData()[0]);

		DataSpace mspace1 = dataset_->getSpace();
		rank = mspace1.getSimpleExtentNdims();
		if (rank != 2) {
			std::cerr << "Wrong rank (" << rank << ") found in HDF5 dataset" << std::endl;
			return -1;
		}

		if (!new_dataset) {
			mspace1.getSimpleExtentDims(&ddims[0], NULL);
			offset[0] = ddims[0];
			ddims[0]++;

			dataset_->extend(&ddims[0]);
		}
		DataSpace fspace2 = dataset_->getSpace();
		fspace2.selectHyperslab( H5S_SELECT_SET, &dims[0], &offset[0] );

		DataSpace mspace2( rank, &dims[0] );
		dataset_->write( &tmp, *datatype, mspace2, fspace2 );

	} catch( Exception& e ) {
		std::cout << "Exception caught while writing HDF5 data" << std::endl;
		std::cout << e.getDetailMsg() << std::endl;
		return -1;
	}

	return 0;
}



unsigned long IsmrmrdDataset::getNumberOfAcquisitions()
{
	unsigned long ret = 0;

	if (!linkExists(data_path_.c_str())) {
		std::cerr << "Data path (" << data_path_ << ") does not exist in HDF5 dataset" << std::endl;
		return ret;
	}

	if (!dataset_open_) {
		try{
			dataset_ = boost::shared_ptr<DataSet>(new DataSet(file_->openDataSet(data_path_.c_str())));
			dataset_open_ = true;
		} catch( Exception& e ) {
			std::cout << "Exception caught while opening HDF5 dataset" << std::endl;
			std::cout << e.getDetailMsg() << std::endl;
			return ret;
		}
	}

	try {
		DataSpace dataspace = dataset_->getSpace();
		int rank = dataspace.getSimpleExtentNdims();
		std::vector<hsize_t> dims(rank,0);
		dataspace.getSimpleExtentDims(&dims[0]);
		ret = dims[0];
	} catch (...) {
		std::cout << "Error caught while attempting to access the number of elements in HDF5 file" << std::endl;
		return ret;
	}

	return ret;
}

boost::shared_ptr< Acquisition > IsmrmrdDataset::readAcquisition(unsigned long int index)
{
	boost::shared_ptr<Acquisition> ret;

	if (!linkExists(data_path_.c_str())) {
		std::cerr << "Data path does not exist in HDF5 dataset" << std::endl;
		return ret;
	}


	boost::shared_ptr<DataType> datatype = getIsmrmrdHDF5Type<AcquisitionHeader_with_data>();
	if (!dataset_open_) {
		try{
			dataset_ = boost::shared_ptr<DataSet>(new DataSet(file_->openDataSet(data_path_.c_str())));
			dataset_open_ = true;
		} catch( Exception& e ) {
			std::cout << "Exception caught while opening (creating) HDF5 dataset" << std::endl;
			std::cout << e.getDetailMsg() << std::endl;
			return ret;
		}
	}

	try {
		DataSpace dataspace = dataset_->getSpace();
		DataType dtype = dataset_->getDataType();

		if (!(dtype == *datatype)) {
			std::cout << "Attempting to open HDF5 dataset with the wrong type" << std::endl;
			return ret;
		}

		int rank = dataspace.getSimpleExtentNdims();
		std::vector<hsize_t> dims(rank,0);
		dataspace.getSimpleExtentDims(&dims[0]);

		if (dims[0] <= index) {
			std::cout << "Attempting to access non-existing hyperslice in HDF5 dataset" << std::endl;
			return ret;
		}

		std::vector<hsize_t> slice_dims(rank,1);
		std::vector<hsize_t> offset(rank,0);

		slice_dims[0] = 1;
		offset[0] = index;


		dataspace.selectHyperslab( H5S_SELECT_SET, &slice_dims[0], &offset[0] );

		DataSpace memspace(rank,&slice_dims[0]);

		AcquisitionHeader_with_data tmp;

		//OK finally ready, now read the data.
		dataset_->read(reinterpret_cast<void*>(&tmp), *datatype, memspace, dataspace, H5P_DEFAULT);

		ret = boost::shared_ptr<Acquisition>(new Acquisition());
		ret->setHead(tmp.head);
		//ret->head_ = tmp.head;
        size_t tl = tmp.traj.len;
        size_t dl = tmp.data.len;
        if ((tl != ret->getTraj().size()) || (dl != ret->getData().size())) {
        	std::cout << "Header does not match data length in file" << std::endl;
        	throw;
        }
		memcpy(const_cast<float*>(&ret->getTraj()[0]), tmp.traj.p, sizeof(float)*tl);
		free(tmp.traj.p); //Avoid memory leak
		memcpy(const_cast<float*>(&ret->getData()[0]), tmp.data.p, sizeof(float)*dl);
		free(tmp.data.p); //Avoid memory leak

	} catch (...) {
		std::cout << "Error caught while attempting to read HDF5 file" << std::endl;
		return ret;
	}

	return ret;

}

int IsmrmrdDataset::writeHeader(const std::string& xml) {
	std::vector<hsize_t> dims(1,1);
	std::vector<hsize_t> max_dims(1,1);

	boost::shared_ptr<DataSet> xml_dataset;
	boost::shared_ptr<DataType> datatype(new StrType(0, H5T_VARIABLE));
	try {
		if (!linkExists(xml_header_path_.c_str())) {
			DataSpace mspace1( dims.size(), &dims[0], &max_dims[0]);
			xml_dataset = boost::shared_ptr<DataSet>(new DataSet(file_->createDataSet( xml_header_path_.c_str(), *datatype, mspace1)));
		} else {
			xml_dataset = boost::shared_ptr<DataSet>(new DataSet(file_->openDataSet(xml_header_path_.c_str())));
			DataType mtype = xml_dataset->getDataType();
			if (!(mtype == (*datatype))) {
				std::cout << "Attempting to append XML data to HDF5 dataset with the wrong type" << std::endl;
				return -1;
			}
		}
		DataSpace mspace1 = xml_dataset->getSpace();
		xml_dataset->write(xml,*datatype,mspace1);

	} catch( Exception& e ) {
			std::cout << "Exception caught while writing XML Header to HDF5 file" << std::endl;
			std::cout << e.getDetailMsg() << std::endl;
			return -1;
	}
 return 0;
}

boost::shared_ptr<std::string> IsmrmrdDataset::readHeader()
{
	boost::shared_ptr<std::string> ret(new std::string(""));

	if (!linkExists(xml_header_path_.c_str())) {
		std::cerr << "Path for XML parameters not found in HDF5 file" << std::endl;
		return ret;
	}

	try {
		boost::shared_ptr<DataSet> xml_dataset = boost::shared_ptr<DataSet>(new DataSet(file_->openDataSet(xml_header_path_.c_str())));
		boost::shared_ptr<DataType> datatype(new StrType(0, H5T_VARIABLE));
		DataType dt = xml_dataset->getDataType();
		xml_dataset->read(*ret,dt);
	} catch( Exception& e ) {
		std::cout << "Exception caught while reading XML Header to HDF5 file" << std::endl;
		std::cout << e.getDetailMsg() << std::endl;
		return ret;
    }

	return ret;
}

template <typename T> int IsmrmrdDataset::appendImage(Image<T>& m, const char* varname) {
	ImageHeader_with_data<T> tmp;
	tmp.head = m.getHead();
	tmp.data.len = m.getNumberOfElements();
	tmp.data.p = const_cast<T*>(&m.getData()[0]);
	std::vector<unsigned int> dims(1,1);
	NDArrayContainer<ImageHeader_with_data<T> > cont(dims, &tmp);
	return appendArray<ImageHeader_with_data<T> >(cont, varname);
}

template <typename T> boost::shared_ptr< Image<T> > IsmrmrdDataset::readImage(const char* varname, unsigned long index)
{
	boost::shared_ptr< Image<T> > ret(new Image<T>);

	boost::shared_ptr<NDArrayContainer<ImageHeader_with_data<T> > > tmp = readArray<ImageHeader_with_data<T> >(varname, index);

	if (tmp->elements() != 1) {
		std::cerr << "IsmrmrdDataset::readImage(..): readArray returned an unexpected number of elements (";
		std::cerr << tmp->elements() << "). Should be 1.";
		std::cerr << std::endl;
		return ret;
	}
	//We will copy the header
	ret->setHead(tmp->data_[0].head);

    size_t dlen = tmp->data_[0].data.len;
	memcpy (const_cast<T*>(&ret->getData()[0]), tmp->data_[0].data.p, dlen*sizeof(T));
	free(tmp->data_[0].data.p); //Avoid memory leak
	return ret;
}

boost::shared_ptr< ImageHeader > IsmrmrdDataset::readImageHeader(const char* varname, unsigned long index)
{
	boost::shared_ptr< ImageHeader > ret(new ImageHeader);

	boost::shared_ptr<NDArrayContainer<ImageHeader> > tmp = readArray<ImageHeader>(varname,index);
	if (tmp->elements() != 1) {
		std::cerr << "IsmrmrdDataset::readImageHeader(..): readArray returned an unexpected number of elements (" << tmp->elements() << "). Should be 1." << std::endl;
		return ret;
	}

	*ret = tmp->data_[0];

	return ret;
}

template EXPORTISMRMRD int IsmrmrdDataset::appendImage(Image<float>& m, const char* varname);
template EXPORTISMRMRD int IsmrmrdDataset::appendImage(Image<double>& m, const char* varname);
template EXPORTISMRMRD int IsmrmrdDataset::appendImage(Image<unsigned short>& m, const char* varname);
template EXPORTISMRMRD int IsmrmrdDataset::appendImage(Image< std::complex<float> >& m, const char* varname);
template EXPORTISMRMRD int IsmrmrdDataset::appendImage(Image< std::complex<double> >& m, const char* varname);

template EXPORTISMRMRD boost::shared_ptr< Image<float> > IsmrmrdDataset::readImage(const char* varname, unsigned long index);
template EXPORTISMRMRD boost::shared_ptr< Image<double> > IsmrmrdDataset::readImage(const char* varname, unsigned long index);
template EXPORTISMRMRD boost::shared_ptr< Image<unsigned short> > IsmrmrdDataset::readImage(const char* varname, unsigned long index);
template EXPORTISMRMRD boost::shared_ptr< Image< std::complex<float> > > IsmrmrdDataset::readImage(const char* varname, unsigned long index);
template EXPORTISMRMRD boost::shared_ptr< Image< std::complex<double> > > IsmrmrdDataset::readImage(const char* varname, unsigned long index);

int IsmrmrdDataset::appendImageAttrib(std::string& a, const char* varname)
{
	std::string data_path = groupname_ + std::string("/") + std::string(varname);

	boost::shared_ptr<DataSet> dataset;
    boost::shared_ptr<DataType> datatype = getIsmrmrdHDF5Type<std::string>();

	std::vector<hsize_t> dims;
	std::vector<hsize_t> max_dims;
	if (!linkExists(data_path.c_str()))
    {
		std::vector<hsize_t> dims(1,1);
		std::vector<hsize_t> max_dims(1,1);

		try
        {
			if (createGroupForDataset(groupname_.c_str()) < 0)
            {
				std::cout << "Failed to create group in HDF 5 file." << std::endl;
				return -1;
			}

			DataSpace mspace1( dims.size(), &dims[0], &max_dims[0]);

			DSetCreatPropList cparms;
			cparms.setChunk( dims.size(), &dims[0] );

			dataset = boost::shared_ptr<DataSet>(new DataSet(file_->createDataSet( data_path.c_str(), *datatype, mspace1, cparms)));
			mspace1 = dataset->getSpace();

			DataSpace mspace2( dims.size(), &dims[0] );

			std::vector<hsize_t> offset(dims.size());
			mspace1.selectHyperslab(H5S_SELECT_SET, &dims[0], &offset[0]);
			dataset->write( a, *datatype, mspace2, mspace1 );

		} catch( Exception& e ) {
			std::cerr << "Exception caught while creating HDF5 dataset" << std::endl;
			std::cerr << e.getDetailMsg() << std::endl;
			return -1;
		}
	}
    else
    {
		try
        {  // to determine if the dataset exists in the group
			dataset = boost::shared_ptr<DataSet>(new DataSet(file_->openDataSet(data_path.c_str())));

			DataType mtype = dataset->getDataType();
			if (!(mtype == (*datatype))) {
				std::cout << "Attempting to append data to HDF5 dataset with the wrong type" << std::endl;
				return -1;
			}

            std::vector<hsize_t> dims(1,1);
		    std::vector<hsize_t> max_dims(1,1);
            DataSpace mspace2( dims.size(), &dims[0] );

            DataSpace mspace1( dims.size(), &dims[0], &max_dims[0]);
            mspace1 = dataset->getSpace();

			std::vector<hsize_t> offset(dims.size());
			mspace1.selectHyperslab(H5S_SELECT_SET, &dims[0], &offset[0]);
			dataset->write( a, *datatype, mspace2, mspace1 );

			/*DataSpace mspace1 = dataset->getSpace();
			int rank = mspace1.getSimpleExtentNdims();
			std::vector<hsize_t> ddims(rank,0);
			mspace1.getSimpleExtentDims(&ddims[0], NULL);

			dims.push_back(1);
			for (unsigned int i = 1; i < ddims.size(); i++)
            {
				dims.push_back(ddims[i]);
			}

			std::vector<hsize_t> offset(rank, 0);
			offset[0] = ddims[0];

			DataSpace fspace2 = dataset->getSpace();
			fspace2.selectHyperslab( H5S_SELECT_SET, &dims[0], &offset[0] );

			DataSpace mspace2( rank, &dims[0] );

			dataset->write( a, *datatype, mspace2, fspace2 );*/

		 }
		 catch( FileIException& not_found_error)
		 {
			 std::cout << "Dataset is not found. At this point, it should have been created!" << std::endl;
			 return -1;
		 }
	}
	return 0;
}

HDF5Lock* HDF5Lock::instance()
{
		if (!instance_) instance_ = new HDF5Lock();
		return instance_;
}

void HDF5Lock::acquire()
{
	mutex_.lock();
}

void HDF5Lock::release()
{
	mutex_.unlock();
}

EXPORTISMRMRD HDF5Lock* HDF5Lock::instance_ = NULL;


} //End of ISMRMRD namespace


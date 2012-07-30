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
			file_ = boost::shared_ptr<H5File>(new H5File(filename_, H5F_ACC_RDWR));
		} catch (...) {
			std::cerr << "Failed to open HDF5 file." << std::endl;
			return -1;
		}
		file_open_ = true;
	} else if (create_file_if_needed_){
		try {
			file_ = boost::shared_ptr<H5File>(new H5File(filename_, H5F_ACC_TRUNC));
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


bool IsmrmrdDataset::linkExists(const char* name)
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
		tmp.head = a->head_;
		tmp.traj.len = tmp.head.trajectory_dimensions*tmp.head.number_of_samples;
		tmp.traj.p = static_cast<void*>(a->traj_);

		tmp.data.len = tmp.head.active_channels*tmp.head.number_of_samples;
		tmp.data.p = static_cast<void*>(a->data_);

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

int IsmrmrdDataset::writeHeader(std::string& xml) {
	std::vector<hsize_t> dims(1,1);
	std::vector<hsize_t> max_dims(1,1);

	boost::shared_ptr<DataSet> xml_dataset;
	hid_t datatype = H5Tcopy (H5T_C_S1);
	H5Tset_size (datatype, H5T_VARIABLE);
	try {
		if (!linkExists(xml_header_path_.c_str())) {
			DataSpace mspace1( dims.size(), &dims[0], &max_dims[0]);
			xml_dataset = boost::shared_ptr<DataSet>(new DataSet(file_->createDataSet( xml_header_path_.c_str(), datatype, mspace1)));
		} else {
			xml_dataset = boost::shared_ptr<DataSet>(new DataSet(file_->openDataSet(xml_header_path_.c_str())));
			DataType mtype = dataset_->getDataType();
			if (!(mtype == (datatype))) {
				std::cout << "Attempting to append XML data to HDF5 dataset with the wrong type" << std::endl;
				return -1;
			}
		}
		DataSpace mspace1 = xml_dataset->getSpace();
		xml_dataset->write( xml.c_str(), datatype, mspace1);
	} catch( Exception& e ) {
			std::cout << "Exception caught while writing XML Header to HDF5 file" << std::endl;
			std::cout << e.getDetailMsg() << std::endl;
			return -1;
	}



 return 0;
}

} //End of ISMRMRD namespace

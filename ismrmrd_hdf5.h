#pragma once
#ifndef ISMRMRD_HDF5_H
#define ISMRMRD_HDF5_H

#include "ismrmrd_export.h"
#include "ismrmrd.h"

#include <fstream>
#include <boost/shared_ptr.hpp>


#include <H5Cpp.h>

#ifndef H5_NO_NAMESPACE
using namespace H5;
#endif


namespace ISMRMRD
{

class EXPORTISMRMRD IsmrmrdDataset
{
public:
	IsmrmrdDataset(const char* filename, const char* groupname, bool create_file_if_needed = true)
	: filename_(filename)
	, groupname_(groupname)
	, file_open_(false)
	, dataset_open_(false)
	, create_file_if_needed_(create_file_if_needed)
	{
		std::ifstream ifile(filename_.c_str());
		file_exists_ = ifile;

		if (openHDF5File() < 0) {
			std::cerr << "IsmrmrdDataset: Error opening HDF file." << std::endl;
		}

		if (!linkExists(groupname_.c_str())) {
			if (createGroupForDataset(groupname_.c_str()) < 0) {
				std::cerr << "IsmrmrdDataset: Error create HDF5 group." << std::endl;
			}
		}

		xml_header_path_ = groupname_ + std::string("/xml");
		data_path_ = groupname_ + std::string("/data");
 	}

	int appendAcquisition(Acquisition* a);
	int writeHeader(std::string& xml);

	boost::shared_ptr<std::string> readHeader();
	boost::shared_ptr<Acquisition> readAcquisition(unsigned long index = 0);
	unsigned long getNumberOfAcquisitions();

protected:
	int openHDF5File();
	bool linkExists(const char* name);
	int createGroupForDataset(const char* name);

	bool fileExists() {
		return file_exists_;
	}

	std::string filename_;
	std::string groupname_;
	std::string xml_header_path_;
	std::string data_path_;

	bool file_open_;
	bool file_exists_;
	bool create_file_if_needed_;
	bool dataset_open_;

	boost::shared_ptr<H5File> file_;
	boost::shared_ptr<DataSet> dataset_;

};


struct AcquisitionHeader_with_data
{
	AcquisitionHeader head;
	hvl_t traj;
	hvl_t data;
};



} //end of ISMRMRD namespace

#endif //ISMRMRD_HDF5_H

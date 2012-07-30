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
	, create_file_if_needed_(create_file_if_needed)
	{
		std::ifstream ifile(filename_.c_str());
		file_exists_ = ifile;

		if (openHDF5File() < 0) {
			std::cerr << "Error opening HDF file" << std::endl;
		}
 	}

	bool fileExists() {
		return file_exists_;
	}

protected:
	int openHDF5File();

	std::string filename_;
	std::string groupname_;
	bool file_open_;
	bool file_exists_;
	bool create_file_if_needed_;

	boost::shared_ptr<H5File> file_;

};

EXPORTISMRMRD int H5AppendAcquisition(Acquisition* a, const char* filename, const char* varname);
EXPORTISMRMRD boost::shared_ptr<Acquisition> H5ReadAcquisition(const char* filename, const char* varname, unsigned long index = 0);



} //end of ISMRMRD namespace

#endif //ISMRMRD_HDF5_H

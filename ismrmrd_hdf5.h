#pragma once
#ifndef ISMRMRD_HDF5_H
#define ISMRMRD_HDF5_H

#include "ismrmrd_export.h"
#include "ismrmrd.h"

#include <fstream>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>


#include <H5Cpp.h>

#ifndef H5_NO_NAMESPACE
using namespace H5;
#endif


namespace ISMRMRD
{

struct AcquisitionHeader_with_data
{
	AcquisitionHeader head;
	hvl_t traj;
	hvl_t data;
};

template <typename T> struct ImageHeader_with_data
{
	ImageHeader head;
	hvl_t data;
};

}


#include "ismrmrd_hdf5_datatypes.h"

namespace ISMRMRD
{


/**
 *   This class provides a simple interface for accessing an ISMRMRD Data Set stored
 *   in HDF5 format.
 *
 *
 *   A given ISMRMRD dataset if assumed to be stored under one group name in the HDF5 file. To make the datasets consistent,
 *   this library enforces that the XML configuration is stored in the variable groupname/xml and the Acquisitions are
 *   stored in groupname/data.
 *
 */
class EXPORTISMRMRD IsmrmrdDataset
{
public:
	/**
	 *   Constructor.
	 *
	 *   @param filename is the full path to HDF5 file used for storing the data set
	 *   @param groupname is the group name within the HDF5 file used for this data set.
	 *   @param create_file_if_needed can be used to automatically create the file if needed.
	 *
	 */
	IsmrmrdDataset(const char* filename, const char* groupname, bool create_file_if_needed = true)
	: filename_(filename)
	, groupname_(groupname)
	, file_open_(false)
	, dataset_open_(false)
	, create_file_if_needed_(create_file_if_needed)
	{
		std::ifstream ifile(filename_.c_str());
		file_exists_ = ifile.is_open();

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

        /**
         * Closes all references to the underlying HDF5 file.
         *
         *  This method does not NEED to be called. It is most useful in
         *  a Python interpreter when a dataset instance has not been
         *  garbage collected but another process needs to read the HDF5 file.
         */
        void close(void);

	/**
	 *   Appends and NMR/MRI acquisition to the data set.
	 *
	 *	 Please consult @See Acquisition class for details.
	 */
	int appendAcquisition(Acquisition* a);

	/**
	 *  Reads the acquisition with the specified index from the HDF5 file.
	 */
	boost::shared_ptr<Acquisition> readAcquisition(unsigned long index = 0);

	/**
	 *  Return the number of acquisitions in the dataset.
	 */
	unsigned long getNumberOfAcquisitions();

	/**
	 *  Writes the xml string to the XML header field.
	 *
	 *  @warning There is no check of whether the string is a valid XML document at this point.
	 *
	 */
	int writeHeader(const std::string& xml);

	/**
	 *  Reads the XML configuration header from the data set.
	 */
	boost::shared_ptr<std::string> readHeader();

	/**
	 *  Appends an Image to the variable named varname.
	 *
	 *  With this function, the image data is stored as a field in the variable (as a one dimensional array)
	 *  and when reading it, it would need to be reshaped according to the image dimensions.
	 *
	 *  If you would like to store header and image data separately for easy viewing and reading in other applications,
	 *  please use the appendImageHeader and appendArray functions to store the data in separate variables.
	 *
	 */
	template <typename T> int appendImage(Image<T>& m, const char* varname);

	/**
	 *   Reads an image stored with appendImage above. The index indicates which image to read.
	 */
	template <typename T> boost::shared_ptr< Image<T> > readImage(const char* varname, unsigned long index = 0);

	/**
	 *  This function appends only the ImageHeader to the data set. Typically used in conjunction with
	 *  appendArray to store data and header in separate variables.
	 *
	 */
	int appendImageHeader(ImageHeader& h, const char* varname) {
		std::vector<unsigned int> dims(1,1);
		NDArrayContainer<ImageHeader> tmp(dims,&h);
		return appendArray<ImageHeader>(tmp,varname);
	}

	/**
	 *   Returns the image header with the indicated index.
	 */
	boost::shared_ptr< ImageHeader > readImageHeader(const char* varname, unsigned long index = 0);

	/**
	 * Appends a generic array (which can contain simple data elements such as float, double, etc).
	 *
	 * In the file the variable (with name varname) will contain a multidimensional array with one
	 * more dimension than the array in a. The first dimension can expand and when new arrays are appended the
	 * first dimension will grow accordingly.
	 *
	 */
	template <typename T> int appendArray(NDArrayContainer<T>& a, const char* varname);

	/**
	 * Appends an array directly without using the NDArrayContainer class.
	 *
	 */
	template <typename T> int appendArray(std::vector<unsigned int>& dimensions, T* data, const char* varname) {
		NDArrayContainer<T> tmp(dimensions, data);
		return appendArray<T>(tmp,varname);
	}

	/**
	 *  Reads a multi-dimensional array from the data file.
	 */
	template <typename T> boost::shared_ptr< NDArrayContainer<T> > readArray(const char* varname, unsigned long index = 0);


        /**
	 *  This function appends the meta attribute information to the data set. The meta attributes are stored as an xml file.
	 */
	int appendImageAttrib(std::string& a, const char* varname);

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

/**
 *  Convenience class to provide thread-safe access to HDF5 in cases
 *  where that is not compiled into the HDF5 library.
 */
class EXPORTISMRMRD  HDF5Lock
{

public:
	static HDF5Lock* instance();

	void acquire();
	void release();

protected:
	HDF5Lock()
	: mutex_() { }

	virtual ~HDF5Lock() { }

	static HDF5Lock* instance_;

	boost::mutex mutex_;
};

class HDF5Exclusive
{
public:
	HDF5Exclusive() {
		HDF5Lock::instance()->acquire();
	}

	~HDF5Exclusive() {
		HDF5Lock::instance()->release();
	}

};


} //end of ISMRMRD namespace

#endif //ISMRMRD_HDF5_H

#include "ismrmrd_hdf5.h"
#include "ismrmrd_hdf5_datatypes.h"


#include <iostream>
#include <boost/algorithm/string.hpp>


namespace ISMRMRD
{

int IsmrmrdDataset::openHDF5File()
{

	std::cout << "Opening dataset..." << std::endl;
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
	}

	return 0;
}




  int H5AppendAcquisition(const Acquisition& a, const char* filename, const char* varname)
  {
	   /*

    H5Acquisition tmp;
    tmp.head = a.head_;

    tmp.traj.len = a.head_.trajectory_dimensions*a.head_.number_of_samples;
    tmp.traj.p = (void*) a.traj_;

    tmp.data.len = a.head_.number_of_samples*a.head_.active_channels*2;
    tmp.data.p = (void*) a.data_;

    boost::shared_ptr<DataType> structdatatype = getHDF5CompositeType<STRUCT>();
    boost::shared_ptr<DataType> vdatatype = getHDF5Type<DATATYPE>();
    vdatatype = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create (vdatatype->getId())));

	CompType* ct = new CompType(sizeof(local_hdf5_append_struct<STRUCT>));
	ct->insertMember( "h", HOFFSET(local_hdf5_append_struct<STRUCT>,h),  *structdatatype);
	ct->insertMember( "d", HOFFSET(local_hdf5_append_struct<STRUCT>,d),  *vdatatype);

	boost::shared_ptr<DataType> datatype(ct);


	return hdf5_append_struct(&tmp, datatype, filename, varname);
	*/
    return 0;
}

  /*
template <class T> boost::shared_ptr<CompType> getHDF5CompositeType();
template <class T> boost::shared_ptr<DataType> getHDF5ArrayType(int LENGTH);

template <> boost::shared_ptr<DataType> getHDF5ArrayType<float>(int LENGTH)
{
	std::vector<hsize_t> dims(1,LENGTH);
	//hid_t array_tid = H5Tarray_create(H5T_NATIVE_FLOAT, 1, &dims[0]);
	boost::shared_ptr<DataType> ret(new ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]));
	return ret;
}

template <> boost::shared_ptr<DataType> getHDF5ArrayType<ACE_UINT16>(int LENGTH)
{
	std::vector<hsize_t> dims(1,LENGTH);
	//hid_t array_tid = H5Tarray_create(H5T_NATIVE_USHORT, 1, &dims[0]);
	//boost::shared_ptr<DataType> ret(new DataType(array_tid));
	boost::shared_ptr<DataType> ret(new ArrayType(PredType::NATIVE_USHORT, 1, &dims[0]));
	return ret;
}

template <> boost::shared_ptr<CompType> getHDF5CompositeType<LoopCounters>()
{
	boost::shared_ptr<CompType> ret;

	try {
		ret = boost::shared_ptr<CompType>(new CompType(sizeof(LoopCounters)));

		ret->insertMember( "line",        HOFFSET(LoopCounters,line),        PredType::NATIVE_USHORT);
		ret->insertMember( "acquisition", HOFFSET(LoopCounters,acquisition), PredType::NATIVE_USHORT);
		ret->insertMember( "slice",       HOFFSET(LoopCounters,slice),       PredType::NATIVE_USHORT);
		ret->insertMember( "partition",   HOFFSET(LoopCounters,partition),   PredType::NATIVE_USHORT);
		ret->insertMember( "echo",        HOFFSET(LoopCounters,echo),        PredType::NATIVE_USHORT);
		ret->insertMember( "phase",       HOFFSET(LoopCounters,phase),       PredType::NATIVE_USHORT);
		ret->insertMember( "repetition",  HOFFSET(LoopCounters,repetition),  PredType::NATIVE_USHORT);
		ret->insertMember( "set",         HOFFSET(LoopCounters,set),         PredType::NATIVE_USHORT);
		ret->insertMember( "segment",     HOFFSET(LoopCounters,segment),     PredType::NATIVE_USHORT);
		ret->insertMember( "channel",     HOFFSET(LoopCounters,channel),     PredType::NATIVE_USHORT);
	} catch ( ... ) {
		std::cout << "Exception caught while creating HDF5 compound datatype for LoopCounter" << std::endl;
	}
	return ret;
}

template <> boost::shared_ptr<CompType> getHDF5CompositeType<GadgetMessageAcquisition>()
{
	boost::shared_ptr<CompType> ret(new CompType(sizeof(GadgetMessageAcquisition)));
	ret->insertMember( "flags",              HOFFSET(GadgetMessageAcquisition,flags),               PredType::NATIVE_UINT);
	ret->insertMember( "meas_uid",           HOFFSET(GadgetMessageAcquisition,meas_uid),            PredType::NATIVE_UINT);
	ret->insertMember( "scan_counter",       HOFFSET(GadgetMessageAcquisition, scan_counter),       PredType::NATIVE_UINT);
	ret->insertMember( "time_stamp",         HOFFSET(GadgetMessageAcquisition, time_stamp),         PredType::NATIVE_UINT);
	ret->insertMember( "pmu_time_stamp",     HOFFSET(GadgetMessageAcquisition, pmu_time_stamp),         PredType::NATIVE_UINT);
	ret->insertMember( "samples",            HOFFSET(GadgetMessageAcquisition, samples),            PredType::NATIVE_USHORT);
	ret->insertMember( "channels",           HOFFSET(GadgetMessageAcquisition, channels),           PredType::NATIVE_USHORT);
	ret->insertMember( "centre_column",      HOFFSET(GadgetMessageAcquisition, centre_column),           PredType::NATIVE_USHORT);

	boost::shared_ptr<DataType> position_type = getHDF5ArrayType<float>(3);
	boost::shared_ptr<DataType> quarterion_type = getHDF5ArrayType<float>(4);
	boost::shared_ptr<CompType> loopcounters_type = getHDF5CompositeType<LoopCounters>();

	ret->insertMember( "position",           HOFFSET(GadgetMessageAcquisition, position),       *position_type);
	ret->insertMember( "quarternion",        HOFFSET(GadgetMessageAcquisition, quarternion),        *quarterion_type);
	ret->insertMember( "table_position",     HOFFSET(GadgetMessageAcquisition, table_position),     PredType::NATIVE_FLOAT);
	ret->insertMember( "idx",                HOFFSET(GadgetMessageAcquisition, idx),                *loopcounters_type);
	ret->insertMember( "min_idx",            HOFFSET(GadgetMessageAcquisition, min_idx),            *loopcounters_type);
	ret->insertMember( "max_idx",            HOFFSET(GadgetMessageAcquisition, max_idx),            *loopcounters_type);

	return ret;
}


  int H5AppendAcquisition(const Acquisition& a, const char* filename, const char* varname);
  {

    H5Acquisition tmp;
    tmp.head = a.head_;

    tmp.traj.len = a.head_.trajectory_dimensions*a.head_.number_of_samples;
    tmp.traj.p = (void*) a.traj_;
    
    tmp.data.len = a.head_.number_of_samples*a.head_.active_channels*2;
    tmp.data.p = (void*) a.data_;

    boost::shared_ptr<DataType> structdatatype = getHDF5CompositeType<STRUCT>();
    boost::shared_ptr<DataType> vdatatype = getHDF5Type<DATATYPE>();
    vdatatype = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create (vdatatype->getId())));

	CompType* ct = new CompType(sizeof(local_hdf5_append_struct<STRUCT>));
	ct->insertMember( "h", HOFFSET(local_hdf5_append_struct<STRUCT>,h),  *structdatatype);
	ct->insertMember( "d", HOFFSET(local_hdf5_append_struct<STRUCT>,d),  *vdatatype);

	boost::shared_ptr<DataType> datatype(ct);


	return hdf5_append_struct(&tmp, datatype, filename, varname);
}
*/

} //End of ISMRMRD namespace

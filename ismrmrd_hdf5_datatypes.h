#pragma once
#ifndef ISMRMRD_HDF5_DATATYPES_H_
#define ISMRMRD_HDF5_DATATYPES_H_

#include "ismrmrd.h"

#include <H5Cpp.h>
#include <boost/shared_ptr.hpp>


#include <vector>

#ifndef H5_NO_NAMESPACE
	using namespace H5;
#endif

namespace ISMRMRD
{

template <typename T> boost::shared_ptr<DataType> getIsmrmrdHDF5Type();

template <> boost::shared_ptr<DataType> getIsmrmrdHDF5Type<EncodingCounters>()
{

	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(EncodingCounters)));

	ret->insertMember( "kspace_encode_step_1", 	HOFFSET(EncodingCounters, kspace_encode_step_1), PredType::NATIVE_UINT16);
	ret->insertMember( "kspace_encode_step_2", 	HOFFSET(EncodingCounters, kspace_encode_step_2), PredType::NATIVE_UINT16);
	ret->insertMember( "average", 				HOFFSET(EncodingCounters, average), 				PredType::NATIVE_UINT16);
	ret->insertMember( "slice", 				HOFFSET(EncodingCounters, slice), 				PredType::NATIVE_UINT16);
	ret->insertMember( "contrast", 				HOFFSET(EncodingCounters, contrast), 			PredType::NATIVE_UINT16);
	ret->insertMember( "phase", 				HOFFSET(EncodingCounters, phase), 				PredType::NATIVE_UINT16);
	ret->insertMember( "repetition", 			HOFFSET(EncodingCounters, repetition), 			PredType::NATIVE_UINT16);
	ret->insertMember( "set", 					HOFFSET(EncodingCounters, set), 					PredType::NATIVE_UINT16);
	ret->insertMember( "segment", 				HOFFSET(EncodingCounters, segment), 				PredType::NATIVE_UINT16);

	std::vector<hsize_t> dims(1,8);
	boost::shared_ptr<DataType> array_type(new ArrayType(PredType::NATIVE_UINT16, 1, &dims[0]));
	ret->insertMember( "user", 					HOFFSET(EncodingCounters, user), 				*array_type);

	return ret;
};

template <> boost::shared_ptr<DataType> getIsmrmrdHDF5Type<AcquisitionHeader>()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(AcquisitionHeader)));
	ret->insertMember( "version", 					HOFFSET(AcquisitionHeader, version), 				PredType::NATIVE_UINT16);
	ret->insertMember( "flags", 					HOFFSET(AcquisitionHeader, flags), 					PredType::NATIVE_UINT64);
	ret->insertMember( "measurement_uid", 			HOFFSET(AcquisitionHeader, measurement_uid), 		PredType::NATIVE_UINT32);
	ret->insertMember( "scan_counter", 				HOFFSET(AcquisitionHeader, scan_counter), 			PredType::NATIVE_UINT32);
	ret->insertMember( "acquisition_time_stamp", 	HOFFSET(AcquisitionHeader, acquisition_time_stamp), PredType::NATIVE_UINT32);

	std::vector<hsize_t> dims(1,3);
	boost::shared_ptr<DataType> array_type(new ArrayType(PredType::NATIVE_UINT32, 1, &dims[0]));
	ret->insertMember( "physiology_time_stamp", HOFFSET(AcquisitionHeader, physiology_time_stamp), 		*array_type);

	ret->insertMember( "number_of_samples", 		HOFFSET(AcquisitionHeader, number_of_samples), 		PredType::NATIVE_UINT16);
	ret->insertMember( "available_channels", 		HOFFSET(AcquisitionHeader, available_channels), 	PredType::NATIVE_UINT16);
	ret->insertMember( "active_channels", 			HOFFSET(AcquisitionHeader, active_channels), 		PredType::NATIVE_UINT16);

	dims[0] = 16;
	boost::shared_ptr<DataType> mask_array_type(new ArrayType(PredType::NATIVE_UINT64, 1, &dims[0]));
	ret->insertMember( "channel_mask", 				HOFFSET(AcquisitionHeader, channel_mask), 			*mask_array_type);
	ret->insertMember( "discard_pre", 				HOFFSET(AcquisitionHeader, discard_pre), 			PredType::NATIVE_UINT16);
	ret->insertMember( "discard_post", 				HOFFSET(AcquisitionHeader, discard_post), 			PredType::NATIVE_UINT16);
	ret->insertMember( "center_sample", 			HOFFSET(AcquisitionHeader, center_sample), 			PredType::NATIVE_UINT16);
	ret->insertMember( "encoding_space_ref", 		HOFFSET(AcquisitionHeader, encoding_space_ref), 	PredType::NATIVE_UINT16);
	ret->insertMember( "trajectory_dimensions",		HOFFSET(AcquisitionHeader, trajectory_dimensions), 	PredType::NATIVE_UINT16);
	ret->insertMember( "sample_time_us", 			HOFFSET(AcquisitionHeader, sample_time_us), 		PredType::NATIVE_FLOAT);

	dims[0] = 3;
	boost::shared_ptr<DataType> position_array_type(new ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]));
	ret->insertMember( "position", 					HOFFSET(AcquisitionHeader, position), 				*position_array_type);

	dims[0] = 4;
	boost::shared_ptr<DataType> quaterion_array_type(new ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]));
	ret->insertMember( "quarternion", 				HOFFSET(AcquisitionHeader, quarternion), 			*quaterion_array_type);

	dims[0] = 3;
	boost::shared_ptr<DataType> table_array_type(new ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]));
	ret->insertMember( "patient_table_position", 	HOFFSET(AcquisitionHeader, patient_table_position), *table_array_type);

	boost::shared_ptr<DataType>  ec_type = getIsmrmrdHDF5Type<EncodingCounters>();
	ret->insertMember( "idx", 						HOFFSET(AcquisitionHeader, idx), 					*ec_type);

	dims[0] = 8;
	boost::shared_ptr<DataType> user_int_array_type(new ArrayType(PredType::NATIVE_INT32, 1, &dims[0]));
	ret->insertMember( "user_int", 					HOFFSET(AcquisitionHeader, user_int), 				*user_int_array_type);

	dims[0] = 8;
	boost::shared_ptr<DataType> user_float_array_type(new ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]));
	ret->insertMember( "user_float", 					HOFFSET(AcquisitionHeader, user_float), 		*user_float_array_type);

	return ret;

}


struct complex_t
{
	float real;
	float imag;
};

template <> boost::shared_ptr<DataType> getIsmrmrdHDF5Type<complex_t>()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(complex_t)));
	ret->insertMember( "real",  HOFFSET(complex_t,real),   PredType::NATIVE_FLOAT);
	ret->insertMember( "imag",  HOFFSET(complex_t,imag),   PredType::NATIVE_FLOAT);
	return ret;
}

template <> boost::shared_ptr<DataType> getIsmrmrdHDF5Type<AcquisitionHeader_with_data>()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(AcquisitionHeader_with_data)));

	boost::shared_ptr<DataType>  head_type = getIsmrmrdHDF5Type<AcquisitionHeader>();
	boost::shared_ptr<DataType> cxvdatatype = getIsmrmrdHDF5Type<complex_t>();
	cxvdatatype = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create (cxvdatatype->getId())));
	boost::shared_ptr<DataType> realvdatatype = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create (PredType::NATIVE_FLOAT.getId())));



	ret->insertMember( "head",  HOFFSET(AcquisitionHeader_with_data,head),   	*head_type);
	ret->insertMember( "traj", HOFFSET(AcquisitionHeader_with_data,traj),  		*cxvdatatype);
	ret->insertMember( "data", HOFFSET(AcquisitionHeader_with_data,data),  		*cxvdatatype);
	return ret;
}

}

#endif /* ISMRMRD_HDF5_DATATYPES_H_ */

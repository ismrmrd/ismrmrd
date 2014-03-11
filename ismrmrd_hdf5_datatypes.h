#pragma once
#ifndef ISMRMRD_HDF5_DATATYPES_H_
#define ISMRMRD_HDF5_DATATYPES_H_

#include "ismrmrd.h"

#include <H5Cpp.h>
#include <boost/shared_ptr.hpp>


#include <vector>
#include <complex>
#include <string>

#ifndef H5_NO_NAMESPACE
	using namespace H5;
#endif

namespace ISMRMRD
{


template <typename T> boost::shared_ptr<DataType> getIsmrmrdHDF5Type();


template <> inline boost::shared_ptr<DataType> getIsmrmrdHDF5Type<float>()
{
	boost::shared_ptr<DataType> ret(new DataType(H5Tcopy(H5T_NATIVE_FLOAT)));
	return ret;
}

template <> inline boost::shared_ptr<DataType> getIsmrmrdHDF5Type<double>()
{
	boost::shared_ptr<DataType> ret(new DataType(H5Tcopy(H5T_NATIVE_DOUBLE)));
	return ret;
}

template <> inline boost::shared_ptr<DataType> getIsmrmrdHDF5Type<char>()
{
	boost::shared_ptr<DataType> ret(new DataType(H5Tcopy(H5T_NATIVE_CHAR)));
	return ret;
}

template <> inline boost::shared_ptr<DataType> getIsmrmrdHDF5Type< std::complex<float> >()
{
	CompType* ct = new CompType(sizeof( std::complex<float> ));
	ct->insertMember( "real",  0,              PredType::NATIVE_FLOAT);
	ct->insertMember( "imag",  sizeof(float),  PredType::NATIVE_FLOAT);
	boost::shared_ptr<DataType> ret(ct);
	return ret;
}

template <> inline boost::shared_ptr<DataType> getIsmrmrdHDF5Type< std::complex<double> >()
{
	CompType* ct = new CompType(sizeof( std::complex<double> ));
	ct->insertMember( "real",  0,              PredType::NATIVE_DOUBLE);
	ct->insertMember( "imag",  sizeof(double), PredType::NATIVE_DOUBLE);
	boost::shared_ptr<DataType> ret(ct);
	return ret;
}


template <> inline boost::shared_ptr<DataType> getIsmrmrdHDF5Type< unsigned short >()
{
	boost::shared_ptr<DataType> ret(new DataType(H5Tcopy(H5T_NATIVE_USHORT)));
	return ret;
}

template <> inline boost::shared_ptr<DataType> getIsmrmrdHDF5Type<EncodingCounters>()
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

template <> inline boost::shared_ptr<DataType> getIsmrmrdHDF5Type<AcquisitionHeader>()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(AcquisitionHeader)));
	ret->insertMember( "version", 					HOFFSET(AcquisitionHeader, version), 				PredType::NATIVE_UINT16);
	ret->insertMember( "flags", 					HOFFSET(AcquisitionHeader, flags), 					PredType::NATIVE_UINT64);
	ret->insertMember( "measurement_uid", 			HOFFSET(AcquisitionHeader, measurement_uid), 		PredType::NATIVE_UINT32);
	ret->insertMember( "scan_counter", 				HOFFSET(AcquisitionHeader, scan_counter), 			PredType::NATIVE_UINT32);
	ret->insertMember( "acquisition_time_stamp", 	HOFFSET(AcquisitionHeader, acquisition_time_stamp), PredType::NATIVE_UINT32);

        // TODO: change ISMRMRD_PHYS_STAMPS to 3 (Major change)
        // we should use the size defines and not hard coded numbers.
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

	dims[0] = 3;
	boost::shared_ptr<DataType> read_dir_array_type(new ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]));
	ret->insertMember( "read_dir", 				HOFFSET(AcquisitionHeader, read_dir), 			*read_dir_array_type);

	dims[0] = 3;
	boost::shared_ptr<DataType> phase_dir_array_type(new ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]));
	ret->insertMember( "phase_dir", 				HOFFSET(AcquisitionHeader, phase_dir), 			*phase_dir_array_type);

	dims[0] = 3;
	boost::shared_ptr<DataType> slice_dir_array_type(new ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]));
	ret->insertMember( "slice_dir", 				HOFFSET(AcquisitionHeader, slice_dir), 			*slice_dir_array_type);

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

struct double_complex_t
{
	double real;
	double imag;
};


template <> inline boost::shared_ptr<DataType> getIsmrmrdHDF5Type<complex_t>()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(complex_t)));
	ret->insertMember( "real",  HOFFSET(complex_t,real),   PredType::NATIVE_FLOAT);
	ret->insertMember( "imag",  HOFFSET(complex_t,imag),   PredType::NATIVE_FLOAT);
	return ret;
}

template <> inline boost::shared_ptr<DataType> getIsmrmrdHDF5Type<double_complex_t>()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(double_complex_t)));
	ret->insertMember( "real",  HOFFSET(complex_t,real),   PredType::NATIVE_DOUBLE);
	ret->insertMember( "imag",  HOFFSET(complex_t,imag),   PredType::NATIVE_DOUBLE);
	return ret;
}

template <> inline boost::shared_ptr<DataType> getIsmrmrdHDF5Type<AcquisitionHeader_with_data>()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(AcquisitionHeader_with_data)));

	boost::shared_ptr<DataType>  head_type = getIsmrmrdHDF5Type<AcquisitionHeader>();
	//boost::shared_ptr<DataType> cxvdatatype = getIsmrmrdHDF5Type<complex_t>();
	//cxvdatatype = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create (cxvdatatype->getId())));
	boost::shared_ptr<DataType> realvdatatype = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create (PredType::NATIVE_FLOAT.getId())));

	ret->insertMember( "head",  HOFFSET(AcquisitionHeader_with_data,head),   	*head_type);
	ret->insertMember( "traj", HOFFSET(AcquisitionHeader_with_data,traj),  		*realvdatatype);
	ret->insertMember( "data", HOFFSET(AcquisitionHeader_with_data,data),  		*realvdatatype);
	return ret;
}

template <> inline boost::shared_ptr<DataType> getIsmrmrdHDF5Type<ImageHeader>()
{

	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(ImageHeader)));

	ret->insertMember( "version", 					HOFFSET(ImageHeader, version), 					PredType::NATIVE_UINT16);
	ret->insertMember( "flags", 					HOFFSET(ImageHeader, flags), 				   	PredType::NATIVE_UINT64);
	ret->insertMember( "measurement_uid", 			HOFFSET(ImageHeader, measurement_uid), 		    PredType::NATIVE_UINT32);

	std::vector<hsize_t> dims(1,0);

	dims[0] = 3;
	boost::shared_ptr<DataType> mat_size_array_type(new ArrayType(PredType::NATIVE_UINT16, 1, &dims[0]));
	ret->insertMember( "matrix_size", 				HOFFSET(ImageHeader, matrix_size), 		*mat_size_array_type);

	dims[0] = 3;
	boost::shared_ptr<DataType> fov_array_type(new ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]));
	ret->insertMember( "field_of_view", 			HOFFSET(ImageHeader, field_of_view), 		*fov_array_type);

	ret->insertMember( "channels", 					HOFFSET(ImageHeader, channels), 					PredType::NATIVE_UINT16);

	dims[0] = 3;
	boost::shared_ptr<DataType> position_array_type(new ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]));
	ret->insertMember( "position", 					HOFFSET(ImageHeader, position), 				*position_array_type);

	dims[0] = 3;
	boost::shared_ptr<DataType> read_dir_array_type(new ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]));
	ret->insertMember( "read_dir", 				HOFFSET(ImageHeader, read_dir), 			*read_dir_array_type);

	dims[0] = 3;
	boost::shared_ptr<DataType> phase_dir_array_type(new ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]));
	ret->insertMember( "phase_dir", 				HOFFSET(ImageHeader, phase_dir), 			*phase_dir_array_type);

	dims[0] = 3;
	boost::shared_ptr<DataType> slice_dir_array_type(new ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]));
	ret->insertMember( "slice_dir", 				HOFFSET(ImageHeader, slice_dir), 			*slice_dir_array_type);

	dims[0] = 3;
	boost::shared_ptr<DataType> table_array_type(new ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]));
	ret->insertMember( "patient_table_position", 	HOFFSET(ImageHeader, patient_table_position), *table_array_type);


	ret->insertMember( "average", 					HOFFSET(ImageHeader, average), 					PredType::NATIVE_UINT16);
	ret->insertMember( "slice", 					HOFFSET(ImageHeader, slice), 					PredType::NATIVE_UINT16);
	ret->insertMember( "contrast", 					HOFFSET(ImageHeader, contrast), 				PredType::NATIVE_UINT16);
	ret->insertMember( "phase", 					HOFFSET(ImageHeader, phase), 					PredType::NATIVE_UINT16);
	ret->insertMember( "repetition", 				HOFFSET(ImageHeader, repetition), 				PredType::NATIVE_UINT16);
	ret->insertMember( "set",   					HOFFSET(ImageHeader, set), 						PredType::NATIVE_UINT16);
	ret->insertMember( "acquisition_time_stamp", 	HOFFSET(ImageHeader, acquisition_time_stamp),   PredType::NATIVE_UINT32);

        // TODO: ISMRMRD_PHYS_STAMPS should be 3 (Major change)
        // we should use the size defines and not hard coded numbers.
	dims[0] = 3;
	boost::shared_ptr<DataType> array_type(new ArrayType(PredType::NATIVE_UINT32, 1, &dims[0]));
	ret->insertMember( "physiology_time_stamp", HOFFSET(ImageHeader, physiology_time_stamp), 		*array_type);

	ret->insertMember( "image_data_type",   		HOFFSET(ImageHeader, image_data_type),			PredType::NATIVE_UINT16);
	ret->insertMember( "image_type",   				HOFFSET(ImageHeader, image_type),				PredType::NATIVE_UINT16);
	ret->insertMember( "image_index",   			HOFFSET(ImageHeader, image_index),				PredType::NATIVE_UINT16);
	ret->insertMember( "image_series_index",		HOFFSET(ImageHeader, image_series_index),		PredType::NATIVE_UINT16);

	dims[0] = 8;
	boost::shared_ptr<DataType> user_int_array_type(new ArrayType(PredType::NATIVE_INT32, 1, &dims[0]));
	ret->insertMember( "user_int", 				HOFFSET(ImageHeader, user_int), *user_int_array_type);

	dims[0] = 8;
	boost::shared_ptr<DataType> user_float_array_type(new ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]));
	ret->insertMember( "user_float", 				HOFFSET(ImageHeader, user_float), *user_float_array_type);

	return ret;
}


template <> inline boost::shared_ptr<DataType> getIsmrmrdHDF5Type<ImageHeader_with_data<float> >()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(ImageHeader_with_data<float>)));
	boost::shared_ptr<DataType>  head_type = getIsmrmrdHDF5Type<ImageHeader>();
	boost::shared_ptr<DataType> vdatatype = getIsmrmrdHDF5Type<float>();
	vdatatype = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create (vdatatype->getId())));
	ret->insertMember( "head",  HOFFSET(ImageHeader_with_data<float>,head),   	*head_type);
	ret->insertMember( "data", HOFFSET(ImageHeader_with_data<float>,data),  	*vdatatype);
	return ret;
}

template <> inline boost::shared_ptr<DataType> getIsmrmrdHDF5Type<ImageHeader_with_data<double> >()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(ImageHeader_with_data<double>)));
	boost::shared_ptr<DataType>  head_type = getIsmrmrdHDF5Type<ImageHeader>();
	boost::shared_ptr<DataType> vdatatype = getIsmrmrdHDF5Type<double>();
	vdatatype = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create (vdatatype->getId())));
	ret->insertMember( "head",  HOFFSET(ImageHeader_with_data<double>,head),   	*head_type);
	ret->insertMember( "data", HOFFSET(ImageHeader_with_data<double>,data),  	*vdatatype);
	return ret;
}

template <> inline boost::shared_ptr<DataType> getIsmrmrdHDF5Type<ImageHeader_with_data<unsigned short> >()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(ImageHeader_with_data<unsigned short>)));
	boost::shared_ptr<DataType>  head_type = getIsmrmrdHDF5Type<ImageHeader>();
	boost::shared_ptr<DataType> vdatatype = getIsmrmrdHDF5Type<unsigned short>();
	vdatatype = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create (vdatatype->getId())));
	ret->insertMember( "head",  HOFFSET(ImageHeader_with_data<unsigned short>,head),   	*head_type);
	ret->insertMember( "data", HOFFSET(ImageHeader_with_data<unsigned short>,data),  	*vdatatype);
	return ret;
}

template <> inline boost::shared_ptr<DataType> getIsmrmrdHDF5Type<ImageHeader_with_data<complex_t> >()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(ImageHeader_with_data<complex_t>)));
	boost::shared_ptr<DataType>  head_type = getIsmrmrdHDF5Type<ImageHeader>();
	boost::shared_ptr<DataType> vdatatype = getIsmrmrdHDF5Type<complex_t>();
	vdatatype = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create (vdatatype->getId())));
	ret->insertMember( "head",  HOFFSET(ImageHeader_with_data<complex_t>,head),   	*head_type);
	ret->insertMember( "data", HOFFSET(ImageHeader_with_data<complex_t>,data),  	*vdatatype);
	return ret;
}

template <> inline boost::shared_ptr<DataType> getIsmrmrdHDF5Type<ImageHeader_with_data<double_complex_t> >()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(ImageHeader_with_data<double_complex_t>)));
	boost::shared_ptr<DataType>  head_type = getIsmrmrdHDF5Type<ImageHeader>();
	boost::shared_ptr<DataType> vdatatype = getIsmrmrdHDF5Type<double_complex_t>();
	vdatatype = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create (vdatatype->getId())));
	ret->insertMember( "head",  HOFFSET(ImageHeader_with_data<double_complex_t>,head),   	*head_type);
	ret->insertMember( "data", HOFFSET(ImageHeader_with_data<double_complex_t>,data),  	*vdatatype);
	return ret;
}

template <> inline boost::shared_ptr<DataType> getIsmrmrdHDF5Type<ImageHeader_with_data< std::complex<float> > >()
{
	return getIsmrmrdHDF5Type<ImageHeader_with_data<complex_t> >();
}

template <> inline boost::shared_ptr<DataType> getIsmrmrdHDF5Type<ImageHeader_with_data< std::complex<double> > >()
{
	return getIsmrmrdHDF5Type<ImageHeader_with_data<double_complex_t> >();
}

template <> inline boost::shared_ptr<DataType> getIsmrmrdHDF5Type<std::string>()
{
	boost::shared_ptr<DataType> ret(new StrType(0, H5T_VARIABLE));
	return ret;
}

}

#endif /* ISMRMRD_HDF5_DATATYPES_H_ */

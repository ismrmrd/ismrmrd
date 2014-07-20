#pragma once
#ifndef ISMRMRD_HDF5_DATATYPES_H_
#define ISMRMRD_HDF5_DATATYPES_H_

#include "ismrmrd.h"

#include <hdf5.h>
#include <boost/shared_ptr.hpp>


#include <vector>
#include <complex>
#include <string>

//#ifndef H5_NO_NAMESPACE
//	using namespace H5;
//#endif

namespace ISMRMRD
{

  /* HDF5 C Interface (begin) */

  class IsmrmrdHDF5TypeContainer {
    
  public:

    ~IsmrmrdHDF5TypeContainer() {
      std::map<const std::type_info*, hid_t>::iterator it = hdf5_type_map_.begin();
      while (it != hdf5_type_map_.end()) {
	H5Tclose(it->second);
	it++;
      }
      it = hdf5_vlen_type_map_.begin();
      while (it != hdf5_vlen_type_map_.end()) {
	H5Tclose(it->second);
	it++;
      }
      
    }

    template <typename T> hid_t get_type() {
      std::map<const std::type_info*, hid_t>::iterator it = hdf5_type_map_.find(&typeid(T));
      if (it != hdf5_type_map_.end()) {
	return it->second;
      } else {
	hid_t t = this->getIsmrmrdHDF5Type<T>();
	hdf5_type_map_[&typeid(T)] = t;
	return t;
      }
    }
    
    template <typename T, int N> hid_t get_array_type() {
      std::map<const std::type_info*, hid_t>::iterator it = hdf5_type_map_.find(&typeid( T[N] ));
      if (it != hdf5_type_map_.end()) {
	return it->second;
      } else {
	hid_t t = this->getIsmrmrdHDF5ArrayType<T,N>();
	hdf5_type_map_[&typeid(T[N])] = t;
	return t;
      }
    }
    
    template <typename T> hid_t get_vlen_type() {
      std::map<const std::type_info*, hid_t>::iterator it = hdf5_vlen_type_map_.find(&typeid(T));
      if (it != hdf5_vlen_type_map_.end()) {
	return it->second;
      } else {
	hid_t t = this->getIsmrmrdHDF5VLenType<T>();
	hdf5_vlen_type_map_[&typeid(T)] = t;
	return t;
      }
    }
    
    
  protected:
    struct compare_typeinfo {
      bool operator ()(const std::type_info* a, const std::type_info* b) const {
	return a->before(*b);
      }
    };
    
    std::map<const std::type_info*, hid_t, compare_typeinfo> hdf5_type_map_;    
    std::map<const std::type_info*, hid_t, compare_typeinfo> hdf5_vlen_type_map_;    
    template <typename T> hid_t getIsmrmrdHDF5Type();
    template <typename T, int N> hid_t getIsmrmrdHDF5ArrayType();
    template <typename T> hid_t getIsmrmrdHDF5VLenType();

  }; //class IsmrmrdHDF5TypeContainer
  
  
  template <> inline hid_t IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5Type<float>() {
    return H5Tcopy(H5T_NATIVE_FLOAT);
  }
  
  template <> inline hid_t IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5Type<double>() {
    return H5Tcopy(H5T_NATIVE_DOUBLE);
  }
  
  template <> inline hid_t IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5Type<char>() {
    return H5Tcopy(H5T_NATIVE_CHAR);;
  }
  
  template <> inline hid_t IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5Type<unsigned short>() {
    return H5Tcopy(H5T_NATIVE_USHORT);;
  }
  
  template <> inline hid_t  IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5Type< std::complex<float> >() {
    hid_t t  = H5Tcreate(H5T_COMPOUND, sizeof(std::complex<float>));
    H5Tinsert(t, "real", 0, H5T_NATIVE_FLOAT);
    H5Tinsert(t, "imag", sizeof(float), H5T_NATIVE_FLOAT);
    return t;
  }
  
  template <> inline hid_t  IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5Type< std::complex<double> >() {
    hid_t t  = H5Tcreate(H5T_COMPOUND, sizeof(std::complex<double>));
    H5Tinsert(t, "real", 0, H5T_NATIVE_DOUBLE);
    H5Tinsert(t, "imag", sizeof(double), H5T_NATIVE_DOUBLE);
    return t;
  }
  
  template <> inline hid_t  IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5ArrayType< uint16_t, 3 >() {
    std::vector<hsize_t> dims(1,3);
    hid_t t  = H5Tarray_create(H5T_NATIVE_UINT16, 1, &dims[0]);
    return t;
  }

  template <> inline hid_t  IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5ArrayType< uint16_t, ISMRMRD_USER_INTS >() {
    std::vector<hsize_t> dims(1,ISMRMRD_USER_INTS);
    hid_t t  = H5Tarray_create(H5T_NATIVE_UINT16, 1, &dims[0]);
    return t;
  }

  template <> inline hid_t  IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5ArrayType< uint32_t, 3 >() { //TODO: The 3 should be replace with ISMRMRD_PHYS_TIME_STAMPS when that is corrected
    std::vector<hsize_t> dims(1,3);//TODO: Replace when header is updated
    hid_t t  = H5Tarray_create(H5T_NATIVE_UINT32, 1, &dims[0]);
    return t;
  }

  template <> inline hid_t  IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5ArrayType< int32_t, ISMRMRD_USER_INTS >() {
    std::vector<hsize_t> dims(1,ISMRMRD_USER_INTS);
    hid_t t  = H5Tarray_create(H5T_NATIVE_INT32, 1, &dims[0]);
    return t;
  }

  template <> inline hid_t  IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5ArrayType< uint64_t, ISMRMRD_CHANNEL_MASKS >() {
    std::vector<hsize_t> dims(1,ISMRMRD_CHANNEL_MASKS);
    hid_t t  = H5Tarray_create(H5T_NATIVE_UINT64, 1, &dims[0]);
    return t;
  }

  template <> inline hid_t  IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5ArrayType< float, ISMRMRD_POSITION_LENGTH >() {
    std::vector<hsize_t> dims(1,ISMRMRD_POSITION_LENGTH);
    hid_t t  = H5Tarray_create(H5T_NATIVE_FLOAT, 1, &dims[0]);
    return t;
  }
  
  template <> inline hid_t  IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5ArrayType< float, ISMRMRD_USER_FLOATS >() {
    std::vector<hsize_t> dims(1,ISMRMRD_USER_FLOATS);
    hid_t t  = H5Tarray_create(H5T_NATIVE_FLOAT, 1, &dims[0]);
    return t;
  }

  template <> inline hid_t  IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5Type<EncodingCounters>() {
    hid_t t  = H5Tcreate(H5T_COMPOUND, sizeof(EncodingCounters));    
    H5Tinsert(t, "kspace_encode_step_1", 	HOFFSET(EncodingCounters, kspace_encode_step_1), H5T_NATIVE_UINT16);
    H5Tinsert(t, "kspace_encode_step_2", 	HOFFSET(EncodingCounters, kspace_encode_step_2), H5T_NATIVE_UINT16);
    H5Tinsert(t, "average", 			HOFFSET(EncodingCounters, average), 		 H5T_NATIVE_UINT16);
    H5Tinsert(t, "slice", 			HOFFSET(EncodingCounters, slice), 		 H5T_NATIVE_UINT16);
    H5Tinsert(t, "contrast", 			HOFFSET(EncodingCounters, contrast), 		 H5T_NATIVE_UINT16);
    H5Tinsert(t, "phase", 			HOFFSET(EncodingCounters, phase), 		 H5T_NATIVE_UINT16);
    H5Tinsert(t, "repetition", 			HOFFSET(EncodingCounters, repetition), 		 H5T_NATIVE_UINT16);
    H5Tinsert(t, "set", 			HOFFSET(EncodingCounters, set), 		 H5T_NATIVE_UINT16);
    H5Tinsert(t, "segment", 			HOFFSET(EncodingCounters, segment), 		 H5T_NATIVE_UINT16);
    H5Tinsert(t, "user", 			HOFFSET(EncodingCounters, user), 	         get_array_type< uint16_t, ISMRMRD_USER_INTS >());
    return t;
  }
  
  template <> inline hid_t  IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5Type<AcquisitionHeader>() {
    hid_t t  = H5Tcreate(H5T_COMPOUND, sizeof(AcquisitionHeader));    
    H5Tinsert(t,"version", 			HOFFSET(AcquisitionHeader, version), 		       H5T_NATIVE_UINT16);
    H5Tinsert(t,"flags", 			HOFFSET(AcquisitionHeader, flags), 		       H5T_NATIVE_UINT64);
    H5Tinsert(t, "measurement_uid", 		HOFFSET(AcquisitionHeader, measurement_uid), 	       H5T_NATIVE_UINT32);
    H5Tinsert(t, "scan_counter", 		HOFFSET(AcquisitionHeader, scan_counter), 	       H5T_NATIVE_UINT32);
    H5Tinsert(t, "acquisition_time_stamp", 	HOFFSET(AcquisitionHeader, acquisition_time_stamp),    H5T_NATIVE_UINT32);
    H5Tinsert(t, "physiology_time_stamp",       HOFFSET(AcquisitionHeader, physiology_time_stamp),     get_array_type<uint32_t,3>()); //TODO: should be get_array_type<uint32_t,ISMRMRD_PHYS_STAMPS>()
    H5Tinsert(t, "number_of_samples", 		HOFFSET(AcquisitionHeader, number_of_samples), 	       H5T_NATIVE_UINT16);
    H5Tinsert(t, "available_channels", 		HOFFSET(AcquisitionHeader, available_channels),        H5T_NATIVE_UINT16);
    H5Tinsert(t, "active_channels", 		HOFFSET(AcquisitionHeader, active_channels),           H5T_NATIVE_UINT16);    
    H5Tinsert(t, "channel_mask", 		HOFFSET(AcquisitionHeader, channel_mask), 	       get_array_type<uint64_t,ISMRMRD_CHANNEL_MASKS>());
    H5Tinsert(t, "discard_pre", 		HOFFSET(AcquisitionHeader, discard_pre), 	       H5T_NATIVE_UINT16);
    H5Tinsert(t, "discard_post", 		HOFFSET(AcquisitionHeader, discard_post), 	       H5T_NATIVE_UINT16);
    H5Tinsert(t, "center_sample", 		HOFFSET(AcquisitionHeader, center_sample), 	       H5T_NATIVE_UINT16);
    H5Tinsert(t, "encoding_space_ref", 		HOFFSET(AcquisitionHeader, encoding_space_ref),        H5T_NATIVE_UINT16);
    H5Tinsert(t, "trajectory_dimensions",	HOFFSET(AcquisitionHeader, trajectory_dimensions),     H5T_NATIVE_UINT16);
    H5Tinsert(t, "sample_time_us", 		HOFFSET(AcquisitionHeader, sample_time_us), 	       H5T_NATIVE_FLOAT);    
    H5Tinsert(t, "position", 			HOFFSET(AcquisitionHeader, position), 		       get_array_type<float,ISMRMRD_POSITION_LENGTH>());
    H5Tinsert(t, "read_dir", 			HOFFSET(AcquisitionHeader, read_dir), 		       get_array_type<float,ISMRMRD_DIRECTION_LENGTH>());
    H5Tinsert(t, "phase_dir", 			HOFFSET(AcquisitionHeader, phase_dir), 	               get_array_type<float,ISMRMRD_DIRECTION_LENGTH>());
    H5Tinsert(t, "slice_dir", 			HOFFSET(AcquisitionHeader, slice_dir), 	               get_array_type<float,ISMRMRD_DIRECTION_LENGTH>());
    H5Tinsert(t, "patient_table_position", 	HOFFSET(AcquisitionHeader, patient_table_position),    get_array_type<float,ISMRMRD_DIRECTION_LENGTH>());
    H5Tinsert(t, "idx", 			HOFFSET(AcquisitionHeader, idx), 	               get_type<EncodingCounters>());
    H5Tinsert(t, "user_int", 			HOFFSET(AcquisitionHeader, user_int), 		       get_array_type<int32_t,ISMRMRD_USER_INTS>());
    H5Tinsert(t, "user_float", 			HOFFSET(AcquisitionHeader, user_float), 	       get_array_type<float,ISMRMRD_USER_FLOATS>());    
    return t; 
  }

  struct ccomplex_t
  {
    float real;
    float imag;
  };

  struct cdouble_complex_t
  {
    double real;
    double imag;
  };
  
  
  template <> inline hid_t IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5Type< ccomplex_t >() {
    hid_t t  = H5Tcreate(H5T_COMPOUND, sizeof(ccomplex_t));
    H5Tinsert(t, "real", HOFFSET(ccomplex_t, real), H5T_NATIVE_FLOAT);
    H5Tinsert(t, "imag", HOFFSET(ccomplex_t, imag),  H5T_NATIVE_FLOAT);
    return t;
  }
  
  template <> inline hid_t IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5Type< cdouble_complex_t >() {
    hid_t t  = H5Tcreate(H5T_COMPOUND, sizeof(cdouble_complex_t));
    H5Tinsert(t, "real", HOFFSET(cdouble_complex_t, real), H5T_NATIVE_FLOAT);
    H5Tinsert(t, "imag", HOFFSET(cdouble_complex_t, imag),  H5T_NATIVE_FLOAT);
    return t;
  }

  template <> inline hid_t IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5VLenType< float >() {
    return H5Tvlen_create(H5T_NATIVE_FLOAT);
  }

  template <> inline hid_t IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5VLenType< double >() {
    return H5Tvlen_create(H5T_NATIVE_DOUBLE);
  }

  template <> inline hid_t IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5VLenType< ccomplex_t >() {
    return H5Tvlen_create(get_type<ccomplex_t>());
  }

  template <> inline hid_t IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5VLenType< cdouble_complex_t >() {
    return H5Tvlen_create(get_type<cdouble_complex_t>());
  }

  template <> inline hid_t IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5VLenType< uint16_t >() {
    return H5Tvlen_create(H5T_NATIVE_UINT16);
  }

  template <> inline hid_t IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5Type<AcquisitionHeader_with_data>() {
    hid_t t  = H5Tcreate(H5T_COMPOUND, sizeof(AcquisitionHeader_with_data));
    H5Tinsert(t, "head", HOFFSET(AcquisitionHeader_with_data,head),    get_type<AcquisitionHeader>());
    H5Tinsert(t, "traj", HOFFSET(AcquisitionHeader_with_data,traj),    get_vlen_type<float>());
    H5Tinsert(t, "data", HOFFSET(AcquisitionHeader_with_data,data),    get_vlen_type< ccomplex_t>());
    return t;
  }


  template <> inline hid_t IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5Type<ImageHeader>() {
    hid_t t  = H5Tcreate(H5T_COMPOUND, sizeof(ImageHeader));
    
    H5Tinsert(t, "version", 				HOFFSET(ImageHeader, version), 			    H5T_NATIVE_UINT16);
    H5Tinsert(t, "flags", 				HOFFSET(ImageHeader, flags), 			    H5T_NATIVE_UINT64);
    H5Tinsert(t, "measurement_uid", 			HOFFSET(ImageHeader, measurement_uid), 		    H5T_NATIVE_UINT32);
    H5Tinsert(t, "matrix_size", 		        HOFFSET(ImageHeader, matrix_size), 		    get_array_type<uint16_t,3>());
    H5Tinsert(t, "field_of_view", 			HOFFSET(ImageHeader, field_of_view), 		    get_array_type<float, ISMRMRD_POSITION_LENGTH>());    
    H5Tinsert(t, "channels", 				HOFFSET(ImageHeader, channels), 	            H5T_NATIVE_UINT16);    
    H5Tinsert(t, "position", 				HOFFSET(ImageHeader, position), 		    get_array_type<float, ISMRMRD_POSITION_LENGTH>());
    H5Tinsert(t, "read_dir", 				HOFFSET(ImageHeader, read_dir), 		    get_array_type<float, ISMRMRD_DIRECTION_LENGTH>());
    H5Tinsert(t, "phase_dir", 				HOFFSET(ImageHeader, phase_dir), 		    get_array_type<float, ISMRMRD_DIRECTION_LENGTH>());
    H5Tinsert(t, "slice_dir", 				HOFFSET(ImageHeader, slice_dir), 		    get_array_type<float, ISMRMRD_DIRECTION_LENGTH>());
    H5Tinsert(t, "patient_table_position", 	        HOFFSET(ImageHeader, patient_table_position),       get_array_type<float, ISMRMRD_POSITION_LENGTH>());
    H5Tinsert(t, "average", 				HOFFSET(ImageHeader, average), 			    H5T_NATIVE_UINT16);
    H5Tinsert(t, "slice", 				HOFFSET(ImageHeader, slice), 			    H5T_NATIVE_UINT16);
    H5Tinsert(t, "contrast", 				HOFFSET(ImageHeader, contrast), 		    H5T_NATIVE_UINT16);
    H5Tinsert(t, "phase", 				HOFFSET(ImageHeader, phase), 			    H5T_NATIVE_UINT16);
    H5Tinsert(t, "repetition", 				HOFFSET(ImageHeader, repetition), 		    H5T_NATIVE_UINT16);
    H5Tinsert(t, "set",   				HOFFSET(ImageHeader, set), 			    H5T_NATIVE_UINT16);
    H5Tinsert(t, "acquisition_time_stamp", 	        HOFFSET(ImageHeader, acquisition_time_stamp),       H5T_NATIVE_UINT32);    
    H5Tinsert(t, "physiology_time_stamp",               HOFFSET(ImageHeader, physiology_time_stamp), 	    get_array_type<uint32_t, 3>()); //TODO: get_array_type<uint32_t, ISMRMRD_PHYS_STAMPS>()
    H5Tinsert(t, "image_data_type",   		        HOFFSET(ImageHeader, image_data_type),		    H5T_NATIVE_UINT16);
    H5Tinsert(t, "image_type",   			HOFFSET(ImageHeader, image_type),		    H5T_NATIVE_UINT16);
    H5Tinsert(t, "image_index",   			HOFFSET(ImageHeader, image_index),		    H5T_NATIVE_UINT16);
    H5Tinsert(t, "image_series_index",		        HOFFSET(ImageHeader, image_series_index),	    H5T_NATIVE_UINT16);
    H5Tinsert(t, "user_int", 				HOFFSET(ImageHeader, user_int),                     get_array_type<int32_t, ISMRMRD_USER_INTS>());
    H5Tinsert(t, "user_float", 				HOFFSET(ImageHeader, user_float),                   get_array_type<float, ISMRMRD_USER_FLOATS>());    
    return t;
}

  template <> inline hid_t IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5Type<ImageHeader_with_data<float> >() {
    hid_t t  = H5Tcreate(H5T_COMPOUND, sizeof(ImageHeader_with_data<float>));
    H5Tinsert(t, "head", HOFFSET(ImageHeader_with_data<float>,head),    get_type<ImageHeader>());
    H5Tinsert(t, "data", HOFFSET(ImageHeader_with_data<float>,data),    get_vlen_type<float>());
    return t;
  }

  template <> inline hid_t IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5Type<ImageHeader_with_data<double> >() {
    hid_t t  = H5Tcreate(H5T_COMPOUND, sizeof(ImageHeader_with_data<double>));
    H5Tinsert(t, "head", HOFFSET(ImageHeader_with_data<double>,head),    get_type<ImageHeader>());
    H5Tinsert(t, "data", HOFFSET(ImageHeader_with_data<double>,data),    get_vlen_type<double>());
    return t;
  }

  template <> inline hid_t IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5Type<ImageHeader_with_data<unsigned short> >() {
    hid_t t  = H5Tcreate(H5T_COMPOUND, sizeof(ImageHeader_with_data<unsigned short>));
    H5Tinsert(t, "head", HOFFSET(ImageHeader_with_data<unsigned short>,head),    get_type<ImageHeader>());
    H5Tinsert(t, "data", HOFFSET(ImageHeader_with_data<unsigned short>,data),    get_vlen_type<unsigned short>());
    return t;
  }

  template <> inline hid_t IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5Type<ImageHeader_with_data<ccomplex_t> >() {
    hid_t t  = H5Tcreate(H5T_COMPOUND, sizeof(ImageHeader_with_data<ccomplex_t>));
    H5Tinsert(t, "head", HOFFSET(ImageHeader_with_data<ccomplex_t>,head),    get_type<ImageHeader>());
    H5Tinsert(t, "data", HOFFSET(ImageHeader_with_data<ccomplex_t>,data),    get_vlen_type<ccomplex_t>());
    return t;
  }

  template <> inline hid_t IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5Type<ImageHeader_with_data<cdouble_complex_t> >() {
    hid_t t  = H5Tcreate(H5T_COMPOUND, sizeof(ImageHeader_with_data<cdouble_complex_t>));
    H5Tinsert(t, "head", HOFFSET(ImageHeader_with_data<cdouble_complex_t>,head),    get_type<ImageHeader>());
    H5Tinsert(t, "data", HOFFSET(ImageHeader_with_data<cdouble_complex_t>,data),    get_vlen_type<cdouble_complex_t>());
    return t;
  }

  template <> inline hid_t IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5Type<ImageHeader_with_data< std::complex<float> > >() {
    return IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5Type<ImageHeader_with_data<ccomplex_t> >();
  }

  template <> inline hid_t IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5Type<ImageHeader_with_data< std::complex<double> > >() {
    return IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5Type<ImageHeader_with_data<cdouble_complex_t> >();
  }


  template <> inline hid_t IsmrmrdHDF5TypeContainer::getIsmrmrdHDF5Type< std::string >() {
    hid_t t = H5Tcopy (H5T_C_S1);
    H5Tset_size (t,H5T_VARIABLE);
    return t;
  }
  /* HDF5 C Interface (end) */

#endif /* ISMRMRD_HDF5_DATATYPES_H_ */

classdef hdf5_datatypes
% This convenience class defines the HDF5 types used in the
% ISMRMRD HDF5 file
    properties
        T_float;
        T_double;
        T_char;
        T_complexfloat;
        T_complexdouble;
        T_ushort;
        T_EncodingCounters;
        T_AcquisitionHeader;
        T_Acquisition;
    end
    
    methods
        
        function obj = hdf5_datatypes()
            obj.T_float = ismrmrd.util.hdf5_datatypes.getType_float();
            obj.T_double = ismrmrd.util.hdf5_datatypes.getType_double();
            obj.T_char = ismrmrd.util.hdf5_datatypes.getType_char();
            obj.T_complexfloat = ismrmrd.util.hdf5_datatypes.getType_complexfloat();
            obj.T_complexdouble = ismrmrd.util.hdf5_datatypes.getType_complexdouble();
            obj.T_ushort = ismrmrd.util.hdf5_datatypes.getType_ushort();
            obj.T_EncodingCounters = ismrmrd.util.hdf5_datatypes.getType_EncodingCounters();
            obj.T_AcquisitionHeader = ismrmrd.util.hdf5_datatypes.getType_AcquisitionHeader();
            obj.T_Acquisition = ismrmrd.util.hdf5_datatypes.getType_Acquisition();
        end
        
    end
   
    methods (Static)

        function b = getType_float()
            b = H5T.copy('H5T_NATIVE_FLOAT');
        end

        function b = getType_double()
            b = H5T.copy('H5T_NATIVE_DOUBLE');
        end

        function b = getType_char()
            b = H5T.copy('H5T_NATIVE_CHAR');
        end

        function b = getType_complexfloat()
            typesize = 2*H5T.get_size('H5T_NATIVE_FLOAT');
            b = H5T.create ('H5T_COMPOUND', typesize);
            H5T.insert (b, 'real', 0, 'H5T_NATIVE_FLOAT');
            H5T.insert (b, 'imag', H5T.get_size('H5T_NATIVE_FLOAT'), 'H5T_NATIVE_FLOAT');
        end

        function b = getType_complexdouble()
            b = H5T.create ('H5T_COMPOUND', ...
                             2*H5T.get_size('H5T_NATIVE_DOUBLE'));
            H5T.insert (b, 'real', 0, 'H5T_NATIVE_DOUBLE');
            H5T.insert (b, 'imag', H5T.get_size('H5T_NATIVE_DOUBLE'), 'H5T_NATIVE_DOUBLE');
        end

        function b = getType_ushort()
            b = H5T.copy('H5T_NATIVE_USHORT');
        end

        function b = getType_EncodingCounters()

            typesize = 17*H5T.get_size('H5T_NATIVE_UINT16');
            b = H5T.create ('H5T_COMPOUND', typesize);

            offset = 0;
            H5T.insert(b, 'kspace_encode_step_1', offset, 'H5T_NATIVE_UINT16');

            offset = offset + H5T.get_size('H5T_NATIVE_UINT16');
            H5T.insert(b, 'kspace_encode_step_2', offset, 'H5T_NATIVE_UINT16');
            offset = offset + H5T.get_size('H5T_NATIVE_UINT16');

            H5T.insert(b, 'average', offset, 'H5T_NATIVE_UINT16');
            offset = offset + H5T.get_size('H5T_NATIVE_UINT16');

            H5T.insert(b, 'slice', offset, 'H5T_NATIVE_UINT16');
            offset = offset + H5T.get_size('H5T_NATIVE_UINT16');

            H5T.insert(b, 'contrast', offset, 'H5T_NATIVE_UINT16');
            offset = offset + H5T.get_size('H5T_NATIVE_UINT16');

            H5T.insert(b, 'phase', offset, 'H5T_NATIVE_UINT16');
            offset = offset + H5T.get_size('H5T_NATIVE_UINT16');

            H5T.insert(b, 'repetition', offset, 'H5T_NATIVE_UINT16');
            offset = offset + H5T.get_size('H5T_NATIVE_UINT16');

            H5T.insert(b, 'set', offset, 'H5T_NATIVE_UINT16');
            offset = offset + H5T.get_size('H5T_NATIVE_UINT16');

            H5T.insert(b, 'segment', offset, 'H5T_NATIVE_UINT16');
            offset = offset + H5T.get_size('H5T_NATIVE_UINT16');

            H5T.insert(b, 'user', offset, H5T.array_create('H5T_NATIVE_UINT16',[8]));

        end

        function b = getType_AcquisitionHeader()

            typesize = 0;
            typesize = typesize + H5T.get_size('H5T_NATIVE_UINT16'); % version
            typesize = typesize + H5T.get_size('H5T_NATIVE_UINT64'); % flag
            typesize = typesize + H5T.get_size('H5T_NATIVE_UINT32'); % measurement_uid
            typesize = typesize + H5T.get_size('H5T_NATIVE_UINT32'); % scan_counter
            typesize = typesize + H5T.get_size('H5T_NATIVE_UINT32'); % acquisition_time_stamp
            typesize = typesize + 3*H5T.get_size('H5T_NATIVE_UINT32'); % physio_time_stamps
            typesize = typesize + H5T.get_size('H5T_NATIVE_UINT16'); % number_of_samples
            typesize = typesize + H5T.get_size('H5T_NATIVE_UINT16'); % available_channels
            typesize = typesize + H5T.get_size('H5T_NATIVE_UINT16'); % active_channels
            typesize = typesize + 16*H5T.get_size('H5T_NATIVE_UINT64'); % channel_mask
            typesize = typesize + H5T.get_size('H5T_NATIVE_UINT16'); % discard_pre
            typesize = typesize + H5T.get_size('H5T_NATIVE_UINT16'); % discard_post
            typesize = typesize + H5T.get_size('H5T_NATIVE_UINT16'); % center_sample
            typesize = typesize + H5T.get_size('H5T_NATIVE_UINT16'); % encoding_space_ref
            typesize = typesize + H5T.get_size('H5T_NATIVE_UINT16'); % trajectory_dimension
            typesize = typesize + H5T.get_size('H5T_NATIVE_FLOAT');  % sample_time_us
            typesize = typesize + 3*H5T.get_size('H5T_NATIVE_FLOAT'); % position
            typesize = typesize + 3*H5T.get_size('H5T_NATIVE_FLOAT'); % read_dir
            typesize = typesize + 3*H5T.get_size('H5T_NATIVE_FLOAT'); % phase_dir
            typesize = typesize + 3*H5T.get_size('H5T_NATIVE_FLOAT'); % slice_dir
            typesize = typesize + 3*H5T.get_size('H5T_NATIVE_FLOAT'); % patient_table_position
            typesize = typesize + H5T.get_size(ismrmrd.util.hdf5_datatypes.getType_EncodingCounters()); % idx
            typesize = typesize + 8*H5T.get_size('H5T_NATIVE_INT32'); % user_int
            typesize = typesize + 8*H5T.get_size('H5T_NATIVE_FLOAT'); % user_float

            b = H5T.create ('H5T_COMPOUND', typesize);

            offset = 0;
            H5T.insert(b, 'version', offset, 'H5T_NATIVE_UINT16');
            offset = offset + H5T.get_size('H5T_NATIVE_UINT16');

            H5T.insert(b, 'flags', offset, 'H5T_NATIVE_UINT64');
            offset = offset + H5T.get_size('H5T_NATIVE_UINT64');

            H5T.insert(b, 'measurement_uid', offset, 'H5T_NATIVE_UINT32');
            offset = offset + H5T.get_size('H5T_NATIVE_UINT32');

            H5T.insert(b, 'scan_counter', offset, 'H5T_NATIVE_UINT32');
            offset = offset + H5T.get_size('H5T_NATIVE_UINT32');

            H5T.insert(b, 'acquisition_time_stamp', offset, 'H5T_NATIVE_UINT32');
            offset = offset + H5T.get_size('H5T_NATIVE_UINT32');

            H5T.insert(b, 'physiology_time_stamp', offset, H5T.array_create('H5T_NATIVE_UINT32',[3]));
            offset = offset + 3*H5T.get_size('H5T_NATIVE_UINT32');

            H5T.insert(b, 'number_of_samples', offset, 'H5T_NATIVE_UINT16');
            offset = offset + H5T.get_size('H5T_NATIVE_UINT16');

            H5T.insert(b, 'available_channels', offset, 'H5T_NATIVE_UINT16');
            offset = offset + H5T.get_size('H5T_NATIVE_UINT16');

            H5T.insert(b, 'active_channels', offset, 'H5T_NATIVE_UINT16');
            offset = offset + H5T.get_size('H5T_NATIVE_UINT16');

            H5T.insert(b, 'channel_mask', offset, H5T.array_create('H5T_NATIVE_UINT64',[16]));
            offset = offset + 16*H5T.get_size('H5T_NATIVE_UINT64');

            H5T.insert(b, 'discard_pre', offset, 'H5T_NATIVE_UINT16');
            offset = offset + H5T.get_size('H5T_NATIVE_UINT16');

            H5T.insert(b, 'discard_post', offset, 'H5T_NATIVE_UINT16');
            offset = offset + H5T.get_size('H5T_NATIVE_UINT16');

            H5T.insert(b, 'center_sample', offset, 'H5T_NATIVE_UINT16');
            offset = offset + H5T.get_size('H5T_NATIVE_UINT16');

            H5T.insert(b, 'encoding_space_ref', offset, 'H5T_NATIVE_UINT16');
            offset = offset + H5T.get_size('H5T_NATIVE_UINT16');

            H5T.insert(b, 'trajectory_dimensions', offset, 'H5T_NATIVE_UINT16');
            offset = offset + H5T.get_size('H5T_NATIVE_UINT16');

            H5T.insert(b, 'sample_time_us', offset, 'H5T_NATIVE_FLOAT');
            offset = offset + H5T.get_size('H5T_NATIVE_FLOAT');

            H5T.insert(b, 'position', offset, H5T.array_create('H5T_NATIVE_FLOAT',[3]));
            offset = offset + 3*H5T.get_size('H5T_NATIVE_FLOAT');

            H5T.insert(b, 'read_dir', offset, H5T.array_create('H5T_NATIVE_FLOAT',[3]));
            offset = offset + 3*H5T.get_size('H5T_NATIVE_FLOAT');

            H5T.insert(b, 'phase_dir', offset, H5T.array_create('H5T_NATIVE_FLOAT',[3]));
            offset = offset + 3*H5T.get_size('H5T_NATIVE_FLOAT');

            H5T.insert(b, 'slice_dir', offset, H5T.array_create('H5T_NATIVE_FLOAT',[3]));
            offset = offset + 3*H5T.get_size('H5T_NATIVE_FLOAT');

            H5T.insert(b, 'patient_table_position', offset, H5T.array_create('H5T_NATIVE_FLOAT',[3]));
            offset = offset + 3*H5T.get_size('H5T_NATIVE_FLOAT');

            H5T.insert(b, 'idx', offset, ismrmrd.util.hdf5_datatypes.getType_EncodingCounters);
            offset = offset + H5T.get_size(ismrmrd.util.hdf5_datatypes.getType_EncodingCounters);

            H5T.insert(b, 'user_int', offset, H5T.array_create('H5T_NATIVE_INT32',[8]));
            offset = offset + 8*H5T.get_size('H5T_NATIVE_INT32');

            H5T.insert(b, 'user_float', offset, H5T.array_create('H5T_NATIVE_FLOAT',[8]));
            %offset = offset + 8*H5T.get_size('H5T_NATIVE_FLOAT');

        end

        function b = getType_Acquisition()

            head = H5T.copy(ismrmrd.util.hdf5_datatypes.getType_AcquisitionHeader());
            traj = H5T.vlen_create(ismrmrd.util.hdf5_datatypes.getType_float());
            data = H5T.vlen_create(ismrmrd.util.hdf5_datatypes.getType_float());

            typesize = H5T.get_size(head) + H5T.get_size(traj) + H5T.get_size(data);

            b = H5T.create ('H5T_COMPOUND', typesize);

            offset = 0;
            H5T.insert(b, 'head', offset, head);
            offset = offset + H5T.get_size(head);
            H5T.insert(b, 'traj', offset, traj);
            offset = offset + H5T.get_size(traj);
            H5T.insert(b, 'data', offset, data);

        end
    end % Methods (Static)

end

% 	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(AcquisitionHeader_with_data)));
%
% 	boost::shared_ptr<DataType>  head_type = getIsmrmrdHDF5Type<AcquisitionHeader>();
% 	boost::shared_ptr<DataType> cxvdatatype = getIsmrmrdHDF5Type<complex_t>();
% 	cxvdatatype = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create (cxvdatatype->getId())));
% 	boost::shared_ptr<DataType> realvdatatype = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create (PredType::NATIVE_FLOAT.getId())));
%
% 	ret->insertMember( "head",  HOFFSET(AcquisitionHeader_with_data,head),   	*head_type);
% 	ret->insertMember( "traj", HOFFSET(AcquisitionHeader_with_data,traj),  		*realvdatatype);
% 	ret->insertMember( "data", HOFFSET(AcquisitionHeader_with_data,data),  		*cxvdatatype);



%
% template <> boost::shared_ptr<DataType> getIsmrmrdHDF5Type<ImageHeader>()
% {
%
% 	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(ImageHeader)));
%
% 	ret->insertMember( "version", 					HOFFSET(ImageHeader, version), 					PredType::NATIVE_UINT16);
% 	ret->insertMember( "flags", 					HOFFSET(ImageHeader, flags), 				   	PredType::NATIVE_UINT64);
% 	ret->insertMember( "measurement_uid", 			HOFFSET(ImageHeader, measurement_uid), 		    PredType::NATIVE_UINT32);
%
% 	std::vector<hsize_t> dims(1,0);
%
% 	dims[0] = 3;
% 	boost::shared_ptr<DataType> mat_size_array_type(new ArrayType(PredType::NATIVE_UINT16, 1, &dims[0]));
% 	ret->insertMember( "matrix_size", 				HOFFSET(ImageHeader, matrix_size), 		*mat_size_array_type);
%
% 	dims[0] = 3;
% 	boost::shared_ptr<DataType> fov_array_type(new ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]));
% 	ret->insertMember( "field_of_view", 			HOFFSET(ImageHeader, field_of_view), 		*fov_array_type);
%
% 	ret->insertMember( "channels", 					HOFFSET(ImageHeader, channels), 					PredType::NATIVE_UINT16);
%
% 	dims[0] = 3;
% 	boost::shared_ptr<DataType> position_array_type(new ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]));
% 	ret->insertMember( "position", 					HOFFSET(ImageHeader, position), 				*position_array_type);
%
%	dims[0] = 3;
%	boost::shared_ptr<DataType> phase_dir_array_type(new ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]));
%	ret->insertMember( "phase_dir", 				HOFFSET(ImageHeader, phase_dir), 			*phase_dir_array_type);
%
%	dims[0] = 3;
%	boost::shared_ptr<DataType> slice_dir_array_type(new ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]));
%	ret->insertMember( "slice_dir", 				HOFFSET(ImageHeader, slice_dir), 			*slice_dir_array_type);
%
%	dims[0] = 3;
%	boost::shared_ptr<DataType> table_array_type(new ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]));
%	ret->insertMember( "patient_table_position", 	HOFFSET(ImageHeader, patient_table_position), *table_array_type);
%
% 	dims[0] = 3;
% 	boost::shared_ptr<DataType> table_array_type(new ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]));
% 	ret->insertMember( "patient_table_position", 	HOFFSET(ImageHeader, patient_table_position), *table_array_type);
%
%
% 	ret->insertMember( "average", 					HOFFSET(ImageHeader, average), 					PredType::NATIVE_UINT16);
% 	ret->insertMember( "slice", 					HOFFSET(ImageHeader, slice), 					PredType::NATIVE_UINT16);
% 	ret->insertMember( "contrast", 					HOFFSET(ImageHeader, contrast), 				PredType::NATIVE_UINT16);
% 	ret->insertMember( "phase", 					HOFFSET(ImageHeader, phase), 					PredType::NATIVE_UINT16);
% 	ret->insertMember( "repetition", 				HOFFSET(ImageHeader, repetition), 				PredType::NATIVE_UINT16);
% 	ret->insertMember( "set",   					HOFFSET(ImageHeader, set), 						PredType::NATIVE_UINT16);
% 	ret->insertMember( "acquisition_time_stamp", 	HOFFSET(ImageHeader, acquisition_time_stamp),   PredType::NATIVE_UINT32);
%
% 	dims[0] = 3;
% 	boost::shared_ptr<DataType> array_type(new ArrayType(PredType::NATIVE_UINT32, 1, &dims[0]));
% 	ret->insertMember( "physiology_time_stamp", HOFFSET(ImageHeader, physiology_time_stamp), 		*array_type);
%
% 	ret->insertMember( "image_data_type",   		HOFFSET(ImageHeader, image_data_type),			PredType::NATIVE_UINT16);
% 	ret->insertMember( "image_type",   				HOFFSET(ImageHeader, image_type),				PredType::NATIVE_UINT16);
% 	ret->insertMember( "image_index",   			HOFFSET(ImageHeader, image_index),				PredType::NATIVE_UINT16);
% 	ret->insertMember( "image_series_index",		HOFFSET(ImageHeader, image_series_index),		PredType::NATIVE_UINT16);
%
% 	dims[0] = 8;
% 	boost::shared_ptr<DataType> user_int_array_type(new ArrayType(PredType::NATIVE_INT32, 1, &dims[0]));
% 	ret->insertMember( "user_int", 				HOFFSET(ImageHeader, user_int), *user_int_array_type);
%
% 	dims[0] = 8;
% 	boost::shared_ptr<DataType> user_float_array_type(new ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]));
% 	ret->insertMember( "user_float", 				HOFFSET(ImageHeader, user_float), *user_float_array_type);
%
% 	return ret;
% }
%
%
% template <> boost::shared_ptr<DataType> getIsmrmrdHDF5Type<ImageHeader_with_data<float> >()
% {
% 	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(ImageHeader_with_data<float>)));
% 	boost::shared_ptr<DataType>  head_type = getIsmrmrdHDF5Type<ImageHeader>();
% 	boost::shared_ptr<DataType> vdatatype = getIsmrmrdHDF5Type<float>();
% 	vdatatype = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create (vdatatype->getId())));
% 	ret->insertMember( "head",  HOFFSET(ImageHeader_with_data<float>,head),   	*head_type);
% 	ret->insertMember( "data", HOFFSET(ImageHeader_with_data<float>,data),  	*vdatatype);
% 	return ret;
% }
%
% template <> boost::shared_ptr<DataType> getIsmrmrdHDF5Type<ImageHeader_with_data<double> >()
% {
% 	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(ImageHeader_with_data<double>)));
% 	boost::shared_ptr<DataType>  head_type = getIsmrmrdHDF5Type<ImageHeader>();
% 	boost::shared_ptr<DataType> vdatatype = getIsmrmrdHDF5Type<double>();
% 	vdatatype = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create (vdatatype->getId())));
% 	ret->insertMember( "head",  HOFFSET(ImageHeader_with_data<double>,head),   	*head_type);
% 	ret->insertMember( "data", HOFFSET(ImageHeader_with_data<double>,data),  	*vdatatype);
% 	return ret;
% }
%
% template <> boost::shared_ptr<DataType> getIsmrmrdHDF5Type<ImageHeader_with_data<unsigned short> >()
% {
% 	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(ImageHeader_with_data<unsigned short>)));
% 	boost::shared_ptr<DataType>  head_type = getIsmrmrdHDF5Type<ImageHeader>();
% 	boost::shared_ptr<DataType> vdatatype = getIsmrmrdHDF5Type<unsigned short>();
% 	vdatatype = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create (vdatatype->getId())));
% 	ret->insertMember( "head",  HOFFSET(ImageHeader_with_data<unsigned short>,head),   	*head_type);
% 	ret->insertMember( "data", HOFFSET(ImageHeader_with_data<unsigned short>,data),  	*vdatatype);
% 	return ret;
% }
%
% template <> boost::shared_ptr<DataType> getIsmrmrdHDF5Type<ImageHeader_with_data<complex_t> >()
% {
% 	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(ImageHeader_with_data<complex_t>)));
% 	boost::shared_ptr<DataType>  head_type = getIsmrmrdHDF5Type<ImageHeader>();
% 	boost::shared_ptr<DataType> vdatatype = getIsmrmrdHDF5Type<complex_t>();
% 	vdatatype = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create (vdatatype->getId())));
% 	ret->insertMember( "head",  HOFFSET(ImageHeader_with_data<complex_t>,head),   	*head_type);
% 	ret->insertMember( "data", HOFFSET(ImageHeader_with_data<complex_t>,data),  	*vdatatype);
% 	return ret;
% }
%
% template <> boost::shared_ptr<DataType> getIsmrmrdHDF5Type<ImageHeader_with_data<double_complex_t> >()
% {
% 	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(ImageHeader_with_data<double_complex_t>)));
% 	boost::shared_ptr<DataType>  head_type = getIsmrmrdHDF5Type<ImageHeader>();
% 	boost::shared_ptr<DataType> vdatatype = getIsmrmrdHDF5Type<double_complex_t>();
% 	vdatatype = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create (vdatatype->getId())));
% 	ret->insertMember( "head",  HOFFSET(ImageHeader_with_data<double_complex_t>,head),   	*head_type);
% 	ret->insertMember( "data", HOFFSET(ImageHeader_with_data<double_complex_t>,data),  	*vdatatype);
% 	return ret;
% }
%
% template <> boost::shared_ptr<DataType> getIsmrmrdHDF5Type<ImageHeader_with_data< std::complex<float> > >()
% {
% 	return getIsmrmrdHDF5Type<ImageHeader_with_data<complex_t> >();
% }
%
% template <> boost::shared_ptr<DataType> getIsmrmrdHDF5Type<ImageHeader_with_data< std::complex<double> > >()
% {
% 	return getIsmrmrdHDF5Type<ImageHeader_with_data<double_complex_t> >();
% }
%
% }
%

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


/*
template <> boost::shared_ptr<DataType> getSiemensHDF5Type<mdhCutOff>()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(mdhCutOff)));

	ret->insertMember( "ushPre",             HOFFSET(mdhCutOff,ushPre),         PredType::NATIVE_UINT16);
	ret->insertMember( "ushPost",            HOFFSET(mdhCutOff,ushPost),        PredType::NATIVE_UINT16);

	return ret;
};

template <> boost::shared_ptr<DataType> getSiemensHDF5Type<mdhSlicePosVec>()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(mdhSlicePosVec)));

	ret->insertMember( "flSag",             HOFFSET(mdhSlicePosVec,flSag),         PredType::NATIVE_FLOAT);
	ret->insertMember( "flCor",             HOFFSET(mdhSlicePosVec,flCor),         PredType::NATIVE_FLOAT);
	ret->insertMember( "flTra",             HOFFSET(mdhSlicePosVec,flTra),         PredType::NATIVE_FLOAT);

	return ret;
};

template <> boost::shared_ptr<DataType> getSiemensHDF5Type<mdhSliceData>()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(mdhSliceData)));

	std::vector<hsize_t> dims(1,4);
	boost::shared_ptr<DataType> array_type(new ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]));
	boost::shared_ptr<DataType>  slicepos_type = getSiemensHDF5Type<mdhSlicePosVec>();

	ret->insertMember( "sSlicePosVec",             HOFFSET(mdhSliceData,sSlicePosVec),  *slicepos_type);
	ret->insertMember( "aflQuaternion",            HOFFSET(mdhSliceData,aflQuaternion), *array_type);
	return ret;
};

template <> boost::shared_ptr<DataType> getSiemensHDF5Type<sMDH>()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(sMDH)));

	std::vector<hsize_t> dims(1,2);
	boost::shared_ptr<DataType> array_type_eval(new ArrayType(PredType::NATIVE_UINT, 1, &dims[0]));

	dims[0] = 4;
	boost::shared_ptr<DataType> array_type_icepara(new ArrayType(PredType::NATIVE_USHORT, 1, &dims[0]));
	boost::shared_ptr<DataType> array_type_freepara(new ArrayType(PredType::NATIVE_USHORT, 1, &dims[0]));

	boost::shared_ptr<DataType>  lc_type = getSiemensHDF5Type<mdhLC>();
	boost::shared_ptr<DataType>  cutoff_type = getSiemensHDF5Type<mdhCutOff>();

	boost::shared_ptr<DataType>  slicedata_type = getSiemensHDF5Type<mdhSliceData>();

	ret->insertMember( "ulFlagsAndDMALength",        HOFFSET(sMDH,ulFlagsAndDMALength),        PredType::NATIVE_UINT32);
	ret->insertMember( "lMeasUID",                   HOFFSET(sMDH,lMeasUID),                   PredType::NATIVE_INT32);
	ret->insertMember( "ulScanCounter",              HOFFSET(sMDH,ulScanCounter),              PredType::NATIVE_UINT32);
	ret->insertMember( "ulTimeStamp",                HOFFSET(sMDH,ulTimeStamp),                PredType::NATIVE_UINT32);
	ret->insertMember( "ulPMUTimeStamp",             HOFFSET(sMDH,ulPMUTimeStamp),             PredType::NATIVE_UINT32);
	ret->insertMember( "aulEvalInfoMask",            HOFFSET(sMDH,aulEvalInfoMask),            *array_type_eval);
	ret->insertMember( "ushSamplesInScan",           HOFFSET(sMDH,ushSamplesInScan),           PredType::NATIVE_UINT16);
	ret->insertMember( "ushUsedChannels",            HOFFSET(sMDH,ushUsedChannels),            PredType::NATIVE_UINT16);
	ret->insertMember( "sLC",                        HOFFSET(sMDH,sLC),                        *lc_type);
	ret->insertMember( "sCutOff",                    HOFFSET(sMDH,sCutOff),                    *cutoff_type);
	ret->insertMember( "ushKSpaceCentreColumn",      HOFFSET(sMDH,ushKSpaceCentreColumn),      PredType::NATIVE_UINT16);
	ret->insertMember( "ushCoilSelect",              HOFFSET(sMDH,ushCoilSelect),              PredType::NATIVE_UINT16);
	ret->insertMember( "fReadOutOffcentre",          HOFFSET(sMDH,fReadOutOffcentre),          PredType::NATIVE_FLOAT);
	ret->insertMember( "ulTimeSinceLastRF",          HOFFSET(sMDH,ulTimeSinceLastRF),          PredType::NATIVE_UINT16);
	ret->insertMember( "ushKSpaceCentreLineNo",      HOFFSET(sMDH,ushKSpaceCentreLineNo),      PredType::NATIVE_UINT16);
	ret->insertMember( "ushKSpaceCentrePartitionNo", HOFFSET(sMDH,ushKSpaceCentrePartitionNo), PredType::NATIVE_UINT16);
	ret->insertMember( "aushIceProgramPara",         HOFFSET(sMDH,aushIceProgramPara),         *array_type_icepara);
	ret->insertMember( "aushFreePara",               HOFFSET(sMDH,aushFreePara),               *array_type_freepara);
	ret->insertMember( "sSliceData",                 HOFFSET(sMDH,sSliceData),                 *slicedata_type);
	ret->insertMember( "ushChannelId",               HOFFSET(sMDH,ushChannelId),               PredType::NATIVE_UINT16);
	ret->insertMember( "ushPTABPosNeg",              HOFFSET(sMDH,ushPTABPosNeg),              PredType::NATIVE_UINT16);

	return ret;
}

struct MDH_with_data
{
	sMDH mdh;
	hvl_t data;
};

struct complex_t
{
	float real;
	float imag;
};

template <> boost::shared_ptr<DataType> getSiemensHDF5Type<complex_t>()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(complex_t)));
	ret->insertMember( "real",  HOFFSET(complex_t,real),   PredType::NATIVE_FLOAT);
	ret->insertMember( "imag",  HOFFSET(complex_t,imag),   PredType::NATIVE_FLOAT);
	return ret;
}

template <> boost::shared_ptr<DataType> getSiemensHDF5Type<MDH_with_data>()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(MDH_with_data)));

	boost::shared_ptr<DataType>  mdh_type = getSiemensHDF5Type<sMDH>();
	boost::shared_ptr<DataType> vdatatype = getSiemensHDF5Type<complex_t>();
	vdatatype = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create (vdatatype->getId())));

	ret->insertMember( "mdh",  HOFFSET(MDH_with_data,mdh),   *mdh_type);
	ret->insertMember( "data", HOFFSET(MDH_with_data,data),  *vdatatype);

	return ret;
}

template <> boost::shared_ptr<DataType> getSiemensHDF5Type<sScanHeader>()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(sScanHeader)));

	std::vector<hsize_t> dims(1,2);
	boost::shared_ptr<DataType> array_type_eval(new ArrayType(PredType::NATIVE_UINT, 1, &dims[0]));

	dims[0] = 24;
	boost::shared_ptr<DataType> array_type_icepara(new ArrayType(PredType::NATIVE_USHORT, 1, &dims[0]));
	dims[0] = 4;
	boost::shared_ptr<DataType> array_type_reserved(new ArrayType(PredType::NATIVE_USHORT, 1, &dims[0]));

	boost::shared_ptr<DataType>  lc_type = getSiemensHDF5Type<mdhLC>();
	boost::shared_ptr<DataType>  cutoff_type = getSiemensHDF5Type<mdhCutOff>();

	boost::shared_ptr<DataType>  slicedata_type = getSiemensHDF5Type<mdhSliceData>();

	ret->insertMember( "ulFlagsAndDMALength",        HOFFSET(sScanHeader,ulFlagsAndDMALength),        PredType::NATIVE_UINT32);
	ret->insertMember( "lMeasUID",                   HOFFSET(sScanHeader,lMeasUID),                   PredType::NATIVE_INT32);
	ret->insertMember( "ulScanCounter",              HOFFSET(sScanHeader,ulScanCounter),              PredType::NATIVE_UINT32);
	ret->insertMember( "ulTimeStamp",                HOFFSET(sScanHeader,ulTimeStamp),                PredType::NATIVE_UINT32);
	ret->insertMember( "ulPMUTimeStamp",             HOFFSET(sScanHeader,ulPMUTimeStamp),             PredType::NATIVE_UINT32);
	ret->insertMember( "ushSystemType",              HOFFSET(sScanHeader,ushSystemType),              PredType::NATIVE_UINT16);
	ret->insertMember( "ulPTABPosDelay",             HOFFSET(sScanHeader,ulPTABPosDelay),             PredType::NATIVE_UINT16);
	ret->insertMember( "lPTABPosX",                  HOFFSET(sScanHeader,lPTABPosX),                  PredType::NATIVE_INT32);
	ret->insertMember( "lPTABPosY",                  HOFFSET(sScanHeader,lPTABPosY),                  PredType::NATIVE_INT32);
	ret->insertMember( "lPTABPosZ",                  HOFFSET(sScanHeader,lPTABPosZ),                  PredType::NATIVE_INT32);
	ret->insertMember( "ulReserved1",                HOFFSET(sScanHeader,ulReserved1),                PredType::NATIVE_UINT32);
	ret->insertMember( "aulEvalInfoMask",            HOFFSET(sScanHeader,aulEvalInfoMask),            *array_type_eval);
	ret->insertMember( "ushSamplesInScan",           HOFFSET(sScanHeader,ushSamplesInScan),           PredType::NATIVE_UINT16);
	ret->insertMember( "ushUsedChannels",            HOFFSET(sScanHeader,ushUsedChannels),            PredType::NATIVE_UINT16);
	ret->insertMember( "sLC",                        HOFFSET(sScanHeader,sLC),                        *lc_type);
	ret->insertMember( "sCutOff",                    HOFFSET(sScanHeader,sCutOff),                    *cutoff_type);
	ret->insertMember( "ushKSpaceCentreColumn",      HOFFSET(sScanHeader,ushKSpaceCentreColumn),      PredType::NATIVE_UINT16);
	ret->insertMember( "ushCoilSelect",              HOFFSET(sScanHeader,ushCoilSelect),              PredType::NATIVE_UINT16);
	ret->insertMember( "fReadOutOffcentre",          HOFFSET(sScanHeader,fReadOutOffcentre),          PredType::NATIVE_FLOAT);
	ret->insertMember( "ulTimeSinceLastRF",          HOFFSET(sScanHeader,ulTimeSinceLastRF),          PredType::NATIVE_UINT16);
	ret->insertMember( "ushKSpaceCentreLineNo",      HOFFSET(sScanHeader,ushKSpaceCentreLineNo),      PredType::NATIVE_UINT16);
	ret->insertMember( "ushKSpaceCentrePartitionNo", HOFFSET(sScanHeader,ushKSpaceCentrePartitionNo), PredType::NATIVE_UINT16);
	ret->insertMember( "sSliceData",                 HOFFSET(sScanHeader,sSliceData),                 *slicedata_type);
	ret->insertMember( "aushIceProgramPara",         HOFFSET(sScanHeader,aushIceProgramPara),         *array_type_icepara);
	ret->insertMember( "aushReservedPara",           HOFFSET(sScanHeader,aushReservedPara),           *array_type_reserved);
	ret->insertMember( "ushApplicationCounter",      HOFFSET(sScanHeader,ushApplicationCounter),      PredType::NATIVE_UINT16);
	ret->insertMember( "ushApplicationMask",         HOFFSET(sScanHeader,ushApplicationMask),         PredType::NATIVE_UINT16);
	ret->insertMember( "ulCRC",                      HOFFSET(sScanHeader,ulCRC),                      PredType::NATIVE_UINT32);

	return ret;
}

template <> boost::shared_ptr<DataType> getSiemensHDF5Type<sChannelHeader>()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(sChannelHeader)));

	ret->insertMember( "ulTypeAndChannelLength", HOFFSET(sChannelHeader,ulTypeAndChannelLength),     PredType::NATIVE_UINT32);
	ret->insertMember( "lMeasUID",               HOFFSET(sChannelHeader,lMeasUID),                   PredType::NATIVE_INT32);
	ret->insertMember( "ulScanCounter",          HOFFSET(sChannelHeader,ulScanCounter),              PredType::NATIVE_UINT32);
	ret->insertMember( "ulReserved1",            HOFFSET(sChannelHeader,ulReserved1),                PredType::NATIVE_UINT32);
	ret->insertMember( "ulSequenceTime",         HOFFSET(sChannelHeader,ulSequenceTime),             PredType::NATIVE_UINT32);
	ret->insertMember( "ulUnused2",              HOFFSET(sChannelHeader,ulUnused2),                  PredType::NATIVE_UINT32);
	ret->insertMember( "ulChannelId",            HOFFSET(sChannelHeader,ulChannelId),                PredType::NATIVE_UINT16);
	ret->insertMember( "ulUnused3",              HOFFSET(sChannelHeader,ulUnused3),                  PredType::NATIVE_UINT16);
	ret->insertMember( "ulCRC",                  HOFFSET(sChannelHeader,ulCRC),                      PredType::NATIVE_UINT32);

	return ret;
}

struct sChannelHeader_with_data
{
	sChannelHeader channelHeader;
	hvl_t data;
};

void ClearsChannelHeader_with_data(sChannelHeader_with_data* b)
{
	if (b->data.len) {
		if (b->data.p) {
			complex_t* ptr = reinterpret_cast<complex_t*>(b->data.p);
			delete [] ptr;
		}
		b->data.p = 0;
		b->data.len = 0;
	}
}

template <> boost::shared_ptr<DataType> getSiemensHDF5Type<sChannelHeader_with_data>()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(sChannelHeader_with_data)));

	boost::shared_ptr<DataType>  channelheader_type = getSiemensHDF5Type<sChannelHeader>();
	boost::shared_ptr<DataType> vdatatype = getSiemensHDF5Type<complex_t>();
	vdatatype = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create (vdatatype->getId())));
	ret->insertMember( "channelHeader",  HOFFSET(sChannelHeader_with_data, channelHeader),   *channelheader_type);
	ret->insertMember( "data",            HOFFSET(sChannelHeader_with_data,data),              *vdatatype);
	return ret;
}

struct sScanHeader_with_data
{
	sScanHeader scanHeader;
	hvl_t data;
};

struct sScanHeader_with_syncdata
{
	sScanHeader scanHeader;
	uint32_t last_scan_counter;
	hvl_t syncdata;
};

void ClearsScanHeader_with_data(sScanHeader_with_data* c)
{
	if (c->data.len) {
		if (c->data.p) {
			for (unsigned int i = 0; i < c->data.len; i++) {
				sChannelHeader_with_data* ptr = reinterpret_cast<sChannelHeader_with_data*>(c->data.p);
				ClearsChannelHeader_with_data(ptr+i);
			}
		}
		c->data.p = 0;
		c->data.len = 0;
	}
}

template <> boost::shared_ptr<DataType> getSiemensHDF5Type<sScanHeader_with_data>()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(sScanHeader_with_data)));

	boost::shared_ptr<DataType>  scanheader_type = getSiemensHDF5Type<sScanHeader>();
	boost::shared_ptr<DataType> vdatatype = getSiemensHDF5Type<sChannelHeader_with_data>();
	vdatatype = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create (vdatatype->getId())));
	ret->insertMember( "scanHeader",  HOFFSET(sScanHeader_with_data, scanHeader), *scanheader_type);
	ret->insertMember( "data",    HOFFSET(sScanHeader_with_data,data),    *vdatatype);
	return ret;
}

template <> boost::shared_ptr<DataType> getSiemensHDF5Type<sScanHeader_with_syncdata>()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(sScanHeader_with_syncdata)));

	boost::shared_ptr<DataType>  scanheader_type = getSiemensHDF5Type<sScanHeader>();
	boost::shared_ptr<DataType> vdatatype = boost::shared_ptr<DataType>(new DataType(PredType::NATIVE_CHAR));
	vdatatype = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create (vdatatype->getId())));
	ret->insertMember( "scanHeader",  HOFFSET(sScanHeader_with_syncdata, scanHeader), *scanheader_type);
	ret->insertMember( "last_scan_counter",  HOFFSET(sScanHeader_with_syncdata, last_scan_counter), PredType::NATIVE_UINT32);
	ret->insertMember( "syncdata",    HOFFSET(sScanHeader_with_syncdata,syncdata),    *vdatatype);
	return ret;
}

struct MeasurementHeaderBuffer
{
	hvl_t bufName_;
	uint32_t bufLen_;
	hvl_t buf_;
};

void ClearMeasurementHeaderBuffer(MeasurementHeaderBuffer* b)
{
	if (b->bufName_.len) {
		if (b->bufName_.p) {
			char* ptr = reinterpret_cast<char*>(b->bufName_.p);
			delete [] ptr;
		}
		b->bufName_.p = 0;
		b->bufName_.len = 0;
	}

	if (b->buf_.len) {
		if (b->buf_.p) {
			char* ptr = reinterpret_cast<char*>(b->buf_.p);
			delete [] ptr;
		}
		b->buf_.len = 0;
		b->buf_.p = 0;
	}
}

struct MeasurementHeader
{

public:
	uint32_t dma_length;
	uint32_t nr_buffers;
	hvl_t buffers;

};

void ClearMeasurementHeader(MeasurementHeader* h)
{
	if (h->buffers.len) {
		if (h->buffers.p) {
			MeasurementHeaderBuffer* ptr = reinterpret_cast<MeasurementHeaderBuffer*>(h->buffers.p);
			for (unsigned int i = 0; i < h->buffers.len; i++) {
				ClearMeasurementHeaderBuffer(ptr+i);
			}
		}
		h->buffers.p = 0;
		h->buffers.len = 0;
	}
}


template <> boost::shared_ptr<DataType> getSiemensHDF5Type<MeasurementHeaderBuffer>()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(MeasurementHeaderBuffer)));

	boost::shared_ptr<DataType> buf_type(new DataType(PredType::NATIVE_CHAR));
	buf_type = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create ( buf_type->getId())));

	ret->insertMember( "bufName_",   HOFFSET(MeasurementHeaderBuffer, bufName_), *buf_type);
	ret->insertMember( "bufLen_",    HOFFSET(MeasurementHeaderBuffer,bufLen_),   PredType::NATIVE_UINT32);
	ret->insertMember( "buf_",       HOFFSET(MeasurementHeaderBuffer, buf_),     *buf_type);

	return ret;
}



template <> boost::shared_ptr<DataType> getSiemensHDF5Type<MeasurementHeader>()
{
	boost::shared_ptr<CompType> ret = boost::shared_ptr<CompType>(new CompType(sizeof(MeasurementHeader)));

	boost::shared_ptr<DataType> vtype = getSiemensHDF5Type<MeasurementHeaderBuffer>();
	vtype = boost::shared_ptr<DataType>(new DataType(H5Tvlen_create ( vtype->getId())));

	ret->insertMember( "dma_length",    HOFFSET(MeasurementHeader,dma_length),   PredType::NATIVE_UINT32);
	ret->insertMember( "nr_buffers",    HOFFSET(MeasurementHeader,nr_buffers),   PredType::NATIVE_UINT32);
	ret->insertMember( "buffers",       HOFFSET(MeasurementHeader, buffers),     *vtype);

	return ret;
}
*/

}

#endif /* ISMRMRD_HDF5_DATATYPES_H_ */

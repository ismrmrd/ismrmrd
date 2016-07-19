#include "ismrmrd/xml.h"
#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/version.h"

#include <fstream>
#include <boost/test/unit_test.hpp>

using namespace ISMRMRD;

const std::string XML_HEADER("\
<?xml version=\"1.0\"?>\n\
<ismrmrdHeader xmlns=\"http://www.ismrm.org/ISMRMRD\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xs=\"http://www.w3.org/2001/XMLSchema\" xsi:schemaLocation=\"http://www.ismrm.org/ISMRMRD ismrmrd.xsd\">\n\
	<EntityHeader>\n\
		<stream>1</stream>\n\
		<signature>367583234</signature>\n\
		<entity_type>6</entity_type>\n\
		<storage_type>0</storage_type>\n\
	</EntityHeader>\n\
	<version>4</version>\n\
	<subjectInformation>\n\
		<patientName>phantom</patientName>\n\
		<patientWeight_kg>72.500000</patientWeight_kg>\n\
	</subjectInformation>\n\
	<studyInformation>\n\
		<studyDate>2001-01-01</studyDate>\n\
		<studyTime>00:01:02</studyTime>\n\
		<studyDescription>Image Synthesis</studyDescription>\n\
	</studyInformation>\n\
	<measurementInformation>\n\
		<measurementID>ABC123</measurementID>\n\
		<seriesDate>2001-01-01</seriesDate>\n\
		<seriesTime>01:02:03</seriesTime>\n\
		<patientPosition>HFS</patientPosition>\n\
		<protocolName>ISMRM Synthetic Imaging Protocol</protocolName>\n\
		<measurementDependency>\n\
			<dependencyType>calibration</dependencyType>\n\
			<measurementID>BCD234</measurementID>\n\
		</measurementDependency>\n\
		<measurementDependency>\n\
			<dependencyType>noise</dependencyType>\n\
			<measurementID>CDE345</measurementID>\n\
		</measurementDependency>\n\
	</measurementInformation>\n\
	<acquisitionSystemInformation>\n\
		<receiverChannels>8</receiverChannels>\n\
		<coilLabel>\n\
			<coilNumber>0</coilNumber>\n\
			<coilName>16Ch_Body_A</coilName>\n\
		</coilLabel>\n\
		<coilLabel>\n\
			<coilNumber>1</coilNumber>\n\
			<coilName>16Ch_Body_B</coilName>\n\
		</coilLabel>\n\
		<institutionName>ISMRM Synthetic Imaging Lab</institutionName>\n\
	</acquisitionSystemInformation>\n\
	<experimentalConditions>\n\
		<H1resonanceFrequency_Hz>63500000</H1resonanceFrequency_Hz>\n\
	</experimentalConditions>\n\
	<encoding>\n\
		<encodedSpace>\n\
			<matrixSize>\n\
				<x>512</x>\n\
				<y>256</y>\n\
				<z>1</z>\n\
			</matrixSize>\n\
			<fieldOfView_mm>\n\
				<x>600.000000</x>\n\
				<y>300.000000</y>\n\
				<z>6.000000</z>\n\
			</fieldOfView_mm>\n\
		</encodedSpace>\n\
		<reconSpace>\n\
			<matrixSize>\n\
				<x>256</x>\n\
				<y>256</y>\n\
				<z>1</z>\n\
			</matrixSize>\n\
			<fieldOfView_mm>\n\
				<x>300.000000</x>\n\
				<y>300.000000</y>\n\
				<z>6.000000</z>\n\
			</fieldOfView_mm>\n\
		</reconSpace>\n\
		<encodingLimits>\n\
			<kspace_encoding_step_1>\n\
				<minimum>0</minimum>\n\
				<maximum>255</maximum>\n\
				<center>128</center>\n\
			</kspace_encoding_step_1>\n\
			<repetition>\n\
				<minimum>0</minimum>\n\
				<maximum>3</maximum>\n\
				<center>0</center>\n\
			</repetition>\n\
		</encodingLimits>\n\
		<trajectory>cartesian</trajectory>\n\
		<parallelImaging>\n\
			<accelerationFactor>\n\
				<kspace_encoding_step_1>2</kspace_encoding_step_1>\n\
				<kspace_encoding_step_2>1</kspace_encoding_step_2>\n\
			</accelerationFactor>\n\
			<calibrationMode>interleaved</calibrationMode>\n\
		</parallelImaging>\n\
	</encoding>\n\
	<streams>\n\
		<stream>\n\
			<label>k-space</label>\n\
			<entityType>MRAcquisition</entityType>\n\
			<number>0</number>\n\
			<storageType>cxfloat</storageType>\n\
		</stream>\n\
		<stream>\n\
			<label>ECG</label>\n\
			<entityType>MRAcquisition</entityType>\n\
			<number>2</number>\n\
			<storageType>float</storageType>\n\
		</stream>\n\
	</streams>\n\
	<sequenceParameters>\n\
		<TR>5.860000</TR>\n\
		<TE>2.960000</TE>\n\
		<TE>4.240000</TE>\n\
	</sequenceParameters>\n\
	<userParameters>\n\
		<userParameterLong>\n\
			<name>age</name>\n\
			<value>42</value>\n\
		</userParameterLong>\n\
		<userParameterDouble>\n\
			<name>center frequency</name>\n\
			<value>63500000.000000</value>\n\
		</userParameterDouble>\n\
		<userParameterString>\n\
			<name>tag</name>\n\
			<value>synthetic</value>\n\
		</userParameterString>\n\
		<userParameterString>\n\
			<name>author</name>\n\
			<value>ISMRMRD Developers</value>\n\
		</userParameterString>\n\
	</userParameters>\n\
</ismrmrdHeader>\n\
");

IsmrmrdHeader build_model_header (void)
{
  unsigned int matrix_size = 256;
  unsigned int ros = 2; // readout oversampling
  unsigned int repetitions = 2;
  unsigned int readout = matrix_size * ros;
  unsigned int ncoils = 8;
  unsigned int acc_factor = 2; // acceleration factor

  IsmrmrdHeader head;

  //head.version = ISMRMRD_XMLHDR_VERSION;
  head.experimentalConditions.H1resonanceFrequency_Hz = 63500000; // ~1.5T

  SubjectInformation subj;
  subj.patientName = "phantom";
  subj.patientWeight_kg = 72.5;
  head.subjectInformation = subj;

  StudyInformation study;
  study.studyDate = "2001-01-01";
  study.studyTime = "00:01:02";
  study.studyDescription = "Image Synthesis";
  head.studyInformation = study;

  AcquisitionSystemInformation sys;
  sys.institutionName = "ISMRM Synthetic Imaging Lab";
  sys.receiverChannels = ncoils;
  std::vector<CoilLabel> coil_labels(2);
  coil_labels[0].coilNumber = 0;
  coil_labels[0].coilName = "16Ch_Body_A";
  coil_labels[1].coilNumber = 1;
  coil_labels[1].coilName = "16Ch_Body_B";
  sys.coilLabel = coil_labels;
  head.acquisitionSystemInformation = sys;

  MeasurementInformation meas;
  meas.measurementID = "ABC123";
  meas.seriesDate = "2001-01-01";
  meas.seriesTime = "01:02:03";
  meas.patientPosition = "HFS";
  meas.protocolName = "ISMRM Synthetic Imaging Protocol";
  std::vector<MeasurementDependency> deps(2);
  deps[0].dependencyType = "calibration";
  deps[0].measurementID = "BCD234";
  deps[1].dependencyType = "noise";
  deps[1].measurementID = "CDE345";
  meas.measurementDependency = deps;
  head.measurementInformation = meas;

  Encoding e;
  e.encodedSpace.matrixSize.x = readout;
  e.encodedSpace.matrixSize.y = matrix_size;
  e.encodedSpace.matrixSize.z = 1;
  e.encodedSpace.fieldOfView_mm.x = 600;
  e.encodedSpace.fieldOfView_mm.y = 300;
  e.encodedSpace.fieldOfView_mm.z = 6;
  e.reconSpace.matrixSize.x = readout / 2;
  e.reconSpace.matrixSize.y = matrix_size;
  e.reconSpace.matrixSize.z = 1;
  e.reconSpace.fieldOfView_mm.x = 300;
  e.reconSpace.fieldOfView_mm.y = 300;
  e.reconSpace.fieldOfView_mm.z = 6;
  e.trajectory = "cartesian";
  e.encodingLimits.kspace_encoding_step_1 = Limit(0, matrix_size - 1, matrix_size / 2);
  e.encodingLimits.repetition = Limit(0, repetitions * acc_factor - 1, 0);

  ParallelImaging parallel;
  parallel.accelerationFactor.kspace_encoding_step_1 = acc_factor;
  parallel.accelerationFactor.kspace_encoding_step_2 = 1;
  parallel.calibrationMode = "interleaved";
  e.parallelImaging = parallel;

  head.encoding.push_back(e);

  Stream stream1;
  stream1.label = "k-space";
  stream1.storageType = ISMRMRD_CXFLOAT;
  stream1.entityType = ISMRMRD_MRACQUISITION;
  stream1.number = 0;
  head.streams.push_back(stream1);

  Stream stream2;
  stream2.label = "ECG";
  stream2.storageType = ISMRMRD_FLOAT;
  stream2.entityType = ISMRMRD_MRACQUISITION;
  stream2.number = 2;
  head.streams.push_back(stream2);

  SequenceParameters seq;
  std::vector<float> TE, TR;
  TR.push_back(5.86);
  TE.push_back(2.96);
  TE.push_back(4.24);
  seq.TR = TR;
  seq.TE = TE;
  head.sequenceParameters = seq;

  UserParameters params;
  UserParameterLong uplong;
  uplong.name = "age";
  uplong.value = 42;

  UserParameterDouble updouble;
  updouble.name = "center frequency";
  updouble.value = 63500000.0;

  std::vector<UserParameterString> upstrings(2);
  upstrings[0].name = "tag";
  upstrings[0].value = "synthetic";
  upstrings[1].name = "author";
  upstrings[1].value = "ISMRMRD Developers";

  params.userParameterLong.push_back(uplong);
  params.userParameterDouble.push_back(updouble);
  params.userParameterString = upstrings;
  head.userParameters = params;

  return head;
}

BOOST_AUTO_TEST_SUITE (XMLTest)

BOOST_AUTO_TEST_CASE (test_serialize_header)
{
  IsmrmrdHeader head = build_model_header();

  //std::stringstream str;
  //ISMRMRD::serialize(head, str);
  //std::string xml_header = str.str();

  std::vector<unsigned char> buffer =  head.serialize ();
  std::string xml_header (buffer.begin(), buffer.end());

  BOOST_CHECK_EQUAL(xml_header, XML_HEADER);
}

BOOST_AUTO_TEST_CASE(test_deserialize_header)
{
  //IsmrmrdHeader head;
  //ISMRMRD::deserialize(XML_HEADER.c_str(), head);

  IsmrmrdHeader head;
  std::vector<unsigned char> buffer (XML_HEADER.begin(), XML_HEADER.end());
  head.deserialize (buffer);
  IsmrmrdHeader model = build_model_header();

  BOOST_CHECK_EQUAL (head.ent_head.stream, model.ent_head.stream);
  BOOST_CHECK_EQUAL (head.ent_head.signature, model.ent_head.signature);
  BOOST_CHECK_EQUAL (head.ent_head.entity_type, model.ent_head.entity_type);
  BOOST_CHECK_EQUAL (head.ent_head.storage_type, model.ent_head.storage_type);

  BOOST_CHECK_EQUAL (head.getStream(), ISMRMRD_HEADER_STREAM);
  BOOST_CHECK_EQUAL (model.getStream(), ISMRMRD_HEADER_STREAM);
  BOOST_CHECK_EQUAL (head.getSignature(), ISMRMRD_SIGNATURE);
  BOOST_CHECK_EQUAL (model.getSignature(), ISMRMRD_SIGNATURE);
  BOOST_CHECK_EQUAL (head.getVersion(), ISMRMRD_VERSION_MAJOR);
  BOOST_CHECK_EQUAL (model.getVersion(), ISMRMRD_VERSION_MAJOR);
  BOOST_CHECK_EQUAL (head.getEntityType(), ISMRMRD_XML_HEADER);
  BOOST_CHECK_EQUAL (model.getEntityType(), ISMRMRD_XML_HEADER);
  BOOST_CHECK_EQUAL (head.getStorageType(), ISMRMRD_STORAGE_NONE);
  BOOST_CHECK_EQUAL (model.getStorageType(), ISMRMRD_STORAGE_NONE);

  BOOST_CHECK_EQUAL(head.version, model.version);

  BOOST_CHECK_EQUAL(head.experimentalConditions.H1resonanceFrequency_Hz, model.experimentalConditions.H1resonanceFrequency_Hz);

  BOOST_CHECK_EQUAL(head.subjectInformation.get().patientName, model.subjectInformation.get().patientName);
  BOOST_CHECK_EQUAL(head.subjectInformation.get().patientWeight_kg, model.subjectInformation.get().patientWeight_kg);

  BOOST_CHECK_EQUAL(head.studyInformation.get().studyDate, model.studyInformation.get().studyDate);
  BOOST_CHECK_EQUAL(head.studyInformation.get().studyTime, model.studyInformation.get().studyTime);
  BOOST_CHECK_EQUAL(head.studyInformation.get().studyDescription, model.studyInformation.get().studyDescription);

  BOOST_CHECK_EQUAL(head.acquisitionSystemInformation.get().institutionName, model.acquisitionSystemInformation.get().institutionName);
  BOOST_CHECK_EQUAL(head.acquisitionSystemInformation.get().receiverChannels, model.acquisitionSystemInformation.get().receiverChannels);
  BOOST_CHECK_EQUAL(head.acquisitionSystemInformation.get().coilLabel[0].coilNumber, model.acquisitionSystemInformation.get().coilLabel[0].coilNumber);
  BOOST_CHECK_EQUAL(head.acquisitionSystemInformation.get().coilLabel[0].coilName, model.acquisitionSystemInformation.get().coilLabel[0].coilName);
  BOOST_CHECK_EQUAL(head.acquisitionSystemInformation.get().coilLabel[1].coilNumber, model.acquisitionSystemInformation.get().coilLabel[1].coilNumber);
  BOOST_CHECK_EQUAL(head.acquisitionSystemInformation.get().coilLabel[1].coilName, model.acquisitionSystemInformation.get().coilLabel[1].coilName);

  BOOST_CHECK_EQUAL(head.measurementInformation.get().measurementID, model.measurementInformation.get().measurementID);
  BOOST_CHECK_EQUAL(head.measurementInformation.get().seriesDate, model.measurementInformation.get().seriesDate);
  BOOST_CHECK_EQUAL(head.measurementInformation.get().seriesTime, model.measurementInformation.get().seriesTime);
  BOOST_CHECK_EQUAL(head.measurementInformation.get().patientPosition, model.measurementInformation.get().patientPosition);
  BOOST_CHECK_EQUAL(head.measurementInformation.get().protocolName, model.measurementInformation.get().protocolName);
  BOOST_CHECK_EQUAL(head.measurementInformation.get().measurementDependency[0].dependencyType, model.measurementInformation.get().measurementDependency[0].dependencyType);
  BOOST_CHECK_EQUAL(head.measurementInformation.get().measurementDependency[0].measurementID, model.measurementInformation.get().measurementDependency[0].measurementID);
  BOOST_CHECK_EQUAL(head.measurementInformation.get().measurementDependency[1].dependencyType, model.measurementInformation.get().measurementDependency[1].dependencyType);
  BOOST_CHECK_EQUAL(head.measurementInformation.get().measurementDependency[1].measurementID, model.measurementInformation.get().measurementDependency[1].measurementID);

  BOOST_CHECK_EQUAL(head.encoding[0].encodedSpace.matrixSize.x, model.encoding[0].encodedSpace.matrixSize.x);
  BOOST_CHECK_EQUAL(head.encoding[0].encodedSpace.matrixSize.y, model.encoding[0].encodedSpace.matrixSize.y);
  BOOST_CHECK_EQUAL(head.encoding[0].encodedSpace.matrixSize.z, model.encoding[0].encodedSpace.matrixSize.z);
  BOOST_CHECK_EQUAL(head.encoding[0].encodedSpace.fieldOfView_mm.x, model.encoding[0].encodedSpace.fieldOfView_mm.x);
  BOOST_CHECK_EQUAL(head.encoding[0].encodedSpace.fieldOfView_mm.y, model.encoding[0].encodedSpace.fieldOfView_mm.y);
  BOOST_CHECK_EQUAL(head.encoding[0].encodedSpace.fieldOfView_mm.z, model.encoding[0].encodedSpace.fieldOfView_mm.z);
  BOOST_CHECK_EQUAL(head.encoding[0].reconSpace.matrixSize.x, model.encoding[0].reconSpace.matrixSize.x);
  BOOST_CHECK_EQUAL(head.encoding[0].reconSpace.matrixSize.y, model.encoding[0].reconSpace.matrixSize.y);
  BOOST_CHECK_EQUAL(head.encoding[0].reconSpace.matrixSize.z, model.encoding[0].reconSpace.matrixSize.z);
  BOOST_CHECK_EQUAL(head.encoding[0].reconSpace.fieldOfView_mm.x, model.encoding[0].reconSpace.fieldOfView_mm.x);
  BOOST_CHECK_EQUAL(head.encoding[0].reconSpace.fieldOfView_mm.y, model.encoding[0].reconSpace.fieldOfView_mm.y);
  BOOST_CHECK_EQUAL(head.encoding[0].reconSpace.fieldOfView_mm.z, model.encoding[0].reconSpace.fieldOfView_mm.z);
  BOOST_CHECK_EQUAL(head.encoding[0].trajectory, model.encoding[0].trajectory);
  BOOST_CHECK_EQUAL(head.encoding[0].encodingLimits.kspace_encoding_step_1.get().minimum, model.encoding[0].encodingLimits.kspace_encoding_step_1.get().minimum);
  BOOST_CHECK_EQUAL(head.encoding[0].encodingLimits.kspace_encoding_step_1.get().maximum, model.encoding[0].encodingLimits.kspace_encoding_step_1.get().maximum);
  BOOST_CHECK_EQUAL(head.encoding[0].encodingLimits.kspace_encoding_step_1.get().center, model.encoding[0].encodingLimits.kspace_encoding_step_1.get().center);
  BOOST_CHECK_EQUAL(head.encoding[0].encodingLimits.repetition.get().minimum, model.encoding[0].encodingLimits.repetition.get().minimum);
  BOOST_CHECK_EQUAL(head.encoding[0].encodingLimits.repetition.get().maximum, model.encoding[0].encodingLimits.repetition.get().maximum);
  BOOST_CHECK_EQUAL(head.encoding[0].encodingLimits.repetition.get().center, model.encoding[0].encodingLimits.repetition.get().center);
  BOOST_CHECK_EQUAL(head.encoding[0].parallelImaging.get().accelerationFactor.kspace_encoding_step_1, model.encoding[0].parallelImaging.get().accelerationFactor.kspace_encoding_step_1);
  BOOST_CHECK_EQUAL(head.encoding[0].parallelImaging.get().accelerationFactor.kspace_encoding_step_2, model.encoding[0].parallelImaging.get().accelerationFactor.kspace_encoding_step_2);
  BOOST_CHECK_EQUAL(head.encoding[0].parallelImaging.get().calibrationMode, model.encoding[0].parallelImaging.get().calibrationMode);

  BOOST_CHECK_EQUAL(head.streams[0].label, model.streams[0].label);
  BOOST_CHECK_EQUAL(head.streams[0].storageType, model.streams[0].storageType);
  BOOST_CHECK_EQUAL(head.streams[0].entityType, model.streams[0].entityType);
  BOOST_CHECK_EQUAL(head.streams[0].number, model.streams[0].number);
  BOOST_CHECK_EQUAL(head.streams[1].label, model.streams[1].label);
  BOOST_CHECK_EQUAL(head.streams[1].storageType, model.streams[1].storageType);
  BOOST_CHECK_EQUAL(head.streams[1].entityType, model.streams[1].entityType);
  BOOST_CHECK_EQUAL(head.streams[1].number, model.streams[1].number);

  BOOST_CHECK_EQUAL(head.sequenceParameters.get().TR.get()[0], model.sequenceParameters.get().TR.get()[0]);
  BOOST_CHECK_EQUAL(head.sequenceParameters.get().TE.get()[0], model.sequenceParameters.get().TE.get()[0]);
  BOOST_CHECK_EQUAL(head.sequenceParameters.get().TE.get()[1], model.sequenceParameters.get().TE.get()[1]);

  BOOST_CHECK_EQUAL(head.userParameters.get().userParameterLong[0].name, model.userParameters.get().userParameterLong[0].name);
  BOOST_CHECK_EQUAL(head.userParameters.get().userParameterLong[0].value, model.userParameters.get().userParameterLong[0].value);
  BOOST_CHECK_EQUAL(head.userParameters.get().userParameterDouble[0].name, model.userParameters.get().userParameterDouble[0].name);
  BOOST_CHECK_EQUAL(head.userParameters.get().userParameterDouble[0].value, model.userParameters.get().userParameterDouble[0].value);

  BOOST_CHECK_EQUAL(head.userParameters.get().userParameterString[0].name, model.userParameters.get().userParameterString[0].name);
  BOOST_CHECK_EQUAL(head.userParameters.get().userParameterString[0].value, model.userParameters.get().userParameterString[0].value);
  BOOST_CHECK_EQUAL(head.userParameters.get().userParameterString[1].name, model.userParameters.get().userParameterString[1].name);
  BOOST_CHECK_EQUAL(head.userParameters.get().userParameterString[1].value, model.userParameters.get().userParameterString[1].value);
}

BOOST_AUTO_TEST_SUITE_END()

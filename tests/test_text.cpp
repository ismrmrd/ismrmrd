#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/version.h"
#include <boost/test/unit_test.hpp>

using namespace ISMRMRD;

const std::string xml_header("\
<?xml version=\"1.0\"?>\n\
<ismrmrdHeader xmlns=\"http://www.ismrm.org/ISMRMRD\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xs=\"http://www.w3.org/2001/XMLSchema\" xsi:schemaLocation=\"http://www.ismrm.org/ISMRMRD ismrmrd.xsd\">\n\
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

BOOST_AUTO_TEST_SUITE (TextEntity)

BOOST_AUTO_TEST_CASE (text_create)
{
  IsmrmrdText txt;

  BOOST_CHECK_EQUAL (txt.getEntityType(), ISMRMRD_TEXT);
  BOOST_CHECK_EQUAL (txt.getStorageType(), ISMRMRD_CHAR);
  BOOST_CHECK_EQUAL (txt.getTextType(), ISMRMRD_TEXT_ERROR);
  BOOST_CHECK_EQUAL (txt.getSize(), 0);

  txt.setText (xml_header, ISMRMRD_XML_HEADER_TEXT);
  BOOST_CHECK_EQUAL (txt.getSize(), xml_header.size());

  IsmrmrdText txt1 (xml_header, ISMRMRD_XML_HEADER_TEXT);

  BOOST_CHECK_EQUAL (txt1.getEntityType(), ISMRMRD_TEXT);
  BOOST_CHECK_EQUAL (txt1.getStorageType(), ISMRMRD_CHAR);
  BOOST_CHECK_EQUAL (txt1.getTextType(), ISMRMRD_XML_HEADER_TEXT);
  BOOST_CHECK_EQUAL (txt1.getSize(), xml_header.size());
  BOOST_CHECK_EQUAL (txt1.getTextString(), xml_header);

  std::vector<unsigned char> head_vec (xml_header.begin(), xml_header.end());

  IsmrmrdText txt2 (head_vec, ISMRMRD_XML_HEADER_TEXT);

  BOOST_CHECK_EQUAL (txt2.getEntityType(), ISMRMRD_TEXT);
  BOOST_CHECK_EQUAL (txt2.getStorageType(), ISMRMRD_CHAR);
  BOOST_CHECK_EQUAL (txt2.getTextType(), ISMRMRD_XML_HEADER_TEXT);
  BOOST_CHECK_EQUAL (txt2.getSize(), xml_header.size());
  BOOST_CHECK_EQUAL (txt2.getSize(), head_vec.size());
  BOOST_CHECK_EQUAL (txt2.getTextString(), xml_header);
  BOOST_CHECK (txt2.getTextVector() == head_vec);
}

BOOST_AUTO_TEST_CASE (text_check_types)
{
  std::vector<unsigned char> head_vec (xml_header.begin(), xml_header.end());

  IsmrmrdText txt1 (xml_header, ISMRMRD_XML_HEADER_TEXT);
  IsmrmrdText txt2 (head_vec, ISMRMRD_XML_HEADER_TEXT);

  BOOST_CHECK (txt1.getTextString() == txt2.getTextString());
  BOOST_CHECK (txt1.getTextVector() == txt2.getTextVector());
}

BOOST_AUTO_TEST_CASE (text_serialize)
{
  IsmrmrdText txt1 (xml_header, ISMRMRD_XML_HEADER_TEXT);
  std::vector<unsigned char> temp (txt1.serialize());

  IsmrmrdText txt2;
  txt2.deserialize (temp);
  BOOST_CHECK_EQUAL (txt2.getEntityType(), ISMRMRD_TEXT);
  BOOST_CHECK_EQUAL (txt2.getStorageType(), ISMRMRD_CHAR);
  BOOST_CHECK_EQUAL (txt2.getTextType(), ISMRMRD_XML_HEADER_TEXT);
  BOOST_CHECK_EQUAL (txt2.getSize(), xml_header.size());
  BOOST_CHECK_EQUAL (txt2.getTextString(), xml_header);
}

BOOST_AUTO_TEST_CASE (waveform_getters_setters)
{
  IsmrmrdText txt1;

  txt1.setText (xml_header, ISMRMRD_XML_HEADER_TEXT);
  BOOST_CHECK_EQUAL (txt1.getTextType(), ISMRMRD_XML_HEADER_TEXT);
  BOOST_CHECK_EQUAL (txt1.getSize(), xml_header.size());
  BOOST_CHECK_EQUAL (txt1.getTextString(), xml_header);

  std::vector<unsigned char> head_vec (xml_header.begin(), xml_header.end());
  IsmrmrdText txt2;
  txt2.setText (head_vec, ISMRMRD_XML_HEADER_TEXT);
  BOOST_CHECK_EQUAL (txt2.getTextType(), ISMRMRD_XML_HEADER_TEXT);
  BOOST_CHECK_EQUAL (txt2.getSize(), head_vec.size());
  BOOST_CHECK (txt2.getTextVector() == head_vec);
}

BOOST_AUTO_TEST_SUITE_END()

#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/dataset.h"
#include <fstream>
#include <boost/test/unit_test.hpp>

using namespace ISMRMRD;

const std::string g_filename("test-data.h5");
const std::string g_groupname("test-dataset");
const std::string g_xml_header("\
<?xml version=\"1.0\"?>\n\
<ismrmrdHeader xmlns=\"http://www.ismrm.org/ISMRMRD\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xs=\"http://www.w3.org/2001/XMLSchema\" xsi:schemaLocation=\"http://www.ismrm.org/ISMRMRD ismrmrd.xsd\">\n\
	<version>3</version>\n\
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
			<name>k-space</name>\n\
			<dataType>MRAcquisition</dataType>\n\
			<number>0</number>\n\
			<storageType>cxfloat</storageType>\n\
		</stream>\n\
		<stream>\n\
			<name>ECG</name>\n\
			<dataType>MRAcquisition</dataType>\n\
			<number>2</number>\n\
			<storageType>float</storageType>\n\
		</stream>\n\
	</streams>\n\
</ismrmrdHeader>\n\
");

void cleanup(void)
{
    std::ifstream ifile(g_filename);
    if (ifile) {
        ifile.close();
        remove(g_filename.c_str());
    }
}

BOOST_AUTO_TEST_SUITE(Datasets)

BOOST_AUTO_TEST_CASE(dataset_nonexistent)
{
    cleanup();
    BOOST_CHECK_THROW(Dataset dataset(g_filename, g_groupname, false), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(dataset_create)
{
    cleanup();

    {
        BOOST_CHECK_NO_THROW(Dataset dataset(g_filename, g_groupname));
    }
}

BOOST_AUTO_TEST_CASE(dataset_read_write_header)
{
    cleanup();

    Dataset dataset(g_filename, g_groupname);
    BOOST_CHECK_NO_THROW(dataset.writeHeader(g_xml_header));
    std::string header = dataset.readHeader();
    BOOST_CHECK_EQUAL(g_xml_header, header);
}

BOOST_AUTO_TEST_CASE(dataset_append_acquisitions)
{
    cleanup();

    Dataset dataset(g_filename, g_groupname);
    BOOST_CHECK_NO_THROW(dataset.writeHeader(g_xml_header));

    Acquisition acq;
    acq.resize(256, 8);
    acq.setFlag(ISMRMRD_ACQ_IS_NOISE_MEASUREMENT);

    // add fake data
    for (size_t i = 0; i < acq.getNumberOfDataElements(); i++) {
        acq.getData()[i] = i / acq.getNumberOfDataElements();
    }

    BOOST_CHECK_NO_THROW(dataset.appendAcquisition(acq, 0));

    BOOST_CHECK_EQUAL(dataset.getNumberOfAcquisitions(0), 1);
}

BOOST_AUTO_TEST_SUITE_END()

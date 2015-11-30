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
    std::ifstream ifile(g_filename.c_str());
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

    Acquisition<float> acq;

    unsigned int matrix_size = 256;
    unsigned int ncoils = 8;
    unsigned int readout_oversampling = 2;
    unsigned int repetitions = 2;
    unsigned int acc_factor = 2;
    unsigned int readout = matrix_size * readout_oversampling;

    acq.resize(readout, ncoils);
    acq.setFlag(ISMRMRD_ACQ_IS_NOISE_MEASUREMENT);
    acq.setDwellTime_ns(5000);
    // add fake noise data
    for (size_t i = 0; i < acq.getNumberOfDataElements(); i++) {
        acq.getData()[i] = i / acq.getNumberOfDataElements();
    }
    unsigned int noise_stream = repetitions;
    BOOST_CHECK_NO_THROW(dataset.appendAcquisition<float>(acq, noise_stream));
    BOOST_CHECK_EQUAL(dataset.getNumberOfAcquisitions(noise_stream), 1);

    acq.setAvailableChannels(ncoils);
    acq.setCenterSample(readout / 2);

    unsigned int total_nacq = 1;

    for (unsigned int r = 0; r < repetitions; ++r) {
        unsigned int nacq = 0;
        for (unsigned int a = 0; a < acc_factor; ++a) {
            for (unsigned int i = a; i < matrix_size; i += acc_factor) {
                acq.clearAllFlags();
                if (i == a) {
                    acq.setFlag(ISMRMRD_ACQ_FIRST_IN_SLICE);
                } else if (i >= (matrix_size - acc_factor)) {
                    acq.setFlag(ISMRMRD_ACQ_LAST_IN_SLICE);
                }

                acq.getEncodingCounters().kspace_encode_step_1 = i;
                acq.getEncodingCounters().repetition = r * acc_factor + a;
                acq.setDwellTime_ns(5000);

                for (unsigned int c = 0; c < ncoils; ++c) {
                    for (unsigned int s = 0; s < readout; ++s) {
                        acq.at(s, c) = c * s;
                    }
                }
                BOOST_CHECK_NO_THROW(dataset.appendAcquisition<float>(acq, r));
                nacq++;
                BOOST_CHECK_EQUAL(dataset.getNumberOfAcquisitions(r), nacq);
            }
        }
        total_nacq += nacq;
    }
    BOOST_CHECK_EQUAL(dataset.getNumberOfAcquisitions(), total_nacq);
}

BOOST_AUTO_TEST_CASE(dataset_read_acquisitions)
{
    cleanup();

    Dataset dataset(g_filename, g_groupname);
    BOOST_CHECK_NO_THROW(dataset.writeHeader(g_xml_header));

    unsigned int matrix_size = 256;
    unsigned int ncoils = 8;
    unsigned int nechoes = 5;
    unsigned int readout_oversampling = 2;
    unsigned int readout = matrix_size * readout_oversampling;

    Acquisition<float> acq_in;
    acq_in.resize(readout, ncoils);
    acq_in.setFlag(ISMRMRD_ACQ_IS_NOISE_MEASUREMENT);
    acq_in.setDwellTime_ns(5000);
    // add fake noise data
    for (size_t i = 0; i < acq_in.getNumberOfDataElements(); i++) {
        acq_in.getData()[i] = i / acq_in.getNumberOfDataElements();
    }
    unsigned int noise_stream = nechoes;
    BOOST_CHECK_NO_THROW(dataset.appendAcquisition<float>(acq_in, noise_stream));
    Acquisition<float> acq_out = dataset.readAcquisition<float>(0, noise_stream);
    BOOST_CHECK(acq_in.getHead() == acq_out.getHead());
    BOOST_CHECK_EQUAL_COLLECTIONS(acq_in.getData().begin(), acq_in.getData().end(),
            acq_out.getData().begin(), acq_out.getData().end());
    // TODO: test trajectory

    for (unsigned int l = 0; l < matrix_size; ++l) {
        for (unsigned int e = 0; e < nechoes; ++e) {
            acq_in.clearAllFlags();
            if (l == 0) {
                acq_in.setFlag(ISMRMRD_ACQ_FIRST_IN_SLICE);
            } else if (l == matrix_size) {
                acq_in.setFlag(ISMRMRD_ACQ_LAST_IN_SLICE);
            }

            acq_in.getEncodingCounters().kspace_encode_step_1 = l;
            acq_in.getEncodingCounters().contrast = e;
            acq_in.setDwellTime_ns(5000);

            // stuff very fake data into the acquisition
            std::vector<std::complex<float> > data(ncoils*readout, std::complex<float>(l, e));
            acq_in.setData(data);
            BOOST_CHECK_NO_THROW(dataset.appendAcquisition<float>(acq_in, e));
        }
    }

    for (unsigned int l = 0; l < matrix_size; ++l) {
        for (unsigned int e = 0; e < nechoes; ++e) {
            // check both methods of retrieving acquisitions
            // 1. nth acquisition in a given stream
            // 2. nth acquisition in the entire dataset
            Acquisition<float> acq = dataset.readAcquisition<float>(l, e);
            Acquisition<float> acq2 = dataset.readAcquisition<float>(1 + e + l * nechoes);

            BOOST_CHECK(acq.getHead() == acq2.getHead());

            if (l == 0) {
                BOOST_CHECK(acq.isFlagSet(ISMRMRD_ACQ_FIRST_IN_SLICE));
            } else if (l == matrix_size) {
                BOOST_CHECK(acq.isFlagSet(ISMRMRD_ACQ_LAST_IN_SLICE));
            }

            BOOST_CHECK_EQUAL(acq.getEncodingCounters().kspace_encode_step_1, l);
            BOOST_CHECK_EQUAL(acq.getEncodingCounters().contrast, e);
            BOOST_CHECK_EQUAL(acq.getDwellTime_ns(), 5000);

            std::vector<std::complex<float> > data(ncoils*readout, std::complex<float>(l, e));

            BOOST_CHECK_EQUAL_COLLECTIONS(data.begin(), data.end(),
                    acq.getData().begin(), acq.getData().end());
            BOOST_CHECK_EQUAL_COLLECTIONS(data.begin(), data.end(),
                    acq2.getData().begin(), acq2.getData().end());
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()

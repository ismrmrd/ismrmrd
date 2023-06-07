#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/serialize.h"
#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/xml.hpp>
#include <sstream>

using namespace cereal;

namespace cereal {
// Discard BinaryData for XMLOutput in error messages
template <typename T>
void serialize(XMLOutputArchive &, BinaryData<T> &) {}
} // namespace cereal

namespace ISMRMRD {
template <typename T>
std::ostream &operator<<(std::ostream &out, T const &value) {
    cereal::XMLOutputArchive archive(out);
    archive(value);
    return out;
}
} // namespace ISMRMRD

template <typename T>
void check(T &value1, T &value2, bool postCheck=true) {
    BOOST_CHECK_NE(value1, value2);

    std::stringstream ss;

    ISMRMRD::CompressionParameters parameters;
    parameters.tolerance = 0.0;
    parameters.precision = 0;

    {
        ISMRMRD::CompressiblePortableBinaryOutputArchive oarchive(parameters, ss);
        oarchive(value1);
    }
    {
        ISMRMRD::CompressiblePortableBinaryInputArchive iarchive(ss);
        iarchive(value2);
    }

    if (postCheck)
        BOOST_CHECK_EQUAL(value1, value2);
}

BOOST_AUTO_TEST_SUITE(SerializeTest)

BOOST_AUTO_TEST_CASE(test_ndarray_serialize) {
    ISMRMRD::ISMRMRD_NDArray value1;
    ISMRMRD::ISMRMRD_NDArray value2;
    BOOST_CHECK_EQUAL(ismrmrd_init_ndarray(&value1), ISMRMRD::ISMRMRD_NOERROR);
    BOOST_CHECK_EQUAL(ismrmrd_init_ndarray(&value2), ISMRMRD::ISMRMRD_NOERROR);

    value1.ndim = ISMRMRD::ISMRMRD_NDARRAY_MAXDIM;
    for (int idx = 0; idx < ISMRMRD::ISMRMRD_NDARRAY_MAXDIM; idx++) {
        value1.dims[idx] = idx + 1;
    }
    value1.data_type = ISMRMRD::ISMRMRD_SHORT;

    BOOST_CHECK_EQUAL(ismrmrd_make_consistent_ndarray(&value1), ISMRMRD::ISMRMRD_NOERROR);

    check(value1, value2);
}

BOOST_AUTO_TEST_CASE(test_ISMRMRD_EncodingCounters_serialize) {
    ISMRMRD::ISMRMRD_EncodingCounters value1;
    ISMRMRD::ISMRMRD_EncodingCounters value2;

    value1.kspace_encode_step_1 = 1;
    value1.kspace_encode_step_2 = 2;
    value1.average = 3;
    value1.slice = 4;
    value1.contrast = 5;
    value1.phase = 6;
    value1.repetition = 7;
    value1.set = 8;
    value1.segment = 9;

    for (size_t i = 0; i < ISMRMRD::ISMRMRD_USER_INTS; i++)
        value1.user[i] = i + 10;

    check(value1, value2);
}

BOOST_AUTO_TEST_CASE(test_ISMRMRD_AcquisitionHeader_serialize) {
    ISMRMRD::ISMRMRD_AcquisitionHeader value1;
    ISMRMRD::ISMRMRD_AcquisitionHeader value2;

    BOOST_CHECK_EQUAL(ismrmrd_init_acquisition_header(&value1), ISMRMRD::ISMRMRD_NOERROR);
    BOOST_CHECK_EQUAL(ismrmrd_init_acquisition_header(&value2), ISMRMRD::ISMRMRD_NOERROR);

    value1.measurement_uid = 5;

    check(value1, value2);
}

BOOST_AUTO_TEST_CASE(test_ISMRMRD_ImageHeader_serialize) {
    ISMRMRD::ISMRMRD_ImageHeader value1;
    ISMRMRD::ISMRMRD_ImageHeader value2;

    BOOST_CHECK_EQUAL(ismrmrd_init_image_header(&value1), ISMRMRD::ISMRMRD_NOERROR);
    BOOST_CHECK_EQUAL(ismrmrd_init_image_header(&value2), ISMRMRD::ISMRMRD_NOERROR);

    value1.measurement_uid = 5;

    check(value1, value2);
}

BOOST_AUTO_TEST_CASE(test_ISMSMRD_WaveformHeader_serialize) {
    ISMRMRD::ISMRMRD_WaveformHeader value1;
    ISMRMRD::ISMRMRD_WaveformHeader value2;
    BOOST_CHECK_EQUAL(ismrmrd_init_waveformheader(&value1), ISMRMRD::ISMRMRD_NOERROR);
    BOOST_CHECK_EQUAL(ismrmrd_init_waveformheader(&value2), ISMRMRD::ISMRMRD_NOERROR);

    value1.measurement_uid = 5;

    check(value1, value2);
}

BOOST_AUTO_TEST_CASE(test_ISMRMRD_Acquisition_serialize) {
    ISMRMRD::ISMRMRD_Acquisition value1;
    ISMRMRD::ISMRMRD_Acquisition value2;

    BOOST_CHECK_EQUAL(ismrmrd_init_acquisition(&value1), ISMRMRD::ISMRMRD_NOERROR);
    BOOST_CHECK_EQUAL(ismrmrd_init_acquisition(&value2), ISMRMRD::ISMRMRD_NOERROR);

    value1.head.measurement_uid = 5;
    check(value1, value2);
}

BOOST_AUTO_TEST_CASE(test_ISMRMRD_Image_serialize) {
    ISMRMRD::ISMRMRD_Image value1;
    ISMRMRD::ISMRMRD_Image value2;

    BOOST_CHECK_EQUAL(ismrmrd_init_image(&value1), ISMRMRD::ISMRMRD_NOERROR);
    BOOST_CHECK_EQUAL(ismrmrd_init_image(&value2), ISMRMRD::ISMRMRD_NOERROR);

    value1.head.matrix_size[0] = 4;
    value1.head.matrix_size[1] = 4;
    value1.head.matrix_size[2] = 4;
    value1.head.channels = 4;
    value1.head.data_type = ISMRMRD::ISMRMRD_DataTypes::ISMRMRD_FLOAT;

    BOOST_CHECK_EQUAL(ismrmrd_make_consistent_image(&value1), ISMRMRD::ISMRMRD_NOERROR);

    auto data = reinterpret_cast<float *>(value1.data);
    size_t datasize = ISMRMRD::ismrmrd_size_of_image_data(&value1) / ISMRMRD::ismrmrd_sizeof_data_type(value1.head.data_type);
    for (size_t i = 0; i < datasize; i++)
        data[i] = 1;

    value1.head.measurement_uid = 5;

    check(value1, value2);

    ismrmrd_set_flag(&(value1.head.flags), ISMRMRD::ISMRMRD_ACQ_COMPRESSION1);

    check(value1, value2, false);

    ismrmrd_clear_flag (&(value1.head.flags), ISMRMRD::ISMRMRD_ACQ_COMPRESSION1);

    BOOST_CHECK_EQUAL(value1, value2);
    ismrmrd_cleanup_image(&value1);
    ismrmrd_cleanup_image(&value2);
}

BOOST_AUTO_TEST_CASE(test_ISMRMRD_Waveform_serialize) {
    ISMRMRD::ISMRMRD_Waveform value1;
    ISMRMRD::ISMRMRD_Waveform value2;
    BOOST_CHECK_EQUAL(ISMRMRD::ismrmrd_init_waveform(&value1), ISMRMRD::ISMRMRD_NOERROR);
    BOOST_CHECK_EQUAL(ISMRMRD::ismrmrd_init_waveform(&value2), ISMRMRD::ISMRMRD_NOERROR);

    value1.head.measurement_uid = 5;

    check(value1, value2);
}

BOOST_AUTO_TEST_CASE(test_AcquisitionHeader_serialize) {
    ISMRMRD::AcquisitionHeader value1;
    ISMRMRD::AcquisitionHeader value2;

    value1.measurement_uid = 5;

    check(value1, value2);
}

BOOST_AUTO_TEST_CASE(test_ImageHeader_serialize) {
    ISMRMRD::ImageHeader value1;
    ISMRMRD::ImageHeader value2;

    value1.measurement_uid = 5;

    check(value1, value2);
}

BOOST_AUTO_TEST_CASE(test_WaveformHeader_serialize) {
    ISMRMRD::WaveformHeader value1;
    ISMRMRD::WaveformHeader value2;

    value1.measurement_uid = 5;

    check(value1, value2);
}

BOOST_AUTO_TEST_CASE(test_Acquisition_serialize) {
    ISMRMRD::Acquisition value1;
    ISMRMRD::Acquisition value2;

    value1.measurement_uid() = 5;

    check(value1, value2);
}

typedef boost::mpl::list<uint16_t, int16_t, uint32_t, int32_t, float, double, complex_float_t, complex_double_t> test_types;

BOOST_AUTO_TEST_CASE_TEMPLATE(test_Image_serialize, T, test_types) {
    ISMRMRD::Image<T> value1(4,4,4,4);
    ISMRMRD::Image<T> value2;

    value1.setMeasurementUid(5);

    check(value1, value2);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_Image_serialize_compression, T, test_types) {
    ISMRMRD::Image<T> value1(4,4,4,4);
    ISMRMRD::Image<T> value2;

    value1.setMeasurementUid(5);

    auto data = value1.getDataPtr();
    size_t datasize = value1.getNumberOfDataElements();
    for (size_t i = 0; i < datasize; i++)
        data[i] = 1;

    value1.setFlag(ISMRMRD::ISMRMRD_ACQ_COMPRESSION1);

    check(value1, value2, false);

    value1.clearFlag(ISMRMRD::ISMRMRD_ACQ_COMPRESSION1);

    BOOST_REQUIRE_EQUAL(value1.getHead(), value2.getHead());

    for (size_t i = 0; i < datasize; i++)
    {
        T a = value1.getDataPtr()[i];
        T b = value2.getDataPtr()[i];
        std::stringstream ss;
        ss << "Failed on index " << i;
        BOOST_REQUIRE_MESSAGE(a == b,ss.str());
    }

    BOOST_CHECK_EQUAL(value1, value2);
}

BOOST_AUTO_TEST_CASE(test_Waveform_serialize) {
    ISMRMRD::Waveform value1;
    ISMRMRD::Waveform value2;

    value1.head.measurement_uid = 5;

    check(value1, value2);
}

BOOST_AUTO_TEST_CASE(test_IsmrmrdHeader_serialize) {
    ISMRMRD::IsmrmrdHeader value1;
    ISMRMRD::IsmrmrdHeader value2;

    ISMRMRD::Encoding encoding;
    encoding.encodedSpace = { {32,32,1}, {256,256,5}};
    encoding.encodingLimits.kspace_encoding_step_0 = ISMRMRD::Limit();
    encoding.encodingLimits.kspace_encoding_step_1 = ISMRMRD::Limit();
    encoding.encodingLimits.kspace_encoding_step_0->minimum = 0;
    encoding.encodingLimits.kspace_encoding_step_0->center = 16;
    encoding.encodingLimits.kspace_encoding_step_0->maximum = 31;
    encoding.encodingLimits.kspace_encoding_step_1->minimum = 0;
    encoding.encodingLimits.kspace_encoding_step_1->center = 16;
    encoding.encodingLimits.kspace_encoding_step_1->maximum = 31;
    encoding.reconSpace = { {32, 32, 1}, { 256, 256, 5}};
    encoding.trajectory = ISMRMRD::TrajectoryType::CARTESIAN;
    value1.experimentalConditions.H1resonanceFrequency_Hz = 63.87 * 1e6;

    value1.encoding = { encoding };

    check(value1, value2);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_NDArray_serialize, T, test_types) {
    std::vector<size_t> data(3);
    data[0] = data[1] = data[2] = 1;
    ISMRMRD::NDArray<T> value1(data);
    ISMRMRD::NDArray<T> value2(data);

    *(value1.begin()) = static_cast<T>(5);

    check(value1, value2);
}

BOOST_AUTO_TEST_SUITE_END()

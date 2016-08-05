#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/version.h"
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>

using namespace ISMRMRD;

typedef boost::mpl::list<uint16_t, int16_t, uint32_t, int32_t, float,
                         double, std::complex<float>, std::complex<double> > test_types;

BOOST_AUTO_TEST_SUITE(Waveforms)

static void check_header(const WaveformHeader& head);

BOOST_AUTO_TEST_CASE_TEMPLATE(waveform_create, T, test_types)
{
  Waveform<T> wvf;
  WaveformHeader head = wvf.getHead();

  size_t expected_size = sizeof(uint64_t) * 2 +
                         sizeof(uint32_t) * 2 + 
                         sizeof(int32_t) * ISMRMRD_USER_INTS + 
                         sizeof(float) * ISMRMRD_USER_FLOATS;

  BOOST_CHECK_EQUAL(sizeof(head), expected_size);

  // Check that header is initialized properly
  check_header(head);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(waveform_copy, T, test_types)
{
  Waveform<T> wvf1;
  check_header(wvf1.getHead());
  Waveform<T> wvf2(wvf1);
  check_header(wvf2.getHead());

  BOOST_CHECK(wvf1.getHead() == wvf2.getHead());

  Waveform<T> wvf3;
  wvf3.setHead(wvf1.getHead());
  BOOST_CHECK(wvf3.getHead() == wvf1.getHead());
  BOOST_CHECK_EQUAL(wvf3.getNumberOfSamples(), wvf1.getNumberOfSamples());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(waveform_getters_setters, T, test_types)
{
  Waveform<T> wvf;

  wvf.setBeginTimeStamp(3456789012);
  BOOST_CHECK_EQUAL(wvf.getBeginTimeStamp(), 3456789012);

  wvf.setEndTimeStamp(2134567890);
  BOOST_CHECK_EQUAL(wvf.getEndTimeStamp(), 2134567890);

  wvf.resize(4096);
  BOOST_CHECK_EQUAL(wvf.getNumberOfSamples(), 4096);

  wvf.setDwellTime_ns(1321);
  BOOST_CHECK_EQUAL(wvf.getDwellTime_ns(), 1321);

  for (int ii = 0; ii < ISMRMRD_USER_INTS; ii++)
  {
    wvf.setUserInt(ii, ii + 99);
    BOOST_CHECK_EQUAL(wvf.getUserInt(ii), ii + 99);
  }

  for (int ii = 0; ii < ISMRMRD_USER_FLOATS; ii++)
  {
    wvf.setUserFloat(ii, ii + 147.5);
    BOOST_CHECK_CLOSE(wvf.getUserFloat(ii), ii + 147.5, 0.01);
  }

  wvf.resize(256);
  std::vector<T> data(256);
  for (int ii = 0; ii < 256; ii++)
  {
    data[ii] = ii * 1.234;
  }
  wvf.setData(data);
  std::vector<T> data1(wvf.getData());
  for (int ii = 0; ii < 256; ii++)
  {
    BOOST_CHECK_EQUAL(data[ii], data1[ii]);
    //BOOST_CHECK_CLOSE(data[ii], data1[ii], 0.01);
  }

  for (uint32_t ii = 0; ii < 256; ii++)
  {
    BOOST_CHECK_EQUAL(wvf.at(ii), data1[ii]);
    //BOOST_CHECK_CLOSE(wvf.at(ii), data1[ii], 0.01);
  }

  BOOST_CHECK_EQUAL(wvf.getEntityType(), ISMRMRD_WAVEFORM);
  BOOST_CHECK_EQUAL(wvf.getStorageType(), ISMRMRD_DOUBLE);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(waveform_resize, T, test_types)
{
  Waveform<T> wvf;

  check_header(wvf.getHead());
  BOOST_CHECK_EQUAL(wvf.getData().size(), 0);
  BOOST_CHECK_EQUAL(wvf.getNumberOfSamples(), 0);

  wvf.resize(9192);
  std::vector<T> temp(9192, 0);

  BOOST_CHECK_EQUAL_COLLECTIONS(temp.begin(), temp.end(), wvf.getData().begin(), wvf.getData().end());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(serialize, T, test_types)
{
  Waveform<T> wvf(2048);
  std::vector <unsigned char> buf = wvf.serialize();

  Waveform<T> wvf1;
  wvf1.deserialize(buf);

  BOOST_CHECK(wvf.getHead() == wvf1.getHead());
  BOOST_CHECK_EQUAL_COLLECTIONS(wvf.getData().begin(), wvf.getData().end(),
                                 wvf1.getData().begin(), wvf1.getData().end());
}

static void check_header(const WaveformHeader& head)
{
  BOOST_CHECK_EQUAL(head.begin_time_stamp, 0);
  BOOST_CHECK_EQUAL(head.end_time_stamp, 0);
  BOOST_CHECK_EQUAL(head.number_of_samples, 0);
  BOOST_CHECK_EQUAL(head.dwell_time_ns, 0);

  for (uint32_t ii = 0; ii < ISMRMRD_USER_INTS; ii++)
  {
    BOOST_CHECK_EQUAL(head.user_int[ii], 0);
  }

  for (uint32_t ii = 0; ii < ISMRMRD_USER_FLOATS; ii++)
  {
    BOOST_CHECK_CLOSE(head.user_float[ii], 0, 0.01);
  }
}

BOOST_AUTO_TEST_SUITE_END()

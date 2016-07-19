#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/version.h"
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>

using namespace ISMRMRD;

typedef boost::mpl::list<uint16_t, int16_t, uint32_t, int32_t, float, double,
                         std::complex<float>, std::complex<double> > test_types;

BOOST_AUTO_TEST_SUITE(Images)

static void check_header (const ImageHeader& head);

BOOST_AUTO_TEST_CASE_TEMPLATE (image_create, T, test_types)
{
  Image<T> img;
  ImageHeader head = img.getHead();
  EntityHeader ehdr = head.ent_head;

  // Check that header is of expected size
  size_t ehdr_size = sizeof (uint32_t) * 4;
  BOOST_CHECK_EQUAL (sizeof (ehdr), ehdr_size);

  size_t expected_size = sizeof(ehdr) +
                         sizeof(uint32_t) * ISMRMRD_PHYS_STAMPS +
                         sizeof(uint32_t) * 14 +
                         sizeof(int32_t) * ISMRMRD_USER_INTS +
                         sizeof(uint64_t) * 2 +
                         sizeof(float) * 3 +
                         sizeof(float) * ISMRMRD_POSITION_LENGTH * 2 +
                         sizeof(float) * ISMRMRD_DIRECTION_LENGTH * 3 +
                         sizeof(float) * ISMRMRD_USER_FLOATS;

  BOOST_CHECK_EQUAL (sizeof(head), expected_size);

  // Check that header is initialized properly
  check_header (head);
}

BOOST_AUTO_TEST_CASE_TEMPLATE (image_copy, T, test_types)
{
  Image<T> img1;
  check_header (img1.getHead());
  Image<T> img2 (img1);
  check_header (img2.getHead());

  BOOST_CHECK (img1.getHead() == img2.getHead());

  Image<T> img3;
  img3.setHead (img1.getHead());
  BOOST_CHECK (img3.getHead() == img1.getHead());
}

BOOST_AUTO_TEST_CASE_TEMPLATE (image_getters_setters, T, test_types)
{
  Image<T> img;

  img.setMatrixSizeX (0);
  BOOST_CHECK_EQUAL (img.getMatrixSizeX(), 0);
  BOOST_CHECK_EQUAL (img.getNumberOfElements(), 0);

  img.setMatrixSizeX (42);
  BOOST_CHECK_EQUAL (img.getMatrixSizeX(), 42);
  BOOST_CHECK_EQUAL (img.getNumberOfElements(), 42);

  img.setMatrixSizeY (142);
  BOOST_CHECK_EQUAL (img.getMatrixSizeY(), 142);
  BOOST_CHECK_EQUAL (img.getNumberOfElements(), 42 * 142);

  img.setMatrixSizeZ (32);
  BOOST_CHECK_EQUAL (img.getMatrixSizeZ(), 32);
  BOOST_CHECK_EQUAL (img.getNumberOfElements(), 42 * 142 * 32);

  img.setNumberOfChannels (8);
  BOOST_CHECK_EQUAL (img.getNumberOfChannels(), 8);
  BOOST_CHECK_EQUAL (img.getNumberOfElements(), 42 * 142 * 32 * 8);

  // Field of view
  img.setFieldOfView (320.0, 480.0, 16.0);
  BOOST_CHECK_CLOSE (img.getFieldOfViewX(), 320.0, 0.01);
  BOOST_CHECK_CLOSE (img.getFieldOfViewY(), 480.0, 0.01);
  BOOST_CHECK_CLOSE (img.getFieldOfViewZ(), 16.0, 0.01);

  img.setFieldOfViewX (147.5);
  BOOST_CHECK_CLOSE (img.getFieldOfViewX(), 147.5, 0.01);

  img.setFieldOfViewY (123.4);
  BOOST_CHECK_CLOSE (img.getFieldOfViewY(), 123.4, 0.01);

  img.setFieldOfViewZ (43.2);
  BOOST_CHECK_CLOSE (img.getFieldOfViewZ(), 43.2, 0.01);

  // Positions and orientations
  img.setPosition (123.4, 234.5, 345.6);
  BOOST_CHECK_CLOSE (img.getPositionX(), 123.4, 0.01);
  BOOST_CHECK_CLOSE (img.getPositionY(), 234.5, 0.01);
  BOOST_CHECK_CLOSE (img.getPositionZ(), 345.6, 0.01);
  img.setPositionX (345.6);
  BOOST_CHECK_CLOSE (img.getPositionX(), 345.6, 0.01);
  img.setPositionY (123.4);
  BOOST_CHECK_CLOSE (img.getPositionY(), 123.4, 0.01);
  img.setPositionZ (234.5);
  BOOST_CHECK_CLOSE (img.getPositionZ(), 234.5, 0.01);

  img.setReadDirection (3.21, 4.32, 5.43);
  BOOST_CHECK_CLOSE (img.getReadDirectionX(), 3.21, 0.01);
  BOOST_CHECK_CLOSE (img.getReadDirectionY(), 4.32, 0.01);
  BOOST_CHECK_CLOSE (img.getReadDirectionZ(), 5.43, 0.01);
  img.setReadDirectionX (7.12);
  BOOST_CHECK_CLOSE (img.getReadDirectionX(), 7.12, 0.01);
  img.setReadDirectionY (5.98);
  BOOST_CHECK_CLOSE (img.getReadDirectionY(), 5.98, 0.01);
  img.setReadDirectionZ (8.76);
  BOOST_CHECK_CLOSE (img.getReadDirectionZ(), 8.76, 0.01);

  img.setPhaseDirection (5.23, 3.93, 4.19);
  BOOST_CHECK_CLOSE (img.getPhaseDirectionX(), 5.23, 0.01);
  BOOST_CHECK_CLOSE (img.getPhaseDirectionY(), 3.93, 0.01);
  BOOST_CHECK_CLOSE (img.getPhaseDirectionZ(), 4.19, 0.01);
  img.setPhaseDirectionX (5.01);
  BOOST_CHECK_CLOSE (img.getPhaseDirectionX(), 5.01, 0.01);
  img.setPhaseDirectionY (2.19);
  BOOST_CHECK_CLOSE (img.getPhaseDirectionY(), 2.19, 0.01);
  img.setPhaseDirectionZ (8.91);
  BOOST_CHECK_CLOSE (img.getPhaseDirectionZ(), 8.91, 0.01);

  img.setSliceDirection (6.79, 8.39, 4.89);
  BOOST_CHECK_CLOSE (img.getSliceDirectionX(), 6.79, 0.01);
  BOOST_CHECK_CLOSE (img.getSliceDirectionY(), 8.39, 0.01);
  BOOST_CHECK_CLOSE (img.getSliceDirectionZ(), 4.89, 0.01);
  img.setSliceDirectionX (7.26);
  BOOST_CHECK_CLOSE (img.getSliceDirectionX(), 7.26, 0.01);
  img.setSliceDirectionY (0.62);
  BOOST_CHECK_CLOSE (img.getSliceDirectionY(), 0.62, 0.01);
  img.setSliceDirectionZ ( 2.84);
  BOOST_CHECK_CLOSE (img.getSliceDirectionZ(),  2.84, 0.01);

  img.setPatientTablePosition (7.31, 1.23, 2.08);
  BOOST_CHECK_CLOSE (img.getPatientTablePositionX(), 7.31, 0.01);
  BOOST_CHECK_CLOSE (img.getPatientTablePositionY(), 1.23, 0.01);
  BOOST_CHECK_CLOSE (img.getPatientTablePositionZ(), 2.08, 0.01);
  img.setPatientTablePositionX (2.08);
  BOOST_CHECK_CLOSE (img.getPatientTablePositionX(), 2.08, 0.01);
  img.setPatientTablePositionY (8.54);
  BOOST_CHECK_CLOSE (img.getPatientTablePositionY(), 8.54, 0.01);
  img.setPatientTablePositionZ (4.93);
  BOOST_CHECK_CLOSE (img.getPatientTablePositionZ(), 4.93, 0.01);

  // Counters and labels
  img.setAverage (67);
  BOOST_CHECK_EQUAL (img.getAverage(), 67);

  img.setSlice (43);
  BOOST_CHECK_EQUAL (img.getSlice(), 43);

  img.setContrast (456);
  BOOST_CHECK_EQUAL (img.getContrast(), 456);

  img.setPhase (531);
  BOOST_CHECK_EQUAL (img.getPhase(), 531);

  img.setRepetition (6);
  BOOST_CHECK_EQUAL (img.getRepetition(), 6);

  img.setSet (159);
  BOOST_CHECK_EQUAL (img.getSet(), 159);

  img.setTimeStamp (1234567890);
  BOOST_CHECK_EQUAL (img.getTimeStamp(), 1234567890);

  for (int idx = 0; idx < ISMRMRD_PHYS_STAMPS; idx++)
  {
    img.setPhysiologyTimeStamp (idx, 54637 * (idx + 1));
    BOOST_CHECK_EQUAL (img.getPhysiologyTimeStamp (idx), 54637 * (idx + 1));
  }

  img.setImageType (3);
  BOOST_CHECK_EQUAL (img.getImageType(), 3);

  img.setImageIndex (2);
  BOOST_CHECK_EQUAL (img.getImageIndex(), 2);

  img.setImageSeriesIndex (4);
  BOOST_CHECK_EQUAL (img.getImageSeriesIndex(), 4);

  // User parameters
  for (int ii = 0; ii < ISMRMRD_USER_FLOATS; ii++)
  {
    img.setUserFloat (ii, ii + 19.54);
    BOOST_CHECK_CLOSE (img.getUserFloat (ii), ii + 19.54, 0.01);
  }

  for (int ii = 0; ii < ISMRMRD_USER_INTS; ii++)
  {
    img.setUserInt(ii, ii + 37);
    BOOST_CHECK_EQUAL (img.getUserInt (ii), ii + 37);
  }

  // Flags
  uint64_t val = 0x23456789ABCDEF01;
  img.setFlags (val);
  BOOST_CHECK_EQUAL (img.getFlags(), val);

  img.clearAllFlags ();
  BOOST_CHECK_EQUAL (img.getFlags(), 0);

  val = 0x6000;
  BOOST_CHECK_EQUAL (img.isFlagSet (val), 0);

  img.setFlag (val);
  BOOST_CHECK_EQUAL (img.isFlagSet (val), 1);

  img.clearFlag (val);
  BOOST_CHECK_EQUAL (img.isFlagSet (val), 0);

  // Data set/get
  img.resize (42, 86, 16, 4);
  std::vector<T> data (42 * 86 * 16 * 4);
  for (int ii = 0; ii < 42 * 86 * 16 * 4; ii++)
  {
    data[ii] = (T)(ii + 52);
  }

  for (int ix = 0; ix < 42; ix++)
  {
    for (int iy = 0; iy < 86; iy++)
    {
      for (int iz = 0; iz < 16; iz++)
      {
        for (int ic = 0; ic < 4; ic++)
        {
          size_t idx = ix + (42 * iy) + (42 * 86 * iz) + (42 * 86 * 16 * ic);
          img.at (ix, iy, iz, ic) = data[idx];
        }
      }
    }
  }

  std::vector<T> data1;
  data1 = img.getData();

  for (int ix = 0; ix < 42; ix++)
  {
    for (int iy = 0; iy < 86; iy++)
    {
      for (int iz = 0; iz < 16; iz++)
      {
        for (int ic = 0; ic < 4; ic++)
        {
          size_t idx = ix + (42 * iy) + (42 * 86 * iz) + (42 * 86 * 16 * ic);
          BOOST_CHECK_EQUAL (data[idx], data1[idx]);
        }
      }
    }
  }

  // Attribute string
  std::string temp ("This is a test string with spaces");
  img.setAttributeString (temp);
  BOOST_CHECK_EQUAL (img.getAttributeStringLength(), temp.size());
  BOOST_CHECK_EQUAL (std::strcmp (img.getAttributeString().data(),
                                  temp.data()), 0);

  std::string temp1;
  img.getAttributeString (temp1);
  BOOST_CHECK_EQUAL (std::strcmp (temp1.data(), temp.data()), 0);


  img.setStream (123);
  BOOST_CHECK_EQUAL (img.getStream(), 123);

  BOOST_CHECK_EQUAL (img.getSignature(), ISMRMRD_SIGNATURE);
  BOOST_CHECK_EQUAL (img.getVersion(), 2); //TODO: Use ISMRMRD_VERSION_MAJOR?
  BOOST_CHECK_EQUAL (img.getEntityType(), ISMRMRD_IMAGE);
  BOOST_CHECK_EQUAL (img.getStorageType(), get_storage_type<T>());
}

BOOST_AUTO_TEST_CASE_TEMPLATE (image_resize, T, test_types)
{
  Image<T> img;
  check_header (img.getHead());
  BOOST_CHECK_EQUAL (img.getData().size(), 0);

  img.resize (72, 72, 24, 0);
  BOOST_CHECK_EQUAL (img.getMatrixSizeX(), 72);
  BOOST_CHECK_EQUAL (img.getMatrixSizeY(), 72);
  BOOST_CHECK_EQUAL (img.getMatrixSizeZ(), 24);
  BOOST_CHECK_EQUAL (img.getData().size(), 72 * 72 * 24);

  std::vector<T> zeros (72 * 72 * 24, 0);
  BOOST_CHECK_EQUAL_COLLECTIONS (zeros.begin(), zeros.end(),
                                 img.getData().begin(), img.getData().end());
}

BOOST_AUTO_TEST_CASE_TEMPLATE (image_serialize, T, test_types)
{
  Image<T> im (256, 256, 10, 12);

  std::vector<unsigned char> buffer = im.serialize();

  Image<T> im2;
  im2.deserialize (buffer);
    
  BOOST_CHECK_EQUAL_COLLECTIONS (im.getData().begin(), im.getData().end(),
                                 im2.getData().begin(), im2.getData().end());
    
  BOOST_CHECK (im.getAttributeString() == im2.getAttributeString());
  
  BOOST_CHECK (im.getHead() == im2.getHead());
}

static void check_header (const ImageHeader& chead)
{
  BOOST_CHECK_EQUAL (chead.ent_head.stream, 0);
  BOOST_CHECK_EQUAL (chead.ent_head.signature, ISMRMRD_SIGNATURE);
  BOOST_CHECK_EQUAL (chead.ent_head.entity_type, ISMRMRD_IMAGE);

  BOOST_CHECK_EQUAL (chead.time_stamp, 0);
  BOOST_CHECK_EQUAL (chead.flags, 0);

  BOOST_CHECK_EQUAL (chead.matrix_size[0], 0);
  BOOST_CHECK_EQUAL (chead.matrix_size[1], 1);
  BOOST_CHECK_EQUAL (chead.matrix_size[2], 1);

  for (int idx = 0; idx < 3; idx++)
  {
    BOOST_CHECK_EQUAL (chead.field_of_view[idx], 0);
  }

  BOOST_CHECK_EQUAL (chead.channels, 1);

  for (int idx = 0; idx < ISMRMRD_POSITION_LENGTH; idx++)
  {
    BOOST_CHECK_EQUAL (chead.position[idx], 0);
  }

  for (int idx = 0; idx < ISMRMRD_DIRECTION_LENGTH; idx++)
  {
    BOOST_CHECK_EQUAL (chead.read_dir[idx], 0);
  }

  for (int idx = 0; idx < ISMRMRD_DIRECTION_LENGTH; idx++)
  {
    BOOST_CHECK_EQUAL (chead.phase_dir[idx], 0);
  }

  for (int idx = 0; idx < ISMRMRD_DIRECTION_LENGTH; idx++)
  {
    BOOST_CHECK_EQUAL (chead.slice_dir[idx], 0);
  }

  for (int idx = 0; idx < ISMRMRD_POSITION_LENGTH; idx++)
  {
    BOOST_CHECK_EQUAL (chead.patient_table_position[idx], 0);
  }

  BOOST_CHECK_EQUAL (chead.average, 0);
  BOOST_CHECK_EQUAL (chead.slice, 0);
  BOOST_CHECK_EQUAL (chead.contrast, 0);
  BOOST_CHECK_EQUAL (chead.phase, 0);
  BOOST_CHECK_EQUAL (chead.repetition, 0);
  BOOST_CHECK_EQUAL (chead.set, 0);

  for (int idx = 0; idx < ISMRMRD_PHYS_STAMPS; idx++)
  {
    BOOST_CHECK_EQUAL (chead.physiology_time_stamp[idx], 0);
  }

  BOOST_CHECK_EQUAL (chead.image_type, 0);
  BOOST_CHECK_EQUAL (chead.image_index, 0);
  BOOST_CHECK_EQUAL (chead.image_series_index, 0);

  for (int idx = 0; idx < ISMRMRD_USER_INTS; idx++)
  {
    BOOST_CHECK_EQUAL (chead.user_int[idx], 0);
  }

  for (int idx = 0; idx < ISMRMRD_USER_FLOATS; idx++)
  {
    BOOST_CHECK_EQUAL (chead.user_float[idx], 0);
  }

  BOOST_CHECK_EQUAL (chead.attribute_string_len, 0);
}

BOOST_AUTO_TEST_SUITE_END()

#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/version.h"
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>

using namespace ISMRMRD;

typedef boost::mpl::list<int16_t, int32_t, float, double> test_types;

BOOST_AUTO_TEST_SUITE (Acquisitions)

static void check_header (const AcquisitionHeader& chead);

BOOST_AUTO_TEST_CASE_TEMPLATE (acquisition_create, T, test_types)
{
  Acquisition<T>    acq;
  AcquisitionHeader head = acq.getHead();
  EncodingCounters  ec   = head.idx;

  size_t ec_size = sizeof (uint32_t) * (10 + ISMRMRD_USER_INTS);
  BOOST_CHECK_EQUAL (sizeof (ec), ec_size);

  // Check that header is of expected size
  size_t expected_size = sizeof (uint32_t) * (10 + ISMRMRD_PHYS_STAMPS) +
                         sizeof (int32_t)  * ISMRMRD_USER_INTS +
                         sizeof (uint64_t) * (2 + ISMRMRD_CHANNEL_MASKS) +
                         sizeof (float)    * (ISMRMRD_POSITION_LENGTH * 2 +
                                              ISMRMRD_DIRECTION_LENGTH * 3 +
                                              ISMRMRD_USER_FLOATS) +
                         sizeof (ec);

  BOOST_CHECK_EQUAL (sizeof(head), expected_size);

  // Check that header is initialized properly
  check_header (head);
}

BOOST_AUTO_TEST_CASE_TEMPLATE (acquisition_copy, T, test_types)
{
  Acquisition<T> acq1;
  check_header (acq1.getHead());
  Acquisition<T> acq2 (acq1);
  check_header (acq2.getHead());

  BOOST_CHECK (acq1.getHead() == acq2.getHead());

  Acquisition<T> acq3;
  acq3.setHead (acq1.getHead());
  BOOST_CHECK (acq3.getHead() == acq2.getHead());
}


BOOST_AUTO_TEST_CASE_TEMPLATE (acquisition_getters_setters, T, test_types)
{
  Acquisition<T> acq;

  acq.setScanCounter (2);
  BOOST_CHECK_EQUAL (acq.getScanCounter(), 2);

  acq.setTimeStamp (1234567890);
  BOOST_CHECK_EQUAL (acq.getTimeStamp(), 1234567890);

  for (int idx = 0; idx < ISMRMRD_PHYS_STAMPS; idx++)
  {
    acq.setPhysiologyTimeStamp (idx, 12345 * (idx + 1));
    BOOST_CHECK_EQUAL (acq.getPhysiologyTimeStamp (idx), 12345 * (idx + 1));
  }

  acq.setNumberOfSamples (256);
  BOOST_CHECK_EQUAL (acq.getNumberOfSamples(), 256);
  BOOST_CHECK_EQUAL (acq.getData().size(), 256 * acq.getActiveChannels());
  BOOST_CHECK_EQUAL (acq.getTraj().size(), 256 * acq.getTrajectoryDimensions());

  acq.setAvailableChannels (8);
  BOOST_CHECK_EQUAL (acq.getAvailableChannels(), 8);

  acq.setActiveChannels (5);
  BOOST_CHECK_EQUAL (acq.getActiveChannels(), 5);

  for (int idx = 0; idx < ISMRMRD_CHANNEL_MASKS; idx++)
  {
    acq.setChannelMask(idx, 23456 * (idx + 2));
    BOOST_CHECK_EQUAL (acq.getChannelMask (idx), 23456 * (idx + 2));
  }

  acq.setDiscardPre (147);
  BOOST_CHECK_EQUAL (acq.getDiscardPre(), 147);

  acq.setDiscardPost (31);
  BOOST_CHECK_EQUAL (acq.getDiscardPost(), 31);

  acq.setCenterSample (71);
  BOOST_CHECK_EQUAL (acq.getCenterSample(), 71);

  acq.setEncodingSpaceRef (32);
  BOOST_CHECK_EQUAL (acq.getEncodingSpaceRef(), 32);

  acq.setTrajectoryDimensions (2);
  BOOST_CHECK_EQUAL (acq.getTrajectoryDimensions(), 2);
  BOOST_CHECK_EQUAL (acq.getTraj().size(), acq.getTrajectoryDimensions() *
                                           acq.getNumberOfSamples());
  BOOST_CHECK_EQUAL (acq.getNumberOfTrajElements(),
                     acq.getTrajectoryDimensions() *
                     acq.getNumberOfSamples());

  acq.setDwellTime_ns (321);
  BOOST_CHECK_EQUAL (acq.getDwellTime_ns(), 321);

  acq.setPositionX (345.6);
  BOOST_CHECK_CLOSE (acq.getPositionX(), 345.6, 0.01);

  acq.setPositionY (456.7);
  BOOST_CHECK_CLOSE (acq.getPositionY(), 456.7, 0.01);

  acq.setPositionZ (567.8);
  BOOST_CHECK_CLOSE (acq.getPositionZ(), 567.8, 0.01);

  acq.setPosition (432.1, 543.2, 654.3);
  BOOST_CHECK_CLOSE (acq.getPositionX(), 432.1, 0.01);
  BOOST_CHECK_CLOSE (acq.getPositionY(), 543.2, 0.01);
  BOOST_CHECK_CLOSE (acq.getPositionZ(), 654.3, 0.01);

  acq.setReadDirectionX (987.9);
  BOOST_CHECK_CLOSE (acq.getReadDirectionX(), 987.9, 0.01);

  acq.setReadDirectionY (876.8);
  BOOST_CHECK_CLOSE (acq.getReadDirectionY(), 876.8, 0.01);

  acq.setReadDirectionZ (765.7);
  BOOST_CHECK_CLOSE (acq.getReadDirectionZ(), 765.7, 0.01);

  acq.setReadDirection (357.9, 135.7, 534.1);
  BOOST_CHECK_CLOSE (acq.getReadDirectionX(), 357.9, 0.01);
  BOOST_CHECK_CLOSE (acq.getReadDirectionY(), 135.7, 0.01);
  BOOST_CHECK_CLOSE (acq.getReadDirectionZ(), 534.1, 0.01);

  acq.setPhaseDirectionX (421.3);
  BOOST_CHECK_CLOSE (acq.getPhaseDirectionX(), 421.3, 0.01);

  acq.setPhaseDirectionY (564.2);
  BOOST_CHECK_CLOSE (acq.getPhaseDirectionY(), 564.2, 0.01);

  acq.setPhaseDirectionZ (462.3);
  BOOST_CHECK_CLOSE (acq.getPhaseDirectionZ(), 462.3, 0.01);

  acq.setPhaseDirection (102.9, 524.3, 832.1);
  BOOST_CHECK_CLOSE (acq.getPhaseDirectionX(), 102.9, 0.01);
  BOOST_CHECK_CLOSE (acq.getPhaseDirectionY(), 524.3, 0.01);
  BOOST_CHECK_CLOSE (acq.getPhaseDirectionZ(), 832.1, 0.01);

  acq.setSliceDirectionX (368.1);
  BOOST_CHECK_CLOSE (acq.getSliceDirectionX(), 368.1, 0.01);

  acq.setSliceDirectionY (283.1);
  BOOST_CHECK_CLOSE (acq.getSliceDirectionY(), 283.1, 0.01);

  acq.setSliceDirectionZ (592.1);
  BOOST_CHECK_CLOSE (acq.getSliceDirectionZ(), 592.1, 0.01);

  acq.setSliceDirection (598.2, 215.2, 871.1);
  BOOST_CHECK_CLOSE (acq.getSliceDirectionX(), 598.2, 0.01);
  BOOST_CHECK_CLOSE (acq.getSliceDirectionY(), 215.2, 0.01);
  BOOST_CHECK_CLOSE (acq.getSliceDirectionZ(), 871.1, 0.01);

  acq.setPatientTablePositionX (825.1);
  BOOST_CHECK_CLOSE (acq.getPatientTablePositionX(), 825.1, 0.01);

  acq.setPatientTablePositionY (714.2);
  BOOST_CHECK_CLOSE (acq.getPatientTablePositionY(), 714.2, 0.01);

  acq.setPatientTablePositionZ (509.1);
  BOOST_CHECK_CLOSE (acq.getPatientTablePositionZ(), 509.1, 0.01);

  acq.setPatientTablePosition (742.4, 410.3, 401.9);
  BOOST_CHECK_CLOSE (acq.getPatientTablePositionX(), 742.4, 0.01);
  BOOST_CHECK_CLOSE (acq.getPatientTablePositionY(), 410.3, 0.01);
  BOOST_CHECK_CLOSE (acq.getPatientTablePositionZ(), 401.9, 0.01);


  EncodingCounters ec;
  ec.kspace_encode_step_0 = 21;
  ec.kspace_encode_step_1 = 31;
  ec.kspace_encode_step_2 = 41;
  ec.average              = 51;
  ec.slice                = 61;
  ec.contrast             = 71;
  ec.phase                = 81;
  ec.repetition           = 91;
  ec.set                  = 101;
  ec.segment              = 111;

  for (int ii = 0; ii < ISMRMRD_USER_INTS; ii++)
  {
    ec.user[ii] = ii + 147;
  }

  acq.setEncodingCounters (ec);
  BOOST_CHECK_EQUAL (acq.getEncodingCounters().kspace_encode_step_0,
                     ec.kspace_encode_step_0);
  BOOST_CHECK_EQUAL (acq.getEncodingCounters().kspace_encode_step_1,
                     ec.kspace_encode_step_1);
  BOOST_CHECK_EQUAL (acq.getEncodingCounters().kspace_encode_step_2,
                     ec.kspace_encode_step_2);
  BOOST_CHECK_EQUAL (acq.getEncodingCounters().average, ec.average);
  BOOST_CHECK_EQUAL (acq.getEncodingCounters().slice, ec.slice);
  BOOST_CHECK_EQUAL (acq.getEncodingCounters().contrast, ec.contrast);
  BOOST_CHECK_EQUAL (acq.getEncodingCounters().phase, ec.phase);
  BOOST_CHECK_EQUAL (acq.getEncodingCounters().repetition, ec.repetition);
  BOOST_CHECK_EQUAL (acq.getEncodingCounters().set, ec.set);
  BOOST_CHECK_EQUAL (acq.getEncodingCounters().segment, ec.segment);

  for (int ii = 0; ii < ISMRMRD_USER_INTS; ii++)
  {
    BOOST_CHECK_EQUAL (acq.getEncodingCounters().user[ii],  ec.user[ii]);
  }

  for (int ii = 0; ii < ISMRMRD_USER_INTS; ii++)
  {
    acq.setUserInt (ii, ii + 42);
    BOOST_CHECK_EQUAL (acq.getUserInt (ii), ii + 42);
  }

  for (int ii = 0; ii < ISMRMRD_USER_FLOATS; ii++)
  {
    acq.setUserFloat (ii, ii + 147.5);
    BOOST_CHECK_EQUAL (acq.getUserFloat (ii), ii + 147.5);
  }


  acq.resize (144, 32, 3);
  std::vector<std::complex< T > > data (144 * 32);
  for (int ii = 0; ii < 144 * 32; ii++)
  {
    data[ii] = (T)(ii + 123);
  }

  acq.setData (data);
  for (int ii = 0; ii < 144 * 32; ii++)
  {
    BOOST_CHECK_EQUAL (acq.getData()[ii], data[ii]);
  }

  for (int ii = 0; ii < 144; ii++)
  {
    for (int yy = 0; yy < 32; yy++)
    {
      BOOST_CHECK_EQUAL (acq.at (ii, yy),
                         data[ii + yy * acq.getNumberOfSamples()]);
    }
  }
  

  std::vector<float> traj (acq.getNumberOfSamples() *
                           acq.getTrajectoryDimensions());

  for (int ii = 0; ii < acq.getTraj().size(); ii++)
  {
    traj[ii] = ii + 147.5;
  }

  acq.setTraj (traj);
  for (int ii = 0; ii < acq.getTraj().size(); ii++)
  {
    BOOST_CHECK_EQUAL (acq.getTraj()[ii], traj[ii]);
  }

  for (int ii = 0; ii < 144; ii++)
  {
    for (int yy = 0; yy < 3; yy++)
    {
      BOOST_CHECK_EQUAL (acq.trajAt (yy, ii),
                         traj[yy + ii * acq.getTrajectoryDimensions()]);
    }
  }


  uint64_t val = 0xABCDEFABCDEFABCD;
  acq.setFlags (val);
  BOOST_CHECK_EQUAL (acq.getFlags(), val);

  acq.clearAllFlags ();
  BOOST_CHECK_EQUAL (acq.getFlags(), 0);

  val = 0x8000;
  BOOST_CHECK_EQUAL (acq.isFlagSet (val), 0);

  acq.setFlag (val);
  BOOST_CHECK_EQUAL (acq.isFlagSet (val), 1);

  acq.clearFlag (val);
  BOOST_CHECK_EQUAL (acq.isFlagSet (val), 0);

  
  FlagBit fb (4);
  BOOST_CHECK_EQUAL (acq.isFlagSet (fb), 0);

  acq.setFlag (fb);
  BOOST_CHECK_EQUAL (acq.isFlagSet (fb), 1);

  acq.clearFlag (fb);
  BOOST_CHECK_EQUAL (acq.isFlagSet (fb), 0);


  acq.setAllChannelsNotActive();
  for (int ii = 0; ii < acq.getAvailableChannels(); ii++)
  {
    BOOST_CHECK_EQUAL (acq.isChannelActive (ii), 0);
  }

  for (int ii = 0; ii < acq.getAvailableChannels(); ii++)
  {
    acq.setChannelActive (ii);
    BOOST_CHECK_EQUAL (acq.isChannelActive (ii), 1);
  }

  for (int ii = 0; ii < acq.getAvailableChannels(); ii++)
  {
    acq.setChannelNotActive (ii);
    BOOST_CHECK_EQUAL (acq.isChannelActive (ii), 0);
  }


  BOOST_CHECK_EQUAL (acq.getEntityType(), ISMRMRD_MRACQUISITION);
  BOOST_CHECK_EQUAL (acq.getStorageType(), get_storage_type<T>());

}

BOOST_AUTO_TEST_CASE_TEMPLATE (acquisition_resize, T, test_types)
{
  Acquisition<T> acq;
  check_header (acq.getHead());
  BOOST_CHECK_EQUAL (acq.getData().size(), 0);

  acq.resize (72, 32);
  BOOST_CHECK_EQUAL (acq.getNumberOfSamples(), 72);
  BOOST_CHECK_EQUAL (acq.getActiveChannels(), 32);
  BOOST_CHECK_EQUAL (acq.getData().size(), 72 * 32);
  BOOST_CHECK_EQUAL (acq.getNumberOfDataElements(), 72 * 32);

  std::vector<T> zeros (72 * 32, 0);
  BOOST_CHECK_EQUAL_COLLECTIONS (zeros.begin(), zeros.end(),
                                 acq.getData().begin(), acq.getData().end());
}

BOOST_AUTO_TEST_CASE_TEMPLATE (acquisition_serialize, T, test_types)
{
  Acquisition<T> acq;
  acq.resize (144, 32);
  acq.setTrajectoryDimensions (3);

  std::vector<unsigned char> buffer = acq.serialize();

  Acquisition<T> acq2;
  acq2.deserialize (buffer);
  BOOST_CHECK_EQUAL_COLLECTIONS (acq.getData().begin(), acq.getData().end(),
                                 acq2.getData().begin(), acq2.getData().end());
    
  BOOST_CHECK_EQUAL_COLLECTIONS (acq.getTraj().begin(), acq.getTraj().end(),
                                 acq2.getTraj().begin(), acq2.getTraj().end());
    
  BOOST_CHECK (acq.getHead() == acq2.getHead());
}

static void check_header (const AcquisitionHeader& chead)
{
  BOOST_CHECK_EQUAL (chead.time_stamp, 0);
  BOOST_CHECK_EQUAL (chead.flags, 0);
  BOOST_CHECK_EQUAL (chead.scan_counter, 0);

  for (int idx = 0; idx < ISMRMRD_PHYS_STAMPS; idx++)
  {
    BOOST_CHECK_EQUAL (chead.physiology_time_stamp[idx], 0);
  }

  BOOST_CHECK_EQUAL (chead.number_of_samples, 0);
  BOOST_CHECK_EQUAL (chead.available_channels, 1);
  BOOST_CHECK_EQUAL (chead.active_channels, 1);

  for (int idx = 0; idx < ISMRMRD_CHANNEL_MASKS; idx++)
  {
    BOOST_CHECK_EQUAL (chead.channel_mask[idx], 0);
  }

  BOOST_CHECK_EQUAL (chead.discard_pre, 0);
  BOOST_CHECK_EQUAL (chead.discard_post, 0);
  BOOST_CHECK_EQUAL (chead.center_sample, 0);
  BOOST_CHECK_EQUAL (chead.encoding_space_ref, 0);
  BOOST_CHECK_EQUAL (chead.trajectory_dimensions, 0);
  BOOST_CHECK_EQUAL (chead.dwell_time_ns, 0);

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

  // EncodingCounters
  BOOST_CHECK_EQUAL (chead.idx.kspace_encode_step_0, 0);
  BOOST_CHECK_EQUAL (chead.idx.kspace_encode_step_1, 0);
  BOOST_CHECK_EQUAL (chead.idx.kspace_encode_step_2, 0);
  BOOST_CHECK_EQUAL (chead.idx.average, 0);
  BOOST_CHECK_EQUAL (chead.idx.slice, 0);
  BOOST_CHECK_EQUAL (chead.idx.contrast, 0);
  BOOST_CHECK_EQUAL (chead.idx.phase, 0);
  BOOST_CHECK_EQUAL (chead.idx.repetition, 0);
  BOOST_CHECK_EQUAL (chead.idx.set, 0);
  BOOST_CHECK_EQUAL (chead.idx.segment, 0);

  for (int idx = 0; idx < ISMRMRD_USER_INTS; idx++)
  {
    BOOST_CHECK_EQUAL(chead.idx.user[idx], 0);
  }

  for (int idx = 0; idx < ISMRMRD_USER_INTS; idx++)
  {
    BOOST_CHECK_EQUAL(chead.user_int[idx], 0);
  }

  for (int idx = 0; idx < ISMRMRD_USER_FLOATS; idx++)
  {
    BOOST_CHECK_EQUAL(chead.user_float[idx], 0);
  }
}

BOOST_AUTO_TEST_SUITE_END()

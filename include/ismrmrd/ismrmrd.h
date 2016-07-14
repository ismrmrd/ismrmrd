/* ISMRMRD MR Raw Data Structures (version 2.x)            */
/*                                                         */
/* Authors:                                                */
/*    Michael S. Hansen (michael.hansen@nih.gov)           */
/*    Sebastian Kozerke (kozerke@biomed.ee.ethz.ch)        */
/*    Hui Xue           (hui.xue@nih.gov)                  */
/*    Souheil Inati     (souheil.inati@nih.gov)            */
/*    Joseph Naegele    (joseph.naegele@nih.gov)           */

/**
 * @file ismrmrd.h
 */

#pragma once
#ifndef ISMRMRD_H
#define ISMRMRD_H

#ifdef _MSC_VER /* MS compiler */
#ifndef HAS_INT_TYPE
typedef __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#endif /* HAS_INT_TYPE */
#else  /* non MS C or C++ compiler */
#include <stdint.h>
#endif /* _MSC_VER */

#include <complex>
#include <vector>
#include <map>

/* Exports needed for MS C++ */
#include "ismrmrd/export.h"

#pragma pack(push, 2) /* Use 2 byte alignment */

namespace ISMRMRD {

const char ISMRMRD_DATA_ID[4] = {'M', 'R', 'I', 'D'};

/** Global Constants */
enum Constant {
    ISMRMRD_USER_INTS = 8,
    ISMRMRD_USER_FLOATS = 8,
    ISMRMRD_PHYS_STAMPS = 3,
    ISMRMRD_CHANNEL_MASKS = 16,
    ISMRMRD_NDARRAY_MAXDIM = 7,
    ISMRMRD_POSITION_LENGTH = 3,
    ISMRMRD_DIRECTION_LENGTH = 3
};

/** Storage Types */
enum StorageType {
    ISMRMRD_CHAR = 0,      /**< corresponds to char           */
    ISMRMRD_USHORT = 1,    /**< corresponds to uint16_t       */
    ISMRMRD_SHORT = 2,     /**< corresponds to int16_t        */
    ISMRMRD_UINT = 3,      /**< corresponds to uint32_t       */
    ISMRMRD_INT = 4,       /**< corresponds to int32_t        */
    ISMRMRD_ULONG = 5,     /**< corresponds to uint64_t       */
    ISMRMRD_LONG = 6,      /**< corresponds to int64_t        */
    ISMRMRD_FLOAT = 7,     /**< corresponds to float          */
    ISMRMRD_DOUBLE = 8,    /**< corresponds to double         */
    ISMRMRD_CXFLOAT = 9,   /**< corresponds to complex float  */
    ISMRMRD_CXDOUBLE = 10, /**< corresponds to complex double */
    ISMRMRD_CXSHORT = 9,   /**< corresponds to complex int16_t  */
    ISMRMRD_CXINT = 10,    /**< corresponds to complex int32_t */
    ISMRMRD_STORAGE_NONE = 100 /**< Storage type not applicable */
};

enum EntityType {
    ISMRMRD_HANDSHAKE = 0,       /**< first package sent                   */
    ISMRMRD_COMMAND = 1,        /**< commands to control recon system     */
    ISMRMRD_MRACQUISITION = 2,  /**< MR raw data                          */
    ISMRMRD_WAVEFORM = 3,       /**< Gradient, physiology, etc. waveform  */
    ISMRMRD_IMAGE = 4,          /**< Reconstructed image                  */
    ISMRMRD_HEADER = 5,         /**< The XML header describing the data   */
    ISMRMRD_ERROR_REPORT = 6,   /**< Something went wrong                 */
    ISMRMRD_BLOB = 7            /**< Some binary object, with description */
};

enum CommandType
{
  ISMRMRD_COMMAND_NO_COMMAND         = 1000,
  ISMRMRD_COMMAND_STOP_FROM_CLIENT   = 1001,
  ISMRMRD_COMMAND_DONE_FROM_SERVER   = 1002,
  ISMRMRD_COMMAND_CLOSE_CONNECTION   = 1003,
  ISMRMRD_COMMAND_CONFIGURATION      = 1004,
  ISMRMRD_COMMAND_USER_DEFINED_1     = 2001,
  ISMRMRD_COMMAND_USER_DEFINED_2     = 2002,
  ISMRMRD_COMMAND_USER_DEFINED_3     = 2003,
  ISMRMRD_COMMAND_USER_DEFINED_4     = 2004
};

enum ErrorType
{
  ISMRMRD_ERROR_NONE                 = 20000,
  ISMRMRD_ERROR_INPUT_DATA_ERROR     = 20001,
  ISMRMRD_ERROR_MANIFEST_ERROR       = 20002,
  ISMRMRD_ERROR_INTERNAL_ERROR       = 20003,
  ISMRMRD_ERROR_UNKNOWN_ERROR        = 20004
};

//TODO: Not sure how the stream numbers are assigned, or when.
enum ReservedStream
{
  HANDSHAKE_STREAM             = 0,
  COMMAND_STREAM               = 1,
  ERROR_REPORT_STREAM          = 2,
  ISMRMRD_HEADER_STREAM        = 50,
  BLOB_STREAM                  = 100,
  UNRESERVED_STREAM_START      = 1000,
  UNRESERVED_STREAM_END        = 4000000000 // stream type is uint32_t
};

/** Acquisition Flags */
enum AcquisitionFlags {
    ISMRMRD_ACQ_FIRST_IN_ENCODE_STEP1 = 1,
    ISMRMRD_ACQ_LAST_IN_ENCODE_STEP1 = 2,
    ISMRMRD_ACQ_FIRST_IN_ENCODE_STEP2 = 3,
    ISMRMRD_ACQ_LAST_IN_ENCODE_STEP2 = 4,
    ISMRMRD_ACQ_FIRST_IN_AVERAGE = 5,
    ISMRMRD_ACQ_LAST_IN_AVERAGE = 6,
    ISMRMRD_ACQ_FIRST_IN_SLICE = 7,
    ISMRMRD_ACQ_LAST_IN_SLICE = 8,
    ISMRMRD_ACQ_FIRST_IN_CONTRAST = 9,
    ISMRMRD_ACQ_LAST_IN_CONTRAST = 10,
    ISMRMRD_ACQ_FIRST_IN_PHASE = 11,
    ISMRMRD_ACQ_LAST_IN_PHASE = 12,
    ISMRMRD_ACQ_FIRST_IN_REPETITION = 13,
    ISMRMRD_ACQ_LAST_IN_REPETITION = 14,
    ISMRMRD_ACQ_FIRST_IN_SET = 15,
    ISMRMRD_ACQ_LAST_IN_SET = 16,
    ISMRMRD_ACQ_FIRST_IN_SEGMENT = 17,
    ISMRMRD_ACQ_LAST_IN_SEGMENT = 18,
    ISMRMRD_ACQ_IS_NOISE_MEASUREMENT = 19,
    ISMRMRD_ACQ_IS_PARALLEL_CALIBRATION = 20,
    ISMRMRD_ACQ_IS_PARALLEL_CALIBRATION_AND_IMAGING = 21,
    ISMRMRD_ACQ_IS_REVERSE = 22,
    ISMRMRD_ACQ_IS_NAVIGATION_DATA = 23,
    ISMRMRD_ACQ_IS_PHASECORR_DATA = 24,
    ISMRMRD_ACQ_LAST_IN_MEASUREMENT = 25,
    ISMRMRD_ACQ_IS_HPFEEDBACK_DATA = 26,
    ISMRMRD_ACQ_IS_DUMMYSCAN_DATA = 27,
    ISMRMRD_ACQ_IS_RTFEEDBACK_DATA = 28,
    ISMRMRD_ACQ_IS_SURFACECOILCORRECTIONSCAN_DATA = 29,

    ISMRMRD_ACQ_USER1 = 57,
    ISMRMRD_ACQ_USER2 = 58,
    ISMRMRD_ACQ_USER3 = 59,
    ISMRMRD_ACQ_USER4 = 60,
    ISMRMRD_ACQ_USER5 = 61,
    ISMRMRD_ACQ_USER6 = 62,
    ISMRMRD_ACQ_USER7 = 63,
    ISMRMRD_ACQ_USER8 = 64
};

/** Image Types */
enum ImageTypes {
    ISMRMRD_IMTYPE_MAGNITUDE = 1,
    ISMRMRD_IMTYPE_PHASE = 2,
    ISMRMRD_IMTYPE_REAL = 3,
    ISMRMRD_IMTYPE_IMAG = 4,
    ISMRMRD_IMTYPE_COMPLEX = 5
};

/** Image Flags */
enum ImageFlags {
    ISMRMRD_IMAGE_IS_NAVIGATION_DATA = 1,
    ISMRMRD_IMAGE_USER1 = 57,
    ISMRMRD_IMAGE_USER2 = 58,
    ISMRMRD_IMAGE_USER3 = 59,
    ISMRMRD_IMAGE_USER4 = 60,
    ISMRMRD_IMAGE_USER5 = 61,
    ISMRMRD_IMAGE_USER6 = 62,
    ISMRMRD_IMAGE_USER7 = 63,
    ISMRMRD_IMAGE_USER8 = 64
};

enum ConnectionStatus
{
  CONNECTION_NOT_ASSIGNED          = 200,   /**< Client to server */
  CONNECTION_REQUEST               = 201,   /**< Client to server */
  CONNECTION_ACCEPTED              = 202,   /**< Server to client */
  CONNECTION_DENIED_UNKNOWN_USER   = 301,   /**< Server to client */
  CONNECTION_DENIED_SERVER_BUSY    = 302,   /**< Server to client */
  CONNECTION_DENIED_MANIFEST_ERROR = 303    /**< Server to client */
};

enum ConfigurationType
{
  CONFIGURATION_NONE               = 700,
  CONFIGURATION_FILE               = 701, // Filename in the config buffer
  CONFIGURATION_ENCLOSED           = 702, // Contents in the config buffer
  CONFIGURATION_RECON_SHORT        = 703,
  CONFIGURATION_RECON_INT          = 704,
  CONFIGURATION_RECON_FLOAT        = 705,
  CONFIGURATION_RECON_DOUBLE       = 706
};

/// Entity type interface
class EXPORTISMRMRD Entity
{
  public:

  virtual             ~Entity (){};

  virtual uint32_t    getVersion ()     const = 0;
  virtual EntityType  getEntityType ()  const = 0;
  virtual StorageType getStorageType () const = 0;
  virtual uint32_t    getStream ()      const = 0;

  virtual std::vector<unsigned char> serialize() = 0;
  virtual void deserialize(const std::vector<unsigned char>& buffer) = 0;
};

 
class EntityHeader 
: public Entity
{
  public:

  void setId   () { id = ISMRMRD_DATA_ID; };
  bool checkId () { return id == ISMRMRD_DATA_ID; };

  virtual uint32_t  getVersion () const { return version; }
  virtual uint32_t  getStream ()  const { return stream; }

  virtual EntityType  getEntityType ()  const
    { return static_cast<EntityType>(entity_type); }

  virtual StorageType getStorageType () const
    { return static_cast<StorageType>(storage_type); }

  virtual std::vector<unsigned char> serialize();
  virtual void deserialize(const std::vector<unsigned char>& buffer);

  std::string id;            /**< ISMRMRD data identity field */
  uint32_t    version;       /**< First unsigned int indicates the version */
  uint32_t    entity_type;   /**< Entity type code */
  uint32_t    storage_type;  /**< numeric type of each sample */
  uint32_t    stream;        /**< which stream this belongs to */
};

struct Manifest
{
  uint32_t             stream;
  ISMRMRD::EntityType  entity_type;
  ISMRMRD::StorageType storage_type;
  //uint32_t             descr_length;
  //std::vector<char>    description;
};

struct HandshakeHeader
{
  uint32_t version;       /**< First unsigned int indicates the version  */
  uint32_t entity_type;   /**< Entity type code */
  uint32_t storage_type;  /**< numeric type of each sample */
  uint32_t stream;        /**< which stream this belongs to */
};

class Handshake
: public Entity
{
  public:
                   Handshake ();
  void             setTimestamp (uint64_t timestamp_);
  uint64_t         getTimestamp () const;
  void             setClientName (std::string name);
  std::string      getClientName () const;
  uint32_t         setClientNameLength ();
  uint32_t         getClientNameLength () const;
  void             setConnectionStatus (ConnectionStatus status);
  ConnectionStatus getConnectionStatus () const;
  void             addManifestEntry (uint32_t             stream,
                                     ISMRMRD::EntityType  etype,
                                     ISMRMRD::StorageType stype);//,
                                    // std::string          description);
  uint32_t         getManifestSize () const;
  std::map<std::string, Manifest> getManifest () const;
  
  virtual uint32_t getVersion () const { return head_.version; }
  virtual uint32_t getStream ()  const { return head_.stream; }

  virtual EntityType getEntityType ()  const
    { return static_cast<EntityType>(head_.entity_type); }

  virtual StorageType getStorageType () const
    { return static_cast<StorageType>(head_.storage_type); }

  virtual std::vector<unsigned char> serialize();
  virtual void deserialize(const std::vector<unsigned char>& buffer);

  protected:

  HandshakeHeader   head_;
  uint64_t          timestamp_;   /**< Session start set by client */
  uint32_t          conn_status_; /**< Connection Status set by server */
  uint32_t          client_name_length_;
  uint32_t          manifest_size_;
  std::vector<char> client_name_;
  std::vector<Manifest> manifest_;
};

struct CommandHeader
{
  uint32_t version;       /**< First unsigned int indicates the version  */
  uint32_t entity_type;   /**< Entity type code */
  uint32_t storage_type;  /**< numeric type of each sample */
  uint32_t stream;        /**< which stream this belongs to */
};

class Command
: public Entity
{
  public:
                        Command ();
  CommandType           getCommandType () const;
  void                  setCommandType (CommandType command_type);
  uint32_t              getCommandId () const;
  void                  setCommandId (uint32_t command_id);
  ConfigurationType     getConfigType () const;
  void                  setConfigType (ConfigurationType config_type);
  std::string           getConfigFile () const;
  void                  setConfigFile (std::string config_file);
  std::vector<uint32_t> getConfigEntities () const;
  void                  setConfigEntities (std::vector<uint32_t> entities);
    
  virtual uint32_t getVersion () const { return head_.version; }
  virtual uint32_t getStream ()  const { return head_.stream; }

  virtual EntityType getEntityType ()  const
    { return static_cast<EntityType>(head_.entity_type); }

  virtual StorageType getStorageType () const
    { return static_cast<StorageType>(head_.storage_type); }

  virtual std::vector<unsigned char> serialize();
  virtual void deserialize(const std::vector<unsigned char>& buffer);

  protected:

  CommandHeader         head_;
  uint32_t              command_type_; /**< CommandType maps to CommandType enum  */
  uint32_t              command_id_;   /** ID number used to refer to the command */
  uint32_t              config_type_;  /**< maps to ConfigurationType enum */
  uint32_t              config_file_length_;  /**< Config file length */
  std::vector<char>     config_file_;  /** Either filename or actual config data */
  uint32_t              num_entities_;  /**< size of vector entities_ */
  std::vector<uint32_t> entities_;
};

struct ErrorReportHeader
{
  uint32_t version;       /**< First unsigned int indicates the version  */
  uint32_t entity_type;   /**< Entity type code */
  uint32_t storage_type;  /**< numeric type of each sample */
  uint32_t stream;        /**< which stream this belongs to */
};

class ErrorReport
: public Entity
{
  public:
                        ErrorReport ();
  ErrorType             getErrorType () const;
  void                  setErrorType (ErrorType error_type);
  CommandType           getErrorCommandType () const;
  void                  setErrorCommandType (CommandType command_type);
  uint32_t              getErrorCommandId () const;
  void                  setErrorCommandId (uint32_t command_id);
  EntityType            getErrorEntityType () const;
  void                  setErrorEntityType (EntityType entity_type);
  std::string           getErrorDescription () const;
  void                  setErrorDescription (std::string description);

  virtual uint32_t getVersion () const { return head_.version; }
  virtual uint32_t getStream ()  const { return head_.stream; }

  virtual EntityType getEntityType ()  const
    { return static_cast<EntityType>(head_.entity_type); }

  virtual StorageType getStorageType () const
    { return static_cast<StorageType>(head_.storage_type); }

  virtual std::vector<unsigned char> serialize();
  virtual void deserialize(const std::vector<unsigned char>& buffer);

  protected:

  ErrorReportHeader  head_;
  uint32_t           error_type_;         /**< maps to ErrorType enum  */
  uint32_t           error_command_type_; /**< maps to CommandType enum  */
  uint32_t           error_command_id_;   /** refers to the command id number*/
  uint32_t           error_entity_type_;  /**< maps to EntityType enum */
  uint32_t           description_length_;
  std::vector<char>  error_description_;
};

/** EncodingCounters keeps track of typical loop counters in MR experiment. */
struct EncodingCounters
{
  uint32_t kspace_encode_step_0;    /**< e.g. used for spectroscopic imaging */
  uint32_t kspace_encode_step_1;    /**< e.g. phase encoding line number */
  uint32_t kspace_encode_step_2;    /**< e.g. partition encoding number */
  uint32_t average;                 /**< e.g. signal average number */
  uint32_t slice;                   /**< e.g. imaging slice number */
  uint32_t contrast;                /**< e.g. echo number in multi-echo */
  uint32_t phase;                   /**< e.g. cardiac phase number */
  uint32_t repetition;              /**< e.g. dynamic number for dynamic scanning */
  uint32_t set;                     /**< e.g. flow encoding set */
  uint32_t segment;                 /**< e.g. segment number for segmented acquisition */
  uint32_t user[ISMRMRD_USER_INTS]; /**< Free user parameters */
};

/** Header for each MR acquisition. */
struct AcquisitionHeader
{
  uint32_t version;              /**< First unsigned int indicates the version  */
  uint32_t entity_type;          /**< Entity type code */
  uint32_t storage_type;         /**< numeric type of each sample */
  uint32_t stream;               /**< which stream this belongs to */
  uint64_t time_stamp;           /**< Experiment time stamp - in nano seconds */
  uint64_t flags;                /**< bit field with flags */
  uint32_t scan_counter;         /**< Current acquisition number in the measurement */
  uint32_t physiology_time_stamp[ISMRMRD_PHYS_STAMPS]; /**< Physiology time stamps,
                                                            e.g. ecg, breating, etc. */
  uint32_t number_of_samples;    /**< Number of samples acquired */
  uint32_t available_channels;   /**< Available coils */
  uint32_t active_channels;      /**< Active coils on current acquisiton */
  uint64_t channel_mask[ISMRMRD_CHANNEL_MASKS]; /**< Mask to indicate which channels are
                                                     active. Support for 1024 channels */
  uint32_t discard_pre;          /**< Samples to be discarded at the beginning of
                                      acquisition */
  uint32_t discard_post;         /**< Samples to be discarded at the end of
                                      acquisition */
  uint32_t center_sample;        /**< Sample at the center of k-space */
  uint32_t encoding_space_ref;   /**< Reference to an encoding space, typically only
                                      one per acquisition */
  uint32_t trajectory_dimensions; /**< Indicates the dimensionality of the trajectory
                                       vector (0 means no trajectory) */
  uint32_t dwell_time_ns;        /**< Time between samples in nano seconds,
                                      sampling BW */
  float position[3];             /**< Three-dimensional spatial offsets from isocenter */
  float read_dir[3];             /**< Directional cosines of the readout/frequency
                                      encoding */
  float phase_dir[3];            /**< Directional cosines of the phase */
  float slice_dir[3];            /**< Directional cosines of the slice direction */
  float patient_table_position[3];  /**< Patient table off-center */
  EncodingCounters idx;          /**< Encoding loop counters, see above */
  int32_t user_int[ISMRMRD_USER_INTS];     /**< Free user parameters */
  float user_float[ISMRMRD_USER_FLOATS];   /**< Free user parameters */
};

/** Header for each Image. */
struct ImageHeader
{
  uint32_t version;        /**< First unsigned int indicates the version */
  uint32_t entity_type;    /**< Entity type code */
  uint32_t storage_type;   /**< numeric type of each sample */
  uint32_t stream;         /**< which stream this belongs to */
  uint64_t time_stamp;     /**< Experiment time stamp - in nano seconds */
  uint64_t flags;          /**< bit field with flags */
  uint32_t matrix_size[3]; /**< Pixels in the 3 spatial dimensions */
  float field_of_view[3];  /**< Size (in mm) of the 3 spatial dimensions */
  uint32_t channels;       /**< Number of receive channels */
  float position[3];       /**< Three-dimensional spatial offsets from isocenter */
  float read_dir[3];       /**< Directional cosines of the readout/frequency encoding */
  float phase_dir[3];      /**< Directional cosines of the phase */
  float slice_dir[3];      /**< Directional cosines of the slice direction */
  float patient_table_position[3];        /**< Patient table off-center */
  uint32_t average;        /**< e.g. signal average number */
  uint32_t slice;          /**< e.g. imaging slice number */
  uint32_t contrast;       /**< e.g. echo number in multi-echo */
  uint32_t phase;          /**< e.g. cardiac phase number */
  uint32_t repetition;     /**< e.g. dynamic number for dynamic scanning */
  uint32_t set;            /**< e.g. flow encodning set */
  uint32_t physiology_time_stamp[ISMRMRD_PHYS_STAMPS]; /**< Physiology time stamps,
                                e.g. ecg, breathing, etc. */
  uint32_t image_type;     /**< e.g. magnitude, phase, complex, real, imag, etc. */
  uint32_t image_index;    /**< e.g. image number in series of images */
  uint32_t image_series_index;            /**< e.g. series number */
  int32_t user_int[ISMRMRD_USER_INTS];    /**< Free user parameters */
  float user_float[ISMRMRD_USER_FLOATS];  /**< Free user parameters */
  uint32_t attribute_string_len;          /**< Length of attributes string */
};

#pragma pack(pop) /* Restore old alignment */

/** @addtogroup cxxapi
 *  @{
 */

bool operator==(const AcquisitionHeader &h1, const AcquisitionHeader &h2);
bool operator==(const ImageHeader &h1, const ImageHeader &h2);

/// Allowed data types for Images and NDArrays
template <typename T> EXPORTISMRMRD StorageType get_storage_type();

/// Convenience class for flags
class EXPORTISMRMRD FlagBit
{
  public:

  FlagBit(unsigned short b)
  : bitmask_(0)
  {
    if (b > 0)
    {
      bitmask_ = 1;
      bitmask_ = (bitmask_ << (b - 1));
    }
  }

  bool isSet (const uint64_t &m) const
  {
    return ((m & bitmask_) > 0);
  }

  uint64_t bitmask_;
};

/// MR Acquisition type
template <typename T>
class EXPORTISMRMRD Acquisition
: public Entity
{
  public:
  Acquisition (uint32_t num_samples           = 0,
               uint32_t active_channels       = 1,
               uint32_t trajectory_dimensions = 0);

  void setStream(uint32_t);

  uint32_t getScanCounter() const;
  void setScanCounter(uint32_t);

  uint64_t getTimeStamp() const;
  void setTimeStamp(uint64_t);

  uint32_t getPhysiologyTimeStamp(int idx);
  void setPhysiologyTimeStamp(int idx, uint32_t ts);

  uint32_t getNumberOfSamples() const;
  void setNumberOfSamples(uint32_t);

  uint32_t getAvailableChannels() const;
  void setAvailableChannels(uint32_t);

  uint32_t getActiveChannels() const;
  void setActiveChannels(uint32_t);

  uint64_t getChannelMask(int idx);
  void setChannelMask(int idx, uint64_t mask);

  uint32_t getDiscardPre() const;
  void setDiscardPre(uint32_t);

  uint32_t getDiscardPost() const;
  void setDiscardPost(uint32_t);

  uint32_t getCenterSample() const;
  void setCenterSample(uint32_t);

  uint32_t getEncodingSpaceRef() const;
  void setEncodingSpaceRef(uint32_t);

  uint32_t getTrajectoryDimensions() const;
  void setTrajectoryDimensions(uint32_t);

  uint32_t getDwellTime_ns() const;
  void setDwellTime_ns(uint32_t);

  float getPositionX() const;
  float getPositionY() const;
  float getPositionZ() const;
  void setPosition(float, float, float);
  void setPositionX(float);
  void setPositionY(float);
  void setPositionZ(float);

  float getReadDirectionX() const;
  float getReadDirectionY() const;
  float getReadDirectionZ() const;
  void setReadDirection(float, float, float);
  void setReadDirectionX(float);
  void setReadDirectionY(float);
  void setReadDirectionZ(float);

  float getPhaseDirectionX() const;
  float getPhaseDirectionY() const;
  float getPhaseDirectionZ() const;
  void setPhaseDirection(float, float, float);
  void setPhaseDirectionX(float);
  void setPhaseDirectionY(float);
  void setPhaseDirectionZ(float);

  float getSliceDirectionX() const;
  float getSliceDirectionY() const;
  float getSliceDirectionZ() const;
  void setSliceDirection(float, float, float);
  void setSliceDirectionX(float);
  void setSliceDirectionY(float);
  void setSliceDirectionZ(float);

  float getPatientTablePositionX() const;
  float getPatientTablePositionY() const;
  float getPatientTablePositionZ() const;
  void setPatientTablePosition(float, float, float);
  void setPatientTablePositionX(float);
  void setPatientTablePositionY(float);
  void setPatientTablePositionZ(float);

  EncodingCounters &getEncodingCounters();
  const EncodingCounters &getEncodingCounters() const;
  void setEncodingCounters(const EncodingCounters &);

  int32_t getUserInt(int idx) const;
  void setUserInt(int idx, int32_t val);

  float getUserFloat(int idx) const;
  void setUserFloat(int idx, float val);

  // Sizes
  void resize (uint32_t num_samples,
               uint32_t active_channels = 1,
               uint32_t trajectory_dimensions = 0);

  size_t getNumberOfDataElements() const;
  size_t getNumberOfTrajElements() const;

  // Header, data and trajectory accessors
  AcquisitionHeader &getHead();
  const AcquisitionHeader &getHead() const;
  void setHead(const AcquisitionHeader &other);

  std::vector<std::complex<T> > &getData();
  const std::vector<std::complex<T> > &getData() const;
  void setData(const std::vector<std::complex<T> > &data);
  /** Returns a reference to a data point */
  std::complex<T> &at(uint32_t sample, uint32_t channel);

  const std::vector<float> &getTraj() const;
  void setTraj(const std::vector<float> &traj);
  /** Returns a reference to a trajectory point */
  float &trajAt(uint32_t dimension, uint32_t sample);

  // Flag methods
  uint64_t getFlags() const;
  void setFlags(uint64_t val);
  bool isFlagSet(uint64_t val) const;
  void setFlag(uint64_t val);
  void clearFlag(uint64_t val);
  void clearAllFlags();

  bool isFlagSet (const FlagBit &val) const { return isFlagSet(val.bitmask_); }
  void setFlag   (const FlagBit &val)       { setFlag(val.bitmask_); }
  void clearFlag (const FlagBit &val)       { clearFlag(val.bitmask_); }

  // Channel mask methods
  bool isChannelActive (uint32_t chan) const;
  void setChannelActive (uint32_t chan);
  void setChannelNotActive (uint32_t chan);
  void setAllChannelsNotActive ();


  // Functions inherited from Entity
  virtual uint32_t     getVersion ()     const { return head_.version; }
  virtual uint32_t     getStream ()      const { return head_.stream; }

  virtual EntityType   getEntityType ()  const
    { return static_cast<EntityType>(head_.entity_type); }

  virtual StorageType  getStorageType () const
    { return static_cast<StorageType>(head_.storage_type); }

  virtual std::vector<unsigned char> serialize();
  virtual void deserialize(const std::vector<unsigned char>& buffer);

  protected:

  void makeConsistent();

  AcquisitionHeader head_;
  std::vector<float> traj_;
  std::vector<std::complex< T > > data_;
};


/// MR Image type
template <typename T>
class EXPORTISMRMRD Image
: public Entity
{
  public:
  // Constructors
  Image (uint32_t matrix_size_x = 0,
         uint32_t matrix_size_y = 1,
         uint32_t matrix_size_z = 1,
         uint32_t channels = 1);

  // Image dimensions
  void resize (uint32_t matrix_size_x,
               uint32_t matrix_size_y,
               uint32_t matrix_size_z,
               uint32_t channels);

  //uint32_t getStream () const;
  void setStream (uint32_t stream);

  uint32_t getMatrixSizeX() const;
  uint32_t getMatrixSizeY() const;
  uint32_t getMatrixSizeZ() const;
  void setMatrixSizeX(uint32_t x);
  void setMatrixSizeY(uint32_t y);
  void setMatrixSizeZ(uint32_t z);

  uint32_t getNumberOfChannels() const;
  void setNumberOfChannels(uint32_t channels);

  // Field of view
  float getFieldOfViewX() const;
  float getFieldOfViewY() const;
  float getFieldOfViewZ() const;
  void setFieldOfView(float fov_x, float fov_y, float fov_z);
  void setFieldOfViewX(float f);
  void setFieldOfViewY(float f);
  void setFieldOfViewZ(float f);

  // Positions and orientations
  float getPositionX() const;
  float getPositionY() const;
  float getPositionZ() const;
  void setPosition(float x, float y, float z);
  void setPositionX(float x);
  void setPositionY(float y);
  void setPositionZ(float z);

  float getReadDirectionX() const;
  float getReadDirectionY() const;
  float getReadDirectionZ() const;
  void setReadDirection(float x, float y, float z);
  void setReadDirectionX(float x);
  void setReadDirectionY(float y);
  void setReadDirectionZ(float z);

  float getPhaseDirectionX() const;
  float getPhaseDirectionY() const;
  float getPhaseDirectionZ() const;
  void setPhaseDirection(float x, float y, float z);
  void setPhaseDirectionX(float x);
  void setPhaseDirectionY(float y);
  void setPhaseDirectionZ(float z);

  float getSliceDirectionX() const;
  float getSliceDirectionY() const;
  float getSliceDirectionZ() const;
  void setSliceDirection(float x, float y, float z);
  void setSliceDirectionX(float x);
  void setSliceDirectionY(float y);
  void setSliceDirectionZ(float z);

  float getPatientTablePositionX() const;
  float getPatientTablePositionY() const;
  float getPatientTablePositionZ() const;
  void setPatientTablePosition(float x, float y, float z);
  void setPatientTablePositionX(float x);
  void setPatientTablePositionY(float y);
  void setPatientTablePositionZ(float z);

  // Counters and labels
  uint32_t getAverage() const;
  void setAverage(uint32_t average);

  uint32_t getSlice() const;
  void setSlice(uint32_t slice);

  uint32_t getContrast() const;
  void setContrast(uint32_t contrast);

  uint32_t getPhase() const;
  void setPhase(uint32_t phase);

  uint32_t getRepetition() const;
  void setRepetition(uint32_t repetition);

  uint32_t getSet() const;
  void setSet(uint32_t set);

  uint64_t getTimeStamp() const;
  void setTimeStamp(uint64_t acquisition_time_stamp);

  uint32_t getPhysiologyTimeStamp(unsigned int idx) const;
  void setPhysiologyTimeStamp(unsigned int idx, uint32_t value);

  uint32_t getImageType() const;
  void setImageType(uint32_t image_type);

  uint32_t getImageIndex() const;
  void setImageIndex(uint32_t image_index);

  uint32_t getImageSeriesIndex() const;
  void setImageSeriesIndex(uint32_t image_series_index);

  // User parameters
  float getUserFloat(unsigned int index) const;
  void setUserFloat(unsigned int index, float value);

  int32_t getUserInt(unsigned int index) const;
  void setUserInt(unsigned int index, int32_t value);

  // Flags
  uint64_t getFlags() const;
  void setFlags(uint64_t flags);
  bool isFlagSet(uint64_t val) const;
  void setFlag(uint64_t val);
  void clearFlag(uint64_t val);
  void clearAllFlags();

  // Header
  ImageHeader &getHead();
  const ImageHeader &getHead() const;
  void setHead(const ImageHeader &head);

  // Attribute string
  void getAttributeString(std::string &attr) const;
  const std::string &getAttributeString() const;
  void setAttributeString(const std::string &attr);
  size_t getAttributeStringLength() const;

  size_t getNumberOfElements() const;

  std::vector<T> &getData();
  const std::vector<T> &getData() const;

  /** Returns a reference to the image data **/
  T &at(uint32_t x, uint32_t y = 0, uint32_t z = 0, uint32_t channel = 0);

  // Functions inherited from Entity
  virtual uint32_t  getVersion () const { return head_.version; }
  virtual uint32_t  getStream ()  const { return head_.stream; }

  virtual EntityType   getEntityType ()  const
    { return static_cast<EntityType>(head_.entity_type); }

  virtual StorageType  getStorageType () const
    { return static_cast<StorageType>(head_.storage_type); }

  virtual std::vector<unsigned char> serialize();
  virtual void deserialize(const std::vector<unsigned char>& buffer);

  protected:
  void makeConsistent();

  ImageHeader head_;
  std::string attribute_string_;
  std::vector<T> data_;
};

struct WaveformHeader
{
  uint32_t version;              /**< First unsigned int indicates the version  */
  uint32_t entity_type;          /**< Entity type code */
  uint32_t storage_type;         /**< numeric type of each sample */
  uint32_t stream;               /**< which stream this belongs to */
  uint64_t begin_time_stamp;     /**< Experiment time stamp - in nano seconds */
  uint64_t end_time_stamp;       /**< Experiment time stamp - in nano seconds */
  uint32_t dwell_time_ns;        /**< Time between samples in nano seconds */
  uint32_t number_of_samples;    /**< Number of samples acquired */
  int32_t  user_int[ISMRMRD_USER_INTS];     /**< Free user parameters */
  float    user_float[ISMRMRD_USER_FLOATS]; /**< Free user parameters */
};

bool operator== (const WaveformHeader &h1, const WaveformHeader &h2);

class Waveform
: public Entity
{
  public:

  Waveform (uint32_t num_samples = 0);

  void setStream (uint32_t);

  uint64_t getBeginTimeStamp() const;
  void setBeginTimeStamp (uint64_t);

  uint64_t getEndTimeStamp() const;
  void setEndTimeStamp (uint64_t);

  uint32_t getNumberOfSamples() const;
  void setNumberOfSamples (uint32_t);

  uint32_t getDwellTime_ns() const;
  void setDwellTime_ns (uint32_t);

  int32_t getUserInt(int idx) const;
  void setUserInt (int idx, int32_t val);

  float getUserFloat (int idx) const;
  void setUserFloat (int idx, float val);

  void resize (uint32_t num_samples);

  WaveformHeader &getHead();
  const WaveformHeader &getHead() const;
  void setHead (const WaveformHeader &other);

  std::vector<double> &getData();
  const std::vector<double> &getData() const;
  void setData (const std::vector<double> &data);

  double &at (uint32_t sample);

// Functions inherited from Entity
  virtual uint32_t     getVersion ()     const { return head_.version; }
  virtual uint32_t     getStream ()      const { return head_.stream; }

  virtual EntityType   getEntityType ()  const
    { return static_cast<EntityType>(head_.entity_type); }

  virtual StorageType  getStorageType () const
    { return static_cast<StorageType>(head_.storage_type); }

  virtual std::vector<unsigned char> serialize();
  virtual void deserialize(const std::vector<unsigned char>& buffer);

  protected:

  WaveformHeader head_;
  std::vector<double> data_;
};


/// N-Dimensional array type
template <typename T>
class EXPORTISMRMRD NDArray
{
  public:

  // Constructors, destructor and copy
  NDArray();
  NDArray(const std::vector<size_t> &dims);

  // Accessors and mutators
  uint32_t getVersion() const;
  StorageType getStorageType() const;

  uint32_t getNDim() const;
  const  std::vector<size_t>& getDims();

  void   resize (const std::vector<size_t> &dims);
  size_t getNumberOfElements() const;

  std::vector<T> &getData();
  const std::vector<T> &getData() const;

  // Returns a reference to the image data
  T &at (uint32_t x,
         uint32_t y = 0,
         uint32_t z = 0,
         uint32_t w = 0,
         uint32_t n = 0,
         uint32_t m = 0,
         uint32_t l = 0);

  protected:

  void makeConsistent();

  uint32_t version_;
  std::vector<size_t> dims_;
  std::vector<T> data_;
};

// Rotations and Quaternions

/// Calculates the determinant of the matrix and return the sign
EXPORTISMRMRD int sign_of_directions (float read_dir[3],
                                      float phase_dir[3],
                                      float slice_dir[3]);

/// Creates a normalized quaternion from a 3x3 rotation matrix
EXPORTISMRMRD void directions_to_quaternion (float read_dir[3],
                                             float phase_dir[3],
                                             float slice_dir[3],
                                             float quat[4]);

/// Converts a quaternion of the form | a b c d | to a 3x3 rotation matrix
EXPORTISMRMRD void quaternion_to_directions (float quat[4],
                                             float read_dir[3],
                                             float phase_dir[3],
                                             float slice_dir[3]);

/** @} */

} // namespace ISMRMRD

#endif /* ISMRMRD_H */

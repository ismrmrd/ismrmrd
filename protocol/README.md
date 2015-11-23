ISMRM Raw Data Communication Protocol
======================================

This document describes the communication model and protocol that should be used by client/server applications that support the ISMRM Raw Data (ISMRMRD) standard. The communication protocol described in this document is independent from the file storage (currently using HDF5 file containers) that the C++ API uses in application that interact with ISMRMRD files directly.

Network packages and framing
----------------------------

All ISMRMRD packages transmitted on the network are framed network packages. The layout of an ISMRMRD data frame is as follows:

```
----
uint64: frame_size
|
|
.
. //frame_size bytes
.
|
|
----
```

i.e., each frame consists of a 64-bit unsigned integerer that specifies the length of the packge (excluding the 64-bit integer itself). For example, if the contents a data frame is 100 bytes long, it would be written onto the wire as first a 64-bit integer containing the value 100 followed by 100 bytes of data. Consequently the data frame including the envelope would be 108 bytes long.  

Data package headers
--------------------
Each data frame had a common header that follows the frame size. It consists of four (4) 32-bit integers:

```
struct ISMRMRDEntity
{
    uint32_t version;
    uint32_t entity_type;
    uint32_t storage_type;
    uint32_t stream;
};

```

Consequently, the shortest possible ISMRMRD data frame is:

```
uint64_t frame_size;
uint32_t version;
uint32_t entity_type;
uint32_t storage_type;
uint32_t stream;
```
Or a total of 24 bytes. The `version` field indicated the major version of the ISMRMRD standard (current version is 2), the `entity_type` indicates which package type this is. This can be one of the following data types:
```
enum EntityType {
    ISMRMRD_MRACQUISITION = 1,
    ISMRMRD_WAVEFORM = 2,
    ISMRMRD_IMAGE = 3,
    ISMRMRD_XML_HEADER = 4,
    ISMRMRD_RECON_SPECIFICATION = 5,
    ISMRMRD_RECON_CONFIGURATION = 6,
    ISMRMRD_SYSTEM_INFORMATION = 7,
    ISMRMRD_START_RECON = 8,
    ISMRMRD_END_RECON = 9,
    ISMRMRD_ERROR = 10,
    ISMRMRD_BLOB = 11,
    ISMRMRD_OTHER = 12
};
Which correspond directly to classes defined in `ismrmrd.h`. Some of these entity types have associated data storage (e.g. raw data samples) which can be stored in different formats defined by the `storage_type` field, which can have a value defined by:
```
enum StorageType {
    ISMRMRD_USHORT   = 1, /**< corresponds to uint16_t */
    ISMRMRD_SHORT    = 2, /**< corresponds to int16_t */
    ISMRMRD_UINT     = 3, /**< corresponds to uint32_t */
    ISMRMRD_INT      = 4, /**< corresponds to int32_t */
    ISMRMRD_FLOAT    = 5, /**< corresponds to float */
    ISMRMRD_DOUBLE   = 6, /**< corresponds to double */
    ISMRMRD_CXFLOAT  = 7, /**< corresponds to complex float */
    ISMRMRD_CXDOUBLE = 8  /**< corresponds to complex double */
};
```
The final field `stream` indicates which of multiple streams that the present entity belongs to. The ISMRMRD protocol is a multiplexed streaming protocol; on the wire the packages (or frames) are layed out consequtively but conceptually they correspond to multiple consecutive streams. The streams are equivalent to channels and a numbered consequetively from zero (0). The follow stream numbers are reserved by the ISMRMRD standard:
```
0: Handshaking and reconstruction control
1: XML Header
2: ISMRM Acquisition Raw Data
```


```
An application reading packages in ISMRMRD format would be looping through the following pseudocode:

```

while (true) {
      uint64_t frame_size;
      stream.read(&frame_size, sizeof(uint64_t));
      char* buffer = new char[frame_size];
      stream.read(buffer, framesize);
      ISMRMRDEntity* e = reinterpret_cast<ISMRMRD_Entity*>(buffer);
      switch (e.enity_type) {
      case 
      }


```
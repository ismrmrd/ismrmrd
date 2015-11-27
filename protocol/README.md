ISMRM Raw Data Communication Protocol
======================================

This document describes the communication model and protocol that should be used by client/server applications that support the ISMRM Raw Data (ISMRMRD) standard. The communication protocol described in this document is independent from the file storage (currently using HDF5 file containers) that the C++ API uses in applications that interact with ISMRMRD files directly.

Client/Server communication
---------------------------
The client/server communication is intended to be done with regular tcp/ip sockets with continuous bidirectional communication. The recommended application configuration would be as outlined below:

```
      CLIENT                                              SERVER
|----------------------|       TCP/IP SOCKET        |----------------------|
|    Q -> write_thread |--------------------------->| read_thread -> proc  |
|                      |                            |                      |
|                      |                            |                      |
| proc  <- read_thread |<---------------------------| write_thread <- Q    |
|----------------------|                            |----------------------|
```
The role of the `CLIENT` is to send data (possibly from a file or acquisition system) to the `SERVER`, where it is accumulated and processed (by `proc` function in above diagram) and eventually reconstruction output is placed on a queue (`Q`) where an independent thread returns it to the client. The client most commonly will have independent threads for reading and writing too. Except for the initial connection, both client and server can send and receive the same types of packages. However, in a typical application the packages sent and received by client and server would vary, e.g. client sends raw data samples and receives images whereas the server receives raw data samples and sends images. 


Network packages and framing
----------------------------

All ISMRMRD packages transmitted on the network are framed network packages. The layout of an ISMRMRD data frame is as follows:

```
uint64: frame_size //Little endian byte order
----
|
|
.
. //frame_size bytes
.
|
|
----
```

i.e., each frame consists of a 64-bit unsigned integer (little endian) that specifies the length of the package (excluding the 64-bit integer itself). For example, if the contents of a data frame is 100 bytes long, it would be written onto the wire as first a 64-bit integer containing the value 100 followed by 100 bytes of data. Consequently the data frame including the envelope would be 108 bytes long.  

Data package headers
--------------------
Each data frame had a common header that follows the frame size. It consists of four (4) 32-bit integers:

```
class Entity
{
    uint32_t version;
    uint32_t entity_type;
    uint32_t storage_type;
    uint32_t stream;
    
    virtual void deserialize(std::vector<char>& buffer) = 0;
    virtual std::vector<char> serialize() = 0;
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
    ISMRMRD_HANDSHAKE       = 0, /**< first package sent                   */
    ISMRMRD_COMMAND         = 1, /**< commands used to control recon system */
    ISMRMRD_MRACQUISITION   = 2, /**< MR raw data                          */
    ISMRMRD_WAVEFORM        = 3, /**< Gradient, physiology, etc. waveform  */
    ISMRMRD_IMAGE           = 4, /**< Reconstructed image                  */
    ISMRMRD_XML_HEADER      = 5, /**< The XML header describing the data   */
    ISMRMRD_ERROR           = 6, /**< Something went wrong                 */
    ISMRMRD_BLOB            = 7  /**< Some binary object, with description */
};
```

Which correspond directly to classes defined in `ismrmrd.h`. Some of these entity types have associated data storage (e.g. raw data samples) which can be stored in different formats defined by the `storage_type` field, which can have a value defined by:

```
enum StorageType {
    ISMRMRD_CHAR     = 0, /**< corresponds to char           */
    ISMRMRD_USHORT   = 1, /**< corresponds to uint16_t       */
    ISMRMRD_SHORT    = 2, /**< corresponds to int16_t        */
    ISMRMRD_UINT     = 3, /**< corresponds to uint32_t       */
    ISMRMRD_INT      = 4, /**< corresponds to int32_t        */
    ISMRMRD_UINT64   = 5, /**< corresponds to uint64_t       */
    ISMRMRD_INT64    = 6, /**< corresponds to int64_t       */    
    ISMRMRD_FLOAT    = 7, /**< corresponds to float          */
    ISMRMRD_DOUBLE   = 8, /**< corresponds to double         */
    ISMRMRD_CXFLOAT  = 9, /**< corresponds to complex float  */
    ISMRMRD_CXDOUBLE = 10 /**< corresponds to complex double */
};
```
The final field `stream` indicates which of multiple streams that the present entity belongs to. The ISMRMRD protocol is a multiplexed streaming protocol; on the wire the packages (or frames) are laid out consecutively but conceptually they correspond to multiple concurrent streams. The streams are equivalent to channels and are numbered consecutively from zero (0). It is required that all entitys in a given stream are of the same type. An application that receives multiple entity types in the same stream should throw an error. The following stream numbers are reserved by the ISMRMRD standard:

```
0: ISMRM Acquisition Raw Data
1: XML Header
.
.
.
65536+: Handshaking and reconstruction control
```
In general stream numbers below 65535 are reserved for streams of data that would correspond to actual data stored in an ISMRMRD file (with stream 0 being typical MRI raw data) and streams above 65535 are meant to control handshaking and control messages that would only be used for client/server communication. 

An application communication using the ISMRMRD protocol would would typically have two threads reading and writing packages using the following (C++-ish) code:

```
Entity EntityFromBuffer(std::vector<char>& buffer)
{
	Enity* e_ptr = reinterpret_cast<Entity*>(&buffer[0]);
	Entity e = EntityFactory::create_entity(e_ptr->entity_type,
	                                        e_ptr->storage_type);
	                                        
	e.deserialize(buffer);                                        
	return e;                                                
}

std::thread reader([](){
	while (true)
	{
	      uint64_t frame_size;
	      stream.read(&frame_size, sizeof(uint64_t));
	      std::vector<char> buffer(frame_size);
	      stream.read(&buffer[0], frame_size);
	      Entity ent = EntityFromBuffer(buffer);
		      
		   //Do something with entity based on stream and type, etc. 
	}
});

std::thread writer([](){
	while (true)
	{
		   Entity e = Q.get();
		   std::vector<char> buffer;
		   e.serialize(buffer);
		   uint64_t frame_size = buffer.size();
		   stream.write(&frame_size,sizeof(uint64_t));
	      stream.write(&buffer[0], frame_size);
	}
});

writer.join()
reader.join()
```

A typical ISMRMRD reconstruction session (in interaction diagram form) would look like:

```

         +--------+               +--------+
         | CLIENT |               | SERVER |
         +--------+               +--------+
              |                        |
              |                        |
      CONNECT +------------------------> ACCEPT
              |                        |
              |      HANDSHAKE         |
              +------------------------>
              |                        |
              |      HANDSHAKE         |
              <------------------------+
              |                        |
              |                        |
        R +-- |        ENTITY          | --+ R
        E |   +------------------------>   | E
        P |   |                        |   | P
        E |   |        ENTITY          |   | E
        A |   <------------------------+   | A
        T +-- |                        | --+ T
              |      COMMAND: STOP     |
              +------------------------>
              |                        |
        WAIT  |                        | RECON WORK
              |      COMMAND: DONE     |
              <------------------------+
              |                        |
              |                        |
        CLOSE +------------------------> CLEAN UP
              |                        |
              |                        |
              |                        |
              |                        |
         +--------+                +--------+
         | CLIENT |                | SERVER |
         +--------+                +--------+

```
Please note that during the period when client and server are exchanging data entities, there is not a 1:1 relationship between ingoing and outgoing packages. In fact, it would be possible to create a reconstruction program that received no data but returned data (a simulation) or vice versa in the case of a calibration measurement that does not return images but stores calibration data in the reconstruction system. s
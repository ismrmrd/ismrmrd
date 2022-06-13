# MRD Data Primatives

The Magnetic Resonance Data (MRD) format consists of 3 primative data types, raw k-space acquisition data, image data, and waveform data.

## MRD Acquisition Readouts
Raw k-space data is stored in MRD format as individual readout acquisitions.  Each readout contains the complex raw data for all channels, a fixed AcquisitionHeader, and optionally corresponding k-space trajectory information.  Most datasets will be comprised of many acquisitions, each stored indvidiually with its own AcquisitionHeader, optional trajectory, and raw data.

| AcquisitionHeader (required)   | Trajectory (optional)  | Raw Data (required) |
| --                             | --                     | --                  |
| 340 bytes                      | number_of_samples * trajectory_dimensions * 4 bytes | number_of_channels * number_of_samples * 8 bytes |
| mixed                          | float                  | (complex) float     |

The trajectory information is omitted if the ``trajectory_dimensions`` field in the AcquisitionHeader is set to 0.

### MRD AcquisitionHeader
An MRD AcquisitionHeader accompanies each readout.  It is of a fixed size and thus fields cannot be added, removed, or otherwise repurposed.  It contains the following information:
| Element Name           | Format       | Interpretation                                      |
| --                     | --           | --                                                  |
| version                | uint16       | Major version number (currently 1)                  |
| flags                  | uint64       | A bit mask of common attributes applicable to individual acquisition readouts |
| measurement_uid        | uint32       | Unique ID corresponding to the readout              |
| scan_counter           | uint32       | Zero-indexed incrementing counter for readouts      |
| acquisition_time_stamp | uint32       | Clock time stamp (e.g. milliseconds since midnight) |
| physiology_time_stamp  | uint32 (x3)  | Time stamps relative to physiological triggering, e.g. ECG, pulse oximetry, respiratory. Multiplicity defined by ISMRMRD_PHYS_STAMPS (currently 3) |
| number_of_samples      | uint16       | Number of digitized readout samples                 |
| available_channels     | uint16       | Number of possible receiver coils (channels)        |
| active_channels        | uint16       | Number of active receiver coils                     |
| channel_mask           | uint64 (x16) | Bit mask indicating active coils (64\*16 = 1024 bits) |
| discard_pre            | uint16       | Number of readout samples to be discarded at the beginning (e.g. if the ADC is active during gradient events) |
| discard_post           | uint16       | Number of readout samples to be discarded at the end (e.g. if the ADC is active during gradient events) |
| center_sample          | uint16       | Index of the readout sample corresponing to k-space center (zero indexed) |
| encoding_space_ref     | uint16       | Indexed reference to the encoding spaces enumerated in the MRD (xml) header |
| trajectory_dimensions  | uint16       | Dimensionality of the k-space trajectory vector (e.g. 2 for 2D radial (k<sub>x</sub>, k<sub>y</sub>), 0 for no trajectory data) |
| sample_time_us         | float (32 bit) | Readout bandwidth, as time between samples in microseconds |
| position               | float (32 bit) (x3) | Center of the excited volume, in (left, posterior, superior) (LPS) coordinates relative to isocenter in millimeters |
| read_dir               | float (32 bit) (x3) | Directional cosine of readout/frequency encoding |
| phase_dir              | float (32 bit) (x3) | Directional cosine of phase encoding (2D) |
| slice_dir              | float (32 bit) (x3) | Directional cosine of slice normal, i.e. cross-product of read_dir and phase_dir |
| patient_table_position | float (32 bit) (x3) | Offset position of the patient table, in LPS coordinates |
| idx                    | uint16 (x17)        | Encoding loop counters, as defined below     |
| user_int               |  int32 (x8)         | User-defined integer parameters, multiplicity defined by ISMRMRD_USER_INTS (currently 8) |
| user_float             | float (32 bit) (x8) | User-defined float parameters, multiplicity defined by ISMRMRD_USER_FLOATS (currently 8) | 

### MRD EncodingCounters
| Element Name           | Format       | Interpretation                                      |
| --                     | --           | --                                                  |
| kspace_encode_step_1   | uint16       | Phase encoding line                                 |
| kspace_encode_step_2   | uint16       | Partition encoding                                  |
| average                | uint16       | Signal average                                      |
| slice                  | uint16       | Slice number (multi-slice 2D)                       |
| contrast               | uint16       | Echo number in multi-echo                           |
| phase                  | uint16       | Cardiac phase                                       |
| repetition             | uint16       | Counter in repeated/dynamic acquisitions            |
| set                    | uint16       | Sets of different preparation, e.g. flow encoding, diffusion weighting |
| segment                | uint16       | Counter for segmented acquisitions                  |
| user                   | uint16 (x8)  | User defined counters, multiplicity defined by ISMRMRD_USER_INTS (currently 8) |

### MRD AcquisitionFlags
    ISMRMRD_ACQ_FIRST_IN_ENCODE_STEP1               =  1,
    ISMRMRD_ACQ_LAST_IN_ENCODE_STEP1                =  2,
    ISMRMRD_ACQ_FIRST_IN_ENCODE_STEP2               =  3,
    ISMRMRD_ACQ_LAST_IN_ENCODE_STEP2                =  4,
    ISMRMRD_ACQ_FIRST_IN_AVERAGE                    =  5,
    ISMRMRD_ACQ_LAST_IN_AVERAGE                     =  6,
    ISMRMRD_ACQ_FIRST_IN_SLICE                      =  7,
    ISMRMRD_ACQ_LAST_IN_SLICE                       =  8,
    ISMRMRD_ACQ_FIRST_IN_CONTRAST                   =  9,
    ISMRMRD_ACQ_LAST_IN_CONTRAST                    = 10,
    ISMRMRD_ACQ_FIRST_IN_PHASE                      = 11,
    ISMRMRD_ACQ_LAST_IN_PHASE                       = 12,
    ISMRMRD_ACQ_FIRST_IN_REPETITION                 = 13,
    ISMRMRD_ACQ_LAST_IN_REPETITION                  = 14,
    ISMRMRD_ACQ_FIRST_IN_SET                        = 15,
    ISMRMRD_ACQ_LAST_IN_SET                         = 16,
    ISMRMRD_ACQ_FIRST_IN_SEGMENT                    = 17,
    ISMRMRD_ACQ_LAST_IN_SEGMENT                     = 18,
    ISMRMRD_ACQ_IS_NOISE_MEASUREMENT                = 19,
    ISMRMRD_ACQ_IS_PARALLEL_CALIBRATION             = 20,
    ISMRMRD_ACQ_IS_PARALLEL_CALIBRATION_AND_IMAGING = 21,
    ISMRMRD_ACQ_IS_REVERSE                          = 22,
    ISMRMRD_ACQ_IS_NAVIGATION_DATA                  = 23,
    ISMRMRD_ACQ_IS_PHASECORR_DATA                   = 24,
    ISMRMRD_ACQ_LAST_IN_MEASUREMENT                 = 25,
    ISMRMRD_ACQ_IS_HPFEEDBACK_DATA                  = 26,
    ISMRMRD_ACQ_IS_DUMMYSCAN_DATA                   = 27,
    ISMRMRD_ACQ_IS_RTFEEDBACK_DATA                  = 28,
    ISMRMRD_ACQ_IS_SURFACECOILCORRECTIONSCAN_DATA   = 29,


    ISMRMRD_ACQ_COMPRESSION1                        = 53,
    ISMRMRD_ACQ_COMPRESSION2                        = 54,
    ISMRMRD_ACQ_COMPRESSION3                        = 55,
    ISMRMRD_ACQ_COMPRESSION4                        = 56,
    ISMRMRD_ACQ_USER1                               = 57,
    ISMRMRD_ACQ_USER2                               = 58,
    ISMRMRD_ACQ_USER3                               = 59,
    ISMRMRD_ACQ_USER4                               = 60,
    ISMRMRD_ACQ_USER5                               = 61,
    ISMRMRD_ACQ_USER6                               = 62,
    ISMRMRD_ACQ_USER7                               = 63,
    ISMRMRD_ACQ_USER8                               = 64

## MRD Images
MRD image data is consists of 3 parts, a fixed image header, an extensible set of image attributes, and the image data itself.

### MRD ImageHeader
| Element Name           | Format              | Interpretation                                      |
| --                     | --                  | --                                                  |
| version                | uint16              | Major version number (currently 1)                  |
| data_type              | uint16              | Data type of the image data, e.g. short, float, complex float, etc., as defined in [MRD Image Data Types](#MRD-Image-Data-Types) |
| flags                  | uint64              | A bit mask of common attributes applicable to individual images |
| measurement_uid        | uint32              | Unique ID corresponding to the image                |
| matrix_size            | uint16 (x3)         | Number of pixels in each of the 3 dimensions in the image |
| field_of_view          | float (32 bit) (x3) | Physical size (in mm) in each of the 3 dimensions in the image |
| channels               | uint16              | Number of receiver channels in image data (stored in the 4th dimension) |
| position               | float (32 bit) (x3) | Center of the excited volume, in (left, posterior, superior) (LPS) coordinates relative to isocenter in millimeters.  NB this is different than DICOM's ImageOrientationPatient, which defines the center of the first (typically top-left) voxel. |
| read_dir               | float (32 bit) (x3) | Directional cosine of readout/frequency encoding.  If the image is [flipped or rotated to bring them into standard DICOM orientation](http://gdcm.sourceforge.net/wiki/index.php/Orientation), ***this field still corresponds to the acquisition readout/frequency direction***, but the ``ImageRowDir`` must be set in the MetaAttributes. |
| phase_dir              | float (32 bit) (x3) | Directional cosine of phase encoding (2D).  If the image is [flipped or rotated to bring them into standard DICOM orientation](http://gdcm.sourceforge.net/wiki/index.php/Orientation), ***this field still corresponds to the 2D phase encoding direction***, but the ``ImageColumnDir`` must be set in the MetaAttributes. |
| slice_dir              | float (32 bit) (x3) | For 3D data, the directional cosine of 3D phase encoding direction.  For 3D data, the slice normal, i.e. cross-product of ``read_dir`` and ``phase_dir``.  If the image is [flipped or rotated to bring them into standard DICOM orientation](http://gdcm.sourceforge.net/wiki/index.php/Orientation), ***this field still corresponds to the 3D phase encoding direction***, but the ``ImageSliceDir`` must be set in the MetaAttributes. |
| patient_table_position | float (32 bit) (x3) | Offset position of the patient table, in LPS coordinates |
| average                | uint16              | Signal average                                      |
| slice                  | uint16              | Slice number (multi-slice 2D)                       |
| contrast               | uint16              | Echo number in multi-echo                           |
| phase                  | uint16              | Cardiac phase                                       |
| repetition             | uint16              | Counter in repeated/dynamic acquisitions            |
| set                    | uint16              | Sets of different preparation, e.g. flow encoding, diffusion weighting |
| acquisition_time_stamp | uint32              | Clock time stamp (e.g. milliseconds since midnight) |
| physiology_time_stamp  | uint32 (x3)         | Time stamps relative to physiological triggering, e.g. ECG, pulse oximetry, respiratory. Multiplicity defined by ISMRMRD_PHYS_STAMPS (currently 3) |
| image_type             | uint16              | Interpretation type of the image, e.g. magnitude, phase, as defined in [MRD Image Types](#MRD-Image-Types)   |
| image_index            | uint16              | Image index number within a series of images, corresponding to DICOM InstanceNumber (0020,0013)              |
| image_series_index     | uint16              | Series index, used to separate images into different series, corresponding to DICOM SeriesNumber (0020,0011) |
| user_int               |  int32 (x8)         | User-defined integer parameters, multiplicity defined by MRD_USER_INTS (currently 8) |
| user_float             | float (32 bit) (x8) | User-defined float parameters, multiplicity defined by MRD_USER_FLOATS (currently 8) |
| attribute_string_len   | uint32              | Length of serialized MetaAttributes text |

### MRD Image Data Types
| Value | Name         | Size     | Interpretation                                            |
| --    | --           | --       | --                                                        |
| 1     | MRD_USHORT   |  2 bytes | Unsigned short integer, equivalent to C++ uint16_t        |
| 2     | MRD_SHORT    |  2 bytes | Signed short integer, equivalent to C++ int16_t           |
| 3     | MRD_UINT     |  4 bytes | Unsigned integer, equivalent to C++ uint32_t              |
| 4     | MRD_INT      |  4 bytes | Signed integer, equivalent to C++ int32_t                 |
| 5     | MRD_FLOAT    |  4 bytes | Single precision floating point, equivalent to C++ float  |
| 6     | MRD_DOUBLE   |  8 bytes | Double precision floating point, equivalent to C++ double |
| 7     | MRD_CXFLOAT  | 16 bytes | Complex-valued single precision floating point, equivalent to C++ ``std::complex<float>``, where the first value is the real part and the second value is imaginary part |
| 8     | MRD_CXDOUBLE | 32 bytes | Complex-valued double precision floating point, equivalent to C++ ``std::complex<double>``, where the first value is the real part and the second value is imaginary part |

### MRD Image Types
| Value | Name                 | Interpretation                 |
| --    | --                   | --                             |
| 1     | MRD_IMTYPE_MAGNITUDE | Magnitude image data           |
| 2     | MRD_IMTYPE_PHASE     | Phase image data               |
| 3     | MRD_IMTYPE_REAL      | Real component image data      |
| 4     | MRD_IMTYPE_IMAG      | Imaginary component image data |
| 5     | MRD_IMTYPE_COMPLEX   | Complex-valued image data corresponding to an ImageHeader ``data_type`` of ``MRD_CXFLOAT`` or ``MRD_CXDOUBLE`` |
| 6     | MRD_IMTYPE_RGB       | TrueColor image data, where ImageHeader ``data_type`` is ``MRD_USHORT`` and ``channels`` is 3, representing the red, green, and blue channels of the RGB image and image data values are in the range 0-255 (8-bit color depth) |

# MRD Streaming Protocol
## MRD Data Analysis Session Protocol
The MR Data (MRD) streaming protocol describes the communication of data (k-space, image, or waveform) between a client and a server pair.  It consists of a series of [message types](#MRD-Streaming-Message-Types) that are sent through a TCP/IP socket in sessions with the protocol defined as follow:
1. The server is started and listens for incoming connections on a designated TCP port (9002 by default).
1. A client initiates a session by connecting to the TCP port above.
1. The client sends a configuration message to indicate analysis that should be performed by the server.  The message may be either (but not both):
    - [MRD_MESSAGE_CONFIG_FILE](#ID-1-MRD_MESSAGE_CONFIG_FILE), corresponding to the name of a config file that exists on the server
    - [MRD_MESSAGE_CONFIG_TEXT](#ID-2-MRD_MESSAGE_CONFIG_TEXT), configuration parameters for the server, in XML text format.
1. The client sends the MRD acquisition parameter header, [MRD_MESSAGE_PARAMETER_HEADER](#ID-3-MRD_MESSAGE_PARAMETER_HEADER), containing information pertaining to the entire acquisition.  This information is sent as XML formatted text that conforms to a standardized [ismrmrd.xsd schema](https://github.com/ismrmrd/ismrmrd/blob/master/schema/ismrmrd.xsd).  This message is not sent for the special case of dependency queries.
1. The client sends raw k-space, image, or waveform data.  Not all types of data may be sent, depending on the analysis pipeline to be performed.  For example, image processing pipelines may not contain k-space or waveform data.  Data of each type must sent in order by acquisition time, but the order between messages of different types is not guaranteed.  For eample, a waveform message corresponding to a specific time may be sent before or after the raw k-space data from that time.  The 3 data types are:
    - [MRD_MESSAGE_ISMRMRD_ACQUISITION](#ID-1008-MRD_MESSAGE_ISMRMRD_ACQUISITION): Data from a single k-space readout, including a [fixed raw data header](https://ismrmrd.github.io/apidocs/1.4.2/struct_i_s_m_r_m_r_d_1_1_i_s_m_r_m_r_d___acquisition_header.html) and optional k-space trajectory information.
    - [MRD_MESSAGE_ISMRMRD_IMAGE](#ID-1022-MRD_MESSAGE_ISMRMRD_IMAGE): Image data as a 2D or 3D array, including both a [fixed image data header](https://ismrmrd.github.io/apidocs/1.4.2/struct_i_s_m_r_m_r_d_1_1_i_s_m_r_m_r_d___image_header.html) and a [flexible image attribute header](https://ismrmrd.github.io/apidocs/1.4.2/class_i_s_m_r_m_r_d_1_1_meta_container.html) formatted as an XML text string.
    - [MRD_MESSAGE_ISMRMRD_WAVEFORM](#ID-1026-MRD_MESSAGE_ISMRMRD_WAVEFORM): Waveform data such as physiological monitoring (ECG, pulse oximeter, respiratory motion) including a [fixed waveform data header](https://ismrmrd.github.io/apidocs/1.4.2/struct_i_s_m_r_m_r_d_1_1_i_s_m_r_m_r_d___waveform_header.html).
1. At any point after a config message is received, the server may send back raw k-space, image, or waveform data as described above.
1. At any point after a config message is sent, either the client or server may send a [MRD_MESSAGE_TEXT](#ID-5-MRD_MESSAGE_TEXT), containing informational text for the other party.  This text may provide logging information about the status of the analysis or client, but is optional and should not be used for workflow control.
1. When all data has been sent, the client sends [MRD_MESSAGE_CLOSE](#ID-4-MRD_MESSAGE_CLOSE) to indicate that no further data will be sent by the client.
1. When the server has sent all data from its side, it also sends [MRD_MESSAGE_CLOSE](#ID-4-MRD_MESSAGE_CLOSE).
1. The TCP session may be closed by either side and the MRD streaming session is complete.

```{mermaid}

sequenceDiagram
    autonumber
    rect rgba(0, 255, 0, 0.05)
        Note over Client,Server: Startup
        Client->>+Server: Connect socket (TCP)
        Client-->>Server: Configuration information (xml text or filename)
        Client-->>Server: MRD header (xml text)
    end

    rect rgba(0, 0, 255, 0.05)
        Note over Client,Server: Data transfer
        par
            loop Sending data
                Client-->>+Server: k-space, image, and waveform data
            end
        and
            loop Receiving data
                Server-->>-Client: k-space, image, and waveform data
            end
        end
    end

    rect rgba(255, 0, 0, 0.05)
        Note over Client,Server: Shutdown
        par
            Client->>Server: Session complete message
            Server->>-Client: Session complete message
        end
        Server->Client: TCP session closed by either side
    end
```

## MRD Dependency Query Protocol
It may be necessary for a client to query a server to determine if dependent data (e.g. noise data, pre-scan normalize, etc.) has been processed.  This is handled through a dedicated MRD session, following the sequence of events below:
1. The server is started and listens for incoming connections on a designated TCP port (9002 by default).
1. A client initiates a session by connecting to the TCP port above.
1. The client sends a specialized configuration message of type [MRD_MESSAGE_CONFIG_TEXT](#ID-2-MRD_MESSAGE_CONFIG_TEXT) specifying the dependent measurement to be queried in the ``noise_file`` value property:
    ```xml
    <?xml version="1.0" encoding="UTF-8"?>
          <gadgetronStreamConfiguration xsi:schemaLocation=http://gadgetron.sf.net/gadgetron gadgetron.xsd
            xmlns=http://gadgetron.sf.net/gadgetron
          xmlns:xsi=http://www.w3.org/2001/XMLSchema-instance>

        <writer>
          <slot>1019</slot>
          <dll>gadgetron_mricore</dll>
          <classname>DependencyQueryWriter</classname>
        </writer>

        <gadget>
          <name>NoiseSummary</name>
          <dll>gadgetron_mricore</dll>
          <classname>NoiseSummaryGadget</classname>

          <property>
             <name>noise_file</name>
             <value>dependency_file_id</value>
          </property>
        </gadget>

    </gadgetronStreamConfiguration>
    ```
1. The client sends [MRD_MESSAGE_CLOSE](#ID-4-MRD_MESSAGE_CLOSE) to indicate that no further data will be sent by the client.
1. The server replies with a [MRD_MESSAGE_DEPENDENCY_QUERY_RESPONSE](#ID-1019-MRD_MESSAGE_DEPENDENCY_QUERY_RESPONSE) message.
1. The server sends [MRD_MESSAGE_CLOSE](#ID-4-MRD_MESSAGE_CLOSE) to indicate that no further data will be sent by the server.
1. The TCP session may be closed by either side and the MRD streaming session is complete.

```{mermaid}
sequenceDiagram
    autonumber

    Note over Client,Server: Startup
    Client->>+Server: Connect socket (TCP)
    Client->>+Server: Dependency query config (xml text)

    Server-->>-Client: Dependency query response

    par
        Client->>Server: Session complete message
        Server->>-Client: Session complete message
    end

    Server->Client: TCP session closed by either side
```

## MRD Streaming Message Types
<style>
    .mrdMsgTable2 {
        border-collapse: collapse;
        width:           100%;
        text-align:      center;
    }
    .mrdMsgTable2 tr th:nth-child(1) { text-align: center; background: #dbe5f0;}
    .mrdMsgTable2 tr td:nth-child(1) { text-align: center; background: #dbe5f0;}
    .mrdMsgTable2 tr th:nth-child(2) { text-align: center; background: #ebf0de;}
    .mrdMsgTable2 tr td:nth-child(2) { text-align: center; background: #ebf0de;}


    .mrdMsgTable3 {
        border-collapse: collapse;
        width:           100%;
        text-align:      center;
    }
    
    .mrdMsgTable3 tr th:nth-child(1) { text-align: center; background: #dbe5f0;}
    .mrdMsgTable3 tr td:nth-child(1) { text-align: center; background: #dbe5f0;}
    .mrdMsgTable3 tr th:nth-child(2) { text-align: center; background: #f1dbdb;}
    .mrdMsgTable3 tr td:nth-child(2) { text-align: center; background: #f1dbdb;}
    .mrdMsgTable3 tr th:nth-child(3) { text-align: center; background: #ebf0de;}
    .mrdMsgTable3 tr td:nth-child(3) { text-align: center; background: #ebf0de;}


    .mrdMsgTable4 {
        border-collapse: collapse;
        width:           100%;
        text-align:      center;
    }

    .mrdMsgTable4 tr th:nth-child(1) { text-align: center; background: #dbe5f0;}
    .mrdMsgTable4 tr td:nth-child(1) { text-align: center; background: #dbe5f0;}
    .mrdMsgTable4 tr th:nth-child(2) { text-align: center; background: #f1dbdb;}
    .mrdMsgTable4 tr td:nth-child(2) { text-align: center; background: #f1dbdb;}
    .mrdMsgTable4 tr th:nth-child(3) { text-align: center; background: #ebf0de;}
    .mrdMsgTable4 tr td:nth-child(3) { text-align: center; background: #ebf0de;}
    .mrdMsgTable4 tr th:nth-child(4) { text-align: center; background: #e5e0eb;}
    .mrdMsgTable4 tr td:nth-child(4) { text-align: center; background: #e5e0eb;}

    .mrdMsgTable5 {
        border-collapse: collapse;
        width:           100%;
        text-align:      center;
    }

    .mrdMsgTable5 tr th:nth-child(1) { text-align: center; background: #dbe5f0; }
    .mrdMsgTable5 tr td:nth-child(1) { text-align: center; background: #dbe5f0; }
    .mrdMsgTable5 tr th:nth-child(2) { text-align: center; background: #f1dbdb; }
    .mrdMsgTable5 tr td:nth-child(2) { text-align: center; background: #f1dbdb; }
    .mrdMsgTable5 tr th:nth-child(3) { text-align: center; background: #ebf0de; }
    .mrdMsgTable5 tr td:nth-child(3) { text-align: center; background: #ebf0de; }
    .mrdMsgTable5 tr th:nth-child(4) { text-align: center; background: #c4d49f; }
    .mrdMsgTable5 tr td:nth-child(4) { text-align: center; background: #c4d49f; }
    .mrdMsgTable5 tr th:nth-child(5) { text-align: center; background: #e5e0eb; }
    .mrdMsgTable5 tr td:nth-child(5) { text-align: center; background: #e5e0eb; }
</style>


### ID 1: MRD_MESSAGE_CONFIG_FILE
<div class="mrdMsgTable2">

| ID             | Config File Name |
| --             | --               |
| 2 bytes        | 1024 bytes       |
| unsigned short | char             |
</div>

This message type is used to send the file name of a configuration file (local on the server file system) to be used during reconstruction. The file name must not exceed 1023 characters and is formatted as a null terminated, UTF-8 encoded char string.

### ID 2: MRD_MESSAGE_CONFIG_TEXT
<div class="mrdMsgTable3">

| ID             | Length   | Config Text     |
| --             | --       | --              |
| 2 bytes        | 4 bytes  | length * 1 byte |
| unsigned short | uint32_t | char            |
</div>

Alternatively, the text contents of a configuration file can be sent directly via the data stream. The length is sent as an uint32_t. Configuration text is sent as a null terminated char string.

### ID 3: MRD_MESSAGE_PARAMETER_HEADER
<div class="mrdMsgTable3">

| ID             | Length   | XML Header Text |
| --             | --       | --              |
| 2 bytes        | 4 bytes  | length * 1 byte |
| unsigned short | uint32_t | char            |
</div>

Metadata for MRD datasets are stored in a flexible XML scheme, as detailed in http://ismrmrd.github.io/#flexible-data-header. The header length is sent as an uint32_t and the text is sent as a null terminated char string.

## ID 4: MRD_MESSAGE_CLOSE
<div class="mrdMsgTable2">

| ID             |
| --             |
| 2 bytes        |
| unsigned short |
</div>

This message type consists only of an ID with no following data. It is used to indicate that all data related to an acquisition/reconstruction has been sent. IceFire will send this message after receiving a scan marked MDH_ACQEND. After receiving this message from the server, IceFire no longer accepts new data and finishes after completing processing of any already-received data.

## ID 5: MRD_MESSAGE_TEXT
<div class="mrdMsgTable3">

| ID             | Length   | Text            |
| --             | --       | --              |
| 2 bytes        | 4 bytes  | length * 1 byte |
| unsigned short | uint32_t | char            |
</div>

Informational (logging) text can be sent using this message type, typically from the reconstruction side to the acquisition/client side. The length of message text is sent as an uint32_t while the text is sent as a null terminated char string. The first word can optionally be used to define the severity of the message, with the following levels supported:
- DEBUG
- INFO
- WARNING
- ERROR
- CRITICAL

Optional whitespace padding can be used between the severity and the message text to improve readability.  Messages without explicity severity should be treated as INFO.

For the FIRE client, messages are added to logviewer with the following parsing:
- DEBUG    : Hidden unless TRACE_2 is set
- INFO     : Normal (white) text
- WARNING  : Warning (yellow) text
- ERROR    : Error (red) text
- CRITICAL : Error (red) text with pop-up

## ID 1008: MRD_MESSAGE_ISMRMRD_ACQUISITION
<div class="mrdMsgTable4">

| ID             | Fixed Raw Data Header   | Trajectory       | Raw Data |
| --             | --                      | --               | --       |
| 2 bytes        | 340 bytes               | number_of_samples * trajectory_dimensions * 4 bytes | number_of_channels * number_of_samples * 8 bytes |
| unsigned short | mixed                   | float             | float |
</div>

This message type is used to send raw (k-space) acquisition data. A separate message is sent for each readout. A fixed data header contains metadata such as encoding counters, is defined in http://ismrmrd.github.io/#fixed-data-structures. Three fields of the data header must be parsed in order to read the rest of the message:
- **trajectory_dimensions**: defines the number of dimensions in the k-space trajectory data component. For 2D acquisitions (k<sub>x</sub>, k<sub>y</sub>), this is set to 2, while for 3D acquisitions (k<sub>x</sub>, k<sub>y</sub>, k<sub>z</sub>), this is set to 3. If set to 0, the trajectory component is omitted.
- **number_of_samples**: number of readout samples.
- **active_channels**: number of channels for which raw data is acquired.

<style>
 .smalltable td {
   font-size: 80%;
   border-collapse: collapse;
   border-spacing:0;
   border-width:0;
   padding:0px;
 }
</style>

Trajectory data is organized by looping through the dimensions first then the samples:
  - For 2D trajectory data:
    <table class="smalltable">
      <tr>
        <td style="text-align: center" colspan="2">Sample 1</td>
        <td style="text-align: center" colspan="2">Sample 2</td>
        <td style="text-align: center" rowspan="2">...</td>
        <td style="text-align: center" colspan="2">Sample n</td>
      </tr>
      <tr>
        <td style="text-align: center">k<sub>x</sub></td>
        <td style="text-align: center">k<sub>y</sub></td>
        <td style="text-align: center">k<sub>x</sub></td>
        <td style="text-align: center">k<sub>y</sub></td>
        <td style="text-align: center">k<sub>x</sub></td>
        <td style="text-align: center">k<sub>y</sub></td>
      </tr>
    </table>

  - For 3D trajectory data:
    <table class="smalltable">
      <tr>
        <td style="text-align: center" colspan="3">Sample 1</td>
        <td style="text-align: center" colspan="3">Sample 2</td>
        <td style="text-align: center" rowspan="2">...</td>
        <td style="text-align: center" colspan="3">Sample n</td>
      </tr>
      <tr>
        <td style="text-align: center">k<sub>x</sub></td>
        <td style="text-align: center">k<sub>y</sub></td>
        <td style="text-align: center">k<sub>z</sub></td>
        <td style="text-align: center">k<sub>x</sub></td>
        <td style="text-align: center">k<sub>y</sub></td>
        <td style="text-align: center">k<sub>z</sub></td>
        <td style="text-align: center">k<sub>x</sub></td>
        <td style="text-align: center">k<sub>y</sub></td>
        <td style="text-align: center">k<sub>z</sub></td>
      </tr>
    </table>

Raw data is organized by looping through real/imaginary data, samples, then channels:

<table class="smalltable">
  <tr>
    <td style="text-align: center" colspan="6">Channel 1</td>
    <td style="text-align: center" colspan="6">Channel 2</td>
    <td style="text-align: center" rowspan="3">...</td>
    <td style="text-align: center" colspan="6">Channel n</td>
  </tr>
  <tr>
    <td style="text-align: center" colspan="2">Sample 1</td>
    <td style="text-align: center" colspan="2">...</td>
    <td style="text-align: center" colspan="2">Sample n</td>
    <td style="text-align: center" colspan="2">Sample 1</td>
    <td style="text-align: center" colspan="2">...</td>
    <td style="text-align: center" colspan="2">Sample n</td>
    <td style="text-align: center" colspan="2">Sample 1</td>
    <td style="text-align: center" colspan="2">...</td>
    <td style="text-align: center" colspan="2">Sample n</td>
  </tr>
  <tr>
    <td style="text-align: center">Re</td> <td style="text-align: center">Im</td>
    <td style="text-align: center">Re</td> <td style="text-align: center">Im</td>
    <td style="text-align: center">Re</td> <td style="text-align: center">Im</td>
    <td style="text-align: center">Re</td> <td style="text-align: center">Im</td>
    <td style="text-align: center">Re</td> <td style="text-align: center">Im</td>
    <td style="text-align: center">Re</td> <td style="text-align: center">Im</td>
    <td style="text-align: center">Re</td> <td style="text-align: center">Im</td>
    <td style="text-align: center">Re</td> <td style="text-align: center">Im</td>
    <td style="text-align: center">Re</td> <td style="text-align: center">Im</td>
  </tr>
</table>

## ID 1019: MRD_MESSAGE_DEPENDENCY_QUERY_RESPONSE
<div class="mrdMsgTable3">

| ID             | Length   | Text            |
| --             | --       | --              |
| 2 bytes        | 8 bytes  | length * 1 byte |
| unsigned short | uint64_t | char            |
</div>

An XML-formatted list of MRD MetaAttributes in response to a dependency query.  This is commonly used to send a statistical summary of the noise characteristics of raw k-space readout data.  In this case, the following MetaAttributes are present:
- status (bool): False if noise characteristics have not been analyzed.  If so, no further attributes are mandatory.
- min_sigma (double): Minimum noise covariance (sigma) over all coils
- max_sigma (double): Maximum noise covariance (sigma) over all coils
- mean_sigma (double): Mean noise covariance (sigma) over all coils
- noise_dwell_time_us (double): Dwell time for each readout sample of noise data, in microseconds.

## ID 1022: MRD_MESSAGE_ISMRMRD_IMAGE
<div class="mrdMsgTable5">

| ID             | Fixed Image Header  | Attribute Length | Attribute Data  |Image Data |
| --             | --                  | --               | --              | --        |
| 2 bytes        | 198 bytes           | 8 bytes          | length * 1 byte | matrix_size[0] * matrix_size[1] * matrix_size[2] * channels * sizeof(data_type) |
| unsigned short | mixed               | uint_64          | char            | data_type |
</div>

Image data is sent using this message type. The fixed image header contains metadata including fields such as the image type (magnitude, phase, etc.) and indices such as slice and repetition number. It is defined by the [ImageHeader](http://ismrmrd.github.io/#fixed-data-structures) struct. Within this header, there are 3 fields that must be interpreted to parse the rest of the message:
- **matrix_size**: This 3 element array indicates the size of each dimension of the image data.
- **channels**: This value indicates the number of (receive) channels for which image data is sent
- **data_type**: This value is an MRD_DataTypes enum that indicates the type of data sent. The following types are supported:
    | Value        | Name         | Type           | Size        |
    | --           | --           | --             | --          |
    | 1            | MRD_USHORT   | uint16_t       |     2 bytes |
    | 2            | MRD_SHORT    | int16_t        |     2 bytes |
    | 3            | MRD_UINT     | uint32_t       |     4 bytes |
    | 4            | MRD_INT      | int32_t        |     4 bytes |
    | 5            | MRD_FLOAT    | float          |     4 bytes |
    | 6            | MRD_DOUBLE   | double         |     8 bytes |
    | 7            | MRD_CXFLOAT  | complex float  | 2 * 4 bytes |
    | 8            | MRD_CXDOUBLE | complex double | 2 * 8 bytes |

Attributes are used to declare additional image metadata that is not present in the fixed image header. In general, this data is sent as a char string (not null-terminated), with the length sent first as an uint_64 (not uint_32!). The IceFire implementation interprets attribute data as an [ISMRMRD MetaContainer](https://ismrmrd.github.io/api/class_i_s_m_r_m_r_d_1_1_meta_container.html).

Image data is organized by looping through matrix_size[0], matrix_size[1], matrix_size[2], then channels. For example, 2D image data would be formatted as:

<table class="smalltable">
  <tr>
    <td style="text-align: center" colspan="9">Channel 1</td>
    <td style="text-align: center" rowspan="3">...</td>
    <td style="text-align: center" colspan="9">Channel n</td>
  </tr>
  <tr>
    <td style="text-align: center" colspan="3">y<sub>1</sub></td>
    <td style="text-align: center" colspan="3">...</td>
    <td style="text-align: center" colspan="3">y<sub>n</sub></td>
    <td style="text-align: center" colspan="3">y<sub>1</sub></td>
    <td style="text-align: center" colspan="3">...</td>
    <td style="text-align: center" colspan="3">y<sub>n</sub></td>
  </tr>
  <tr>
    <td style="text-align: center">x<sub>1</sub></td>
    <td style="text-align: center">...</td>
    <td style="text-align: center">x<sub>n</sub></td>
    <td style="text-align: center">x<sub>1</sub></td>
    <td style="text-align: center">...</td>
    <td style="text-align: center">x<sub>n</sub></td>
    <td style="text-align: center">x<sub>1</sub></td>
    <td style="text-align: center">...</td>
    <td style="text-align: center">x<sub>n</sub></td>
    <td style="text-align: center">x<sub>1</sub></td>
    <td style="text-align: center">...</td>
    <td style="text-align: center">x<sub>n</sub></td>
    <td style="text-align: center">x<sub>1</sub></td>
    <td style="text-align: center">...</td>
    <td style="text-align: center">x<sub>n</sub></td>
    <td style="text-align: center">x<sub>1</sub></td>
    <td style="text-align: center">...</td>
    <td style="text-align: center">x<sub>n</sub></td>
  </tr>
</table>

The [image_type](https://ismrmrd.github.io/apidocs/1.4.2/struct_i_s_m_r_m_r_d_1_1_i_s_m_r_m_r_d___image_header.html#a285853df7d65973ba3827fc95fc65749) field of the ImageHeader is an enum describing the image type with the following values:

| Value        | Name                 |
| --           | --                   |
| 1            | MRD_IMTYPE_MAGNITUDE |
| 2            | MRD_IMTYPE_PHASE     |
| 3            | MRD_IMTYPE_REAL      |
| 4            | MRD_IMTYPE_IMAG      |
| 5            | MRD_IMTYPE_COMPLEX   |
| 6            | MRD_IMTYPE_RGB       |

RGB (TrueColor) images are supported in MRD with the following settings:
- ``image_type`` is set to ``MRD_IMTYPE_RGB``
- ``data_type`` is set to ``MRD_USHORT``
- ``channels`` is set to 3, representing the red, green, and blue channels of the RGB image
- image data values are in the range 0-255 (8-bit color depth)

## ID 1026: MRD_MESSAGE_ISMRMRD_WAVEFORM
<div class="mrdMsgTable3">

| ID             | Fixed Waveform Header | Waveform Data                        |
| --             | --                    | --                                   |
| 2 bytes        | 240 bytes             | channels * number of samples * bytes |
| unsigned short | mixed                 | uint32_t                             |
</div>

This message type is used to send arbitrary waveform data (e.g. physio signals, gradient waveforms, etc.). The fixed waveform data header is defined by the MRD_WaveformHeader and contains the following members:

| Member Name       | Description                                   | Type     | Size    |
| --                | --                                            | --       | --      |
| version           | Version number                                | uint16_t | 2 bytes |
| flags             | Bit field with flags                          | uint64_t | 8 bytes |
| measurement_uid   | Unique ID for this measurement                | uint32_t | 4 bytes |
| scan_counter      | Number of the acquisition after this waveform | uint32_t | 4 bytes |
| time_stamp        | Starting imestamp of this waveform            | uint32_t | 4 bytes |
| number_of_samples | Number of samples acquired                    | uint16_t | 2 bytes |
| channels          | Active channels                               | uint16_t | 2 bytes |
| sample_time_us    | Time between samples in microseconds          | float    | 4 bytes |
| waveform_id       | ID matching types specified in XML header     | uint16_t | 2 bytes |

The **channels** and **number_of_samples** members fields must be parsed in order to read the rest of the message. Waveform data is sent as an uint32_t array, ordered by looping through samples and then through channels:

<table class="smalltable">
      <tr>
        <td style="text-align: center" colspan="3">Channel 1</td>
        <td style="text-align: center" colspan="3">Channel 2</td>
        <td style="text-align: center" rowspan="2">...</td>
        <td style="text-align: center" colspan="3">Channel n</td>
      </tr>
      <tr>
        <td style="text-align: center">w<sub>1</sub></td>
        <td style="text-align: center">...</td>
        <td style="text-align: center">w<sub>n</sub></td>
        <td style="text-align: center">w<sub>1</sub></td>
        <td style="text-align: center">...</td>
        <td style="text-align: center">w<sub>n</sub></td>
        <td style="text-align: center">w<sub>1</sub></td>
        <td style="text-align: center">...</td>
        <td style="text-align: center">w<sub>n</sub></td>
      </tr>
    </table>

### MRD Waveform Types
| Value | Name                                  |
| --    | --                                    |
|  0    | ECG                                   |
|  1    | Pulse Oximetry                        |
|  2    | Respiratory                           |
|  3    | External Waveform 1                   |
|  4    | External Waveform 2                   |
|  5    | ECG (Training Data)                   |
|  6    | Pulse Oximetry (Training Data)        |
|  7    | Respiratory (Training Data)           |
|  8    | External Waveform 1 (Training Data)   |
|  9    | External Waveform 2 (Training Data)   |
| 10    | Events                                |
| 11    | PilotTone Cardiac                     |
| 12    | PilotTone Respiratory                 |
| 15    | Events (Training Data)                |
| 16    | PilotTone Cardiac (Training Data)     |
| 17    | PilotTone Respiratory (Training Data) |


# ISMRM MetaContainer Format
Image metadata is transmitted using XML text conforming to the ISMRM MetaContainer format, which may look like:

```xml
<ismrmrdMeta>
    <meta>
        <name>DataRole</name>
        <value>Image</value>
        <value>AVE</value>
        <value>NORM</value>
        <value>MAGIR</value>
    </meta>
    <meta>
        <name>ImageNumber</name>
        <value>1</value>
    </meta>
</ismrmrdMeta>
```
A variable number of “meta” elements can be defined, each with a single name and one or more value sub-elements. The following table lists named meta elements interpreted by IceFire:

<div class="smalltable">

| MRD Element Name  | Format       | DICOM Tag                                                                                 | Interpretation                                      |
| --                | --           | --                                                                                        | --                                                  |
| DataRole          | text array   | N/A                                                                                       | Characteristics of the image. <br><br> A value of “Quantitative” indicates that pixel values in the image are parametric and to be interpreted directly (e.g. T1 values, velocity, etc.). If this role is present, pixel values are not further modified in the ICE chain, e.g. by normalization. |
| SeriesDescription | text array   | [SeriesDescription](http://dicomlookup.com/lookup.asp?sw=Tnumber&q=(0008,103E))           | Brief characteristics of the image. <br><br> The DICOM SeriesDescription (0008,103E) field is constructed by combining this array of values, delimited by "\_" (underscores). |
| SeriesDescriptionAdditional | text array   | [SeriesDescription](http://dicomlookup.com/lookup.asp?sw=Tnumber&q=(0008,103E)) | Brief characteristics of the image. <br><br> The existing DICOM SeriesDescription (0008,103E) field is appended each string in this array, delimited by "\_" (underscores). |
| ImageComment      | text array   | [ImageComment](http://dicomlookup.com/lookup.asp?sw=Tnumber&q=(0020,4000))                | Remarks about the image. <br><br> This array of values is stored in the DICOM ImageComment (0020,4000) field, delimited by "\_" (underscores). |
| ImageType         | text array   | [ImageType](http://dicomlookup.com/lookup.asp?sw=Tnumber&q=(0008,0008))                   | Characteristics of the image. <br><br> This array of values is appended to the DICOM ImageType (0008,0008) field starting in position 4, delimited by “\” (backslash). |
| ImageRowDir       | double array | N/A                                                                                       | A (1x3) vector in indicating the direction along row dimension.  For images reconstructed from raw data and not undergoing any [flipping or rotating to bring them into standard DICOM orientation](http://gdcm.sourceforge.net/wiki/index.php/Orientation), this value is equivalent to the AcquisitionHeader read_dir field. |
| ImageColumnDir    | double array | N/A                                                                                       | A (1x3) vector in indicating the direction along column dimension.  For images reconstructed from raw data and not undergoing any [flipping or rotating to bring them into standard DICOM orientation](http://gdcm.sourceforge.net/wiki/index.php/Orientation), this value is equivalent to the AcquisitionHeader phase_dir field. |
| RescaleIntercept  | double       | [RescaleIntercept](http://dicomlookup.com/lookup.asp?sw=Tnumber&q=(0028,1052))            | Intercept for image pixel values, used in conjunction with RescaleSlope. <br><br> Pixel values are to be interpreted as: ***value = RescaleSlope\*pixelValue + RescaleIntercept***. This value is set in the DICOM RescaleIntercept (0028,1052) field. |
| RescaleSlope      | double       | [RescaleSlope](http://dicomlookup.com/lookup.asp?sw=Tnumber&q=(0028,1053))                | Scaling factor for image pixel values, used in conjunction with RescaleIntercept. <br><br> Pixel values are to be interpreted as: ***value = RescaleSlope\*pixelValue + RescaleIntercept***. This value is set in the DICOM RescaleSlope (0028,1053) field. |
| WindowCenter      | long         | [WindowCenter](http://dicomlookup.com/lookup.asp?sw=Tnumber&q=(0028,1050))                | The window center in the rendered image, used in conjunction with WindowWidth. <br><br> If RescaleIntercept and RescaleSlope are defined, WindowCenter and WindowWidth are applied to rescaled values. This value is set in the DICOM WindowCenter (0028,1050) field. |
| WindowWidth       | long         | [WindowWidth](http://dicomlookup.com/lookup.asp?sw=Tnumber&q=(0028,1051))                 | The window center in the rendered image, used in conjunction with WindowCenter. <br><br> If RescaleIntercept and RescaleSlope are defined, WindowCenter and WindowWidth are applied to rescaled values. This value is set in the DICOM WindowWidth (0028,1051) field. |
| LUTFileName       | text         | PhotometricInterpretation, [RedPaletteColorLookupTable, RedPaletteColorLookupTable, RedPaletteColorLookupTable](http://dicom.nema.org/medical/Dicom/2018d/output/chtml/part03/sect_C.7.9.html) | Path to a color lookup table file to be used for this image. <br><br> LUT files must be in Siemens .pal format and stored in C:\MedCom\config\MRI\ColorLUT. If a value is provided, the DICOM field PhotometricInterpretation (0028,0004) is set to “PALETTE COLOR” |
| EchoTime          | double       | [EchoTime](http://dicomlookup.com/lookup.asp?sw=Tnumber&q=(0018,0081))                    | Echo time of the image in ms. <br><br> This value is set in the DICOM EchoTime (0018,0081) field.
| InversionTime     | double       | [InversionTime](http://dicomlookup.com/lookup.asp?sw=Tnumber&q=(0018,0082))               | Inversion time of the image in ms. <br><br> This value is set in the DICOM InversionTime (0018,0082) field.
| ROI               | double array | N/A                                                                                       | Region of interest polygon. <br><br> For multiple ROIs, the MetaAttribute element names shall start with “ROI_”. These ROIs are stored in a format compatible with the Siemens syngo viewer. The first 6 values are meta attributes of the ROI:
|                   |              |                                                                                           |   1. Red color (normalized to 1)
|                   |              |                                                                                           |   2. Green color (normalized to 1)
|                   |              |                                                                                           |   3. Blue color (normalized to 1)
|                   |              |                                                                                           |   4. Line thickness (default is 1)
|                   |              |                                                                                           |   5. Line style (0 = solid, 1 = dashed)
|                   |              |                                                                                           |   6. Visibility (0 = false, 1 = true)
|                   |              |                                                                                           | The remaining values are (row,col) coordinates for each ROI point, with values between 0 and the number of rows/columns. Data is organized as (point 1<sub>row</sub>, point 1<sub>col</sub>, point2<sub>row</sub>, point 2<sub>col</sub>, etc). The last point should be a duplicate of the first point if a closed ROI is desired.
</div>



| Element Name      | Format       | Interpretation                                      |
| --                | --           | --                                                  |
| DataRole          | text array   | Characteristics of the image. <br><br> A value of “Quantitative” indicates that pixel values in the image are parametric and to be interpreted directly (e.g. T1 values, velocity, etc.). If this role is present, pixel values are not further modified in the ICE chain, e.g. by normalization. |
| SeriesDescription | text array   | Brief characteristics of the image. <br><br> The DICOM SeriesDescription (0008,103E) field is constructed by combining this array of values, delimited by "\_" (underscores). |
| SeriesDescriptionAdditional | text array   | Brief characteristics of the image. <br><br> The existing DICOM SeriesDescription (0008,103E) field is appended each string in this array, delimited by "\_" (underscores). |
| ImageComment      | text array   | Remarks about the image. <br><br> This array of values is stored in the DICOM ImageComment (0020,4000) field, delimited by "\_" (underscores). |
| ImageType         | text array   | Characteristics of the image. <br><br> This array of values is appended to the DICOM ImageType (0008,0008) field starting in position 4, delimited by “\” (backslash). |
| ImageRowDir       | double array | A (1x3) vector in indicating the direction along row dimension.  For images reconstructed from raw data and not undergoing any [flipping or rotating to bring them into standard DICOM orientation](http://gdcm.sourceforge.net/wiki/index.php/Orientation), this value is equivalent to the AcquisitionHeader read_dir field. |
| ImageColumnDir    | double array | A (1x3) vector in indicating the direction along column dimension.  For images reconstructed from raw data and not undergoing any [flipping or rotating to bring them into standard DICOM orientation](http://gdcm.sourceforge.net/wiki/index.php/Orientation), this value is equivalent to the AcquisitionHeader phase_dir field. |
| RescaleIntercept  | double       | Intercept for image pixel values, used in conjunction with RescaleSlope. <br><br> Pixel values are to be interpreted as: ***value = RescaleSlope\*pixelValue + RescaleIntercept***. This value is set in the DICOM RescaleIntercept (0028,1052) field. |
| RescaleSlope      | double       | Scaling factor for image pixel values, used in conjunction with RescaleIntercept. <br><br> Pixel values are to be interpreted as: ***value = RescaleSlope\*pixelValue + RescaleIntercept***. This value is set in the DICOM RescaleSlope (0028,1053) field. |
| WindowCenter      | long         | The window center in the rendered image, used in conjunction with WindowWidth. <br><br> If RescaleIntercept and RescaleSlope are defined, WindowCenter and WindowWidth are applied to rescaled values. This value is set in the DICOM WindowCenter (0028,1050) field. |
| WindowWidth       | long         | The window center in the rendered image, used in conjunction with WindowCenter. <br><br> If RescaleIntercept and RescaleSlope are defined, WindowCenter and WindowWidth are applied to rescaled values. This value is set in the DICOM WindowWidth (0028,1051) field. |
| LUTFileName       | text         | Path to a color lookup table file to be used for this image. <br><br> LUT files must be in Siemens .pal format and stored in C:\MedCom\config\MRI\ColorLUT. If a value is provided, the DICOM field PhotometricInterpretation (0028,0004) is set to “PALETTE COLOR” |
| EchoTime          | double       | Echo time of the image in ms. <br><br> This value is set in the DICOM EchoTime (0018,0081) field.
| InversionTime     | double       | Inversion time of the image in ms. <br><br> This value is set in the DICOM InversionTime (0018,0082) field.
| ROI               | double array | Region of interest polygon. <br><br> For multiple ROIs, the MetaAttribute element names shall start with “ROI_”. These ROIs are stored in a format compatible with the Siemens syngo viewer. The first 6 values are meta attributes of the ROI:
|                   |              |   1. Red color (normalized to 1)
|                   |              |   2. Green color (normalized to 1)
|                   |              |   3. Blue color (normalized to 1)
|                   |              |   4. Line thickness (default is 1)
|                   |              |   5. Line style (0 = solid, 1 = dashed)
|                   |              |   6. Visibility (0 = false, 1 = true)
|                   |              | The remaining values are (row,col) coordinates for each ROI point, with values between 0 and the number of rows/columns. Data is organized as (point 1<sub>row</sub>, point 1<sub>col</sub>, point2<sub>row</sub>, point 2<sub>col</sub>, etc). The last point should be a duplicate of the first point if a closed ROI is desired.

| Element Name      | Format       | Interpretation                                      |
| --                | --           | --                                                  |
| PhaseContrast     | bool         | True if image is from a phase contrast (velocity encoded) acquisition |
| FlowVelocity      | double       | Maximum velocity encoding (venc) for a phase contrast image |
| FlowDirDisplay    | text         | Velocity encoding direction (signed) for a phase contrast image |

In the Siemens implementation of the MRD Format, MetaAttributes 
Additional Siemens-specific fields in the ICE mini-header can be using meta elements with names beginning with “SiemensControl_” or “SiemensDicom_” for entries in the CONTROL and DICOM sections respectively.  For example:
```xml
<meta>
    <name>SiemensDicom_SliceNo</name>
    <value>long</value>
    <value>1</value>
</meta>
```

The first value element indicates the format of the actual value.  Valid formats are long, double, and string.  The second value element indicates the actual value to be stored.  In this example, the ICE mini-header field “SliceNo” is set to (long)1.

All remaining meta elements with values not listed in above table or beginning with “SiemensControl_”/”SiemensDicom_” are ignored.  The XML text representation of the MetaContainer attributes is stored in the DICOM private tag (0029,1020) under “UserDefinedSeries”.


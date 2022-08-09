# Message Types
<style>
    .mrdMsgTable2 {
        border-collapse: collapse;
        width:           100%;
        text-align:      center;
    }
    .mrdMsgTable2 tr th:nth-child(1) { text-align: center; background-color: #dbe5f0 !important;}
    .mrdMsgTable2 tr td:nth-child(1) { text-align: center; background-color: #dbe5f0 !important;}
    .mrdMsgTable2 tr th:nth-child(2) { text-align: center; background-color: #ebf0de !important;}
    .mrdMsgTable2 tr td:nth-child(2) { text-align: center; background-color: #ebf0de !important;}


    .mrdMsgTable3 {
        border-collapse: collapse;
        border-color:    black !important;
        width:           100%;
        text-align:      center;
    }
    
    .mrdMsgTable3 tr th:nth-child(1) { text-align: center; background-color: #dbe5f0 !important;}
    .mrdMsgTable3 tr td:nth-child(1) { text-align: center; background-color: #dbe5f0 !important;}
    .mrdMsgTable3 tr th:nth-child(2) { text-align: center; background-color: #f1dbdb !important;}
    .mrdMsgTable3 tr td:nth-child(2) { text-align: center; background-color: #f1dbdb !important;}
    .mrdMsgTable3 tr th:nth-child(3) { text-align: center; background-color: #ebf0de !important;}
    .mrdMsgTable3 tr td:nth-child(3) { text-align: center; background-color: #ebf0de !important;}


    .mrdMsgTable4 {
        border-collapse: collapse;
        width:           100%;
        text-align:      center;
    }

    .mrdMsgTable4 tr th:nth-child(1) { text-align: center; background-color: #dbe5f0 !important;}
    .mrdMsgTable4 tr td:nth-child(1) { text-align: center; background-color: #dbe5f0 !important;}
    .mrdMsgTable4 tr th:nth-child(2) { text-align: center; background-color: #f1dbdb !important;}
    .mrdMsgTable4 tr td:nth-child(2) { text-align: center; background-color: #f1dbdb !important;}
    .mrdMsgTable4 tr th:nth-child(3) { text-align: center; background-color: #ebf0de !important;}
    .mrdMsgTable4 tr td:nth-child(3) { text-align: center; background-color: #ebf0de !important;}
    .mrdMsgTable4 tr th:nth-child(4) { text-align: center; background-color: #e5e0eb !important;}
    .mrdMsgTable4 tr td:nth-child(4) { text-align: center; background-color: #e5e0eb !important;}

    .mrdMsgTable5 {
        border-collapse: collapse;
        width:           100%;
        text-align:      center;
    }

    .mrdMsgTable5 tr th:nth-child(1) { text-align: center; background-color: #dbe5f0 !important;}
    .mrdMsgTable5 tr td:nth-child(1) { text-align: center; background-color: #dbe5f0 !important;}
    .mrdMsgTable5 tr th:nth-child(2) { text-align: center; background-color: #f1dbdb !important;}
    .mrdMsgTable5 tr td:nth-child(2) { text-align: center; background-color: #f1dbdb !important;}
    .mrdMsgTable5 tr th:nth-child(3) { text-align: center; background-color: #ebf0de !important;}
    .mrdMsgTable5 tr td:nth-child(3) { text-align: center; background-color: #ebf0de !important;}
    .mrdMsgTable5 tr th:nth-child(4) { text-align: center; background-color: #c4d49f !important;}
    .mrdMsgTable5 tr td:nth-child(4) { text-align: center; background-color: #c4d49f !important;}
    .mrdMsgTable5 tr th:nth-child(5) { text-align: center; background-color: #e5e0eb !important;}
    .mrdMsgTable5 tr td:nth-child(5) { text-align: center; background-color: #e5e0eb !important;}
</style>


## ID 1: MRD_MESSAGE_CONFIG_FILE
<div class="mrdMsgTable2">

| ID             | Config File Name |
| --             | --               |
| 2 bytes        | 1024 bytes       |
| unsigned short | char             |
</div>

This message type is used to send the file name of a configuration file (local on the server file system) to be used during reconstruction. The file name must not exceed 1023 characters and is formatted as a null terminated, UTF-8 encoded char string.

## ID 2: MRD_MESSAGE_CONFIG_TEXT
<div class="mrdMsgTable3">

| ID             | Length   | Config Text     |
| --             | --       | --              |
| 2 bytes        | 4 bytes  | length * 1 byte |
| unsigned short | uint32_t | char            |
</div>

Alternatively, the text contents of a configuration file can be sent directly via the data stream. The length is sent as an uint32_t. Configuration text is sent as a null terminated char string.

In addition to specifying a configuration "preset" to be executed on the server, it is often desirable to modify specific parameters of the configuration, such as filter strength or the toggling of intermediate outputs for debugging purposes.  While individual parameters are specific to a given pipeline and server, the format of this configuration is standardized to enable interoperable communications between various clients and servers.

```xml
<?xml version="1.0" encoding="UTF-8"?>
  <mrdConfiguration>
    <!-- This mandatory parameters corresponds to MRD_MESSAGE_CONFIG_FILE -->
    <configName>default.xml</configName>

    <!-- These modify parameters otherwise specified in the config file -->
    <parameter>
      <name>filterBandwidth</name>
      <scope>module1</scope>
      <value>89.2</value>
    </parameter>

    <parameter>
      <name>debugOutputs</name>
      <value>true</value>
    </parameter>

    <parameter>
      <name>flipAngleArray</name>
      <value>1.0</value>
      <value>2.0</value>
      <value>3.0</value>
    </parameter>

    <!-- Information from the client about where various dependencies are found -->
    <dependency>
      <!-- Data to be transmitted inline in the session -->
      <name>noiseCovarianceMatrix</name>
      <value>inline://noise_with_unique_identifier</value>
    </dependency>

    <dependency>
      <name>coilSensitivityMap</name>
      <!-- <format>coilSensitivityMapFormat</format> -->
      <value>https://this-url/this-file.dat?token=eyj564876</value>
    </dependency>

    <dependency>
      <name>gradientImpulseResponseFunction</name>
      <!-- local to server -->
      <value>file:///path/to/girf_file</value>
    </dependency>
  <mrdConfiguration>
</xml>
```

### Querying configurable parameters
Optional: It may be desirable for a server to communicate what parameters are configurable, for example to expose them to the end user through a user interface.  This mechanism can also be used by the server to indicate required dependencies that the server must supply for a given config.  The client requests this information by sending message MRD_MESSAGE_CONFIG_QUERY of format:

<div class="mrdMsgTable3">

| ID             | Length   | Config Name     |
| --             | --       | --              |
| 2 bytes        | 4 bytes  | length * 1 byte |
| unsigned short | uint32_t | char            |
</div>

The server responds with message MRD_MESSAGE_CONFIG_TEXT with blank values for each of the parameters/dependencies, e.g.:

```xml
<?xml version="1.0" encoding="UTF-8"?>
  <mrdConfiguration>
    <!-- The below parameters/dependencies apply to this named config -->
    <configName>default.xml</configName>

    <!-- These parameters should be exposed to the end user -->
    <parameter>
      <name>filterBandwidth</name>
      </type>
      </limits>
      </enums>
    </parameter>

    <parameter>
      <name>debugOutputs</name>
    </parameter>

    <!-- Dependencies that are required by the server -->
    <dependency>
      <name>noiseCovarianceMatrix</name>
    </dependency>

    <dependency>
      <name>coilSensitivityMap</name>
    </dependency>
  <mrdConfiguration>
</xml>
```

## ID 3: MRD_MESSAGE_HEADER
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

## ID 1008: MRD_MESSAGE_ACQUISITION
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
   font-size:       80%;
   border-collapse: collapse;
   border-spacing:  0;
   border-width:    0;
   padding:         3px;
   border:          1px solid lightgray
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

<!-- ## ID 1019: MRD_MESSAGE_DEPENDENCY_QUERY_RESPONSE
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
- noise_dwell_time_us (double): Dwell time for each readout sample of noise data, in microseconds. -->

## ID 1022: MRD_MESSAGE_IMAGE
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

## ID 1026: MRD_MESSAGE_WAVEFORM
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

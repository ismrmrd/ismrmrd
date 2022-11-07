# Raw Acquisition Data
Raw k-space data is stored in MRD format as individual readout acquisitions.  Each readout contains the complex raw data for all channels, a fixed AcquisitionHeader, and optionally corresponding k-space trajectory information.  Most datasets will be comprised of many acquisitions, each stored indvidiually with its own [AcquisitionHeader](AcquisitionHeader), optional [trajectory](Trajectory), and [raw data](RawData).

(AcquisitionHeader)=
## AcquisitionHeader
An MRD AcquisitionHeader accompanies each readout containing metadata common to most data.  It is of a fixed size and thus fields cannot be added, removed, or otherwise repurposed.  It contains the following information:
| Field                  | Description                                                                                                                                        | Type                | Offset    |
| --                     | --                                                                                                                                                 | --                  | --        |
| version                | Major version number (currently 1)                                                                                                                 | uint16              |   0 bytes |
| flags                  | A bit mask of common attributes applicable to individual acquisition readouts                                                                      | uint64              |   2 bytes |
| measurement_uid        | Unique ID corresponding to the readout                                                                                                             | uint32              |  10 bytes |
| scan_counter           | Zero-indexed incrementing counter for readouts                                                                                                     | uint32              |  14 bytes |
| acquisition_time_stamp | Clock time stamp (e.g. milliseconds since midnight)                                                                                                | uint32              |  18 bytes |
| physiology_time_stamp  | Time stamps relative to physiological triggering, e.g. ECG, pulse oximetry, respiratory. Multiplicity defined by ISMRMRD_PHYS_STAMPS (currently 3) | uint32 (x3)         |  22 bytes |
| number_of_samples      | Number of digitized readout samples                                                                                                                | uint16              |  34 bytes |
| available_channels     | Number of possible receiver coils (channels)                                                                                                       | uint16              |  36 bytes |
| active_channels        | Number of active receiver coils                                                                                                                    | uint16              |  38 bytes |
| channel_mask           | Bit mask indicating active coils (64\*16 = 1024 bits)                                                                                              | uint64 (x16)        |  40 bytes |
| discard_pre            | Number of readout samples to be discarded at the beginning (e.g. if the ADC is active during gradient events)                                      | uint16              | 168 bytes |
| discard_post           | Number of readout samples to be discarded at the end (e.g. if the ADC is active during gradient events)                                            | uint16              | 170 bytes |
| center_sample          | Index of the readout sample corresponing to k-space center (zero indexed)                                                                          | uint16              | 172 bytes |
| encoding_space_ref     | Indexed reference to the encoding spaces enumerated in the MRD (xml) header                                                                        | uint16              | 174 bytes |
| trajectory_dimensions  | Dimensionality of the k-space trajectory vector (e.g. 2 for 2D radial (k<sub>x</sub>, k<sub>y</sub>), 0 for no trajectory data)                    | uint16              | 176 bytes |
| sample_time_us         | Readout bandwidth, as time between samples in microseconds                                                                                         | float (32 bit)      | 178 bytes |
| position               | Center of the excited volume, in (left, posterior, superior) (LPS) coordinates relative to isocenter in millimeters                                | float (32 bit) (x3) | 182 bytes |
| read_dir               | Directional cosine of readout/frequency encoding                                                                                                   | float (32 bit) (x3) | 194 bytes |
| phase_dir              | Directional cosine of phase encoding (2D)                                                                                                          | float (32 bit) (x3) | 206 bytes |
| slice_dir              | Directional cosine of slice normal, i.e. cross-product of read_dir and phase_dir                                                                   | float (32 bit) (x3) | 218 bytes |
| patient_table_position | Offset position of the patient table, in LPS coordinates                                                                                           | float (32 bit) (x3) | 230 bytes |
| idx                    | Encoding loop counters, as defined [below](EncodingCounters)                                                                                       | uint16 (x17)        | 242 bytes |
| user_int               | User-defined integer parameters, multiplicity defined by ISMRMRD_USER_INTS (currently 8)                                                           |  int32 (x8)         | 276 bytes |
| user_float             | User-defined float parameters, multiplicity defined by ISMRMRD_USER_FLOATS (currently 8)                                                           | float (32 bit) (x8) | 308 bytes |
|                        |                                                                                                                                          **Total** | **340 bytes**       |           |

A reference implementation for serialization/deserialization of the AcquisitionHeader can be found in [serialization.cpp](../libsrc/serialization.cpp).

(EncodingCounters)=
### MRD EncodingCounters
MR acquisitions often loop through a set of counters (e.g. phase encodes) in a complete experiment.  The following encoding counters are referred to by the ``idx`` field in the AcquisitionHeader.

| Field                  | Format       | Description                                                                    | Type         | Offset   |
| --                     | --           | --                                                                             | --           | --       |
| kspace_encode_step_1   | uint16       | Phase encoding line                                                            | uint16       |  0 bytes |
| kspace_encode_step_2   | uint16       | Partition encoding                                                             | uint16       |  2 bytes |
| average                | uint16       | Signal average                                                                 | uint16       |  4 bytes |
| slice                  | uint16       | Slice number (multi-slice 2D)                                                  | uint16       |  6 bytes |
| contrast               | uint16       | Echo number in multi-echo                                                      | uint16       |  8 bytes |
| phase                  | uint16       | Cardiac phase                                                                  | uint16       | 10 bytes |
| repetition             | uint16       | Counter in repeated/dynamic acquisitions                                       | uint16       | 12 bytes |
| set                    | uint16       | Sets of different preparation, e.g. flow encoding, diffusion weighting         | uint16       | 14 bytes |
| segment                | uint16       | Counter for segmented acquisitions                                             | uint16       | 16 bytes |
| user                   | uint16 (x8)  | User defined counters, multiplicity defined by ISMRMRD_USER_INTS (currently 8) | uint16 (x8)  | 18 bytes |
|                        |              |                                                                      **Total** | **34 bytes** |          |

A reference implementation for serialization/deserialization of the EncodingCounters can be found in [serialization.cpp](../libsrc/serialization.cpp).

### MRD AcquisitionFlags
The ``flags`` field in the AcquisitionHeader is a 64 bit mask that can be used to indicate specific attributes of the corresponding readout.  One usage of these flags is to trigger the processing of data when a condition is met, e.g. the last readout for the current slice.  The following flags are defined in the ``ISMRMRD_AcquisitionFlags`` enum of [ismrmrd.h](../include/ismrmrd/ismrmrd.h):
```
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
```
(Trajectory)=
## k-space Trajectory
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

k-space trajectory information is optionally included with each readout, with dimensionality specified by the ``trajectory_dimensions`` field in the AcquisitionHeader.  Common values are ``2`` for 2D radial (k<sub>x</sub>, k<sub>y</sub>), ``3`` for 3D radial (k<sub>x</sub>, k<sub>y</sub>, k<sub>z</sub>).  Trajectory information is omitted if ``trajectory_dimensions`` is set to ``0``.

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

(RawData)=
## Raw Data
MR acquisition raw data are stored as complex valued floats.  Data from all receiver channels are included in a single readout object.  Data is organized by looping through real/imaginary data, samples, then channels:

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

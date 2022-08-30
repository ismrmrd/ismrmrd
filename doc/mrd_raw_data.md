# Raw Acquisition Data
Raw k-space data is stored in MRD format as individual readout acquisitions.  Each readout contains the complex raw data for all channels, a fixed AcquisitionHeader, and optionally corresponding k-space trajectory information.  Most datasets will be comprised of many acquisitions, each stored indvidiually with its own AcquisitionHeader, optional trajectory, and raw data.

## AcquisitionHeader
An MRD AcquisitionHeader accompanies each readout containing metadata common to most data.  It is of a fixed size and thus fields cannot be added, removed, or otherwise repurposed.  It contains the following information:
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
| idx                    | uint16 (x17)        | Encoding loop counters, as defined [below](#mrd-encodingcounters)     |
| user_int               |  int32 (x8)         | User-defined integer parameters, multiplicity defined by ISMRMRD_USER_INTS (currently 8) |
| user_float             | float (32 bit) (x8) | User-defined float parameters, multiplicity defined by ISMRMRD_USER_FLOATS (currently 8) | 

### MRD EncodingCounters
MR acquisitions often loop through a set of counters (e.g. phase encodes) in a complete experiment.  The following encoding counters are referred to by the ``idx`` field in the AcquisitionHeader.

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

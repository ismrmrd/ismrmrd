# Physiological Waveforms
Physiological monitoring data such as electrocardiograms, pulse oximetry, or external triggering may accompany MR acquisitions.  These physiological data are stored in MRD as a combination of a fixed WaveformHeader and the raw physiological waveforms.

## WaveformHeader
The WaveformHeader contains metadata associated with a set of waveform data and has the following fields:
| Field             | Description                                   | Type     | Size    |
| --                | --                                            | --       | --      |
| version           | Version number                                | uint16_t | 2 bytes |
| flags             | Bit field with flags                          | uint64_t | 8 bytes |
| measurement_uid   | Unique ID for this measurement                | uint32_t | 4 bytes |
| scan_counter      | Number of the acquisition after this waveform | uint32_t | 4 bytes |
| time_stamp        | Starting timestamp of this waveform           | uint32_t | 4 bytes |
| number_of_samples | Number of samples acquired                    | uint16_t | 2 bytes |
| channels          | Active channels                               | uint16_t | 2 bytes |
| sample_time_us    | Time between samples in microseconds          | float    | 4 bytes |
| waveform_id       | ID matching types specified in XML header     | uint16_t | 2 bytes |

### Waveform IDs
The ``waveform_id`` field in the WaveformHeader describes the type of physiological data stored.  The following ID numbers are standardized:
| Value | Name                                  |
| --    | --                                    |
|  0    | ECG                                   |
|  1    | Pulse Oximetry                        |
|  2    | Respiratory                           |
|  3    | External Waveform 1                   |
|  4    | External Waveform 2                   |

For each type of ``waveform_id`` included in the dataset, a corresponding ``WaveformInformation`` entry is found in the MRD header to describe the data interpretation.  For example:
```xml
<waveformInformation>
    <waveformId>0</waveformName>
    <waveformName>ECG1</waveformName>
    <waveformTriggerChannel>4</waveformTriggerChannel>
</waveformInformation>
```

Physiological data used for triggering may have an associated "trigger" channel as detected by the MR system.  The ``waveformTriggerChannel`` indicates the channel index (0-indexed) which contains the detected triggers and is omitted if no trigger data is present.

Waveform ID numbers less than 1024 are reserved while numbers greater than or equal to 1024 can be used to define custom physiological data.  For custom waveform_ids, corresponding WaveformInformation entries should be added to the MRD header to describe the data interpretation.  For example:
```xml
<waveformInformation>
    <waveformId>1024</waveformName>
    <waveformName>CustomName</waveformName>
</waveformInformation>
```

## Waveform Data
Waveform data is sent as an uint32_t array, ordered by looping through samples and then through channels:
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

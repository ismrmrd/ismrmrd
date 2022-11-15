# MRD File Format
MRD data can be stored in various formats, but the [HDF5](https://www.hdfgroup.org/solutions/hdf5) format is commonly used due to its good compatibility across programming languages and platforms.  HDF5 is a hierarchical data format (much like a file system), which can contain multiple variable organized in groups (like folders in a file system). The variables can contain arrays of data values, custom defined structs, or simple text fields.  Interface libraries are provided for C++, Python, and MATLAB to simplify usage.  MRD HDF5 files can also be opened using standard HDF tools such as [HDFView](https://www.hdfgroup.org/downloads/hdfview/) or HDF5 packages such as [h5py](https://www.h5py.org/) for Python or the built-in [h5read](https://www.mathworks.com/help/matlab/ref/h5read.html) and associated functions in MATLAB.  

The extension ``.mrd`` is used to indicate an HDF5 file containing MRD formatted data as follows:
```
/dataset/xml                 text of MRD header
/dataset/data                array of raw data (data + AcquisitionHeader + optional trajectory)
/dataset/waveforms           array of waveform (e.g. PMU) data
/dataset/image_0/data        array of image data
/dataset/image_0/header      array of ImageHeaders
/dataset/image_0/attributes  array of image MetaAttributes (xml text)
/dataset/config              text of configuration parameters for reconstruction or image analysis (optional)
/dataset/config_file         file name of configuration parameters for reconstruction or image analysis (optional)
```

All data from a complete acquisition are stored in a group (``dataset`` in the above example).  An MRD file may contain multiple acquisitions in separate groups, usually in the case of related or dependent acquisitions.

## Reading MRD data in Python
The [ismrmrd-python](https://www.github.com/ismrmrd/ismrmrd-python) library provides a convenient interface for working with MRD files.  It can either be compiled from source or installed from a pip package using the command ``pip install ismrmrd``.  The following code shows an example of getting the number of readout lines from a dataset and reading the first line of k-space data:
```python
>>> import ismrmrd
>>> dset = ismrmrd.Dataset('data.mrd')
>>> nacq = dset.number_of_acquisitions()
>>> acq = dset.read_acquisition(0)
>>> dset.close()
>>> print(acq.getHead())
version: 1
flags: 0
measurement_uid: 281
scan_counter: 3
acquisition_time_stamp: 23187655
physiology_time_stamp: 4304495, 0, 0
number_of_samples: 256
available_channels: 2
active_channels: 2
channel_mask: 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
discard_pre: 0
discard_post: 0
center_sample: 128
encoding_space_ref: 0
trajectory_dimensions: 0
sample_time_us: 15.0
position: 0.0, 0.0, 0.0
read_dir: -0.9999999403953552, 5.960464477539063e-08, 0.0
phase_dir: 5.960464477539063e-08, 0.9999999403953552, 0.0
slice_dir: 0.0, 0.0, 1.0
patient_table_position: 0.0, 0.0, -1374995.0
idx: kspace_encode_step_1: 2
kspace_encode_step_2: 0
average: 0
slice: 0
contrast: 0
phase: 0
repetition: 0
set: 0
segment: 0
user: 0, 0, 0, 0, 0, 64, 0, 0

user_int: 0, 0, 0, 0, 0, 0, 0, 0
user_float: 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0

>>> acq.data.shape
(2, 256)
```

The basic [h5py](https://www.h5py.org) package Python can also be used to read the files:
```python
  >>> import h5py
  >>> import numpy as np
  >>> f = h5py.File('data.mrd')
  >>> acq = np.array(f['dataset']['data'][0])
  >>> { k: acq['head'][k] for k in acq['head'].dtype.fields.keys() }

  {'version': array(1, dtype=uint16),
  'flags': array(64, dtype=uint64),
  'measurement_uid': array(0, dtype=uint32),
  'scan_counter': array(0, dtype=uint32),
  'acquisition_time_stamp': array(0, dtype=uint32),
  'physiology_time_stamp': array([0, 0, 0], dtype=uint32),
  'number_of_samples': array(512, dtype=uint16),
  'available_channels': array(8, dtype=uint16),
  'active_channels': array(8, dtype=uint16),
  'channel_mask': array([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], dtype=uint64),
  'discard_pre': array(0, dtype=uint16),
  'discard_post': array(0, dtype=uint16),
  'center_sample': array(256, dtype=uint16),
  'encoding_space_ref': array(0, dtype=uint16),
  'trajectory_dimensions': array(0, dtype=uint16),
  'sample_time_us': array(5., dtype=float32),
  'position': array([0., 0., 0.], dtype=float32),
  'read_dir': array([0., 0., 0.], dtype=float32),
  'phase_dir': array([0., 0., 0.], dtype=float32),
  'slice_dir': array([0., 0., 0.], dtype=float32),
  'patient_table_position': array([0., 0., 0.], dtype=float32),
  'idx': array((0, 0, 0, 0, 0, 0, 0, 0, 0, [0, 0, 0, 0, 0, 0, 0, 0]),
      dtype=[('kspace_encode_step_1', '<u2'), ('kspace_encode_step_2', '<u2'), ('average', '<u2'), ('slice', '<u2'), ('contrast', '<u2'), ('phase', '<u2'), ('repetition', '<u2'), ('set', '<u2'), ('segment', '<u2'), ('user', '<u2', (8,))]),
  'user_int': array([0, 0, 0, 0, 0, 0, 0, 0], dtype=int32),
  'user_float': array([0., 0., 0., 0., 0., 0., 0., 0.], dtype=float32)}
```

## Reading MRD data in MATLAB
A MATLAB package is also provided in this repository to facilitate easy usage of MRD files.  To use it, add the ``matlab`` folder in this repository to the MATLAB path.  The following code shows an example of getting the number of readout lines from a dataset and reading the first line of k-space data:
```matlab
>> dset = ismrmrd.Dataset('data.mrd');
>> nacq = dset.getNumberOfAcquisitions()
nacq =
   128
>> acq = dset.readAcquisition(1)
acq = 
  Acquisition with properties:

    head: [1×1 ismrmrd.AcquisitionHeader]
    traj: {[0×256 single]}
    data: {[256×2 single]}
>> dset.close();
>> acq.head
ans = 
  AcquisitionHeader with properties:

                   version: 1
                     flags: 64
           measurement_uid: 281
              scan_counter: 1
    acquisition_time_stamp: 23187639
     physiology_time_stamp: [3×1 uint32]
         number_of_samples: 256
        available_channels: 2
           active_channels: 2
              channel_mask: [16×1 uint64]
               discard_pre: 0
              discard_post: 0
             center_sample: 128
        encoding_space_ref: 0
     trajectory_dimensions: 0
            sample_time_us: 15
                  position: [3×1 single]
                  read_dir: [3×1 single]
                 phase_dir: [3×1 single]
                 slice_dir: [3×1 single]
    patient_table_position: [3×1 single]
                       idx: [1×1 struct]
                  user_int: [8×1 int32]
                user_float: [8×1 single]
                     FLAGS: [1×1 struct]
```

MATLAB also provides [native HDF5 support](https://www.mathworks.com/help/matlab/import_export/import-hdf5-files.html) which can be used to read the data without an external library:
```matlab
>> data = h5read('data.mrd', '/dataset/data')
data = 
  struct with fields:

    head: [1×1 struct]
    traj: {128×1 cell}
    data: {128×1 cell}

>> data.head
ans = 
  struct with fields:

                   version: [128×1 uint16]
                     flags: [128×1 uint64]
           measurement_uid: [128×1 uint32]
              scan_counter: [128×1 uint32]
    acquisition_time_stamp: [128×1 uint32]
     physiology_time_stamp: [3×128 uint32]
         number_of_samples: [128×1 uint16]
        available_channels: [128×1 uint16]
           active_channels: [128×1 uint16]
              channel_mask: [16×128 uint64]
               discard_pre: [128×1 uint16]
              discard_post: [128×1 uint16]
             center_sample: [128×1 uint16]
        encoding_space_ref: [128×1 uint16]
     trajectory_dimensions: [128×1 uint16]
            sample_time_us: [128×1 single]
                  position: [3×128 single]
                  read_dir: [3×128 single]
                 phase_dir: [3×128 single]
                 slice_dir: [3×128 single]
    patient_table_position: [3×128 single]
                       idx: [1×1 struct]
                  user_int: [8×128 int32]
                user_float: [8×128 single]
>> 
```

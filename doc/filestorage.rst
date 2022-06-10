HDF5 File Storage
************

The ISMRM Raw Data format can really be stored in any format that you choose, but most commonly it is stored in `HDF5 format https://www.hdfgroup.org/solutions/hdf5`_. Briefly it is a hierarchical data format (much like a file system), which can contain multiple variable organized in groups (like folders in a file system). The variables can contain arrays of data values, custom defined structs, or simple text fields. It is the convention (but not a requirement) that the ISMRMRD datasets are stored in a group called `/dataset`. The XML configuration is stored in the variable `/dataset/xml` and the data is stored in `/dataset/data`. HDF5 files can be viewed with the HDFView application which is available on the HDF5 website for multiple platforms. Many programming languages also have support for working with HDF5 files. 

In Python, the `h5py <https://www.h5py.org/>`_ package can be used to read the files::

    import h5py
    import numpy as np
    f = h5py.File('testdata.h5')
    acq = np.array(f['dataset']['data'][0])
    { k: acq['head'][k] for k in acq['head'].dtype.fields.keys() }

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

There is also a dedicated python libarary for working with the ISMRMRD files. The Python implementation of the API is maintained in `ismrmrd-python <https://www.github.com/ismrmrd/ismrmrd-python>_`

Files can also be read directly in Matlab, in fact Matlab uses (since file format v7.3) HDF5 as its internal data format in the `.mat` files. As an example the data from an ISMRMRD file with name `myfile.h5` can be read in Matlab with a command like::

    >> data = h5read('simple_gre.h5', '/dataset/data');
    >> data

    data =

    head: [1x1 struct]
    traj: {1x1281 cell}
    data: {1x1281 cell}

      >> data.head

      ans =

                    version: [1x1281 uint16]
                      flags: [1x1281 uint64]
            measurement_uid: [1x1281 uint32]
                scan_counter: [1x1281 uint32]
      acquisition_time_stamp: [1x1281 uint32]
      physiology_time_stamp: [3x1281 uint32]
          number_of_samples: [1x1281 uint16]
          available_channels: [1x1281 uint16]
            active_channels: [1x1281 uint16]
                channel_mask: [16x1281 uint64]
                discard_pre: [1x1281 uint16]
                discard_post: [1x1281 uint16]
              center_sample: [1x1281 uint16]
          encoding_space_ref: [1x1281 uint16]
      trajectory_dimensions: [1x1281 uint16]
              sample_time_us: [1x1281 single]
                    position: [3x1281 single]
                    read_dir: [3x1281 single]
                  phase_dir: [3x1281 single]
                  slice_dir: [3x1281 single]
      patient_table_position: [3x1281 single]
                        idx: [1x1 struct]
                    user_int: [8x1281 int32]
                  user_float: [8x1281 single]

      >>

The HDF5 file format can be accessed from C, C++, and java using the libraries provided on the HDF5 website. The ISMRMRD distribution also comes with some C++ wrappers that can be used for easy access (read and write) from C++ programs. 



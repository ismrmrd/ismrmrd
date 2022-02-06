Overview
#########

The raw data format combines a mix of flexible data structures (XML header) and fixed structures (equivalent to C-structs). A raw data set consist mainly of 2 sections:

1. A flexible XML format document that can contain an arbitrary number of fields and accommodate everything from simple values (b-values, etc.) to entire vendor protocols, etc. This purpose of this XML document is to provide parameters that may be meaningful for some experiments but not for others. This XML format is defined by an XML Schema Definition file `schema/ismrmrd.xsd <https://github.com/ismrmrd/ismrmrd/blob/master/schema/ismrmrd.xsd>`_.
2. Raw data section. This section contains all the acquired data in the experiment. Each data item is preceded by a C-struct with encoding numbers, etc. Following this data header is a channel header and data for each acquired channel. The raw data headers are defined in a C/C++ header file `ismrmrd.h <https://github.com/ismrmrd/ismrmrd/blob/master/include/ismrmrd/ismrmrd.h>`_

In addition to these sections, the ISMRMRD format also specifies an image header for storing reconstructed images and the accompanying C++ library provides a convenient way of writing such images into HDF5 files along with generic arrays for storing less well defined data structures, e.g. coil sensitivity maps or other calibration data.

Flexible Data Header
********************

The flexible data structure is defined by the xml schema definition in `schema/ismrmrd.xsd <https://github.com/ismrmrd/ismrmrd/blob/master/schema/ismrmrd.xsd>`_. An example of an XML file for a Cartesian 3D acquisition can be found `schema/ismrmrd_example.xml <https://github.com/ismrmrd/ismrmrd/blob/master/schema/ismrmrd_example.xml>_

The most critical elements for image reconstruction are contained in the `<encoding>` section of the document, which describes the encoded spaced and also the target reconstructed space. Along with the `<encodingLimits>` this section allows the reconstruction program to determine matrix sizes, oversampling factors, partial Fourier, etc. In the example above, data is acquired with two-fold oversampling in the read-out (`x`) direction, which is reflected in the larger matrix size in the encoded space compared to the reconstruction space. The field of view is also twice as large in the encoded space. For the first phase encoding dimension (`y`), we have a combination of oversampling (20%), reduced phase resolution (only 83 lines of k-space acquired, and partial Fourier sampling, which is reflected in the asymmetric center of the encoding limits of the `<kspace_encoding_step_1>`. Specifically, the data lines would be placed into the encoding space like this::

    0                                     70                                         139
    |-------------------------------------|-------------------------------------------|
                            ****************************************************
                            ^               ^                                  ^
                            0              28                                  83

After FFT, only the central 116 lines are kept, i.e. there is a reduced field of view in the phase encoding direction. Center and encoding limits for the readout dimension is not given in the XML header. This is to accommodate sequences where the center of the readout may change from readout to readout (alternating directions of readout). There is a field on the individual data headers (see below) to indicate the center of the readout.

An experiment can have multiple encoding spaces and it is possible to indicate on each acquired data readout, which encoding space the data belongs to (see below).

In addition to the defined field in the xml header, it is possible to add an arbitrary number of user defined parameters to accommodate special sequence parameters. Please consult the xml `schema <https://github.com/ismrmrd/ismrmrd/blob/master/schema/ismrmrd.xsd>`_ to see how user parameters are defined. Briefly, the XML header can have a section at the end which looks like::

    <userParameters>
      <userParameterLong>
        <name>MyVar1</name><value>1003</value>
      </userParameterLong>
      <userParameterLong>
        <name>MyVar2</name><value>1999</value>
      </userParameterLong>
      <userParameterDouble>
        <name>MyDoubleVar</name><value>87.6676</value>
      </userParameterDouble>
    </userParameters>

Fixed Data structures
**********************

Each raw data acquisition is preceded by the following fixed layout structures in `ismrmrd.h <https://github.com/ismrmrd/ismrmrd/blob/master/include/ismrmrd/ismrmrd.h#L225>`_.

The interpretation of some of these fields may vary from sequence to sequence, i.e. for a Cartesian sequence, `kspace_encode_step_1` would be the phase encoding step, for a spiral sequence where phase encoding direction does not make sense, it would be the spiral interleave number. The `encoding_space_ref` enables the user to tie an acquisition to a specific encoding space (see above) in case there are multiple, e.g. in situations where a calibration scan may be integrated in the acquisition.

The flags field is a bit mask, which in principle can be used freely by the user, but suggested flag values are given in `ismrmrd.h`, it is recommended not to use already designated flag bits for custom purposes. There are a set of bits reserved for prototyping (bits 57-64), please see `ismrmrd.h` for details.

The header contains a `trajectory_dimensions` field. If the value of this field is larger than 0, it means that trajectories are stored with each individual acquisition. For a 2D acquisition, the `trajectory_dimensions` would typically be 2 and the convention (for memory layout) is that the header is followed immediately by the trajectory before the complex data. There is an example of how this memory layout could be implemented with a C++ class in the `ismrmrd.h` file::

    class Acquisition
    {

    //....

    AcquisitionHeader head_; //Header, see above

    float* traj_;            //Trajectory, elements = head_.trajectory_dimensions*head_.number_of_samples
                            //   [kx,ky,kx,ky.....]        (for head_.trajectory_dimensions = 2)
                            //   [kx,ky,kz,kx,ky,kz,.....] (for head_.trajectory_dimensions = 3)

    float* data_;            //Actual data, elements = head_.number_of_samples*head_.active_channels*2
                            //   [re,im,re,im,.....,re,im,re,im,.....,re,im,re,im,.....]
                            //    ---channel 1-------channel 2---------channel 3-----

    };

This suggested memory layout is only a suggestion. The HDF5 interface (see below) can be used to read the data into many different data structures. In fact, the user can choose to read only part of the header or not read the data, etc.

As mentioned above, the ISMRMRD format also suggests a way to store reconstructed images (or maybe image data used for calibration). An `ImageHeader` structure is defined in `ismrmrd.h <https://github.com/ismrmrd/ismrmrd/blob/master/include/ismrmrd/ismrmrd.h#L286>_.

In a similar fashion to the raw data acquisition data, the intention is to store a header followed by the image data. Since image data can be in several different format (e.g. float, complex, etc.), the memory layout is less well defined but can be described as::

    template <typename T> class Image {

    ImageHeader head_;     //ImageHeader as defined above
    T* data_;              //Data, array of size (matrix_size[0]*matrix_size[1]*matrix_size[2]*channels),
                            //first spatial dimension is fastest changing array index, channels outer most (slowest changing).
    };

Magnetic Resonance Raw Data Standard
*************************************

DRAFT - Version 0.1 - July 2012

TODO
-------------------------

* Make More Example Datasets

	- Radial
	- Diffusion
	- Phase contrast flow

* Flag definitions.

	- Do we need more flags?

* Converters for vendor raw data 

	- Siemens (Hansen)
	- Philips (Kozerke? Boernert?)
	- GE (Hargreaves?)
	- Bruker (Hansen?)

Change log
----------

August 2012 - First draft.

Preamble
---------

A prerequisite for sharing magnetic resonance (imaging) reconstruction algorithms and code is a common raw data format. This document describes such a common raw data format and attempts to capture all the data fields that are require to describe enough details about the magnetic resonance experiment to reconstruct images from the data.

This standard was developed by a subcommittee of the ISMRM Sedona 2013 workshop. Comments and requests for additions/modifications can be sent to:

* Michael S. Hansen (michael.hansen@nih.gov)
* Wally Block (wblock@cae.wisc.edu)
* Mark Griswold (mag46@case.edu)
* Brian Hargreaves (bah@stanford.edu)
* Peter Boernert (peter.boernert@philips.com)


Overview
---------

The raw data format combines a mix of flexible data structures (XML header) and fixed structures (equivalent to C-structs). A raw data set consist of 2 sections:

1. A flexible XML format document that can contain an arbitrary number of fields and accomodate everything from simple values (b-values, etc.) to entire vendor protocols, etc. This purpose of this XML document is to provide parameters that may be meaningful for some experiments but not for others. This XML format is defined by an XML Schema Definition file (ismrmrd.xsd). 
2. Raw data section. This section contains all the acquired data in the experiment. Each data item is preceeded by a C-struct with encoding numbers, etc. Following this data header is a channel header and data for each acquired channel. The raw data headers are defined in a C/C++ header file (ismrmrd.h)

Flexible Data Header
.....................

The flexible data structure is defined by the xml schema definition in ``schema/ismrmrd.xsd`` (schema_ is included in appendix below).

An example of an XML file for a Cartesian 3D acquisition could look like:

.. include:: ../schema/ismrmrd_example.xml
   :literal:

The most critical elements for image reconstruction are contained in the ``<encoding>`` section of the document, which describes the encoded spaced and also the target reconstructed space. Along with the ``<encodingLimits>`` this section allows the reconstruction program to determine matrix sizes, oversampling factors, partial fourier, etc. In the example above, data is acquired with two-fold oversampling in the read-out (``x``) direction, which is reflected in the larger matrix size in the encoded space compared to the reconstruction space. The field of view is also twice as large in the encoded space. For the first phase encoding dimension (``y``), we have a combination of oversamling (20%), reduced phase resolution (only 83 lines of k-space acquired, and partial Fourier sampling, which is reflected in the asymetric center of the encoding limits of the ``<kspace_encoding_step_1>``. Specifically, the data lines would be placed into the encoding space like this:

::

   0                                     70                                         139
   |-------------------------------------|-------------------------------------------|
                         ****************************************************
                         ^               ^                                  ^
                         0              28                                  83

After FFT, only the central 116 lines are kept, i.e. there is a reduced field of view in the phase encoding direction. Center and encoding limits for the readout dimension is not given in the XML header. This is to accomodate sequences where the center of the readout may change from readout to readout (alternating directions of readout). There is a field on the individual data headers (see below) to indicate the center of the readout.

An experiment can have multiple encoding spaces and it is possible to indicate on each acquired data readout, which encoding space the data belongs to (see below).

In addition to the defined field in the xml header, it is possible to add an arbitrary number of user defined parameters to accomodate special sequence parameters. Please consult the xml schema_ to see how user parameters are defined. Briefly, the XML header can have a section at the end which looks like:

::

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
......................

Each acquisition is preceeded by the following fixed layout structure:

.. include:: ../ismrmrd.h
   :literal:
   :start-line: 116
   :end-line: 141

Where EncodingCounters are defined as:

.. include:: ../ismrmrd.h
   :literal:
   :start-line: 100
   :end-line: 112

The interpretation of some of these fields may vary from sequence to sequence, i.e. for a Cartesian sequence, ``kspace_encode_step_1`` would be the phase encoding step, for a spiral sequence where phase encoding direction does not make sense, it would be the spiral interleave number. The ``encoding_space_ref`` enables the user to tie an acquisition to a specific encoding space (see above) in case there are multiple, e.g. in situations where a calibration scan may be integrated in the acquition. 

The flags field is a bitmask, which in principle can be used freely by the user, but suggested flag values are given in ``ismrmrd.h``, it is recommended not to use already designated flag bits for custom purposes. Ther are a set of bits reserved for prototyping (bits 57-64), please see ``ismrmrd.h`` for details. 

The header contains a ``trajectory_dimensions`` field. If the value of this field is larger than 0, it means that trajectories are stored with each invidual acquisition. For a 2D acquisition, the ``trajectory_dimensions`` would typically be 2 and the convention (for memory layout) is that the header is followed immediately by the trajectory before the complex data. There is an example of how this memory layout could be implemented with a C++ class in the ``ismrmrd.h`` file:

::
   
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

This suggested memory layout is only a suggestion. The HDF5 interface (see below) can be used to read the data into many different datastructures. In fact, the user can choose to read only part of the header or not read the data, etc. 

File Storage
-------------

The ISMRM Raw Data format is stored in HDF5 format. Details on this format can be found at the HDF5_ website. Briefly it is a hierarchical dataformat (much like a file system), which can contain multiple variable organized in groups (like folders in a file system). The variables can contain arrays of data values, custom defined structs, or simple text fields. It is the convention (but not a requirement) that the ISMRMRD datasets are stored in a group called ``/dataset``. The XML configuration is stored in the variable ``/dataset/xml`` and the data is stored in ``/dataset/data``. HDF5 files can ve viewed with the HDFView application which is available on the HDF5 website for multiple platforms. Files can also be read directly in Matlab, in fact Matlab uses (since fileformat v7.3) HDF5 as its internal dataformat in the ``.mat`` files. As an example the data from an ISMRMRD file with name ``myfile.h5`` can be read in matlab with a command like:

::

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
                quaternion: [4x1281 single]
    patient_table_position: [3x1281 single]
                       idx: [1x1 struct]
                  user_int: [8x1281 int32]
                user_float: [8x1281 single]

    >> 

The HDF5 file format can be access from C, C++, and java using the libraries provided on the HDF5 website. The ISMRMRD distribution also comes with some C++ wrappers that can be used for easy access (read and write) from C++ programs. See below.

.. _HDF5: http://www.hdfgroup.org/HDF5/

C++ Support Library
--------------------

To enable easy prototyping of C++ software using the ISMRMRD data format, a simple C++ wrapper class is provided (defined in ``ismrmrd_hdf5.h``):

::

   class EXPORTISMRMRD IsmrmrdDataset
   {
    public:
	    IsmrmrdDataset(const char* filename, const char* groupname, bool create_file_if_needed = true);
	    int appendAcquisition(Acquisition* a);
	    int writeHeader(std::string& xml);

	    boost::shared_ptr<std::string> readHeader();
	    boost::shared_ptr<Acquisition> readAcquisition(unsigned long index = 0);
	    unsigned long getNumberOfAcquisitions();
    };


Using this wrapper, C++ applications can be programmed as:
    
::
    
   boost::shared_ptr<ISMRMRD::IsmrmrdDataset> ismrmrd_dataset(new ISMRMRD::IsmrmrdDataset(hdf5_in_data_file,hdf5_in_group));
   boost::shared_ptr<std::string> xml_config = ismrmrd_dataset->readHeader();

   //Do something with the header

   unsigned long acquisitions = ismrmrd_dataset->getNumberOfAcquisitions();

   for (unsigned long int i = 0; i < acquisitions; i++) {
     boost::shared_ptr<ISMRMRD::Acquisition> acq_tmp = ismrmrd_dataset->readAcquisition(i);	     
     //Do something with the data
   }

Since the XML part of the header is defined in the ``schema/ismrmrd.xsd`` file, it is possible to use XML data binding tools such as CodeSynthesys XSD to generate a C++ class representation of the header for easy access to the fields. The ``cmake`` build files that accompany the ISMRMRD distribution automatically tries to find CodeSynthesis XSD and generate such a binding. With the C++ representation of the header it can be parsed with something like:

::

   xml_schema::properties props;
   props.schema_location ("http://www.ismrm.org/ISMRMRD",std::string("/full/path/to/ismrmrd.xsd"));
   std::istringstream str_stream(xml, std::stringstream::in);
   boost::shared_ptr<ISMRMRD::ismrmrdHeader> cfg;
   
   try {
      cfg = boost::shared_ptr<ISMRMRD::ismrmrdHeader>(ISMRMRD::ismrmrdHeader_ (str_stream,0,props));
   }  catch (const xml_schema::exception& e) {
      std::cout << "Failed to parse XML Parameters: " << e.what() << std::endl;
   }

   //Use the configuration, e.g.:
   std::cout << "Number of encoding spaces: " << cfg->encoding().size() << std::endl;

Again, this is not a requirement for using the ISMRMRD format, the XML can be parsed with numerous other xml parsing libraries. The schema file ``schema/ismrmrd.xsd`` gives the user the option of validating the XML header before parsing, which is recommeded to reduce the chance of hard to detect errors in your code due to missing or malformed parameters. 

Matlab Example Code and Datasets
--------------------------------

The ``examples`` folder contains datasets and some matlab code to illustrate simple interaction with the ISMRMRD data format. For instance, to reconstruct a 2D Cartesian acquisition (10 image repetitions), type (from the ``examples/matlab`` folder):

::

   >> images = simple_cartesian_recon('../data/simple_gre.h5');
   Reconstructing image 1....done
   Reconstructing image 2....done
   Reconstructing image 3....done
   Reconstructing image 4....done
   Reconstructing image 5....done
   Reconstructing image 6....done
   Reconstructing image 7....done
   Reconstructing image 8....done
   Reconstructing image 9....done
   Reconstructing image 10....done
   >> 

You should see one of the reconstructed images display. An example is also given of a 3D acquisition with partial Fourier, phase and slice oversampling, etc. Reconstruct this dataset with:

::

   >> images = simple_cartesian_recon('../data/3D_partial_fourier.h5');
   Reconstructing image 1....done

The center slice of the volume should be displayed at the end of the reconstruction. 

Finally, there is also a spiral dataset. This dataset illustrates how the flexible section of the ``<trajectoryDescription>`` can be used to add user defined parameters and an identifier to describe the trajectory. This dataset is also an example of storing the trajectory with the data for direct reconstruction. Reconstruct this dataset with:

::

   >> images = simple_spiral_recon('../data/simple_spiral.h5');
   Reconstructing image 1....done
   Reconstructing image 2....done
   Reconstructing image 3....done
   Reconstructing image 4....done
   Reconstructing image 5....done
   Reconstructing image 6....done
   Reconstructing image 7....done
   Reconstructing image 8....done
   Reconstructing image 9....done
   Reconstructing image 10....done
   >> 

Appendix
---------

XML Schema Definition
......................
.. _schema:

.. include:: ../schema/ismrmrd.xsd
   :literal:


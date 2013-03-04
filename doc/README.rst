ISMRM Raw Data Format (ISMRMRD)
*******************************

.. contents::

Preamble
---------

A prerequisite for sharing magnetic resonance (imaging) reconstruction algorithms and code is a common raw data format. This document describes such a common raw data format and attempts to capture the data fields that are require to describe enough details about the magnetic resonance experiment to reconstruct images from the data.

This standard was developed by a subcommittee of the ISMRM Sedona 2013 workshop. Comments and requests for additions/modifications can be sent to:

* Michael S. Hansen (michael DOT hansen AT nih DOT gov)
* Wally Block (wblock AT cae DOT wisc DOT edu)
* Mark Griswold (mag46 AT case DOT edu)
* Brian Hargreaves (bah AT stanford DOT edu)
* Peter Boernert (peter DOT boernert AT philips DOT com)
* Sebastian Kozerke (kozerke AT biomed DOT ee DOT ethz DOT ch)
* Craig Meyer (cmeyer AT virginia DOT edu)
* Doug Noll (dnoll AT umich DOT edu)
* Jim Pipe (Jim.Pipe AT DignityHealth DOT org)

Developers/Contributors
------------------------

* Michael S. Hansen, National Institutes of Health, USA
* Nick Zwart, Barrow Neurological Institute, Phoenix, Arizona
* Souheil Inati, National Institutes of Health, USA
* Joe Naegele, National Institutes of Health, USA
* Kaveh Vahedipour, Juelich Research Centre, Juelich, Germany

Obtaining and Installing
-------------------------

The source code, examples, and example datasets can be found on the ISMRM Raw Data Sourceforge website_.

.. _website: http://sourceforge.net/projects/ismrmrd

To download the source code, clone the git archive::

  git clone git://git.code.sf.net/p/ismrmrd/code ismrmrd-code

Alternatively download the zip file with the source code::

  wget https://sourceforge.net/projects/ismrmrd/files/src/ismrmrd_latest.zip

API Documentation can be found at http://ismrmrd.sourceforge.net/api.


Dependencies
.............

The ISMRM Raw Data format is described by an XML schema_ and some C-style structs with fixed memory layout and as such does not have dependencies. However, it uses HDF5 files for storage and a C++ library for reading and writing the ISMRMRD files is included in this distribution. Furthermore, since the XML header is defined with an XML schema_, we encourage using XML data binding when writing software using the format. In the ISMRMRD distribution we use the CodeSynthesis XSD (http://www.codesynthesis.com/products/xsd/) package to generate a C++ class representation of the XML schema. To compile all components of this distribution you need:


* HDF5 (version 1.8 or higher) libraries. Available from http://www.hdfgroup.org/downloads/index.html.
* Boost (http://www.boost.org/)
* CodeSynthesis XSD (http://www.codesynthesis.com/products/xsd/)
* Xerces-C XML parser library (http://xerces.apache.org/xerces-c/)
* Cmake build tool (http://www.cmake.org/)
* Doxygen if you would like to generate API documentation (http://www.doxygen.org)
* Git if you would like to use the source code archive (http://git-scm.com/)
* FFTW if you would like to compile some of the example applications
  (http://www.fftw.org)

.. note:: It is only necessary to install the dependencies if you wish to develop compiled C/C++ software, which uses the ISMRMRD format. The format can be read in Matlab without installing any additional software.


Linux installation
...................

The dependencies mentioned above should be included in most linux distributions. On Ubuntu you can install all required dependencies with::

  sudo apt-get install libhdf5-serial-dev h5utils cmake cmake-curses-gui libboost-dev libboost-thread-dev libboost-system-dev libxerces-c-dev xsdcxx doxygen git

After installation of dependencies, the library can be installed with::

  git clone git://git.code.sf.net/p/ismrmrd/code ismrmrd-code
  cd ismrmrd-code/
  mkdir build
  cd build
  cmake ../
  make
  sudo make install

Last command will install the library in ``/usr/local/ismrmrd``.

Mac OSX Installation
.....................

There are numerous different package management systems for Mac. In this example, we have used MacPorts (http://www.macports.org/). First install the dependencies available in MacPorts::

   sudo port install wget hdf5-18 h5utils boost xercesc3 git-core git-extras cmake doxygen

Next install CodeSynthesis XSD (http://www.codesynthesis.com/products/xsd/)::

   wget http://www.codesynthesis.com/download/xsd/3.3/macosx/i686/xsd-3.3.0-i686-macosx.tar.bz2
   tar -xzf xsd-3.3.0-i686-macosx.tar.bz2
   cd xsd-3.3.0-i686-macosx
   sudo cp bin/xsd /usr/local/bin/
   sudo cp -r libxsd/xsd /usr/local/include/

See faq_ for details on problems on some Mac OSX Lion systems.

Last step is to download and compile::

   git clone git://git.code.sf.net/p/ismrmrd/code ismrmrd-code
   cd ismrmrd-code/
   mkdir build
   cd build/
   cmake ../
   make
   sudo make install

Last command will install the library in ``/usr/local/ismrmrd``.


Windows Installation
.....................

Setting up a Windows development environment is usually a bit more challenging than working on Unix platforms where most library dependencies are easily installed with package management systems (see above). The general Windows installation instructions (you may have to make adjustments for your setup) is as follows:

* Starting with a Windows 7 (64-bit) machine with Visual Studio 2010 installed.

* Install CMake (http://www.cmake.org/files/v2.8/cmake-2.8.9-win32-x86.exe)

* Install Git (http://msysgit.googlecode.com/files/Git-1.7.11-preview20120710.exe)

* Install HDF5 (http://www.hdfgroup.org/ftp/HDF5/current/bin/windows/HDF5189-win64-vs10-shared.zip)

* Install HDFView (http://www.hdfgroup.org/ftp/HDF5/hdf-java/hdfview/hdfview_install_win64.exe)

* Install CodeSynthesis XSD (http://www.codesynthesis.com/download/xsd/3.3/windows/i686/xsd-3.3.msi)

* Install Boost (http://boostpro.com/download/x64/boost_1_51_setup.exe)

  - Just install everything for VS2010 and worry about which versions you need later.

* Install FFTW (ftp://ftp.fftw.org/pub/fftw/fftw-3.3.2-dll64.zip)

  - You need to create ``.lib`` files manually after installing. See instructions at http://www.fftw.org/install/windows.html

* Make sure the paths to your new libraries are in your PATH environment variable:

  - Boost libraries  (typically ``C:\Program Files\boost\boost_1_51\lib``)
  - Code Synthesis XSD (typically ``C:\Program Files (x86)\CodeSynthesis XSD 3.3\bin\;C:\Program Files (x86)\CodeSynthesis XSD 3.3\bin64\``)
  - FFTW libraries (typically ``C:\MRILibraries\fftw3``)
  - HDF5 libraries (typically ``C:\Program Files\HDF Group\HDF5\1.8.9\bin``)
  - ISMRMRD (typically ``C:\Program Files\ismrmrd\bin;C:\Program Files\ismrmrd\bin``)

This can seem a bit daunting, we have included a Windows powershell_ script, which you can use to guide you through the installation process.

After installing all dependencies, download the code, e.g. from a git bash shell::

   git clone git://git.code.sf.net/p/ismrmrd/code ismrmrd-code
   cd ismrmrd-code/
   mkdir build
   cd build/
   cmake-gui.exe

Last command will open CMake's graphical user interface. Hit the configure button and deal with the dependencies that CMake is unable to find. Hit configure again and repeat the process until CMake has enough information to configure. Once the configuration is complete, you can hit generate to generate a Visual Studio project, which you can open and use to build ISMRMRD. There are step-by-step commands included in the powershell_ script below to guide you through the CMake configuration and build process from the command line. The command line CMake configuration line (assuming you have installed with the paths above), would look something like (backslashes are just there to break the command over multiple lines)::

    cmake -G"Visual Studio 10 Win64" -DBOOST_ROOT=C:/Program Files/boost/boost_1_51 \
	-DXERCESC_INCLUDE_DIR=C:/Program Files (x86)/CodeSynthesis XSD 3.3/include/xercesc \
	-DXERCESC_LIBRARIES=C:/Program Files (x86)/CodeSynthesis XSD 3.3/lib64/vc-10.0/xerces-c_3.lib \
	-DXSD_DIR=C:/Program Files (x86)/CodeSynthesis XSD 3.3 \
	-DFFTW3_INCLUDE_DIR=C:/MRILibraries/fftw3 \
	-DFFTW3F_LIBRARY=C:/MRILibraries/fftw3/libfftw3f-3.lib ../

Again, you may have to adjust for your specific installation paths. After generating the Visual Studio project, you can build from a Visual Studio Command Prompt with::

   msbuild .\ISMRMRD.sln /p:Configuration=Release


Overview
---------

The raw data format combines a mix of flexible data structures (XML header) and fixed structures (equivalent to C-structs). A raw data set consist mainly of 2 sections:

1. A flexible XML format document that can contain an arbitrary number of fields and accommodate everything from simple values (b-values, etc.) to entire vendor protocols, etc. This purpose of this XML document is to provide parameters that may be meaningful for some experiments but not for others. This XML format is defined by an XML Schema Definition file (ismrmrd.xsd).
2. Raw data section. This section contains all the acquired data in the experiment. Each data item is preceded by a C-struct with encoding numbers, etc. Following this data header is a channel header and data for each acquired channel. The raw data headers are defined in a C/C++ header file (ismrmrd.h)

In addition to these sections, the ISMRMRD format also specifies an image header for storing reconstructed images and the accompanying C++ library provides a convenient way of writing such images into HDF5 files along with generic arrays for storing less well defined data structures, e.g. coil sensitivity maps or other calibration data.

Flexible Data Header
.....................

The flexible data structure is defined by the xml schema definition in ``schema/ismrmrd.xsd`` (schema_ is included in appendix below).

An example of an XML file for a Cartesian 3D acquisition could look like:

.. include:: ../schema/ismrmrd_example.xml
   :literal:

The most critical elements for image reconstruction are contained in the ``<encoding>`` section of the document, which describes the encoded spaced and also the target reconstructed space. Along with the ``<encodingLimits>`` this section allows the reconstruction program to determine matrix sizes, oversampling factors, partial Fourier, etc. In the example above, data is acquired with two-fold oversampling in the read-out (``x``) direction, which is reflected in the larger matrix size in the encoded space compared to the reconstruction space. The field of view is also twice as large in the encoded space. For the first phase encoding dimension (``y``), we have a combination of oversampling (20%), reduced phase resolution (only 83 lines of k-space acquired, and partial Fourier sampling, which is reflected in the asymmetric center of the encoding limits of the ``<kspace_encoding_step_1>``. Specifically, the data lines would be placed into the encoding space like this:

::

   0                                     70                                         139
   |-------------------------------------|-------------------------------------------|
                         ****************************************************
                         ^               ^                                  ^
                         0              28                                  83

After FFT, only the central 116 lines are kept, i.e. there is a reduced field of view in the phase encoding direction. Center and encoding limits for the readout dimension is not given in the XML header. This is to accommodate sequences where the center of the readout may change from readout to readout (alternating directions of readout). There is a field on the individual data headers (see below) to indicate the center of the readout.

An experiment can have multiple encoding spaces and it is possible to indicate on each acquired data readout, which encoding space the data belongs to (see below).

In addition to the defined field in the xml header, it is possible to add an arbitrary number of user defined parameters to accommodate special sequence parameters. Please consult the xml schema_ to see how user parameters are defined. Briefly, the XML header can have a section at the end which looks like:

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

Each raw data acquisition is preceded by the following fixed layout structure:

.. include:: ../ismrmrd.h
   :literal:
   :start-line: 133
   :end-line: 160

Where EncodingCounters are defined as:

.. include:: ../ismrmrd.h
   :literal:
   :start-line: 117
   :end-line: 129

The interpretation of some of these fields may vary from sequence to sequence, i.e. for a Cartesian sequence, ``kspace_encode_step_1`` would be the phase encoding step, for a spiral sequence where phase encoding direction does not make sense, it would be the spiral interleave number. The ``encoding_space_ref`` enables the user to tie an acquisition to a specific encoding space (see above) in case there are multiple, e.g. in situations where a calibration scan may be integrated in the acquisition.

The flags field is a bit mask, which in principle can be used freely by the user, but suggested flag values are given in ``ismrmrd.h``, it is recommended not to use already designated flag bits for custom purposes. There are a set of bits reserved for prototyping (bits 57-64), please see ``ismrmrd.h`` for details.

The header contains a ``trajectory_dimensions`` field. If the value of this field is larger than 0, it means that trajectories are stored with each individual acquisition. For a 2D acquisition, the ``trajectory_dimensions`` would typically be 2 and the convention (for memory layout) is that the header is followed immediately by the trajectory before the complex data. There is an example of how this memory layout could be implemented with a C++ class in the ``ismrmrd.h`` file:

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

This suggested memory layout is only a suggestion. The HDF5 interface (see below) can be used to read the data into many different data structures. In fact, the user can choose to read only part of the header or not read the data, etc.

As mentioned above, the ISMRMRD format also suggests a way to store reconstructed images (or maybe image data used for calibration). An ``ImageHeader`` structure is defined in ``ismrmrd.h``:

.. include:: ../ismrmrd.h
   :literal:
   :start-line: 196
   :end-line: 224

In a similar fashion to the raw data acquisition data, the intention is to store a header followed by the image data. Since image data can be in several different format (e.g. float, complex, etc.), the memory layout is less well defined but can be described as::

  template <typename T> class Image {

  ImageHeader head_;     //ImageHeader as defined above
  T* data_;              //Data, array of size (matrix_size[0]*matrix_size[1]*matrix_size[2]*channels),
                         //first spatial dimension is fastest changing array index, channels outer most (slowest changing).
  };



File Storage
-------------

The ISMRM Raw Data format is stored in HDF5 format. Details on this format can be found at the HDF5_ website. Briefly it is a hierarchical data format (much like a file system), which can contain multiple variable organized in groups (like folders in a file system). The variables can contain arrays of data values, custom defined structs, or simple text fields. It is the convention (but not a requirement) that the ISMRMRD datasets are stored in a group called ``/dataset``. The XML configuration is stored in the variable ``/dataset/xml`` and the data is stored in ``/dataset/data``. HDF5 files can be viewed with the HDFView application which is available on the HDF5 website for multiple platforms. Files can also be read directly in Matlab, in fact Matlab uses (since file format v7.3) HDF5 as its internal data format in the ``.mat`` files. As an example the data from an ISMRMRD file with name ``myfile.h5`` can be read in matlab with a command like:

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
           readout_cosines: [3x1281 single]
             phase_cosines: [3x1281 single]
             slice_cosines: [3x1281 single]
    patient_table_position: [3x1281 single]
                       idx: [1x1 struct]
                  user_int: [8x1281 int32]
                user_float: [8x1281 single]

    >>

The HDF5 file format can be access from C, C++, and java using the libraries provided on the HDF5 website. The ISMRMRD distribution also comes with some C++ wrappers that can be used for easy access (read and write) from C++ programs. See below.

In addition to storing acquisition data and images as defined by the headers above, the HDF5 format also enables storage of generic multi-dimensional arrays. The ISMRMRD format does not explicitly define how such data should be stored, but leaves it open for the user to add variables and data as dictated by a given application.

.. _HDF5: http://www.hdfgroup.org/HDF5/

C++ Support Library
--------------------

To enable easy prototyping of C++ software using the ISMRMRD data format, a simple C++ wrapper class is provided (defined in ``ismrmrd_hdf5.h``):

::

   class EXPORTISMRMRD IsmrmrdDataset
   {
    public:

	IsmrmrdDataset(const char* filename, const char* groupname, bool create_file_if_needed = true)
	: filename_(filename)
	, groupname_(groupname)
	, file_open_(false)
	, dataset_open_(false)
	, create_file_if_needed_(create_file_if_needed)
	{
		std::ifstream ifile(filename_.c_str());
		file_exists_ = ifile;

		if (openHDF5File() < 0) {
			std::cerr << "IsmrmrdDataset: Error opening HDF file." << std::endl;
		}

		if (!linkExists(groupname_.c_str())) {
			if (createGroupForDataset(groupname_.c_str()) < 0) {
				std::cerr << "IsmrmrdDataset: Error create HDF5 group." << std::endl;
			}
		}

		xml_header_path_ = groupname_ + std::string("/xml");
		data_path_ = groupname_ + std::string("/data");
 	}

	int appendAcquisition(Acquisition* a);
	boost::shared_ptr<Acquisition> readAcquisition(unsigned long index = 0);
	unsigned long getNumberOfAcquisitions();

	int writeHeader(std::string& xml);
	boost::shared_ptr<std::string> readHeader();

	template <typename T> int appendImage(Image<T>& m, const char* varname);
	template <typename T> boost::shared_ptr< Image<T> > readImage(const char* varname, unsigned long index = 0);

	int appendImageHeader(ImageHeader& h, const char* varname);
	boost::shared_ptr< ImageHeader > readImageHeader(const char* varname, unsigned long index = 0);

	template <typename T> int appendArray(NDArrayContainer<T>& a, const char* varname);
	template <typename T> int appendArray(std::vector<unsigned int>& dimensions, T* data, const char* varname);
	template <typename T> boost::shared_ptr< NDArrayContainer<T> > readArray(const char* varname, unsigned long index = 0);
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

Again, this is not a requirement for using the ISMRMRD format, the XML can be parsed with numerous other xml parsing libraries. The schema file ``schema/ismrmrd.xsd`` gives the user the option of validating the XML header before parsing, which is recommended to reduce the chance of hard to detect errors in your code due to missing or malformed parameters.

C++ Example Applications
..........................

The distribution includes two example applications, one that creates a simple 2D single-channel dataset from scratch and one that reconstructs this dataset (you need FFTW installed to compile these test applications). The data generation application looks like this (``test_create_dataset.cpp``):

.. include:: ../examples/c++/test_create_dataset.cpp
   :literal:
   :start-after: /* MAIN APPLICATION */

To reconstruct this synthetic dataset, you can use the test reconstruction application (``test_recon_dataset.cpp``):

.. include:: ../examples/c++/test_recon_dataset.cpp
   :literal:
   :start-after: /* MAIN APPLICATION */


Matlab Example Code and Datasets
--------------------------------

The ``examples`` folder contains some matlab code to illustrate simple interaction with the ISMRMRD data format. The examples use test data sets, wich can be downloaded from the Sourceforge website_. Go to the ``examples/data`` folder and type the following to download the data::

  wget https://sourceforge.net/projects/ismrmrd/files/data/3D_partial_fourier.h5
  wget https://sourceforge.net/projects/ismrmrd/files/data/simple_gre.h5
  wget https://sourceforge.net/projects/ismrmrd/files/data/simple_spiral.h5


For instance, to reconstruct a 2D Cartesian acquisition (10 image repetitions), type (from the ``examples/matlab`` folder):

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

Frequently Asked Questions
...........................
.. _faq:

*   I am trying to compile on Mac OSX Lion and I am getting strange errors.

	in version 3.3.0 of CodeSynthesis XSD there is a problem with the clang compiler. It can build with clang on os x 10.7 (Lion) by editing
	`/usr/local/include/xsd/cxx/zc-istream.txx`. Change line 35 from::

      		setg (b, b, e);

	to::

      		std::streambuf::setg (b, b, e);

	See also:
	http://codesynthesis.com/pipermail/xsd-users/2011-May/003283.html

XML Schema Definition
......................
.. _schema:

.. include:: ../schema/ismrmrd.xsd
   :literal:

Windows 7 (64-bit) Powershell Install Script
.............................................
.. _powershell:

.. include:: WindowsISMRMRDInstallDependencies.ps1
   :literal:

C++ Support Library
####################

To enable easy prototyping of C++ software using the ISMRMRD data format, a simple C++ wrapper class is provided (defined in `dataset.h <https://github.com/ismrmrd/ismrmrd/blob/master/include/ismrmrd/dataset.h>_.

Using this wrapper, C++ applications can be programmed as::

    // Open dataset
    ISMRMRD::Dataset d(datafile.c_str(), "dataset", false);

    std::string xml;
    d.readHeader(xml);
    ISMRMRD::IsmrmrdHeader hdr;
    ISMRMRD::deserialize(xml.c_str(),hdr);

    // Do something with the header

    unsigned int number_of_acquisitions = d.getNumberOfAcquisitions();
    ISMRMRD::Acquisition acq;
    for (unsigned int i = 0; i < number_of_acquisitions; i++) {
        // Read one acquisition at a time
        d.readAcquisition(i, acq);

        // Do something with the data
    }

Since the XML header is defined in the `schema/ismrmrd.xsd` file, it can be
parsed with numerous xml parsing libraries. The ISMRMRD library includes an API
that allows for programmatically deserializing, manipulating, and serializing the
XML header. See the code in the `utilities <https://github.com/ismrmrd/ismrmrd/blob/master/utilities/>`_ directory for examples of how to use the XML API.

C++ Example Applications
************************

The distribution includes two example applications, one that creates a simple 2D single-channel dataset from scratch and one that reconstructs this dataset (you need FFTW installed to compile these test applications). The data generation application can be found in `utilities/generate_cartesian_shepp_logan.cpp <https://github.com/ismrmrd/ismrmrd/blob/master/utilities/generate_cartesian_shepp_logan.cpp>`_:

To reconstruct this synthetic dataset, you can use the test reconstruction application `utilities/recon_cartesian_2d.cpp <https://github.com/ismrmrd/ismrmrd/blob/master/utilities/recon_cartesian_2d.cpp>`_.


External use of ISMRMRD C++ library in other projects
******************************************************

To use ISMRMRD for your externally developed projects, add the following to your CMakeLists.txt file::

  find_package( ISMRMRD REQUIRED )
  include_directories( ${ISMRMRD_INCLUDE_DIR} )
  target_link_libraries( mytarget ISMRMRD::ISMRMRD )

then when configuring your package use set the following cmake variables (command line variant shown)::

  cmake <path to my source tree>

ISMRM Raw Data Format (ISMRMRD)
===============================

A prerequisite for sharing magnetic resonance (imaging) reconstruction algorithms and code is a common raw data format. This document describes such a common raw data format and attempts to capture the data fields that are require to describe enough details about the magnetic resonance experiment to reconstruct images from the data. This standard was developed by a subcommittee of the ISMRM Sedona 2013 workshop. Please see the [ISMRMRD Documentation](https://ismrmrd.github.io/ismrmrd) for more information.

Obtaining and Installing
-------------------------

To download the source code, clone the git archive:

    git clone https://github.com/ismrmrd/ismrmrd

API Documentation can be found at https://ismrmrd.github.io/ismrmrd/api/.

You will need CMake, HDF5, Boost and optionally Doxygen and FFTW to build the C/C++ code. Please see the ISMRMRD documentation for specific installation instructions for [Linux](https://ismrmrd.github.io/ismrmrd/index.html#linux-installation), [Mac OS X](https://ismrmrd.github.io/ismrmrd/index.html#mac-osx-installation), and [Windows](https://ismrmrd.github.io/ismrmrd/index.html#windows-installation).

Overview
---------

The raw data format combines a mix of flexible data structures (XML header) and fixed structures (equivalent to C-structs).

A raw data set consist mainly of 2 sections:

1.  A flexible XML format document that can contain an arbitrary number of fields and accommodate everything from simple values (b-values, etc.) to entire vendor protocols, etc. The purpose of this XML document is to provide parameters that may be meaningful for some experiments but not for others.  This XML format is defined by an XML Schema Definition file (ismrmrd.xsd).  Please see the [example header](https://github.com/ismrmrd/ismrmrd/blob/master/schema/ismrmrd_example_extended.xml), [schema](https://github.com/ismrmrd/ismrmrd/blob/master/schema/ismrmrd.xsd), and the [documentation](https://ismrmrd.github.io/ismrmrd/index.html#flexible-data-header) for more information.

1.  Raw data section. This section contains all the acquired data in the experiment. Each data item is preceded by a C-struct with encoding numbers, etc. Following this data header is a channel header and data for each acquired channel. The raw data headers are defined in a C/C++ header file (ismrmrd.h).  Please see the [C header](https://github.com/ismrmrd/ismrmrd/blob/master/include/ismrmrd/ismrmrd.h) and the [documentation](https://ismrmrd.github.io/ismrmrd/index.html#fixed-data-structures) for more information.

In addition to these sections, the ISMRMRD format also specifies an image header for storing reconstructed images and the accompanying C/C++ library provides a convenient way of writing such images into HDF5 files along with generic arrays for storing less well defined data structures, e.g. coil sensitivity maps or other calibration data.

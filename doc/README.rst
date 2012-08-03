Magnetic Resonance Raw Data Standard
*************************************

DRAFT - Version 0.1 - July 2012

TODO
-------------------------

* Make Example Datasets

	- Cartesian 2D/3D
	- Partial Fourier
	- Spiral
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

Michael S. Hansen, Chair (michael.hansen@nih.gov)
Wally Block (wblock@cae.wisc.edu)
Mark Griswold (mag46@case.edu)
Brian Hargreaves (bah@stanford.edu)
Peter Boernert (peter.boernert@philips.com)


Overview
---------

The raw data format combines a mix of flexible data structures (XML) and fixed structures (equivalent to C-structs). A raw data sey consist of 3 sections:

1. A flexible XML format document that can contain an arbitrary number of fields and accomodate everything from simple values (b-values, etc.) to entire vendor protocols, etc. This purpose of this XML document is to provide parameters that may be meaningful for some experiments but not for others. 
2. A fixed struct describing encoding limits. This is a C-struct which describes minimum, maximum, and center encoding indices for all encoded dimensions. 
3. Raw data section. This section contains all the acquired data in the experiment. Each data item is preceeded by a C-struct with encoding number, etc. Following this data header is a channel header and data for each acquired channel.


Fixed Data structures
----------------------

::

ISMRM Raw Data Format (ISMRMRD)
################################

A prerequisite for sharing magnetic resonance (imaging) reconstruction algorithms and code is a common raw data format. The ISMRMRD project describes such a common raw data format, which attempts to capture the data fields that are required to describe the magnetic resonance experiment with enough detail to reconstruct images. The repository also contains a C/C++ library for working with the format. This standard was developed by a subcommittee of the ISMRM Sedona 2013 workshop and is described in detail in:

Inati SJ, Naegele JD, Zwart NR, Roopchansingh V, Lizak MJ, Hansen DC, Liu CY, Atkinson D, Kellman P, Kozerke S, Xue H, Campbell-Washburn AE, Sørensen TS, Hansen MS. ISMRM Raw data format: A proposed standard for MRI raw datasets. `Magn Reson Med. 2017 Jan;77(1):411-421. <https://onlinelibrary.wiley.com/doi/10.1002/mrm.26089>`_

Please cite this paper if you use the format.

.. toctree::
  :maxdepth: 2
  :caption: Contents:

  overview
  building
  filestorage
  cpplibrary
  api


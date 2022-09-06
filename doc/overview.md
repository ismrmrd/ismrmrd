# Magnetic Resonance Data (MRD) Format
The Magnetic Resonance Data (MRD) format is a vendor neutral standard for describing data from MR acquisitions and reconstructions.  It consists of 4 major components:

1. An [MRD header](mrd_header.md) containing general metadata describing the acquisition, including MR system details and k-space sampling.  The header contains a small set of mandatory parameters common to all MR acquisitions, but is extensible to parameters for specialized acquisitions such as b-values, venc, magnetization preparation durations, etc.  The MRD header is in XML format and described by an [XML schema file](../schema/ismrmrd.xsd).

2. [Raw k-space data](mrd_raw_data.md) is stored as individual readout acquisitions.  Each readout contains the [complex raw data](RawData) for all channels, a fixed [AcquisitionHeader](AcquisitionHeader) for metadata including [encoding loop counters](EncodingCounters), and optionally corresponding [k-space trajectory](Trajectory) information.  Most datasets will be comprised of many acquisitions, each stored individually with its own AcquisitionHeader, optional trajectory, and raw data.

3. [Image data](mrd_image_data.md) is stored as either sets of 2D or 3D arrays with a fixed [ImageHeader](ImageHeader) of common properties and an extensible set of image [MetaAttributes](MetaAttributes).  Images can be organized into series of common types and multi-channel data is supported for non-coil-combined images.

4. Physiological data such as electrocardiograms, pulse oximetry, or external triggering sources are stored as individual [waveforms](mrd_waveform_data.md) along with a fixed [WaveformHeader](WaveformHeader) for metadata.
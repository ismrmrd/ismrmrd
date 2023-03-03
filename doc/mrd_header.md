# MRD Header
The flexible data structure is defined by the xml schema definition in [schema/ismrmrd.xsd](../schema/ismrmrd.xsd). An example of an XML file for a Cartesian 3D acquisition can be found [schema/ismrmrd_example.xml](../schema/ismrmrd_example.xml).

The most critical elements for image reconstruction are contained in the ``<encoding>`` section of the document, which describes the encoded spaced and also the target reconstructed space. Along with the ``<encodingLimits>``, this section allows the reconstruction program to determine matrix sizes, oversampling factors, partial Fourier, etc. In the example above, data is acquired with two-fold oversampling in the read-out (``x``) direction, which is reflected in the larger matrix size in the encoded space compared to the reconstruction space. The field of view is also twice as large in the encoded space. For the first phase encoding dimension (``y``), we have a combination of oversampling (20%), reduced phase resolution (only 83 lines of k-space acquired, and partial Fourier sampling, which is reflected in the asymmetric center of the encoding limits of the ``<kspace_encoding_step_1>``. Specifically, the data lines would be placed into the encoding space like this::

    0                                     70                                         139
    |-------------------------------------|-------------------------------------------|
                            ****************************************************
                            ^               ^                                  ^
                            0              28                                  83

After FFT, only the central 116 lines are kept, i.e. there is a reduced field of view in the phase encoding direction. Center and encoding limits for the readout dimension is not given in the XML header. This is to accommodate sequences where the center of the readout may change from readout to readout (alternating directions of readout). There is a field on the individual data headers (see below) to indicate the center of the readout.

An experiment can have multiple encoding spaces and it is possible to indicate on each acquired data readout, which encoding space the data belongs to (see below).

In addition to the defined field in the xml header, it is possible to add an arbitrary number of user defined parameters to accommodate special sequence parameters. Please consult the xml [schema](../schema/ismrmrd.xsd) to see how user parameters are defined. Briefly, the XML header can have a section at the end which looks like:

```xml
<userParameters>
  <userParameterLong>
    <name>MyVar1</name>
    <value>1003</value>
  </userParameterLong>
  <userParameterLong>
    <name>MyVar2</name>
    <value>1999</value>
  </userParameterLong>
  <userParameterDouble>
    <name>MyDoubleVar</name>
    <value>87.6676</value>
  </userParameterDouble>
</userParameters>
```
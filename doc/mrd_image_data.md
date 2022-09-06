# Image Data
MRD images are stored as a combination of [image data](ImageData), a fixed [ImageHeader](ImageHeader) of common properties, and an extensible set of [MetaAttributes](MetaAttributes).  Images can stores as individual 2D images or 3D volumes and may include multiple channels for individual receiver coils.

(ImageHeader)=
## ImageHeader
| Element Name           | Format              | Interpretation                                      |
| --                     | --                  | --                                                  |
| version                | uint16              | Major version number (currently 1)                  |
| data_type              | uint16              | Data type of the image data, e.g. short, float, complex float, etc., as defined in [MRD Image Data Types](DataTypes) |
| flags                  | uint64              | A bit mask of common attributes applicable to individual images |
| measurement_uid        | uint32              | Unique ID corresponding to the image                |
| matrix_size            | uint16 (x3)         | Number of pixels in each of the 3 dimensions in the image |
| field_of_view          | float (32 bit) (x3) | Physical size (in mm) in each of the 3 dimensions in the image |
| channels               | uint16              | Number of receiver channels in image data (stored in the 4th dimension) |
| position               | float (32 bit) (x3) | Center of the excited volume, in (left, posterior, superior) (LPS) coordinates relative to isocenter in millimeters.  NB this is different than DICOM's ImageOrientationPatient, which defines the center of the first (typically top-left) voxel. |
| read_dir               | float (32 bit) (x3) | Directional cosine of readout/frequency encoding.  If the image is [flipped or rotated to bring them into standard DICOM orientation](http://gdcm.sourceforge.net/wiki/index.php/Orientation), ***this field still corresponds to the acquisition readout/frequency direction***, but the ``ImageRowDir`` must be set in the MetaAttributes. |
| phase_dir              | float (32 bit) (x3) | Directional cosine of phase encoding (2D).  If the image is [flipped or rotated to bring them into standard DICOM orientation](http://gdcm.sourceforge.net/wiki/index.php/Orientation), ***this field still corresponds to the 2D phase encoding direction***, but the ``ImageColumnDir`` must be set in the MetaAttributes. |
| slice_dir              | float (32 bit) (x3) | For 3D data, the directional cosine of 3D phase encoding direction.  For 3D data, the slice normal, i.e. cross-product of ``read_dir`` and ``phase_dir``.  If the image is [flipped or rotated to bring them into standard DICOM orientation](http://gdcm.sourceforge.net/wiki/index.php/Orientation), ***this field still corresponds to the 3D phase encoding direction***, but the ``ImageSliceDir`` must be set in the MetaAttributes. |
| patient_table_position | float (32 bit) (x3) | Offset position of the patient table, in LPS coordinates |
| average                | uint16              | Signal average                                      |
| slice                  | uint16              | Slice number (multi-slice 2D)                       |
| contrast               | uint16              | Echo number in multi-echo                           |
| phase                  | uint16              | Cardiac phase                                       |
| repetition             | uint16              | Counter in repeated/dynamic acquisitions            |
| set                    | uint16              | Sets of different preparation, e.g. flow encoding, diffusion weighting |
| acquisition_time_stamp | uint32              | Clock time stamp (e.g. milliseconds since midnight) |
| physiology_time_stamp  | uint32 (x3)         | Time stamps relative to physiological triggering, e.g. ECG, pulse oximetry, respiratory. Multiplicity defined by ISMRMRD_PHYS_STAMPS (currently 3) |
| image_type             | uint16              | Interpretation type of the image, e.g. magnitude, phase, as defined in [MRD Image Types](ImageTypes)   |
| image_index            | uint16              | Image index number within a series of images, corresponding to DICOM InstanceNumber (0020,0013)              |
| image_series_index     | uint16              | Series index, used to separate images into different series, corresponding to DICOM SeriesNumber (0020,0011) |
| user_int               |  int32 (x8)         | User-defined integer parameters, multiplicity defined by MRD_USER_INTS (currently 8) |
| user_float             | float (32 bit) (x8) | User-defined float parameters, multiplicity defined by MRD_USER_FLOATS (currently 8) |
| attribute_string_len   | uint32              | Length of serialized MetaAttributes text |

(DataTypes)=
### Data Types
The ``data_type`` field of the ImageHeader describes the data type and precision of the image data.  The following types are supported:
| Value        | Name         | Type           | Size        |
| --           | --           | --             | --          |
| 1            | MRD_USHORT   | uint16_t       |     2 bytes |
| 2            | MRD_SHORT    | int16_t        |     2 bytes |
| 3            | MRD_UINT     | uint32_t       |     4 bytes |
| 4            | MRD_INT      | int32_t        |     4 bytes |
| 5            | MRD_FLOAT    | float          |     4 bytes |
| 6            | MRD_DOUBLE   | double         |     8 bytes |
| 7            | MRD_CXFLOAT  | complex float  | 2 * 4 bytes |
| 8            | MRD_CXDOUBLE | complex double | 2 * 8 bytes |

(ImageTypes)=
### Image Types
The ``image_type`` field of the ImageHeader is an enum describing the image type with the following values:
| Value        | Name                 |
| --           | --                   |
| 1            | MRD_IMTYPE_MAGNITUDE |
| 2            | MRD_IMTYPE_PHASE     |
| 3            | MRD_IMTYPE_REAL      |
| 4            | MRD_IMTYPE_IMAG      |
| 5            | MRD_IMTYPE_COMPLEX   |
| 6            | MRD_IMTYPE_RGB       |

A value of ``6`` is used for 8-bit RGB color images, which have the following settings:
- ``image_type`` is set to ``MRD_IMTYPE_RGB``
- ``data_type`` is set to ``MRD_USHORT``
- ``channels`` is set to 3, representing the red, green, and blue channels of the RGB image
- image data values are in the range 0-255 (8-bit color depth)

(MetaAttributes)=
## MetaAttributes
Image metadata can be stored in the extensible MRD MetaContainer format.  This is serialized as XML text such as:
```xml
<ismrmrdMeta>
    <meta>
        <name>DataRole</name>
        <value>Image</value>
        <value>AVE</value>
        <value>NORM</value>
        <value>MAGIR</value>
    </meta>
    <meta>
        <name>ImageNumber</name>
        <value>1</value>
    </meta>
</ismrmrdMeta>
```
A variable number of "meta" elements can be defined, each with a single name and one or more value sub-elements.  The following table lists standardized attributes which should be used when appropriate, but custom "meta" elements can also be added.

| MRD Element Name  | Format       | DICOM Tag                                                                                 | Interpretation                                      |
| --                | --           | --                                                                                        | --                                                  |
| DataRole          | text array   | N/A                                                                                       | Characteristics of the image. <br><br> A value of “Quantitative” indicates that pixel values in the image are parametric and to be interpreted directly (e.g. T1 values, velocity, etc.). If this role is present, pixel values are not further modified in the ICE chain, e.g. by normalization. |
| SeriesDescription | text array   | [SeriesDescription](http://dicomlookup.com/lookup.asp?sw=Tnumber&q=(0008,103E))           | Brief characteristics of the image. <br><br> The DICOM SeriesDescription (0008,103E) field is constructed by combining this array of values, delimited by "\_" (underscores). |
| SeriesDescriptionAdditional | text array   | [SeriesDescription](http://dicomlookup.com/lookup.asp?sw=Tnumber&q=(0008,103E)) | Brief characteristics of the image. <br><br> The existing DICOM SeriesDescription (0008,103E) field is appended each string in this array, delimited by "\_" (underscores). |
| ImageComments     | text array   | [ImageComments](http://dicomlookup.com/lookup.asp?sw=Tnumber&q=(0020,4000))                | Remarks about the image. <br><br> This array of values is stored in the DICOM ImageComment (0020,4000) field, delimited by "\_" (underscores). |
| ImageType         | text array   | [ImageType](http://dicomlookup.com/lookup.asp?sw=Tnumber&q=(0008,0008))                   | Characteristics of the image. <br><br> This array of values is appended to the DICOM ImageType (0008,0008) field starting in position 4, delimited by “\” (backslash). |
| ImageRowDir       | double array | N/A                                                                                       | A (1x3) vector in indicating the direction along row dimension.  For images reconstructed from raw data and not undergoing any [flipping or rotating to bring them into standard DICOM orientation](http://gdcm.sourceforge.net/wiki/index.php/Orientation), this value is equivalent to the AcquisitionHeader read_dir field. |
| ImageColumnDir    | double array | N/A                                                                                       | A (1x3) vector in indicating the direction along column dimension.  For images reconstructed from raw data and not undergoing any [flipping or rotating to bring them into standard DICOM orientation](http://gdcm.sourceforge.net/wiki/index.php/Orientation), this value is equivalent to the AcquisitionHeader phase_dir field. |
| RescaleIntercept  | double       | [RescaleIntercept](http://dicomlookup.com/lookup.asp?sw=Tnumber&q=(0028,1052))            | Intercept for image pixel values, used in conjunction with RescaleSlope. <br><br> Pixel values are to be interpreted as: ***value = RescaleSlope\*pixelValue + RescaleIntercept***. This value is set in the DICOM RescaleIntercept (0028,1052) field. |
| RescaleSlope      | double       | [RescaleSlope](http://dicomlookup.com/lookup.asp?sw=Tnumber&q=(0028,1053))                | Scaling factor for image pixel values, used in conjunction with RescaleIntercept. <br><br> Pixel values are to be interpreted as: ***value = RescaleSlope\*pixelValue + RescaleIntercept***. This value is set in the DICOM RescaleSlope (0028,1053) field. |
| WindowCenter      | long         | [WindowCenter](http://dicomlookup.com/lookup.asp?sw=Tnumber&q=(0028,1050))                | The window center in the rendered image, used in conjunction with WindowWidth. <br><br> If RescaleIntercept and RescaleSlope are defined, WindowCenter and WindowWidth are applied to rescaled values. This value is set in the DICOM WindowCenter (0028,1050) field. |
| WindowWidth       | long         | [WindowWidth](http://dicomlookup.com/lookup.asp?sw=Tnumber&q=(0028,1051))                 | The window center in the rendered image, used in conjunction with WindowCenter. <br><br> If RescaleIntercept and RescaleSlope are defined, WindowCenter and WindowWidth are applied to rescaled values. This value is set in the DICOM WindowWidth (0028,1051) field. |
| LUTFileName       | text         | PhotometricInterpretation, [RedPaletteColorLookupTable, RedPaletteColorLookupTable, RedPaletteColorLookupTable](http://dicom.nema.org/medical/Dicom/2018d/output/chtml/part03/sect_C.7.9.html) | Path to a color lookup table file to be used for this image. <br><br> LUT files must be in Siemens .pal format and stored in C:\MedCom\config\MRI\ColorLUT. If a value is provided, the DICOM field PhotometricInterpretation (0028,0004) is set to “PALETTE COLOR” |
| EchoTime          | double       | [EchoTime](http://dicomlookup.com/lookup.asp?sw=Tnumber&q=(0018,0081))                    | Echo time of the image in ms. <br><br> This value is set in the DICOM EchoTime (0018,0081) field.
| InversionTime     | double       | [InversionTime](http://dicomlookup.com/lookup.asp?sw=Tnumber&q=(0018,0082))               | Inversion time of the image in ms. <br><br> This value is set in the DICOM InversionTime (0018,0082) field.
| ROI               | double array | N/A                                                                                       | Region of interest polygon. <br><br> For multiple ROIs, the MetaAttribute element names shall start with “ROI_”. These ROIs are stored in a format compatible with the Siemens syngo viewer. The first 6 values are meta attributes of the ROI:
|                   |              |                                                                                           |   1. Red color (normalized to 1)
|                   |              |                                                                                           |   2. Green color (normalized to 1)
|                   |              |                                                                                           |   3. Blue color (normalized to 1)
|                   |              |                                                                                           |   4. Line thickness (default is 1)
|                   |              |                                                                                           |   5. Line style (0 = solid, 1 = dashed)
|                   |              |                                                                                           |   6. Visibility (0 = false, 1 = true)
|                   |              |                                                                                           | The remaining values are (row,col) coordinates for each ROI point, with values between 0 and the number of rows/columns. Data is organized as (point 1<sub>row</sub>, point 1<sub>col</sub>, point2<sub>row</sub>, point 2<sub>col</sub>, etc). The last point should be a duplicate of the first point if a closed ROI is desired.

(ImageData)=
## Image Data
Image data is organized by looping through ``matrix_size[0]``, ``matrix_size[1]``, ``matrix_size[2]``, then ``channels``. For example, 2D image data would be formatted as:
<style>
 .smalltable td {
   font-size:       80%;
   border-collapse: collapse;
   border-spacing:  0;
   border-width:    0;
   padding:         3px;
   border:          1px solid lightgray
 }
</style>

<table class="smalltable">
  <tr>
    <td style="text-align: center" colspan="9">Channel 1</td>
    <td style="text-align: center" rowspan="3">...</td>
    <td style="text-align: center" colspan="9">Channel n</td>
  </tr>
  <tr>
    <td style="text-align: center" colspan="3">y<sub>1</sub></td>
    <td style="text-align: center" colspan="3">...</td>
    <td style="text-align: center" colspan="3">y<sub>n</sub></td>
    <td style="text-align: center" colspan="3">y<sub>1</sub></td>
    <td style="text-align: center" colspan="3">...</td>
    <td style="text-align: center" colspan="3">y<sub>n</sub></td>
  </tr>
  <tr>
    <td style="text-align: center">x<sub>1</sub></td>
    <td style="text-align: center">...</td>
    <td style="text-align: center">x<sub>n</sub></td>
    <td style="text-align: center">x<sub>1</sub></td>
    <td style="text-align: center">...</td>
    <td style="text-align: center">x<sub>n</sub></td>
    <td style="text-align: center">x<sub>1</sub></td>
    <td style="text-align: center">...</td>
    <td style="text-align: center">x<sub>n</sub></td>
    <td style="text-align: center">x<sub>1</sub></td>
    <td style="text-align: center">...</td>
    <td style="text-align: center">x<sub>n</sub></td>
    <td style="text-align: center">x<sub>1</sub></td>
    <td style="text-align: center">...</td>
    <td style="text-align: center">x<sub>n</sub></td>
    <td style="text-align: center">x<sub>1</sub></td>
    <td style="text-align: center">...</td>
    <td style="text-align: center">x<sub>n</sub></td>
  </tr>
</table>

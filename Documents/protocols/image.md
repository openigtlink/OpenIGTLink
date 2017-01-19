---
layout: page
title: Specification > Image
header: Pages
---
{% include JB/setup %}


## Summary

The IMAGE format supports 2D or 3D images with metric information including image matrix size,
voxel size, coordinate system type, position, and orientation. The body section of the IMAGE
data consists of two parts: image header to transfer the metric information and image body
to transfer the array of pixel or voxel values. The data type of pixel or voxel can be
either scalar or vector, and numerical values can be 8-, 16-, 32-bit integer, or 32-
or 64-bit floating point. The pixel values can be either big-endian or little-endian,
since the sender software can specify the byte order in the image header. The format
also supports "partial image transfer", in which a region of the image is transferred
instead of the whole image. This mechanism is suitable for real-time applications,
in which images are updated region-by-region. The sub-volume must be box-shaped and
defined by 6 parameters consisting of the indices for the corner voxel of the sub-volume
and matrix size of the sub-volume.


## Message Types
### IMAGE

<table border="1" cellpadding="5" cellspacing="0" align="center">
<tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> V
</td><td align="left"> unsigned short
</td><td align="left"> version number
</td></tr>
<tr>
<td align="left"> T
</td><td align="left"> 8bit unsigned int
</td><td align="left"> Number of Image Components (1:Scalar, &gt;1:Vector). (NOTE: Vector data is stored fully interleaved.)
</td></tr>
<tr>
<td align="left"> S
</td><td align="left"> 8bit unsigned int
</td><td align="left"> Scalar type (2:int8 3:uint8 4:int16 5:uint16 6:int32 7:uint32 10:float32 11:float64)
</td></tr>
<tr>
<td align="left"> E
</td><td align="left"> 8bit unsigned int
</td><td align="left"> Endian for image data (1:BIG 2:LITTLE) (NOTE: values in image header is fixed to BIG endian)
</td></tr>
<tr>
<td align="left"> O
</td><td align="left"> 8bit unsigned int
</td><td align="left"> image coordinate (1:RAS 2:LPS)
</td></tr>
<tr>
<td align="left"> RI, RJ, RK
</td><td align="left"> 16 bit unsigned int
</td><td align="left"> Number of pixels in each direction
</td></tr>
<tr>
<td align="left"> TX, TY, TZ
</td><td align="left"> 32 bit float
</td><td align="left"> Transverse vector (direction for 'i' index) / The length represents pixel size in 'i' direction in millimeter
</td></tr>
<tr>
<td align="left"> SX, SY, SZ
</td><td align="left"> 32 bit float
</td><td align="left"> Transverse vector (direction for 'j' index) / The length represents pixel size in 'j' direction in millimeter
</td></tr>
<tr>
<td align="left"> NX, NY, NZ
</td><td align="left"> 32 bit float
</td><td align="left"> Normal vector of image plane(direction for 'k' index) /  The length represents pixel size in 'z' direction or slice thickness in millimeter
</td></tr>
<tr>
<td align="left"> PX, PY, PZ
</td><td align="left"> 32 bit float
</td><td align="left"> center position of the image (in millimeter) <font color="red">(*)</font>
</td></tr>
<tr>
<td align="left"> DI, DJ, DK
</td><td align="left"> 16 bit unsigned int
</td><td align="left"> Starting index of subvolume
</td></tr>
<tr>
<td align="left"> DRI, DRJ, DRK
</td><td align="left"> 16 bit unsigned int
</td><td align="left"> number of pixels of subvolume
</td></tr>
<tr>
<td align="left"> IMAGE_DATA
</td><td align="left"> Binary image data ()
</td><td align="left"> Image data  (endian is determined by "E" field)
</td></tr>
</table>

<font color="red"> * NOTE 1/10/11: There was a discrepancy between the protocol document and the library. Since the library has been used by most of OpenIGTLink software, the protocol document was modified to be consistent with the library. </font>


### GET_IMAGE

<table border="1" cellpadding="5" cellspacing="0" align="center">
<tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</table>


### STT_IMAGE

<table border="1" cellpadding="5" cellspacing="0" align="center">
<tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</table>

### STP_IMAGE

<table border="1" cellpadding="5" cellspacing="0" align="center">
<tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</table>

### RTS_IMAGE

<table border="1" cellpadding="5" cellspacing="0" align="center">
<tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> Status
</td><td align="left"> 8 bit unsigned
</td><td align="left"> 0: Success 1: Error
</td></tr>
</table>

## Notes

The example application of vector type is:
* Vector field
* Image with complex pixel values (e.g. MRI raw data)
* Color image
* Vector data is stored fully interleaved. For example, if you are storing RGB color image, the order should be RGBRGBRGB... (not RRR....GGG...BBB...)

## Implementations

IMAGE type is implemented in the following files:

* [igtlImageMessage.h](/Source/igtlImageMessage.h)
* [igtlImageMessage.cxx](/Source/igtlImageMessage.cxx)

## Contributors
* Junichi Tokuda

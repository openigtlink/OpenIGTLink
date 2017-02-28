[Back to Index](/Documents/Protocol/index.md)

Image Message
=============

- Protocol Version: 3.0
- Release Date: January 20, 2017


Summary
===================

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


Message Types
===================

IMAGE
-------------------

 Data         | Type      | Description
--------------|-----------|-----------------------------------------------------
V             | uint16    | version number
T             | uint8     | Number of Image Components (1:Scalar, >1:Vector). (NOTE: Vector data is stored fully interleaved.)
S             | uint8     | Scalar type (2:int8 3:uint8 4:int16 5:uint16 6:int32 7:uint32 10:float32 11:float64)
E             | uint8     | Endian for image data (1:BIG 2:LITTLE) (NOTE: values in image header is fixed to BIG endian)
O             | uint8     | Image coordinate (1:RAS 2:LPS)
RI, RJ, RK    | uint16[3] | Number of pixels in each direction
TX, TY, TZ    | float32[3]| Transverse vector (direction for 'i' index) / The length represents pixel size in 'i' direction in millimeter
SX, SY, SZ    | float32[3]| Transverse vector (direction for 'j' index) / The length represents pixel size in 'j' direction in millimeter
NX, NY, NZ    | float32[3]| Normal vector of image plane(direction for 'k' index) / The length represents pixel size in 'z' direction or slice thickness in millimeter
PX, PY, PZ    | float32[3]| Center position of the image (in millimeter) (*)
DI, DJ, DK    | uint16[3] | Starting index of subvolume
DRI, DRJ, DRK | uint16[3] | Number of pixels of subvolume
IMAGE_DATA    | uint8[]   | Binary image data () Image data (Endian is determined by "E" field)


GET_IMAGE
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |


STT_IMAGE
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |


STP_IMAGE
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |

RTS_IMAGE
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 STATUS       | uint8         | 0: Success; 1: Error

Notes
===================

The example application of vector type is:
* Vector field
* Image with complex pixel values (e.g. MRI raw data)
* Color image
* Vector data is stored fully interleaved. For example, if you are storing RGB color image, the order should be RGBRGBRGB... (not RRR....GGG...BBB...)

Implementations
===================

* [igtlImageMessage.h](/Source/igtlImageMessage.h)
* [igtlImageMessage.cxx](/Source/igtlImageMessage.cxx)

Contributors
===================
* Junichi Tokuda

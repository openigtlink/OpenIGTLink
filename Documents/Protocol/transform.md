[Back to Index](/Documents/Protocol/index.md)

Transform Message
=================

- Protocol Version: 3.0
- Release Date: January 20, 2017

Summary
=======

The TRANSFORM data type is used to transfer a homogeneous linear transformation
in 4-by-4 matrix form. One such matrix was shown earlier in equation (1).  Note
that if a device is sending only translation and rotation, then TRANSFORM is
equivalent to POSITION. But TRANSFORM can also be used to transfer affine
transformations or simple scaling. Like IMAGE and POSITION, TRANSFORM carries
information about the coordinate system used.

Message Types
=============

TRANSFORM
---------

The message contains the elements for the upper 3 rows of 4x4 linear transform matrix.

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 R11          | float32       | Element (1, 1)
 R21          | float32       | Element (2, 1)
 R31          | float32       | Element (3, 1)
 R12          | float32       | Element (1, 2)
 R22          | float32       | Element (2, 2)
 R32          | float32       | Element (3, 2)
 R13          | float32       | Element (1, 3)
 R23          | float32       | Element (2, 3)
 R33          | float32       | Element (3, 3)
 TX           | float32       | Element (1, 4) (translation along x-axis in millimeter)
 TY           | float32       | Element (2, 4) (translation along y-axis in millimeter)
 TZ           | float32       | Element (3, 4) (translation along z-axisin millimeter)


GET_TRANS
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |


STT_TRANS
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |


STP_TRANS
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |


RTS_TRANS
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 STATUS       | uint8         | 0: Success; 1: Error


Implementations
===================
* [igtlTransformMessage.h](/Source/igtlTransformMessage.h)
* [igtlTransformMessage.cxx](/Source/igtlTransformMessage.cxx)

Contributors
===================
* Junichi Tokuda

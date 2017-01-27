[Back to Index](/Documents/Protocol/index.md)

QuaternionTransform Message
===========================

- Protocol Version: 3.0
- Release Date: January 20, 2017

Summary
=======

The QTRANS data type is used to transfer position and orientation information.
The data are a combination of three-dimensional vector for the position and
quaternion for the orientation. Although equivalent position and orientation can
be described with the TRANSFORM data type, the QTRANS data type has the
advantage of smaller data size (19%). It is, therefore, more suitable for pushing
high frame-rate data from tracking devices.

Message Types
=============

QTRANS
------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 X            | float32       | X position in millimeter
 Y            | float32       | Y position in millimeter
 Z            | float32       | Z position in millimeter
 OX           | float32       | X element in quaternion
 OY           | float32       | Y element in quaternion
 OZ           | float32       | Z element in quaternion
 W            | float32       | W element in quaternion

GET_QTRANS
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |

STT_QTRANS
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |

STP_QTRANS
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |

RTS_QTRANS
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 STATUS       | uint8         | 0: Success; 1: Error


Implementations
===================

* [igtlPositionMessage.h](/Source/igtlPositionMessage.h)
* [igtlPositionMessage.cxx](/Source/igtlPositionMessage.cxx)


Contributors
===================
Junichi Tokuda


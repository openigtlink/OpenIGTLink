[Back to Index](/Documents/Protocol/index.md)

Point Message
=============

- Protocol Version: 3.0
- Release Date: January 20, 2017

Summary
=======

The POINT message type is designed to transfer information about fiducials,
which are often used in surgical planning and navigation in the image-guided
therapy.

Message Types
=============

POINT
-----

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 POINT_0      | POINT_DATA    | Data for point 0
 ...          |               |
 POINT_(N-1)  | POINT_DATA    | Data for point N-1

- POINT_DATA

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 NAME         | char[64]      | Name or description of the point
 GROUP        | char[32]      | Can be "Labeled Point", "Landmark", "Fiducal", ...
 R,G,B,A      | uint8[4]      | Color in RGBA
 X,Y,Z        | float32[3]    | Coordinate of the point in millimeter
 DIAMETER     | float32       | Diameter of the point in millimeter. Diameter can be 0.
 OWNER        | char[20]      | Id of the owner image/sliceset. Points from different slicesets can be sent if slicesets are fused.

GET_POINT
---------

GET_POINT is used to query for meta data of label data. If the DEVICE_NAME field
in the header is empty, a list of all available point data is returned.

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |

STT_POINT
-------------------

N / A

STP_POINT
-------------------

N/A


RTS_POINT
-------------------

N/A

Implementations
===================

IMGMETA message type is implemented in the following source code.

* [igtlPointMessage.h](/Source/igtlPointMessage.h)
* [igtlPointMessage.cxx](/Source/igtlPointMessage.cxx)

Contributors
===================

* Alexander Schaal


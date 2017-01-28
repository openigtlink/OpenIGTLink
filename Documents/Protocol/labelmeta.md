[Back to Index](/Documents/Protocol/index.md)

LabelMeta Message
=================

- Protocol Version: 3.0
- Release Date: January 20, 2017

Summary
===================

The LBMETA is used to transfer meta information for label maps, which are not
available in the IMAGE message type. To retrieve voxel objects or a label map,
GET_IMAGE / IMAGE can be used. But the client should be able to get a list of
available structures.

Message Types
===================

LBMETA
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 NAME         | char[64]      | Name or description of the image
 ID           | char[20]      | ID to query the IMAGE
 LABEL        | uint8         | Label of the structure (0 if unused)
 --           | uint8         | Reserved.
 R,G,B,A      | uint8[4]      | Color in RGBA (0 0 0 0 if no color is defined)
 RI, RJ, RK   | uint16[3]     | Number of pixels in each direction (same as in IMAGE), bounding box of the structure(s)
 OWNER_IMAGE  | char[20]      | ID of the owner image/sliceset. Voxel objects from different slicesets can be sent if slicesets are fused. Can be empty if n/a


GET_LBMETA
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |

STT_LBMETA
-------------------

N/A

STP_LBMETA
-------------------

N/A

Implementations
===================

IMGMETA message type is implemented in the following source code.

* [igtlLabelMetaMessage.h](/Source/igtlLabelMetaMessage.h)
* [igtlLabelMetaMessage.cxx](/Source/igtlLabelMetaMessage.cxx)

Contributors
===================
* Alexander Schaal

[Back to Index](/Documents/Protocol/index.md)

ImageMeta Message
=================

- Protocol Version: 3.0
- Release Date: January 20, 2017


Summary
=======

The IMGMETA message is used to transfer image meta information which is not
available in IMAGE message type, such as patient name, medical record number,
modality etc. An IMGMETA message can contain meta-data for multiple images.
This message type may be used to obtain a list of images available in the remote
system, such as image database or commercial image-guided surgery (IGS) system.

Message Types
=============

IMGMETA
-------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 NAME         | char[64]      | Name or description of the image
 ID           | char[20]      | ID to query the IMAGE and COLORT
 MODALITY     | char[32]      | String which specifies the modality
 PATIENT_NAME | char[64]      | Name of the patient
 PATIENT_ID   | char[64]      | ID of the patient
 TIMESTMP     | uint64        | Scan time, see [Timestamp](timestamp.md)
 RI, RJ, RK   | uint16[3]     | Number of pixels in each direction (same as in IMAGE)
 S            | uint8         | Scalar type (e.g. 3:uint8, 5:uint16, same as in IMAGE)
 --           | uint8         | Reserved

* More than one item can be transmitted. The number is bodySize/itemSize.
* To get the IMAGE, GET_IMAGE is used with the Id in the device name field.


GET_IMGMETA
-------------------

GET_IMGMETA is used to query for meta data of image sets. If DEVICE_NAME in the
header is empty, a list of meta data for all available images is returned.

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |

STT_IMGMETA
-------------------

N/A

STP_IMGMETA
-------------------

N/A


Implementations
===================

* [igtlImageMetaMessage.h](/Source/igtlImageMetaMessage.h)
* [igtlImageMetaMessage.cxx](/Source/igtlImageMetaMessage.cxx)

Contributors
===================

* Alexander Schaal



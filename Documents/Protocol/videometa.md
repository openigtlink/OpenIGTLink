[Back to Index](/Documents/Protocol/index.md)

VIDEOMeta Message
=================

- Protocol Version: 3.1
- Release Date: July 17, 2017


Summary
=======

The VIDEOMETA message is used to transfer video meta information which are not available in video message type,
such as patient name, medical record number, camera focal length, zoom factor etc. 
An VIDEOMETA message can contain meta data for multiple video sources. 
This message type may be used to obtain a list of videos available in the remote system, such as video database, laparoscope or commercial image-guided surgery (IGS) system.

Message Types
=============

VIDEOMETA
-------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 NAME         | char[64]      | Name or description of the video
 DEVICENAME   | char[64]      | DeviceName to query the VIDEO and COLORT
 PATIENT_NAME | char[64]      | Name of the patient
 PATIENT_ID   | char[64]      | ID of the patient
 ZOOM_LEVEL   | int16         | Defines the zoom level of the camera.
 FOCAL_LENGTH | float64       | Defines the focal length of the camera in millimeter.
 RI, RJ, RK   | uint16[3]     | Number of pixels in each direction
 TX, TY, TZ   | float32[3]		| Transverse vector (direction for 'i' index) / The length represents pixel size in 'i' direction in millimeter
 SX, SY, SZ   | float32[3]		| Transverse vector (direction for 'j' index) / The length represents pixel size in 'j' direction in millimeter
 NX, NY, NZ   | float32[3]		| Normal vector of video frame plane(direction for 'k' index) / The length represents pixel size in 'z' direction or slice thickness in millimeter
 PX, PY, PZ   | float32[3]		| Center position of the video frame (in millimeter) (*)
 S            | uint8         | Scalar type (e.g. 3:uint8, 5:uint16, same as in VIDEO)
 --           | uint8         | Reserved

* More than one item can be transmitted. The number is bodySize/itemSize.
* To get the VIDEO, GET_VIDEO is used with the Id in the device name field.


GET_VMETA
-------------------

GET_VMETA is used to query for meta data of video sets. If DEVICE_NAME in the
header is empty, a list of meta data for all available video sources is returned.

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |

STT_VMETA
-------------------

N/A

STP_VMETA
-------------------

N/A


Implementations
===================

* [igtlVideoMetaMessage.h](/Source/VideoStreaming/igtlVideoMetaMessage.h)
* [igtlVideoMetaMessage.cxx](/Source/VideoStreaming/igtlVideoMetaMessage.cxx)

Contributors
===================

* Longquan Chen



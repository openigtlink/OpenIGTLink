[Back to Index](/Documents/Protocol/index.md)

Video Message
==============

- Protocol Version: 3.1
- Release Date: July 14, 2017


Summary
=======

The Video message supports video codec streaming with metric information including codec protocal name,
frame height, frame width, etc. The body section of the VIDEO
data consists of two parts: video header to transfer the metric information and video body
to transfer encoded video bit stream. The numerical values can be 8-, 16-, 32-bit integer, depending on the encoder.
The pixel values can be either big-endian or little-endian. This message type could be used for real-time video transmission. 

Message Types
=============

VIDEO
------

 Data                 | Type             | Description
----------------------|------------------|-------------------------------------------------
 HEADER_VERSION       | uint16           | Header version number
 ENDIAN               | uint8            | Endian for image data (1:BIG 2:LITTLE)
 ENCODING             | char[4]          | FourCC codec name, Default codec="VP90"
 FRAMETYPE            | uint16           | Field to identify frame type, such as key frame or invalid frame. Also, if the type value is larger than 0xFF, it indicates gray frames are encoded
 COORD                | uint8[3]         | Coordinate system (1:RAS 2:LPS)
 SIZE                 | uint16           | Video frame size
 MATRIX               | float32[12]      | Orientation / origin of image
 SUBVOL_OFFSET        | uint16[3]        | Sub volume offset
 SUBVOL_SIZE          | uint16[3]        | Sub volume size

STT_VIDEO
-------------------

 Data          | Type          | Description
---------------|---------------|-------------------------------------------------
 CODEC         | char[4]       | Command Parameter. The Codec name indicates which codec to use for compression, Default codec="VP90".
 TIME_INTERVAL | uint32        | Command Parameter. Minimum time between two frames. Unit is millisecond. Default = 50 ms

STP_VIDEO
-------------------
N/A

Implementations
===================

The TDATA message type is implemented in the following source code.

* [igtlVideoMessage.h](/Source/VideoStreaming/igtlVideoMessage.h)
* [igtlVideoMessage.cxx](/Source/VideoStreaming/igtlVideoMessage.cxx)

Contributors
===================

*Longquan Chen Junichi Tokuda


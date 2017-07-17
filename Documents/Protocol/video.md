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
 VERSION              | uint16           | version number    
 CODEC                | char[4]          | The codec name, Default codec="VP9 ".
 SCALAR_TYPE          | uint8            | Scalar type (2:int8 3:uint8 4:int16 5:uint16 6:int32 7:uint32 10:float32 11:float64)
 ENDIAN               | uint8            | Endian for image data (1:BIG 2:LITTLE)
 WIDTH                | uint32           | Video Frame Width
 HEIGHT               | uint32           | Video Frame Height
 ADDITIONALZDIMENSION | uint32           | The frame could be a 3D volume, this field is reserved for future development
 FRAMETYPE            | uint16           | Field to identify frame type, such as key frame or invalid frame. Also, if the type value is larger than 0xFF, it indicates gray frames are encoded
 USECOMPRESS          | int8             | Field indicates if current data is compressed or not. Original data without compression could be transmitted as well.
 BITSTREAM            | uint8[]          | Binary bit stream data

STT_VIDEO
-------------------

 Data          | Type          | Description
---------------|---------------|-------------------------------------------------
 CODEC         | char[4]       | Command Parameter. The Codec name indicates which codec to use for compression, Default codec="VP9 ".
 TIME_INTERVAL | uint32        | Command Parameter. Minimum time between two frames. Unit is millisecond. Default = 50 ms
 USECOMPRESS   | int8          | Command Parameter. Field indicates if use compression or not.

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


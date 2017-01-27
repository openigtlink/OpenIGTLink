[Back to Index](/Documents/Protocol/index.md)

QuaternionTrackingData Message
==============================

- Protocol Version: 3.0
- Release Date: January 20, 2017

Summary
===================

The QTDATA message type is intended for transferring 3D positions of surgical
tools, markers etc. Its role is almost identical to TDATA, except that QTDATA
describes orientation by using quaternion.

Message Types
===================

QTDATA
-------------------

 Data          | Type          | Description
---------------|---------------|-------------------------------------------------
 NAME_0        | char[20]      | Name (=Id) of the instrument/tracker # 0
 TYPE_0        | uint8         | Type of instrument for #0
 (--)          | uint8         | Researved
 POSITION_0    | float32[3]    | (X, Y, Z)
 QUATERNION_0  | float32[4]    |Quaternion (QX, QY, QZ, W)
 ...           | ...           | ...
 NAME_(N-1)    | char[20]      | Name (=Id) of the instrument/tracker
 TYPE_(N-1)    | uint8         | Type of instrument for #0
 (--)          | uint8         | Reserved
 POSITION_(N-1)| float32[3]    | (X, Y, Z)
 QUATERNION_(N-1)| float32[4]  | Quaternion (QX, QY, QZ, W)

- TYPE_X should be one of the followings:
  - 1: tracker
  - 2: 6D instrument (regular instrument)
  - 3: 3D instrument (only tip of the instrument defined)
  - 4: 5D instrument (tip and handle are defined, but not the normal vector)

GET_QTDATA
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |

STT_QTDATA
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 STATUS       | uint8         | 0: Success; 1: Error
 RESOL        | uint32        | Minimum time between two frames. Use 0 for as fast as possible. If e.g. 50 ms is specified, the maximum update rate will be 20 Hz.
 COORD_NAME   | char[32]      | Name of coordinate system to use. Can be empty for default coordinate system. (not included if action = 2)


STP_QTDATA
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |


RTS_QDATA
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 STATUS       | uint8         | 0: Success; 1: Error


Implementations
===================

* [igtlQuaternionTrackingDataMessage.h](/Source/igtlQuaternionTrackingDataMessage.h)
* [igtlQuaternionTrackingDataMessage.cxx](/Source/igtlQuaternionTrackingDataMessage.cxx)

Contributors
===================

* Alexander Schaal









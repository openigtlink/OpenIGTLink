[Back to Index](/Documents/Protocol/index.md)

TrackingData Message
====================

- Protocol Version: 3.0
- Release Date: January 20, 2017


Summary
=======

The TDATA message type is intended for transferring 3D positions of surgical
tools, markers etc. Those positions are often measured by the optical,
electromagnetic, or other types of 3D position sensor continuously and transferred
as series of messages. Since it is important for software that receives TDATA to
control data flow, STT_TDATA query data type has interval field to control the
frame rate of consecutive messages.


Message Types
=============

TDATA
-----

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 NAME_0       | char[20]      | Name (=Id) of the instrument/tracker # 0
 TYPE_0       | uint8         | Type of instrument for #0
 (--)         | uint8         | Researved
 MATRIX_0     | float32[12]   | Upper 3 rows of 4x4 transform matrix
 ...          | ...           | ...
 NAME_(N-1)   | char[20]      | Name (=Id) of the instrument/tracker
 TYPE_(N-1)   | uint8         | Type of instrument for # (N-1)
 (--)         | uint8         | Reserved
 MATRIX_(N-1) | float32[12]   | Upper 3 rows of 4x4 transform matrix

- TYPE_X should be one of the followings:
  - 1: tracker
  - 2: 6D instrument (regular instrument)
  - 3: 3D instrument (only tip of the instrument defined)
  - 4: 5D instrument (tip and handle are defined, but not the normal vector)
- MATRIX follows the TRANSFORM message format. See [TRANSFORM message](transform.md).


GET_TDATA
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |

STT_TDATA
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 STATUS       | uint8         | 0: Success; 1: Error
 RESOL        | uint32        | Minimum time between two frames. Use 0 for as fast as possible. If e.g. 50 ms is specified, the maximum update rate will be 20 Hz.
 COORD_NAME   | char[32]      | Name of coordinate system to use. Can be empty for default coordinate system. (not included if action = 2)

* All tracking data from one frame is included.
* Invisible/unavailable trackers/instruments are not included.
* Easy to develop. Sample pseudo code: while(true) { recv(trackingdata); updateView(trackingdata); }
* Usually the tracking data will be sent using the standard coordinate system, which is also used for POINT, IMAGE, ... But this does only work after patient registration. Therefore the body of START_PUSH has an optional field for specifing the coordinate system "CAMERA". To switch back to the standard coordinate system, one has to send STOP_PUSH and afterwards START_PUSH without explicitly specifing the camera coordinate system.

STP_TDATA
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |

RTS_TDATA
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 STATUS       | uint8         | 0: Success; 1: Error


Implementations
===================

* [igtlTrackingDataMessage.h](/Source/igtlTrackingDataMessage.h)
* [igtlTrackingDataMessage.cxx](/Source/igtlTrackingDataMessage.cxx)

Contributors
===================

* Alexander Schaal












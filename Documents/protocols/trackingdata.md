---
layout: page
title: Specification > TrackingData
header: Pages
---
{% include JB/setup %}

## Summary

The TDATA message type is intended for transferring 3D positions of surgical tools, markers etc. Those positions are often measured by optical, electromagnetic or other type of 3D position sensor continuously and transferred as series of messages. Since it is important for software that receives TDATA to control data flow, STT_TDATA query data type has interval field to control the frame rate of consecutive messages.


## Message Types

### TDATA

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> NAME_1
</td><td align="left"> char[20]
</td><td align="left"> Name (=Id) of the instrument/tracker
</td></tr>
<tr>
<td align="left"> TYPE_1
</td><td align="left"> 8 bit unsigned
</td><td align="left"> 1: tracker, 2: 6D instrument (regular instrument), 3: 3D instrument (only tip of the instrument defined), 4: 5D instrument (tip and handle are defined, but not the normal vector)
</td></tr>
<tr>
<td align="left"> --
</td><td align="left"> 8 bit unsigned
</td><td align="left"> Reserved
</td></tr>
<tr>
<td align="left"> MATRIX_1
</td><td align="left"> 32 bit float
</td><td align="left"> 12 values like in TRANSFORM message
</td></tr>
<tr>
<td colspan="3" align="center" style="background:#f0f0f0;"> ...
</td></tr>
<tr>
<td align="left"> NAME_N
</td><td align="left"> char[20]
</td><td align="left"> Name (=Id) of the instrument/tracker
</td></tr>
<tr>
<td align="left"> TYPE_N
</td><td align="left"> 8 bit unsigned
</td><td align="left"> 1: tracker, 2: 6D instrument (regular instrument), 3: 3D instrument (only tip of the instrument defined), 4: 5D instrument (tip and handle are defined, but not the normal vector)
</td></tr>
<tr>
<td align="left"> --
</td><td align="left"> 8 bit unsigned
</td><td align="left"> Reserved
</td></tr>
<tr>
<td align="left"> MATRIX_N
</td><td align="left"> 32 bit float
</td><td align="left"> 12 values like in TRANSFORM message
</td></tr>
</tbody></table>

### GET_TDATA
<table border="1" cellpadding="5" cellspacing="0" align="center">
<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</tbody></table>

### STT_TDATA
<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> Resolution
</td><td align="left"> 32 bit unsigned
</td><td align="left"> Minimum time between two frames. Use 0 for as fast as possible. If e.g. 50 ms is specified, the maximum update rate will be 20 Hz.
</td></tr>
<tr>
<td align="left"> Coordinate system name
</td><td align="left"> char[32]
</td><td align="left"> Coordinate system to use. Can be empty for default coordinate system. (not included if action = 2)
</td></tr>
</tbody></table>
* All tracking data from one frame is included.
* Invisible/unavailable trackers/instruments are not included.
* Easy to develop. Sample pseudo code: while(true) { recv(trackingdata); updateView(trackingdata); }
* Usually the tracking data will be sent using the standard coordinate system, which is also used for POINT, IMAGE, ... But this does only work after patient registration. Therefore the body of START_PUSH has an optional field for specifing the coordinate system "CAMERA". To switch back to the standard coordinate system, one has to send STOP_PUSH and afterwards START_PUSH without explicitly specifing the camera coordinate system.

### STP_TDATA

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</tbody></table>

### RTS_TDATA
<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> Status
</td><td align="left"> 8 bit unsigned
</td><td align="left"> 0: Success 1: Error
</td></tr>
</tbody></table>


## Implementations

The TDATA message type is implemented in the following source code.

* [igtlTrackingDataMessage.h](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlTrackingDataMessage.h)
* [igtlTrackingDataMessage.cxx](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlTrackingDataMessage.cxx)

## Contributors

* Alexander Schaal












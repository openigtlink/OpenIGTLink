---
layout: page
title: Specification > QTrackingData
header: Pages
---
{% include JB/setup %}

## Summary

The QTDATA message type is intended for transferring 3D positions of surgical tools, markers etc. Its role is almost identical to TDATA, except that QTDATA describes orientation by using quaternion.

## Message Types

### QTDATA

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> NAME_1
</td><td align="left"> char[32]
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
<td align="left"> POSITION_1
</td><td align="left"> float32[3]
</td><td align="left"> (X, Y, Z)
</td></tr>
<tr>
<td align="left"> QUATERNION_1
</td><td align="left"> float32[4]
</td><td align="left"> Quaternion (QX, QY, QZ, W)
</td></tr>
<tr>
<td colspan="3" align="center" style="background:#f0f0f0;"> ...
</td></tr>
<tr>
<td align="left"> NAME_N
</td><td align="left"> char[32]
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
<td align="left"> POSITION_N
</td><td align="left"> float32[3]
</td><td align="left"> (X, Y, Z)
</td></tr>
<tr>
<td align="left"> QUATERNION_N
</td><td align="left"> float32[4]
</td><td align="left"> Quaternion (QX, QY, QZ, W)
</td></tr>
</tbody></table>

* Feb 2, 2011: The length of the coordinate system field was corrected. (It should be the same as TDATA.)

### GET_QTDATA

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</tbody></table>

### STT_QTDATA

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


### STP_QTDATA

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</tbody></table>



### RTS_QDATA

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

* [igtlQuaternionTrackingDataMessage.h](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlQuaternionTrackingDataMessage.h)
* [igtlQuaternionTrackingDataMessage.cxx](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlQuaternionTrackingDataMessage.cxx)

## Contributors

* Alexander Schaal









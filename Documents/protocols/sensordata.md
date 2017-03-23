---
layout: page
title: Specification > SensorData
header: Pages
---
{% include JB/setup %}

## Summary

SENSOR is a message type, which is used to transfer sensor reading, 3-axis position, velocity, acceleration, angle, angle velocity and angle acceleration. The message format is intended for manipulator control and various types of sensors.

## Message Types

### SENSOR

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td align="left" style="background:#e0e0e0;"> Data
</td><td align="left" style="background:#e0e0e0;"> Type
</td><td align="left" style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> LARRAY
</td><td align="left"> uint8
</td><td align="left"> Length of array (0-255)
</td></tr>
<tr>
<td align="left"> STATUS
</td><td align="left"> uint8
</td><td align="left"> Sensor status (Reserved)
</td></tr>
<tr>
<td align="left"> UNIT
</td><td align="left"> uint64
</td><td align="left"> See <a href="/Wiki/index.php/OpenIGTLink/ProtocolV2/Unit" title="OpenIGTLink/ProtocolV2/Unit"> 64-bit UNIT field</a>.
</td></tr>
<tr>
<td align="left"> DATA
</td><td align="left"> float64[LARRAY]
</td><td align="left"> value array for sensor 0
</td></tr>
</tbody></table>


### GET_SENSOR

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</tbody></table>


### STT_SENSOR

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td align="left" style="background:#e0e0e0;"> Data
</td><td align="left" style="background:#e0e0e0;"> Type
</td><td align="left" style="background:#e0e0e0;"> Description
</td></tr>
</tbody></table>

or 

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td align="left" style="background:#e0e0e0;"> Data
</td><td align="left" style="background:#e0e0e0;"> Type
</td><td align="left" style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> RESOL
</td><td align="left"> uint64
</td><td align="left"> Minimum interval between message (ns). Same format as <a href="/Wiki/index.php/OpenIGTLink/Timestamp" title="OpenIGTLink/Timestamp">TimeStamp</a>
</td></tr>
</tbody></table>

### STP_SENSOR

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</tbody></table>


### RTS_SENSOR

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

* [igtlSensorMessage.h](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlSensorMessage.h)
* [igtlSensorMessage.cxx](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlSensorMessage.cxx)

## Contributors

* Junichi Tokuda
* Yuichiro Hayashi

## Examples

### Sending 3-axis troque

A device with 3-axis torque (N\*m) sensor is sending data to a data logger program. The following table shows example data format to send 3-axis torque:

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td align="left" style="background:#e0e0e0;"> Data
</td><td align="left" style="background:#e0e0e0;"> Type
</td><td align="left" style="background:#e0e0e0;"> Value
</td></tr>
<tr>
<td align="left"> LARRAY
</td><td align="left"> uint8
</td><td align="left"> 3
</td></tr>
<tr>
<td align="left"> STATUS
</td><td align="left"> uint8
</td><td align="left"> 0
</td></tr>
<tr>
<td align="left"> UNIT
</td><td align="left"> uint64
</td><td align="left"> 00000010 11000000 00010000 00000000 00000000 00000000 00000000 00000000
</td></tr>
<tr>
<td align="left"> DATA
</td><td align="left"> double[3]
</td><td align="left"> {0.0, 0.0, 0.0}
</td></tr>
</tbody></table>

### Sending 3-axis force, troque and acceleration
By binding SENSOR data using BIND type, values from multiple types of sensors can be transferred simultaneously. See [BIND](v2_bind.html) message description page for detail.










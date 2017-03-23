---
layout: page
title: Specification > ImageMeta
header: Pages
---
{% include JB/setup %}

## Summary

The POINT message type is designed to transfer information about fiducials, which are often used in surgical planning and navigation in the image-guided therapy.

## Message Types

### POINT
<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td colspan="3" align="left">SENSOR_1
</td></tr>
<tr>
<td align="left"> Name
</td><td align="left"> char[64]
</td><td align="left"> Name or description of the point
</td></tr>
<tr>
<td align="left"> Group name
</td><td align="left"> char[32]
</td><td align="left"> Can be "Labeled Point", "Landmark", "Fiducal", ...
</td></tr>
<tr>
<td align="left"> R,G,B,A
</td><td align="left"> 8 bit unsigned
</td><td align="left"> Color in RGBA
</td></tr>
<tr>
<td align="left"> X,Y,Z
</td><td align="left"> 32 bit float
</td><td align="left"> Coordinate of the point in millimeter
</td></tr>
<tr>
<td align="left"> Diameter
</td><td align="left"> 32 bit float
</td><td align="left"> Diameter of the point in millimeter. Diameter can be 0.
</td></tr>
<tr>
<td align="left"> Owner image
</td><td align="left"> char[20]
</td><td align="left"> Id of the owner image/sliceset. Points from different slicesets can be sent if slicesets are fused.
</td></tr>
<tr>
<td colspan="3" align="center" style="background:#f0f0f0;"> ...
</td></tr>
<tr>
<td colspan="3" align="left">SENSOR_N
</td></tr>
<tr>
<td align="left"> Name
</td><td align="left"> char[64]
</td><td align="left"> Name or description of the point
</td></tr>
<tr>
<td align="left"> Group name
</td><td align="left"> char[32]
</td><td align="left"> Can be "Labeled Point", "Landmark", "Fiducal", ...
</td></tr>
<tr>
<td align="left"> R,G,B,A
</td><td align="left"> 8 bit unsigned
</td><td align="left"> Color in RGBA
</td></tr>
<tr>
<td align="left"> X,Y,Z
</td><td align="left"> 32 bit float
</td><td align="left"> Coordinate of the point in millimeter.
</td></tr>
<tr>
<td align="left"> Diameter
</td><td align="left"> 32 bit float
</td><td align="left"> Diameter of the point in millimeter. Diameter can be 0.
</td></tr>
<tr>
<td align="left"> Owner image
</td><td align="left"> char[20]
</td><td align="left"> Id of the owner image/sliceset. Points from different slicesets can be sent if slicesets are fused.
</td></tr>
</tbody></table>


### GET_POINT

GET_POINT is used to query for meta data of label data. If the DEVICE_NAME field in the header is empty, a list of all available point data is returned.

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</tbody></table>


### STT_POINT

N / A

### STP_POINT

N/A


### RTS_POINT

N/A

## Implementations

IMGMETA message type is implemented in the following source code.

* [igtlPointMessage.h](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlPointMessage.h)
* [igtlPointMessage.cxx](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlPointMessage.cxx)

## Contributors

* Alexander Schaal


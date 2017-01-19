---
layout: page
title: Specification > LabelMeta
header: Pages
---
{% include JB/setup %}

## Summary

The LBMETA is used to transfer meta information for lable maps, which are not available in the IMAGE message type.
To retreive voxel objects or a label map, GET_IMAGE / IMAGE can be used. But the client should be able to get a list of available structures.

## Message Types

### LBMETA

<table border="1" cellpadding="5" cellspacing="0" align="center">
<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> Name/Description
</td><td align="left"> char[64]
</td><td align="left"> Name or description of the image
</td></tr>
<tr>
<td align="left"> Id
</td><td align="left"> char[20]
</td><td align="left"> Id to query the IMAGE
</td></tr>
<tr>
<td align="left"> Label
</td><td align="left"> 8 bit unsigned int
</td><td align="left"> Label of structure (0 if unused)
</td></tr>
<tr>
<td align="left"> --
</td><td align="left"> 8 bit unsigned int
</td><td align="left"> Reserved.
</td></tr>
<tr>
<td align="left"> R,G,B,A
</td><td align="left"> 8 bit unsigned
</td><td align="left"> Color in RGBA (0 0 0 0 if no color is defined)
</td></tr>
<tr>
<td align="left"> RI, RJ, RK
</td><td align="left"> 16 bit unsigned
</td><td align="left"> Number of pixels in each direction (same as in IMAGE), bounding box of the structure(s)
</td></tr>
<tr>
<td align="left"> Owner image
</td><td align="left"> char[20]
</td><td align="left"> Id of the owner image/sliceset. Voxel objects from different slicesets can be sent if slicesets are fused. Can be empty if n/a.
</td></tr>
</tbody></table>



### GET_LBMETA
<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</tbody></table>

### STT_LBMETA

N/A

### STP_LBMETA

N/A

## Implementations

IMGMETA message type is implemented in the following source code.

* [igtlLabelMetaMessage.h](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlLabelMetaMessage.h)
* [igtlLabelMetaMessage.cxx](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlLabelMetaMessage.cxx)

## Contributors
* Alexander Schaal

---
layout: page
title: Specification > ImageMeta
header: Pages
---
{% include JB/setup %}

## Summary

The IMGMETA message is used to transfer image meta information which are not available in IMAGE message type, such as patient name, medical record number, modality etc. An IMGMETA message can contain meta data for multiple images. This message type may be used to obtain a list of images available in the remote system, such as image database or commercial image-guided surgery (IGS) system.

## Message Types

### IMGMETA
<table border="1" cellpadding="5" cellspacing="0" align="center">
<tr>
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
</td><td align="left"> Id to query the IMAGE and COLORT
</td></tr>
<tr>
<td align="left"> Modality
</td><td align="left"> char[32]
</td><td align="left"> String which specifies the modality
</td></tr>
<tr>
<td align="left"> Patient name
</td><td align="left"> char[64]
</td><td align="left"> Name of the patient
</td></tr>
<tr>
<td align="left"> Patient id
</td><td align="left"> char[64]
</td><td align="left"> Id of the patient
</td></tr>
<tr>
<td align="left"> Timestamp
</td><td align="left"> 64 bit unsigned
</td><td align="left"> Scan time, see <a href="/Wiki/index.php/OpenIGTLink/Timestamp" title="OpenIGTLink/Timestamp">OpenIGTLink/Timestamp</a>
</td></tr>
<tr>
<td align="left"> RI, RJ, RK
</td><td align="left"> 16 bit unsigned
</td><td align="left"> Number of pixels in each direction (same as in IMAGE)
</td></tr>
<tr>
<td align="left"> S
</td><td align="left"> 8 bit unsigned
</td><td align="left"> Scalar type (e.g. 3:uint8, 5:uint16, same as in IMAGE)
</td></tr>
<tr>
<td align="left"> --
</td><td align="left"> 8 bit unsigned
</td><td align="left"> Reserved
</td></tr>
</table>



* More than one item can be transmitted. The number is bodySize/itemSize.
* To get the IMAGE, GET_IMAGE is used with the Id in the device name field.



### GET_IMGMETA

GET_IMGMETA is used to query for meta data of image sets. If DEVICE_NAME in the header is empty, a list of meta data for all available images is returned.

<table border="1" cellpadding="5" cellspacing="0" align="center">
<tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</table>


### STT_IMGMETA

N/A

### STP_IMGMETA

N/A


## Implementations

IMGMETA message type is implemented in the following source code.

* [igtlImageMetaMessage.h](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlImageMetaMessage.h)
* [igtlImageMetaMessage.cxx](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlImageMetaMessage.cxx)

## Contributors

* Alexander Schaal



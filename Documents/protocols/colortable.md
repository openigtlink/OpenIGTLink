---
layout: page
title: Specification > ColorTable
header: Pages
---
{% include JB/setup %}

## Summary

COLORT is used to transfer a color table.

## Message Types

### COLORT

To get the COLORT, GET_COLORT is used with the Id in the device name field.

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> I
</td><td align="left"> 8-bit unsigned int
</td><td align="left"> Index Type  (3:uint8  5:uint16)
</td></tr>
<tr>
<td align="left"> M
</td><td align="left"> 8-bit unsigned int
</td><td align="left"> Map Type (3:uint8 5:uint16 19:RGB color)
</td></tr>
<tr>
<td align="left"> TABLE
</td><td align="left"> Array of 8-bit unsigned int
</td><td align="left"> Color index table
</td></tr>
</tbody></table>


### GET_COLORT

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</tbody></table>

### STT_COLORT

N/A

### STP_COLORT

N/A

### RTS_COLORT

N/A

## Implementations

IMGMETA message type is implemented in the following source code.

* [igtlLabelMetaMessage.h](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlLabelMetaMessage.h)
* [igtlLabelMetaMessage.cxx](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlLabelMetaMessage.cxx)

## Contributors

* Alexander Schaal







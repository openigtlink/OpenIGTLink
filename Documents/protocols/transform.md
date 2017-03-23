---
layout: page
title: Specification > Transform
header: Pages
---
{% include JB/setup %}

Back to [Specification](../spec.html)

## Summary
The TRANSFORM data type is used to transfer a homogeneous linear transformation in 4-by-4 matrix form. One such matrix was shown earlier in equation (1).  Note that if a device is sending only translation and rotation, then TRANSFORM is equivalent to POSITION. But TRANSFORM can also be used to transfer affine transformations or simple scaling. Like IMAGE and POSITION, TRANSFORM carries information about the coordinate system used.

## Message Types
### TRANSFORM

<table border="1" cellpadding="5" cellspacing="0" >
<tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> R11
</td><td align="left"> 32-bit float
</td><td align="left"> Element (1, 1) in 4-by-4 linear transformation matrix
</td></tr>
<tr>
<td align="left"> R21
</td><td align="left"> 32-bit float
</td><td align="left"> Element (2, 1) in 4-by-4 linear transformation matrix
</td></tr>
<tr>
<td align="left"> R31
</td><td align="left"> 32-bit float
</td><td align="left"> Element (3, 1) in 4-by-4 linear transformation matrix
</td></tr>
<tr>
<td align="left"> R12
</td><td align="left"> 32-bit float
</td><td align="left"> Element (1, 2) in 4-by-4 linear transformation matrix
</td></tr>
<tr>
<td align="left"> R22
</td><td align="left"> 32-bit float
</td><td align="left"> Element (2, 2) in 4-by-4 linear transformation matrix
</td></tr>
<tr>
<td align="left"> R32
</td><td align="left"> 32-bit float
</td><td align="left"> Element (3, 2) in 4-by-4 linear transformation matrix
</td></tr>
<tr>
<td align="left"> R13
</td><td align="left"> 32-bit float
</td><td align="left"> Element (1, 3) in 4-by-4 linear transformation matrix
</td></tr>
<tr>
<td align="left"> R23
</td><td align="left"> 32-bit float
</td><td align="left"> Element (2, 3) in 4-by-4 linear transformation matrix
</td></tr>
<tr>
<td align="left"> R33
</td><td align="left"> 32-bit float
</td><td align="left"> Element (3, 3) in 4-by-4 linear transformation matrix
</td></tr>
<tr>
<td align="left"> TX
</td><td align="left"> 32-bit float
</td><td align="left"> Element (1, 4) in 4-by-4 linear transformation matrix (translation along x-axis in millimeter)
</td></tr>
<tr>
<td align="left"> TY
</td><td align="left"> 32-bit float
</td><td align="left"> Element (2, 4) in 4-by-4 linear transformation matrix (translation along y-axis in millimeter)
</td></tr>
<tr>
<td align="left"> TZ
</td><td align="left"> 32-bit float
</td><td align="left"> Element (3, 4) in 4-by-4 linear transformation matrix (translation along z-axisin millimeter)
</td></tr>
</table>

### GET_TRANS
<table border="1" cellpadding="5" cellspacing="0" >
<tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</table>


### STT_TRANS

<table border="1" cellpadding="5" cellspacing="0" >
<tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</table>


### STP_TRANS

<table border="1" cellpadding="5" cellspacing="0" >
<tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</table>


### RTS_TRANS

<table border="1" cellpadding="5" cellspacing="0" >
<tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> Status
</td><td align="left"> 8 bit unsigned
</td><td align="left"> 0: Success 1: Error
</td></tr>
</table>


## Implementations
IMAGE type is implemented in the following files:
* [igtlTransformMessage.h](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlTransformMessage.h)
* [igtlTransformMessage.cxx](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlTransformMessage.cxx)

## Contributors
* Junichi Tokuda

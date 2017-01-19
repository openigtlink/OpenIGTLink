---
layout: page
title: Specification > NDArray
header: Pages
---
{% include JB/setup %}

## Summary

NDARRAY is a data type, which is designed to transfer N-dimensional numerical array.


## Message Types

### NDARRAY

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> TYPE
</td><td align="left"> UINT8
</td><td align="left"> Scalar type (2:int8 3:uint8 4:int16 5:uint16 6:int32 7:uint32 10:float32 11:float64 13:complex float 64*)
</td></tr>
<tr>
<td align="left"> DIM
</td><td align="left"> UINT8
</td><td align="left"> Dimension of array
</td></tr>
<tr>
<td align="left"> SIZE
</td><td align="left"> UINT16[DIM]
</td><td align="left"> Size of array
</td></tr>
<tr>
<td align="left"> DATA
</td><td align="left"> TYPE[SIZE[0]][SIZE[1]]....[[SIZE[DIM-1]]]
</td><td align="left"> Array data. (Must be in network byte order.)
</td></tr>
</tbody></table>

* Complex is a 128-bit variable consisting of a real part in the first 64-bit and an imaginal part in the last 64-bit.

### GET_NDARRAY

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</tbody></table>

### STT_NDARRAY

N/A

### STP_NDARRAY

N/A

### RTS_NDARRAY

N / A


## Notes
### Memory layout of DATA field
Like multi-dimensional array in C/C++, DATA field is laid out in sequentially in a byte stream. For example, if the value of DATA is defined by:

    char data[3][3] = { {1, 2, 3},
                      {4, 5, 6},
                      {7, 8, 9}
                      };

The layout in the byte stream can be visualized as follows:

      +-+-+-+-+-+-+-+-+-+
      |1|2|3|4|5|6|7|8|9|
      +-+-+-+-+-+-+-+-+-+

## Implementations

The NDARRAY message type is implemented in the following source code.

* [igtlNDArrayMessage.h](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlNDArrayMessage.h)
* [igtlNDArrayMessage.cxx](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlNDArrayMessage.cxx)


## Contributors

* Junichi Tokuda
* Yuichiro Hayashi








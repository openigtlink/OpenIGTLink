[Back to Index](/Documents/Protocol/index.md)

NDArray Message
===============

- Protocol Version: 3.0
- Release Date: January 20, 2017

Summary
=======

NDARRAY is a data type, which is designed to transfer N-dimensional numerical
array.


Message Types
=============

NDARRAY
-------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 SCALAR_TYPE  | uint8         | Scalar type (2:int8 3:uint8 4:int16 5:uint16 6:int32 7:uint32 10:float32 11:float64 13:complex float 64*)
 DIM          | uint8         | Dimension of array
 SIZE         | uint16[DIM]   | Size of array
 DATA         | ARRAY         | Array data. (Must be in the network byte order.)

* ARRAY is a byte array SCALAR_TYPE[SIZE[0]][SIZE[1]]....[[SIZE[DIM-1]]]
* Complex is a 128-bit variable consisting of a real part in the first 64-bit and an imaginal part in the last 64-bit.

GET_NDARRAY
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |

STT_NDARRAY
-------------------

N/A

STP_NDARRAY
-------------------

N/A

RTS_NDARRAY
-------------------

N / A


Notes
===================
Memory layout of DATA field
-------------------
Like multi-dimensional array in C/C++, DATA field is laid out in sequentially in a byte stream. For example, if the value of DATA is defined by:

    char data[3][3] = { {1, 2, 3},
                      {4, 5, 6},
                      {7, 8, 9}
                      };

The layout in the byte stream can be visualized as follows:

      +-+-+-+-+-+-+-+-+-+
      |1|2|3|4|5|6|7|8|9|
      +-+-+-+-+-+-+-+-+-+

Implementations
===================

The NDARRAY message type is implemented in the following source code.

* [igtlNDArrayMessage.h](/Source/igtlNDArrayMessage.h)
* [igtlNDArrayMessage.cxx](/Source/igtlNDArrayMessage.cxx)


Contributors
===================

* Junichi Tokuda
* Yuichiro Hayashi








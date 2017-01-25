[Back to Index](/Documents/Protocol/index.md)

Message Header
==============

- Protocol Version: 3.0
- Release Date: January 20, 2017

Header Structure
================
    Bytes
    0   2                       14                                      34
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+--
    | V |          TYPE         |              DEVICE_NAME              |		  
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+--
 
         34             42               50              58	    
       --+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+.....
         |   TIME_STAMP  |   BODY_SIZE   |     CRC64     |   BODY    
       --+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+.....


Byte Order
==========
Big endian should be used for all [numerical values](http://www.opengroup.org/onlinepubs/007908799/xns/htonl.html)
(version, body size, crc). Unused spaces are padded with 0 (binary).

Header Fields
=============


 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 V            | uint16        | Header version number (2 for version 2 and 3)
 TYPE         | char[12]      | Type name of data
 DEVICE_NAME  | char[20]      | Unique device name
 TIME_STAMP   | uint64        | Timestamp or 0 if unused
 BODY_SIZE    | uint64        | Size of the body in bytes
 CRC          | uint64        | 64 bit CRC for body data


Description of Fields
=====================

Version number
--------------
The version number field specifies the header format version. Currently the
version number is *1*. Please note that this is different from the protocol
version.

Type name
---------
The type name field is an ASCII character string specifying the type of the data
contained in the message body e.g. "TRANSFORM". The length of the type name must
be within 12 characters. 

Device name
The device name field contains an ASCII character string specifying the name of
the the message. 

Timestamp
---------
The timestamp field contains a 64-bit timestamp indicating when the data is
generated. Please refer [Timestamp](timestamp.html) for the format of the 64-bit
timestamp.

Body size
---------

CRC
---
The 64-bit CRC used in OpenIGTLink protocol is based on
[ECMA-182 standard](http://www.ecma-international.org/publications/files/ECMA-ST/Ecma-182.pdf).
An example code is available in [igtl_util.c](/Source/igtlutil/igtl_unit.h)
in the OpenIGTLink library.


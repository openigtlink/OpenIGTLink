[Back to Index](/Documents/Protocol/index.md)

Message Header
==============

- Protocol Version: 3.0
- Release Date: January 20, 2017

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


Header version number
---------------------

The header version number field specifies the Header Version. Please note
that the *Header Version* is not the same as the *Protocol Version*, as
the Protocol Version reflects not only the format of the header but also
the available message types. Two header versions exist as of Protocol Version 3.x:

- _Header Version 1_ is used in Protocol Version 1 and 2.
- _Header Version 2_ is used in Protocol Version 3.x.

The major difference between header versions 1 and 2 is the mechanism to include
the extended header and meta data in the message. This feature has been introduced
in Protocol Version 3.

Type name
---------

The type name field is an ASCII character string specifying the type of the data
contained in the message body e.g. "TRANSFORM". The length of the type name must
be within 12 characters. 

Device name
-----------

The device name field contains an ASCII character string specifying the name of
the the message. 

Timestamp
---------

The timestamp field contains a 64-bit timestamp indicating when the data is
generated. Please refer [Timestamp](timestamp.html) for the format of the 64-bit
timestamp.

Body size
---------

The body size field contains the size of the body in bytes. The Body is a section
of the OpenIGTLink message following the Header. Until Version 2, the Body 
only contains the message content, and hence many old programs assumed that
the Body size represents the content size. Since Version 3, the Body is redefined
as a set of Extended Header, Content, and Metadata. The content size must be
calculated by BODY_SIZE - (EXT_HEADER_SIZE + METADAT_SIZE).

CRC
---
The 64-bit CRC used in OpenIGTLink protocol is based on
[ECMA-182 standard](http://www.ecma-international.org/publications/files/ECMA-ST/Ecma-182.pdf).
An example code is available in [igtl_util.c](/Source/igtlutil/igtl_util.c)
in the OpenIGTLink library.

Extended Header Fields
======================

 Data            | Type          | Description
-----------------|---------------|----------------------------------------------
 EXT_HEADER_SIZE | uint16        | Length of Extended Header section
 METADATA_SIZE   | uint16        | Length of Metadata section
 MSG_ID          | uint32        | Message ID
 (Reserved)      | Uint32        | (Reserved)


Metadata Fields
===============

 Data                       | Type        | Description
----------------------------|-------------|-------------------------------------
 INDEX_COUNT                | uint16      | Number of meta data
 META_HEADER_0              | META_HEADER | Header for metadata 0
 ...                        | ...         | ...
 META_HEADER_(INDEX_COUNT-1)| META_HEADER | Header for metadata (INDEX_COUNT-1)
 META_DATA_0                | META_DATA   | Content for metadata 0
 ...                        | ...         | ...
 META_DATA_(INDEX_COUNT-1)  | META_DATA   | Content for metadata (INDEX_COUNT-1)

- META_HEADER

 Data           | Type        | Description
----------------|-------------|--------------------------------------------------
 KEY_SIZE       | uint16      | Size of key (bytes)
 VALUE_ENCODING | uint16      | Character encoding type for value as MIBenum value. 
 VALUE_SIZE     | uint32      | Size of value (bytes)

- META_DATA

 Data  | Type                 | Description
-------|----------------------|--------------------------------------------------
 KEY   | uint16[KEY_SIZE]     | Key (ASCII)
 VALUE | uint16[VALUE_SIZE]   | Value (Encoded according to VALUE_ENCODING)

* For MIBenum character encoding, please refer [IANA Character Sets](http://www.iana.org/assignments/character-sets). US-ASCII (ANSI-X3.4-1968; MIBenum = 3) is strongly recommended.

Byte Order
==========

Big endian should be used for all [numerical values](http://www.opengroup.org/onlinepubs/007908799/xns/htonl.html)
(version, body size, crc). Unused spaces are padded with 0 (binary).




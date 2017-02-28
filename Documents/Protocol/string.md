[Back to Index](/Documents/Protocol/index.md)

String Message
==============

- Protocol Version: 3.0
- Release Date: January 20, 2017


Summary
=======

The STRING message type is used for transferring a character string. It supports
character strings up to 65535 bytes.

Message Types
=============

STRING
------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 ENCODING     | uint16        | Character encoding type as MIBenum value. Default=3.
 LENGTH       | uint16        | Length of string (bytes)
 STRING       | uint8[LENGTH] | Byte array of the string

* For MIBenum character encoding, please refer [IANA Character Sets](http://www.iana.org/assignments/character-sets). US-ASCII (ANSI-X3.4-1968; MIBenum = 3) is strongly recommended.

GET_STRING
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |

STT_STRING
-------------------

N/A

STP_STRING
-------------------

N/A


RTS_STRING
-------------------

N/A

Implementations
===================

The TDATA message type is implemented in the following source code.

* [igtlStringMessage.h](/Source/igtlStringMessage.h)
* [igtlStringMessage.cxx](/Source/igtlStringMessage.cxx)

Contributors
===================

* Junichi Tokuda









---
layout: page
title: Specification > Status
header: Pages
---
{% include JB/setup %}

## Summary

The STATUS data type is used to notify the receiver about the current status of the sender.
The data consist of status code in a 16-bit unsigned integer, sub code in a 64-bit integer,
error name in a 20-byte-length character string, and a status message. The length of
the status message is determined by the size information in the general header.
The status code is defined as a part of the OpenIGTLink protocol specification listed
bellow. The sub code is device specific and is defined by developers. In addition,
developers can build their own error name/code into the status message and additional
optional description in the following data field.


## Message Types

### STATUS

<table border="1" cellpadding="5" cellspacing="0">
<tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> C
</td><td align="left"> Unsigned short (16bit)
</td><td align="left"> Status code groups: 1-Ok, 2-Generic Error, ... (see below)
</td></tr>
<tr>
<td align="left"> Sub Code
</td><td align="left"> 64 bit integer
</td><td align="left"> Sub code for the error (ex. 0x200 - file not found)
</td></tr>
<tr>
<td align="left"> Error name
</td><td align="left"> char[20]
</td><td align="left"> "Error", "OK", "Warning" - can be anything, don't relay on this
</td></tr>
<tr>
<td align="left"> Status Message (optional)
</td><td align="left"> char[ BodySize - 30 ]
</td><td align="left"> Optional (English) description (ex. "File C:\test.ini not found")
</td></tr>
</table>

### Status codes:

(7/6/2013: The list was updated due to the discrepancy between this document and the actual specification.)

- 00: Invalid packet - 0 is not used
- 01: OK (Default status)
- 02: Unknown error
- 03: Panic mode (emergency)
- 04: Not found (file, configuration, device etc)
- 05: Access denied
- 06: Busy
- 07: Time out / Connection lost
- 08: Overflow / Can't be reached
- 09: Checksum error
- 10: Configuration error
- 11: Not enough resource (memory, storage etc)
- 12: Illegal/Unknown instruction (or feature not implemented / Unknown command received)
- 13: Device not ready (starting up)
- 14: Manual mode (device does not accept commands)
- 15: Device disabled
- 16: Device not present
- 17: Device version not known
- 18: Hardware failure
- 19: Exiting / shut down in progress



### GET_STATUS

<table border="1" cellpadding="5" cellspacing="0" align="center">
<tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</table>

### STT_STATUS

N/A

### STP_STATUS

N/A

### RTS_STATUS

N/A

## Implementations

POSITION type is implemented in the following files:

* [igtlStatusMessage.h](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlStatusMessage.h)
* [igtlStatusMessage.cxx](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlStatusMessage.cxx)

## Contributors

* Junichi Tokuda
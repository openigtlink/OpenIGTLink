---
layout: page
title: Specification > Image
header: Pages
---
{% include JB/setup %}


## Summary

STRING message type is used for transferring a character string. It supports character strings up to 65535 bytes.

## Message Types

### STRING

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td align="left" style="background:#e0e0e0;"> Data
</td><td align="left" style="background:#e0e0e0;"> Type
</td><td align="left" style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> ENCODING
</td><td align="left"> uint16
</td><td align="left"> Character encoding type as MIBenum value (defined by IANA). Default=3.
</td></tr>
<tr>
<td align="left"> LENGTH
</td><td align="left"> uint16
</td><td align="left"> Length of string (bytes)
</td></tr>
<tr>
<td align="left"> STRING
</td><td align="left"> uint8[LENGTH]
</td><td align="left"> Byte array of the string
</td></tr>
</tbody></table>

* For character encoding, please refer [IANA Character Sets](http://www.iana.org/assignments/character-sets). US-ASCII (ANSI-X3.4-1968; MIBenum = 3) is strongly recommended.

### GET_STRING

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</tbody></table>

### STT_STRING

N/A

### STP_STRING

N/A


### RTS_STRING

N/A

## Implementations

The TDATA message type is implemented in the following source code.

* [igtlStringMessage.h](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlStringMessage.h)
* [igtlStringMessage.cxx](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlStringMessage.cxx)

## Contributors

* Junichi Tokuda









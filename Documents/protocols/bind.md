---
layout: page
title: Specification > Bind
header: Pages
---
{% include JB/setup %}

## Summary

Bind message format is designed to bind any OpenIGTLink messages into a single message. Messages bound in a BIND message are called 'child messages.' The BIND message format is useful, when one needs to care about synchronization of multiple messages (e.g. multi-channel sensors), or sending associative array (pairs of key string and value). The bind message format consists of the bind header section, which contains types and sizes of child messages, the child message name table section, and the child message body section.

## Message Types

### BIND

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td align="left" style="background:#e0e0e0;"> Data
</td><td align="left" style="background:#e0e0e0;"> Type
</td><td align="left" style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> NCMESSAGES
</td><td align="left"> uint16
</td><td align="left"> Number of child messages
</td></tr>
<tr>
<td colspan="3" align="left" style="background:#e0e0e0;">Bind Header Section
</td></tr>
<tr>
<td align="left"> TYPE 0
</td><td align="left"> char[12]
</td><td align="left"> Data type (OpenIGTLink Device Type string) for child message 0
</td></tr>
<tr>
<td align="left"> DATA SIZE 0
</td><td align="left"> uint 64
</td><td align="left"> Data size for child message 0
</td></tr>
<tr>
<td colspan="3" align="left" style="background:#f0f0f0;">...
</td></tr>
<tr>
<td align="left"> TYPE (NCMESSAGES-1)
</td><td align="left"> char[12]
</td><td align="left"> Data type (OpenIGTLink Device Type string) for child message (NCMESSAGES-1)
</td></tr>
<tr>
<td align="left"> DATA SIZE  (NCMESSAGES-1)
</td><td align="left"> uint 64
</td><td align="left"> Data size for child message (NCMESSAGES-1)
</td></tr>
<tr>
<td colspan="3" align="left" style="background:#e0e0e0;">NAME Table Section (NULL-separated values)
</td></tr>
<tr>
<td align="left"> NAME_TABLE_SIZE
</td><td align="left"> uint16
</td><td align="left"> Size of name table (including the padding)
</td></tr>
<tr>
<td align="left"> NAME 0
</td><td align="left"> char[*]
</td><td align="left"> Name for child message 0
</td></tr>
<tr>
<td align="left"> (null)
</td><td align="left"> uint8
</td><td align="left"> separator
</td></tr>
<tr>
<td colspan="3" align="left" style="background:#f0f0f0;">...
</td></tr>
<tr>
<td align="left"> NAME (NCMESSAGES-1)
</td><td align="left"> char[*]
</td><td align="left"> Name for child message (NCMESSAGES-1)
</td></tr>
<tr>
<td align="left"> (null)
</td><td align="left"> uint8
</td><td align="left"> separator
</td></tr>
<tr>
<td align="left"> Padding*
</td><td align="left"> uint8 or none
</td><td align="left"> padding to align DATA with WORD
</td></tr>
<tr>
<td colspan="3" align="left" style="background:#e0e0e0;">Data Section
</td></tr>
<tr>
<td align="left"> DATA 0
</td><td align="left"> TYPE 0 ( SIZE 0 )
</td><td align="left"> Data array for child message 0
</td></tr>
<tr>
<td align="left"> (Padding*)
</td><td align="left"> uint8 or none
</td><td align="left"> padding to align DATA with WORD
</td></tr>
<tr>
<td align="left"> DATA 1
</td><td align="left"> TYPE 1 ( SIZE 1 )
</td><td align="left"> Data array for child message 1
</td></tr>
<tr>
<td align="left"> (Padding*)
</td><td align="left"> uint8 or none
</td><td align="left"> padding to align DATA with WORD
</td></tr>
<tr>
<td colspan="3" align="left" style="background:#f0f0f0;">...
</td></tr>
<tr>
<td align="left"> DATA (NCMESSAGES-1)
</td><td align="left"> TYPE  NCMESSAGE-1 (SIZE NCMESSAGE-1)
</td><td align="left"> Data array for child message NCMESSAGE-1
</td></tr>
<tr>
<td align="left"> Padding*
</td><td align="left"> uint8 or none
</td><td align="left"> padding to align DATA with WORD
</td></tr>
</tbody></table>

* Padding field is inserted only if the previous field does not aligned with WORD border.


### GET_BIND

GET_BIND is used to request the receiver for associative array data. If a GET_BIND message does not have a body, it requests all data.

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> NCMESSAGES
</td><td align="left"> uint16
</td><td align="left"> Number of elements
</td></tr>
<tr>
<td colspan="3" align="left" style="background:#e0e0e0;">Bind Header Section
</td></tr>
<tr>
<td align="left"> TYPE 0
</td><td align="left"> char[12]
</td><td align="left"> Data type (OpenIGTLink device type string)
</td></tr>
<tr>
<td colspan="3" align="left" style="background:#f0f0f0;">...
</td></tr>
<tr>
<td align="left"> TYPE (NCMESSAGES-1)
</td><td align="left"> char[12]
</td><td align="left"> Data type (OpenIGTLink device type string)
</td></tr>
<tr>
<td colspan="3" align="left" style="background:#e0e0e0;">NAME Table
</td></tr>
<tr>
<td align="left"> NAME_TABLE_SIZE
</td><td align="left"> uint16
</td><td align="left"> Size of name table (including the padding)
</td></tr>
<tr>
<td align="left"> NAME 0
</td><td align="left"> char[NAMELEN]
</td><td align="left"> Name
</td></tr>
<tr>
<td align="left"> NULL
</td><td align="left"> uint 8
</td><td align="left"> Separator
</td></tr>
<tr>
<td colspan="3" align="left" style="background:#f0f0f0;">...
</td></tr>
<tr>
<td align="left"> NAME (NCMESSAGES-1)
</td><td align="left"> char[NAMELEN (NCMESSAGES-1)]
</td><td align="left"> Name
</td></tr>
<tr>
<td align="left"> NULL
</td><td align="left"> uint 8
</td><td align="left"> Separator
</td></tr>
</tbody></table>


### STT_BIND

GET_BIND is used to request the receiver for associative array data. If a GET_BIND message does not have a body, it requests all data.

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td align="left" style="background:#e0e0e0;"> Data
</td><td align="left" style="background:#e0e0e0;"> Type
</td><td align="left" style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> RESOL
</td><td align="left"> uint64
</td><td align="left"> Minimum interval between message (ns). Same format as <a href="/Wiki/index.php/OpenIGTLink/Timestamp" title="OpenIGTLink/Timestamp">TimeStamp</a>
</td></tr>
</tbody></table>

or 

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> RESOL
</td><td align="left"> uint64
</td><td align="left"> Minimum interval between message (ns). Same format as <a href="/Wiki/index.php/OpenIGTLink/Timestamp" title="OpenIGTLink/Timestamp">TimeStamp</a>
</td></tr>
<tr>
<td align="left"> NCMESSAGES
</td><td align="left"> uint16
</td><td align="left"> Number of elements
</td></tr>
<tr>
<td colspan="3" align="left" style="background:#e0e0e0;">Bind Header Section
</td></tr>
<tr>
<td align="left"> TYPE 0
</td><td align="left"> char[12]
</td><td align="left"> Data type (OpenIGTLink device type string)
</td></tr>
<tr>
<td colspan="3" align="left" style="background:#f0f0f0;">...
</td></tr>
<tr>
<td align="left"> TYPE (NCMESSAGES-1)
</td><td align="left"> char[12]
</td><td align="left"> Data type (0: string, 1: scalar, 2: vector)
</td></tr>
<tr>
<td colspan="3" align="left" style="background:#e0e0e0;">NAME Table Section (NULL-separated values)
</td></tr>
<tr>
<td align="left"> NAME_TABLE_SIZE
</td><td align="left"> uint16
</td><td align="left"> Size of name table (including the padding)
</td></tr>
<tr>
<td align="left"> NAME 0
</td><td align="left"> char[*]
</td><td align="left"> Name
</td></tr>
<tr>
<td align="left"> (null)
</td><td align="left"> uint8
</td><td align="left"> separator
</td></tr>
<tr>
<td colspan="3" align="left" style="background:#f0f0f0;">...
</td></tr>
<tr>
<td align="left"> NAME (NCMESSAGES-1)
</td><td align="left"> char[*]
</td><td align="left"> Name
</td></tr>
<tr>
<td align="left"> (null)
</td><td align="left"> uint8
</td><td align="left"> separator
</td></tr>
</tbody></table>


### STP_BIND

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</tbody></table>


### RTS_BIND

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> Status
</td><td align="left"> 8 bit unsigned
</td><td align="left"> 0: Success 1: Error
</td></tr>
</tbody></table>


## Implementations

* [igtlBindMessage.h](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlBindMessage.h)
* [igtlBindMessage.cxx](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlBindMessage.cxx) 


## Contributors

* Junichi Tokuda
















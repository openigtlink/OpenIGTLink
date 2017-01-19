---
layout: page
title: Specification > Capability
header: Pages
---
{% include JB/setup %}

## Summary

The CAPABILITY data type lists the names of message types that the receiver can interpret. Although the OpenIGTLink protocol guarantees that any receiver can at least skip messages with unknown type and continue to interpret the following messages, it is a good idea to get the capability information at system startup to ensure application-level compatibility of various devices. In a CAPABILITY message type, each message type name comes with format version number. If the receiver can interpret multiple versions for a certain message type, they should be listed as independent types.


## Message Types

### CAPABILITY

<table border="1" cellpadding="5" cellspacing="0">
<tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> TYPE_0
</td><td align="left"> unsigned char [12]
</td><td align="left"> Type name #0
</td></tr>
<tr>
<td align="left"> Type_1
</td><td align="left"> unsigned char [12]
</td><td align="left"> Type name #1
</td></tr>
<tr>
<td align="left"> ...
</td><td align="left"> ...
</td><td align="left"> ...
</td></tr>
<tr>
<td align="left"> Type_(N-1)
</td><td align="left"> unsigned char [12]
</td><td align="left"> Type name #N-1
</td></tr>
</table>

* Number of type names <b>N</b> is calculated by (Body size) / 12.

### GET_CAPABIL

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</table>


### STT_CAPABIL

N/A

### STP_CAPABIL

N/A

### RTS_CAPABIL

N/A

## Implementations


## Contributors

Junichi Tokuda





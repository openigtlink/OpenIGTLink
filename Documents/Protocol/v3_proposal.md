---
layout: page
title: Specification > Version 3 Proposal
header: Pages
---
{% include JB/setup %}

## Background

Since the release of version 2 protocol, we have learned that the protocol has several limitations:

* While the naming convention for querying (i.e. GET_, STT_, STP_, and RTS_ prefixes) in version 2 provides a standardized way to identify query and response messages, there is still no standard way to manage multiple queries simaltaneously. This is mainly due to the lack of mechanism to reference the original query message from the response message. One workaround is to embed a unique message ID in the device names of query and response messages (e.g. "GET_STATUS_1234" and "RTS_STATUS_1234"). This approach requires the receiver process to parse the device name every time it receives a message, and reduces the actual length of device name. 
* When developers design new message exchange schemes for specific applications, they often need to attach some application-specific information to the existing data types. While it can be achieved by bundling the data message with string messages using a BIND message, it is not ideal from the performance aspect. It would make more sense to have a way to add custom 'tags' to any messages.

## Overview of Version 3 Proposal

At [Winter Project Week 2016](http://wiki.na-mic.org/Wiki/index.php/2016_Winter_Project_Week/Projects/TrackedUltrasoundStandardization) (January 5-9, 2016, Cambridge, MA), we discussed the limitations above, and potential extension to the existing protocols _with backward compatibility_. The following changes were proposed:

* A new message structure. The body in the former protocol was splitted into extended header, content, and metadata. The message now consists of the following sections:
  * Header (58 bytes)
  * Extended Header (variable length)
  * Content (variable length)
  * Metadata (variable length)
* The _header_ section has the same format as version 2 protocol, and should contain the following information:
  * The header version (the first two bytes) will be incremented to '0x0002'
  * The Body size is the total byte size for the extended header, content, and Metadata. This will allow old clients to skip the entire message and read the sucessive message properly.
  * The other fields are filled in the same as the previous version.
* The _extended header_ section contains the following fields:
  * Extended header size (EXT_HEADER_SIZE) (2 bytes)
  * Metadata size (METADATA_SIZE) (2 bytes)
  * Message ID (MSG_ID) (4 bytes)
* The _content_ section is equivalent to the body section in the previous version.
  * The size of content can be computed as: BODY_SIZZE - (EXT_HEADER_SIZE + METADAT_SIZE)
* The _metadata_ section contains pairs of 'key' and 'value' strings. Keys are ASCII string, while values can be stored using different encodings.

## Messaging Format

### Overall Message Format

    Bytes
    0         58            72 
    +----------+-------------+-------------------+-----------+
    |  HEADER  | EXT_HEADER  |      CONTENT      | META_DATA | 
    +----------+-------------+-------------------+-----------+


### Header + Extended Header

    Bytes
    0   2                       14                                      34              42              50              58
    +---+-----------------------+---------------------------------------+---------------+---------------+---------------+
    | V | TYPE                  | DEVICE_NAME                           | TIME_STAMP    | BODY_SIZE     | CRC64         |
    +---+-----------------------+---------------------------------------+---------------+---------------+---------------+
    
    58                60              64        68         72    
    +-----------------+---------------+---------+-----------+
    | EXT_HEADER_SIZE | METADATA_SIZE | MSG_ID  | RESERVED  |
    +-----------------+---------------+---------+-----------+


### Content

The format of contenst section is type-dependent. Please see individual type definition page. 


### Metadata

The metadata section consists of metadata header and metadata body.

Metadata header:

    Bytes
    0             2             4                   6               10            12                  14              18
    +-------------+-------------+-------------------+---------------+-------------+-------------------+---------------+----
    | INDEX_COUNT | KEY_SIZE_0  | VALUE_ENCODING_0  | VALUE_SIZE_0  | KEY_SIZE_1  | VALUE_ENCODING_1  | VALUE_SIZE_1  | ...
    +-------------+-------------+-------------------+---------------+-------------+-------------------+---------------+----
                  |<-------------- Metadata 0 --------------------->|<--------------- Metadata 1 -------------------->|
    
                                                    INDEX_COUNT*8+2
    ----+-------------+-------------------+---------------+
    ... |KEY_SIZE_N-1 |VALUE_ENCODING_N-1 |VALUE_SIZE_N-1 |
    ----+-------------+-------------------+---------------+
        |<----------Metadata N-1 (=INDEX_COUNT)---------->|

Metadata body:

    Bytes
    +--------+---------+--------+----------+----    ----+--------+-----------+
    | KEY_0  | VALUE_0 | KEY_1  | VALUE_1  |    ...     |KEY_N-1 | VALUE_N-1 |
    +--------+---------+--------+----------+----    ----+--------+-----------+
    |<-- Metadata 0 -->|<-- Metadata 1 --->|            |<-- Metadata N-1 -->|


## Available Message Types

### New Messages Proposed for V3

<table border="1" cellpadding="1">
<tr>
<td style="width:10%; background:#88A8FF">Type name
</td><td style="width:10%; background:#88A8FF">GET query
</td><td style="width:10%; background:#88A8FF">STT query
</td><td style="width:10%; background:#88A8FF">STP query
</td><td style="width:10%; background:#88A8FF">RTS message
</td><td style="width:60%; background:#88A8FF">Description
</td></tr>
<tr>
<td style="width:25%; background:#F0F8FF"> COMMAND
</td><td>--
</td><td>--
</td><td>--
</td><td>RTS_COMMAND
</td><td align="left">Command
</td></tr>
</table>

### Messages from version 1 and 2
<table border="1" cellpadding="1">
<tr>
<td style="width:10%; background:#88A8FF">Type name
</td><td style="width:10%; background:#88A8FF">GET query
</td><td style="width:10%; background:#88A8FF">STT query
</td><td style="width:10%; background:#88A8FF">STP query
</td><td style="width:10%; background:#88A8FF">RTS message
</td><td style="width:60%; background:#88A8FF">Description
</td></tr>
<tr>
<td style="width:25%; background:#F0F8FF"> IMAGE
</td><td>GET_IMAGE
</td><td>STT_IMAGE
</td><td>STP_IMAGE
</td><td>RTS_IMAGE
</td><td align="left">2D/3D image data
</td></tr>
<tr>
<td style="width:25%; background:#F0F8FF"> TRANSFORM
</td><td>GET_TRANSFOR
</td><td>STT_TRANSFOR
</td><td>STP_TRANSFOR
</td><td>RTS_TRANSFOR
</td><td align="left">Affine transform data.
</td></tr>
<tr>
<td style="width:25%; background:#F0F8FF"> POSITION
</td><td>GET_POSITION
</td><td>STT_POSITION
</td><td>STP_POSITION
</td><td>RTS_POSITION
</td><td align="left">Position and orientation (quaternion)
</td></tr>
<tr>
<td style="width:25%; background:#F0F8FF"> CAPABILITY
</td><td>GET_CAPABIL
</td><td>--
</td><td>--
</td><td>RTS_CAPABIL
</td><td align="left">Points or fiducials.
</td></tr>
<tr>
<td style="width:25%; background:#F0F8FF"> STATUS
</td><td>GET_STATUS
</td><td>--
</td><td>--
</td><td>RTS_STATUS
</td><td align="left">Device status
</td></tr>
<tr>
<td style="width:25%; background:#F0F8FF"> TDATA
</td><td>GET_TDATA
</td><td>STT_TDATA
</td><td>STP_TDATA
</td><td>RTS_TDATA
</td><td align="left">Tracking data
</td></tr>
<tr>
<td style="width:25%; background:#F0F8FF"> IMGMETA
</td><td>GET_IMGMETA
</td><td>--
</td><td>--
</td><td>RTS_IMGMETA
</td><td align="left">List of image meta data including patient name, ID (medical record number), size, etc.
</td></tr>
<tr>
<td style="width:25%; background:#F0F8FF"> LBMETA
</td><td>GET_LBMETA
</td><td>--
</td><td>--
</td><td>RTS_LBMETA
</td><td align="left">List of label meta data.
</td></tr>
<tr>
<td style="width:25%; background:#F0F8FF"> POINT
</td><td>GET_POINT
</td><td>--
</td><td>--
</td><td>RTS_POINT
</td><td align="left">Points or fiducials.
</td></tr>
<tr>
<td style="width:25%; background:#F0F8FF"> TRAJ
</td><td>GET_TRAJ
</td><td>--
</td><td>--
</td><td>RTS_TRAJ
</td><td align="left">Trajectory data (needle path etc.)
</td></tr>
<tr>
<td style="width:25%; background:#F0F8FF"> NDARRAY
</td><td>GET_NDARRAY
</td><td>STT_NDARRAY
</td><td>STP_NDARRAY
</td><td>RTS_NDARRAY
</td><td align="left">Associative array to transfer a set of values with key names.
</td></tr>
</table>


## Acknowledgement

The version 3 propsal is drafted by the following members:

* Andras Lasso, Tamas Ungi (PerkLab, Queen's University)
* Christian Askeland, Ingerid Reinertsen, Ole Vegard Solberg (CustusX, IGT research, SINTEF)
* Simon Drouin (Mcgill University, Montreal, Canada)
* Junichi Tokuda (Brigham and Women's Hospital, Boston, MA)
* Steve Pieper (Isomics, Cambridge, MA)
* Adam Rankin (VASST Laboratory, Western University, Canada)
* Thomas Kirchner, Janek Gröhl (MITK, DKFZ, Heidelberg, Germany)





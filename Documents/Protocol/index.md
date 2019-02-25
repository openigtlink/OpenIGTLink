[Back to Index](/Documents/Protocol/index.md)


OpenIGTLink Protocol Description
================================

- Protocol Version: 3.0
- Release Date: January 20, 2017


Overview
========

OpenIGTLink is an open-source network communication interface specifically
designed for image-guided interventions. It aims to provide a plug-and-play
unified real-time communications (URTC) in operating rooms (ORs) for image-guided
interventions, where imagers, sensors, surgical robots,and computers from
different vendors work cooperatively. This URTC will ensure the seamless data
flow among those components and enable a closed-loop process of planning, control,
delivery, and feedback. The specification of OpenIGTLink is open, and can be
used without any license fee; hence OpenIGTLink is suitable for both industrial
and academic developers. 

We provide [the OpenIGTLink C/C++ library](https://github.com/openigtlink/OpenIGTLink)
as free open-source software under the BSD-style license. The OpenIGTLink library
provides application program interface (API) to communicate with other software
over the network. The OpenIGTLink library supports major platforms including
Microsoft Windows, Linux, and macOS (Mac OS X). Those open-interface and 
multi-platform strategies will lead to the widespread use of OpenIGTLink, and
ultimately help to establish the interoperability among a wide variety of
components developed by different groups and companies in the community.


Message Structure
=================

The OpenIGTLink protocol defines a set of message formats for several data types
that are frequently used for image-guided and robot-assisted interventions.
Those formats are used to compose messages that are transferred from one device
to another over the local area network. The application developers can choose
their own transportation layer that fits for their application and environment
(e.g. TCP, UDP, WebSocket).

While the developers are encouraged to use the data types defined in the
standard protocol, they have an option to define a new message format.
User-defined messages can be mixed with the standard message types, since the
protocol has a mechanism to skip a message, if the receiver does not know its
format. 

In OpenIGTLink Version 3, each OpenIGTLink message consists of the following
four sections:

~~~~
  Bytes
  0         58            72 
  +----------+-------------+-------------------+-----------+
  |  HEADER  | EXT_HEADER  |      CONTENT      | META_DATA | 
  +----------+-------------+-------------------+-----------+
             |<----------------- Body -------------------->|
~~~~

* Header (58 bytes)
* Extended Header (variable length) (New in Version 3)
* Content (variable length)
* Metadata (variable length) (New in Version 3)

The last three sections are called "Body" for the sake of convention. Since
there were no Extended Header and Metadata in the older versions before to 
Version 3, the Body section was solely used by the message content.
Therefore, the Body Size in the Header represented the size of the message 
content as well as the size of the rest of the message.
As a result, many old applications assume that the Body Size in the Header
equals the size of the message content. 

When the message structure was revised for Version 3, we redefined the Body
Size as the total size of Extended Header, Content, and Metadata, because
this new definition allows the old OpenIGTLink programs can still tell the
size of the rest of the message, and skip reading the byte stream until the
beginning of the next message. The size of the content can be computed as:
BODY_SIZE - (EXT_HEADER_SIZE + METADATA_SIZE).


Header + Extended Header
------------------------

~~~~
  Bytes
  0   2                       14                                      34
  +---+-----------------------+---------------------------------------+
  | V | TYPE                  | DEVICE_NAME                           | 
  +---+-----------------------+---------------------------------------+

  34              42              50              58
  +---------------+---------------+---------------+
  | TIME_STAMP    | BODY_SIZE     | CRC64         |
  +---------------+---------------+---------------+
  
  
  58                60                     62              66       70
  +-----------------+----------------------+---------------+--------+
  | EXT_HEADER_SIZE | METADATA_HEADER_SIZE | METADATA_SIZE | MSG_ID |
  +-----------------+----------------------+---------------+--------+
~~~~

The formats of the Header and Extended Header sections are consistent among
all message types, and can be interpreted by any software that has an
OpenIGTLink interface. The Header contains device type (or data type) name,
which specifies the format of the body. The Extended Header section provides
a mechanism to attach application-specific meta-data to the message
along with the Metadata section.

Content
-------

The format of contenst section is type-dependent. Please see below for the
detail.

Meta-Data
---------

Meta-data are given in the form of an associative array (i.e. pairs of "key" and
"value") in the protocol. The Content section contains the data. The format of
the Content section is defined
for each data type.


Metadata header:

~~~~
  Bytes
  0             2             4                   6               10
  +-------------+-------------+-------------------+---------------+----
  | INDEX_COUNT | KEY_SIZE_0  | VALUE_ENCODING_0  | VALUE_SIZE_0  | ...
  +-------------+-------------+-------------------+---------------+----
                |<-------------- Metadata 0 --------------------->|


      10            12                  14              18	 
  ----+-------------+-------------------+---------------+----
  ... | KEY_SIZE_1  | VALUE_ENCODING_1  | VALUE_SIZE_1  | ...
  ----+-------------+-------------------+---------------+----
      |<--------------- Metadata 1 -------------------->|    
  
                                                  INDEX_COUNT*8+2
  ----+-------------+-------------------+---------------+
  ... |KEY_SIZE_N-1 |VALUE_ENCODING_N-1 |VALUE_SIZE_N-1 |
  ----+-------------+-------------------+---------------+
      |<----------Metadata N-1 (=INDEX_COUNT-1)-------->|
~~~~

Metadata body:

~~~~
  Bytes
  +--------+---------+--------+----------+----    ----+--------+-----------+
  | KEY_0  | VALUE_0 | KEY_1  | VALUE_1  |    ...     |KEY_N-1 | VALUE_N-1 |
  +--------+---------+--------+----------+----    ----+--------+-----------+
  |<-- Metadata 0 -->|<-- Metadata 1 --->|            |<-- Metadata N-1 -->|
~~~~

Please refer to the [Header](header.md) pages for the detailed format of
the Header, Extended Header, and Meta-Data sections.


Content
=======

The Content section contains the actual message data such as transforms,
images, strings, etc. The format of the Content section depends on the
type of the message, and it is specified in the TYPE section in the message
header.

Whlie the OpenIGTLink protocol allows application developers to define
and use their own data types, it also provides a number of standard message
types listed below. Application developers should not use the type names of
these standard types for their own message types, because the receiver
programs are expected to de-serialize the Content section according to the
standard format.

Message types inherited from version 1
--------------------------------------

* [CAPABILITY](/Documents/Protocol/capability.md)
* [IMAGE](/Documents/Protocol/image.md)
* [POSITION](/Documents/Protocol/position.md) (alias of QTRANS)
* [QTRANS](/Documents/Protocol/qtransform.md) (formerly POSITION)
* [STATUS](/Documents/Protocol/status.md)
* [TRANSFORM](/Documents/Protocol/transform.md)

Message types introduced in version 2
-------------------------------------
* [BIND](/Documents/Protocol/bind.md)
* [COLORT](/Documents/Protocol/colortable.md)
* [IMGMETA](/Documents/Protocol/imagemeta.md)
* [LBMETA](/Documents/Protocol/labelmeta.md)
* [NDARRAY](/Documents/Protocol/ndarray.md)
* [POINT](/Documents/Protocol/point.md)
* [POLYDATA](/Documents/Protocol/polydata.md)
* [QTDATA](/Documents/Protocol/qtrackingdata.md)
* [SENSOR](/Documents/Protocol/sensordata.md)
* [STRING](/Documents/Protocol/string.md)
* [TDATA](/Documents/Protocol/trackingdata.md)
* [TRAJ](/Documents/Protocol/trajectory.md)

Message types introduced in version 3
-------------------------------------
* [COMMAND](/Documents/Protocol/command.md)
* [VIDEO](/Documents/Protocol/video.md)
* [VIDEOMETA](/Documents/Protocol/videometa.md)

Query Mechanism
===============

See [Query Mechanism](query.md) for detail.


Protocol Compatibility
======================

The protocol has been revised several times since the OpenIGTLink protocol was
originally proposed. The current protocol version is Version 3. The differences
among versions 1, 2, and 3 are as follows:

- Difference between Version 1 and 2
  - [Querying mechanism](query.md) was introduced in Version 2.
  - More message types are supported in Version 2.
  - The formats of the message types available in both Version 1 and 2 remained unchanged.
- Difference between Version 2 and 3.0
  - The header format was extended in Version 3.
  - Version 3 messages consist of 4 sections (Header, Extended Header, Content,
    and Metadata), while Version 2 messages consist of 2 sections (Header and Body).
  - The formats of the message content remained unchanged.

All OpenIGTLink interfaces are expected to be backward-compatible, and must be able
to handle older message formats. Older interfaces might not interpret a message
in a newer format, but they are expected to skip the content and continue to
read the following messages.


Version Numbers
===============

Protocol Version
----------------

Starting at Protocol/Library Version 3, we changed the versioning rule
for the protocol version as follows to make it easier to revise the protocol:

- The protocol version may have a minor version number.
- The major version reflects a major change to the protocol such as:
  - Change to the header format
  - Major change to the message format
  - Adding new rules in the message exchange scheme (e.g. querying)
- The minor version reflects a minior change to the format such as:
  - Fixing minor issues of minor format in the prior versions.
  - Adding new message types.

Library Version
---------------

The Library's version number consists of major, minor, and patch versions
(e.g. 3.0.1 means major version 3, minor version 0, and revision 1).
Both major and minor versions match the protocol version after the
protocol/library version 3.0. In ealier protocol/header versions (i.e. 1
and 2), Only the major versions should match the protocol version.





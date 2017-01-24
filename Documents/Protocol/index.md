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
three sections:


    Bytes
    0         58            72 
    +----------+-------------+-------------------+-----------+
    |  HEADER  | EXT_HEADER  |      CONTENT      | META_DATA | 
    +----------+-------------+-------------------+-----------+


* Header (58 bytes)
* Extended Header (variable length) (New in Version 3)
* Content (variable length)
* Metadata (variable length) (New in Version 3)


Header + Extended Header
------------------------

Header + Extended Header
-------------------

    Bytes
    0   2                       14                                      34
    +---+-----------------------+---------------------------------------+
    | V | TYPE                  | DEVICE_NAME                           | 
    +---+-----------------------+---------------------------------------+

    34              42              50              58
    +---------------+---------------+---------------+
    | TIME_STAMP    | BODY_SIZE     | CRC64         |
    +---------------+---------------+---------------+
    
    
    58                60              64        68         72    
    +-----------------+---------------+---------+-----------+
    | EXT_HEADER_SIZE | METADATA_SIZE | MSG_ID  | RESERVED  |
    +-----------------+---------------+---------+-----------+

The formats of the Header and Extended Header sections are consistent among
all message types, and can be interpreted by any software that has an
OpenIGTLink interface. The Header contains device type (or data type) name,
which specifies the format of the body. The Extended Header section provides
a mechanism to attach application-specific meta-data to the message
along with the Metadata section.

Content
-------
The format of contenst section is type-dependent. Please see individual
type definition page linked below.

Meta-Data
---------
Meta-data are given in the form of an associative array (i.e. pairs of "key" and
"value") in the protocol. The Content section contains the data. The format of
the Content section is defined
for each data type.

Please refer the following pages for the detailed format of the Header,
Extended Header, and Meta-Data sections.

* [Header](header.md)
* [Extended Header / Meta Data](eheader.md)


Message Types
=============

Following types are supported.

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

Other Information
-------------------
* [64-bit UNIT field in OpenIGTLink](/Documents/Protocol/unit.md)




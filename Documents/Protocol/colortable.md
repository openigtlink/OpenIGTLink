
ColorTable Message
==================

- Protocol Version: 3.0
- Release Date: January 20, 2017

Summary
===================

COLORT is used to transfer a color table.

Message Types
===================

COLORT
------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 INDEX_TYPE   | uint8         | Index type  (3:uint8  5:uint16)
 MAP_TYPE     | uint8         | Map type (3:uint8 5:uint16 19:RGB color)
 TABLE        | uint8[]       | Color index table

GET_COLORT
-------------------

To get the COLORT, GET_COLORT is used with the Id in the device name field.


 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |

STT_COLORT
-------------------

N/A

STP_COLORT
-------------------

N/A

RTS_COLORT
-------------------

N/A

Implementations
===================

* [igtlColorTableMessage.h](/Source/igtlColorTableMessage.h)
* [igtlColorTableMessage.cxx](/Source/igtlColorTableMessage.cxx)

Contributors
===================

* Alexander Schaal







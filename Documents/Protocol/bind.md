[Back to Index](/Documents/Protocol/index.md)

Bind Message
============

- Protocol Version: 3.0
- Release Date: January 20, 2017

Summary
=======

The Bind message is designed to bind any OpenIGTLink messages into a single
message. Messages bound in a BIND message are called 'child messages.' The BIND
message format is useful, when one needs to care about synchronization of
multiple messages (e.g. multi-channel sensors), or sending associative array
(pairs of key string and value). The bind message format consists of the bind
header section, which contains types and sizes of child messages, the child
message name table section, and the child message body section.

Message Types
=============

BIND
----

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 N_CHLID      | uint16	      | Number of child messages
 BIND_HEADER  | BIND HEADER   | Types and sizes of child messages
 NAME_TABLE   | NAME TABLE    | Name tables for child messages
 DATA         | DATA          | Child messages


- BIND HEADER Section

 Data             | Type      | Description
------------------|-----------|-------------------------------------------------
 CTYPE_0          | char[12]  | Device type string for child message 0
 CSIZE_0          | uint64    | Data size for child message 0
 ...              | ...       |
 CTYPE_(N_CHILD-1)| char[12]  | Device type string for child message (N_CHILD-1)
 CSIZE_(N_CHILD-1)| uint 64   | Data size for child message (N_CHILD-1)


- NAME TABLE Section (NULL-separated values)

 Data           | Type          | Description
----------------|---------------|-----------------------------------------------
 NTABLE_SIZE    | uint16        | Size of name table (including the padding)
 NAME_0         | char[*]       | Name for child message 0
 (null)         | uint8         | separator
 ...            | ...           |
 NAME_(N_CM-1)  | char[*]       | Name for child message (N_CHILD-1)
 (null)	        | uint8         | Separator
 (Padding*)     | uint8 or none | Padding to align with WORD


* Padding field is inserted only if the previous field does not aligned with WORD border.

- DATA Section

 Data             | Type          | Description
------------------|---------------|-----------------------------------------------
 CDATA 0          | CTYPE_0       | Data array for child message 0 (size: CSIZE_0)
 (Padding*)       | uint8 or none | Padding to align DATA with WORD
 CDATA 1          | CTYPE_1       | Data array for child message 1 (size: CSIZE_1)
 (Padding*)       | uint8 or none | Padding to align DATA with WORD
 ...              | ...           | 
 CDATA_(N_CHILD-1)| CTYPE_(N_CHILD-1)|Data array for child message (N_CHILD-1)
 (Padding*)       | uint8 or none | Padding to align DATA with WORD


GET_BIND
--------

GET_BIND is used to request the receiver for associative array data.
If a GET_BIND message does not have a body, it requests all data.

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 N_CHLID      | uint16	      | Number of child messages
 BIND_HEADER_2| BIND HEADER 2 | Types and sizes of child messages
 NAME_TABLE   | NAME TABLE    | Name tables for child messages

- BIND HEADER 2 Section

 Data             | Type      | Description
------------------|-----------|-------------------------------------------------
 CTYPE_0          | char[12]  | Device type string for child message 0
 ...              | ...       |
 CTYPE_(N_CHILD-1)| char[12]  | Device type string for child message (N_CHILD-1)

- NAME_TABLE (see BIND message)


STT_BIND
--------

GET_BIND is used to request the receiver for associative array data.
If a GET_BIND message does not have a body, it requests all data.


 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 RESOL        | uint64        | Minimum interval between message (ns)*

See [Time Stamp](timestamp.md)

or 

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 N_CHLID      | uint16	      | Number of child messages
 BIND_HEADER_2| BIND HEADER 2 | Types and sizes of child messages
 NAME_TABLE   | NAME TABLE    | Name tables for child messages

- BIND HEADER 2 Section  (see BIND message)
- NAME_TABLE Section (see BIND message)


STP_BIND
--------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |

RTS_BIND
--------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |


Implementations
===================

* [igtlBindMessage.h](/Source/igtlBindMessage.h)
* [igtlBindMessage.cxx](/Source/igtlBindMessage.cxx) 


Contributors
===================

* Junichi Tokuda
















[Back to Index](/Documents/Protocol/index.md)

Command Message
===============

- Protocol Version: 3.0
- Release Date: January 20, 2017
- Available: 3.0 and later


Summary
=======

The COMMAND is a message type that can be used to transfer a command string 
structured in XML. While STRING offers a similar functionality, COMMAND has 
additional fields to include command ID and name providing an easy way to 
reference previous messages exchanged between the two hosts during the session. 

IGSIO (http://igsio.github.io/) demonstrates representative usage of the COMMAND
type. 


Message Types
=============

COMMAND
-------

Data           | Type                   | Description
---------------|------------------------|---------------------------------------
COMMAND_ID     | uint32                 | The unique ID of this command
COMMAND_NAME   | uint8[IGT_COMMAND_SIZE]| The name of the command
ENCODING       | uint16                 | Character encoding type as MIBenum
               |                        | (defined by IANA). Default=3
LENGTH         | uint32                 | The length of command
COMMAND        | uint8[LENGTH]          | The command encoded with ENCODING


For MIBenum values, please refer http://www.iana.org/assignments/character-sets 

Implementations
===============

* [igtlCommandMessage.h](/Source/igtlCommandMessage.h)
* [igtlCommandMessage.cxx](/Source/igtlCommandMessage.cxx)



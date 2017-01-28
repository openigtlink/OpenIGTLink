[Back to Index](/Documents/Protocol/index.md)

Capability Message
==================

- Protocol Version: 3.0
- Release Date: January 20, 2017

Summary
=======

The CAPABILITY type lists the names of message types that the receiver can
interpret. Although the OpenIGTLink protocol guarantees that any receiver can at
least skip messages with unknown type and continue to interpret the following
messages, it is a good idea to get the capability information at system startup
to ensure application-level compatibility of various devices. In a CAPABILITY
message type, each message type name comes with format version number. If the
receiver can interpret multiple versions for a certain message type, they should
be listed as independent types.

Message Types
===================

CAPABILITY
----------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 TYPE_0       | uint8[12]     | Type name #0
 ...          | ...           |
 TYPE_(N_TYPE)| uint8[12]     | Type name #(N_TYPE)

* Number of type names (N_TYPE) is calculated by BODY_SIZE / 12.

GET_CAPABIL
-----------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |

STT_CAPABIL
-----------

N/A

STP_CAPABIL
-------------------

N/A

RTS_CAPABIL
-------------------

N/A

Implementations
===================

* [igtlCapabilityMessage.h](/Source/igtlCapabilityMessage.h)
* [igtlCapabilityMessage.cxx](/Source/igtlCapabilityMessage.cxx)


Contributors
===================

Junichi Tokuda





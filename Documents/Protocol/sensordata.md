[Back to Index](/Documents/Protocol/index.md)

SensorData Message
==================

- Protocol Version: 3.0
- Release Date: January 20, 2017

Summary
=======

SENSOR is a message type, which is used to transfer sensor reading, 3-axis
position, velocity, acceleration, angle, angle velocity and angle acceleration.
The message format is intended for manipulator control and various types of
sensors.

Message Types
===================

SENSOR
-------------------

 Data         | Type           | Description
--------------|----------------|-------------------------------------------------
 LARRAY       | uint8          | Length of array (0-255)
 STATUS       | uint8          | Sensor status (Reserved)
 UNIT         | uint64         | See [64-bit UNIT field](unit.md).
 DATA         | float64[LARRAY]| value array for sensor 0


GET_SENSOR
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |


STT_SENSOR
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |

or 

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 STATUS       | uint8         | 0: Success; 1: Error


STP_SENSOR
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |


RTS_SENSOR
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 STATUS       | uint8         | 0: Success; 1: Error


Implementations
===================

* [igtlSensorMessage.h](/Source/igtlSensorMessage.h)
* [igtlSensorMessage.cxx](/Source/igtlSensorMessage.cxx)

Contributors
===================

* Junichi Tokuda
* Yuichiro Hayashi

Examples
===================

Sending 3-axis troque
---------------------

A device with 3-axis torque (N\*m) sensor is sending data to a data logger
program. The following table shows example data format to send 3-axis torque:

 Data         | Type          | Contents
--------------|---------------|-------------------------------------------------
 LARRAY       | uint8         | 3
 STATUS       | uint8         | 0
 UNIT         | uint64        | 00000010 11000000 00010000 00000000 00000000 00000000 00000000 00000000
 DATA         | float64[3]    | {0.0, 0.0, 0.0}


Sending 3-axis force, troque and acceleration
---------------------------------------------

By binding SENSOR data using BIND type, values from multiple types of sensors
can be transferred simultaneously. See [BIND](bind.md) message description page
for detail.










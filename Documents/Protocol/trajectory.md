[Back to Index](/Documents/Protocol/index.md)

Trajectory Message
==================

- Protocol Version: 3.0
- Release Date: January 20, 2017

Summary
=======

The TRAJECTORY message type support to transfer information about 3D trajectory,
which is often used for surgical planning and guidance in image-guided therapy.

Message Types
=============

TRAJ
----

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 NAME         | char[64]	  | Name or description of the trajectory.
 GROUP_NAME	  | char[32]	  | Can be "Trajectory", ...
 TYPE	      | uint8	      | 1: trajectory with only entry point, 2: trajectory with only target point, 3: trajectory with entry and target point
 Reserved     | uint8         | Reserved
 R,G,B,A      | uint8[4]      | Color in RGBA
 X1,Y1,Z1     | float32[3]	  | Entry point of the trajectory
 X2,Y2,Z2     | float32[3]	  | Target point of a trajectory
 DIAMETER     | float32  	  | Diameter of trajectory, can be 0
 OWNER_IMAGE  | char[20]	  | Id of the owner image/sliceset. Trajectories from different slicesets can be sent if slicesets are fused.

GET_TRAJ
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |

STT_TRAJ
-------------------

N/A

STP_TRAJ
-------------------

N/A


Implementations
===================

* [igtlTrajectoryMessage.h](/Source/igtlTrajectoryMessage.h)
* [igtlTrajectoryMessage.cxx](/Source/igtlTrajectoryMessage.cxx)


Contributors
===================

* Alexander Schaal








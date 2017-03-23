---
layout: page
title: Specification > Trajectory
header: Pages
---
{% include JB/setup %}

## Summary

The TRAJECTORY message type support to transfer information about 3D trajectory, which is often used in surgical planning and guidance in image-guided therapy.

## Message Types

### TRAJ

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> Name
</td><td align="left"> char[64]
</td><td align="left"> Name or description of the trajectory.
</td></tr>
<tr>
<td align="left"> Group name
</td><td align="left"> char[32]
</td><td align="left"> Can be "Trajectory", ...
</td></tr>
<tr>
<td align="left"> Type
</td><td align="left"> 8 bit unsigned
</td><td align="left"> 1: trajectory with only entry point, 2: trajectory with only target point, 3: trajectory with entry and target point
</td></tr>
<tr>
<td align="left"> --
</td><td align="left"> 8 bit unsigned
</td><td align="left"> Reserved
</td></tr>
<tr>
<td align="left"> R,G,B,A
</td><td align="left"> 8 bit unsigned
</td><td align="left"> Color in RGBA
</td></tr>
<tr>
<td align="left"> X1,Y1,Z1
</td><td align="left"> 32 bit float
</td><td align="left"> Entry point of the trajectory
</td></tr>
<tr>
<td align="left"> X2,Y2,Z2
</td><td align="left"> 32 bit float
</td><td align="left"> Target point of a trajectory
</td></tr>
<tr>
<td align="left"> Diameter
</td><td align="left"> 32 bit float
</td><td align="left"> Diameter of trajectory, can be 0
</td></tr>
<tr>
<td align="left"> Owner image
</td><td align="left"> char[20]
</td><td align="left"> Id of the owner image/sliceset. Trajectories  from different slicesets can be sent if slicesets are fused.
</td></tr>
</tbody></table>


### GET_TRAJ

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</tbody></table>

### STT_TRAJ

N/A

### STP_TRAJ

N/A


## Implementations

TRAJECTORY message type is implemented in the following source code.


* [igtlTrajectoryMessage.h](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlTrajectoryMessage.h)
* [igtlTrajectoryMessage.cxx](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlTrajectoryMessage.cxx)


## Contributors

* Alexander Schaal








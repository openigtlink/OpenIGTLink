---
layout: page
title: Specification
header: Pages
weight: 2
group: navigation
---
{% include JB/setup %}

## OpenIGTLink Version 2 Summary
* The header format will not be changed. 
* Simple querying mechanism is defined. See [querying mechanism in version 2](protocols/v2_query.html).
* The protocol version 2 continues to focus on message formats. Supporting tools (simulators and interfaces for specific applications e.g. 3D Slicer, Matlab) and transportation layer (use of UDP or other network communication middleware) should be discussed separately.
* More information is available in [Version 2 Summary page](protocols/v2_summary.html).

## OpenIGTLink Protocol Rules
* The OpenIGTLink protocol is a network communication protocol works in the Application Layer in the OSI model.
* The minimum unit in data transfer in OpenIGTLink protocol is an ''OpenIGTLink message''.
* An OpenIGTLink message consists of ''header'' and ''body'' sections. The header format is consistent and can be interpreted by all software that has OpenIGTLink interface. The header contains device type (or data type) name, which specifies the format of the body.
* The OpenIGTLink protocol is stateless.

## Architecture and Header 
* [OpenIGTLink Header](protocols/v2_header.html)

## Proposed Message Types

### Message types inherited from version 1

* [TRANSFORM](protocols/v2_transform.html)
* [QTRANS](protocols/v2_qtransform.html) (formerly POSITION)
* [POSITION](protocols/v2_position.html) (alias of QTRANS)
* [IMAGE](protocols/v2_image.html)
* [STATUS](protocols/v2_status.html)
* [CAPABILITY](protocols/v2_capability.html)

### New message types for image-guided surgery (IGS) systems
* [IMGMETA](protocols/v2_imagemeta.html)
* [LBMETA](protocols/v2_labelmeta.html)
* [COLORT](protocols/v2_colortable.html)
* [POINT](protocols/v2_point.html)
* [TRAJ](protocols/v2_trajectory.html)
* [TDATA](protocols/v2_trackingdata.html)
* [QTDATA](protocols/v2_qtrackingdata.html)

### New message types for other applications
* [SENSOR](protocols/v2_sensordata.html)
* [STRING](protocols/v2_string.html)
* [NDARRAY](protocols/v2_ndarray.html)
* [BIND](protocols/v2_bind.html)
* [POLYDATA](protocols/v2_polydata.html)

### Other Information
* [64-bit UNIT field in OpenIGTLink](protocols/v2_unit.html)




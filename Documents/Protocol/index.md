
OpenIGTLink Protocol Description
================================

- Protocol Version: 3.0
- Release Date: January 20, 2017

## OpenIGTLink Protocol Rules
* The OpenIGTLink protocol is a network communication protocol works in the Application Layer in the OSI model.
* The minimum unit in data transfer in OpenIGTLink protocol is an ''OpenIGTLink message''.
* An OpenIGTLink message consists of ''header'' and ''body'' sections. The header format is consistent and can be interpreted by all software that has OpenIGTLink interface. The header contains device type (or data type) name, which specifies the format of the body.
* The OpenIGTLink protocol is stateless.

## Architecture and Header 
* [OpenIGTLink Header](header.md)

## Proposed Message Types

### Message types inherited from version 1

* [TRANSFORM](/Documents/Protocol/transform.md)
* [QTRANS](/Documents/Protocol/qtransform.md) (formerly POSITION)
* [POSITION](/Documents/Protocol/position.md) (alias of QTRANS)
* [IMAGE](/Documents/Protocol/image.md)
* [STATUS](/Documents/Protocol/status.md)
* [CAPABILITY](/Documents/Protocol/capability.md)

### New message types for image-guided surgery (IGS) systems
* [IMGMETA](/Documents/Protocol/imagemeta.md)
* [LBMETA](/Documents/Protocol/labelmeta.md)
* [COLORT](/Documents/Protocol/colortable.md)
* [POINT](/Documents/Protocol/point.md)
* [TRAJ](/Documents/Protocol/trajectory.md)
* [TDATA](/Documents/Protocol/trackingdata.md)
* [QTDATA](/Documents/Protocol/qtrackingdata.md)

### New message types for other applications
* [SENSOR](/Documents/Protocol/sensordata.md)
* [STRING](/Documents/Protocol/string.md)
* [NDARRAY](/Documents/Protocol/ndarray.md)
* [BIND](/Documents/Protocol/bind.md)
* [POLYDATA](/Documents/Protocol/polydata.md)

### Other Information
* [64-bit UNIT field in OpenIGTLink](/Documents/Protocol/unit.md)




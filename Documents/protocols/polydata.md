---
layout: page
title: Specification > PolyData
header: Pages
---
{% include JB/setup %}


## Summary

POLYDATA is used to transfer 3D polygonal data. The message format is designed based on the [POLY DATA format](http://www.vtk.org/VTK/img/file-formats.pdf) defined in VTK file format and equivalent to [VTK's vtkPolyData class](http://noodle.med.yale.edu/vtk5/classvtkPolyData.html). The message also supports dataset attribute field in VTK, including scalars, calar_scalars, vectors, vectors, normals, texture coordinates and tensor, but not lookup table.

## Message Types

### POLYDATA

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> NPOINTS
</td><td align="left"> uint32
</td><td align="left"> Number of points
</td></tr>
<tr>
<td align="left"> NVERTICES
</td><td align="left"> uint32
</td><td align="left"> Number of vertices
</td></tr>
<tr>
<td align="left"> SIZE_VERTICES
</td><td align="left"> uint32
</td><td align="left"> Total size of vertices data
</td></tr>
<tr>
<td align="left"> NLINES
</td><td align="left"> uint32
</td><td align="left"> Number of lines
</td></tr>
<tr>
<td align="left"> SIZE_LINES
</td><td align="left"> uint32
</td><td align="left"> Total size of line data
</td></tr>
<tr>
<td align="left"> NPOLYGONS
</td><td align="left"> uint32
</td><td align="left"> Number of polygons
</td></tr>
<tr>
<td align="left"> SIZE_POLYGONS
</td><td align="left"> uint32
</td><td align="left"> Total size of polygon data
</td></tr>
<tr>
<td align="left"> NTRIANGLE_STRIPS
</td><td align="left"> uint32
</td><td align="left"> Number of triangle strips
</td></tr>
<tr>
<td align="left"> SIZE_TRIANGLE_STRIPS
</td><td align="left"> uint32
</td><td align="left"> Total size of triangle strips data
</td></tr>
<tr>
<td align="left"> NATTRIBUTES
</td><td align="left"> uint32
</td><td align="left"> Number of dataset attributes
</td></tr>
<tr>
<td align="left"> POINTS
</td><td align="left"> float32 * NPOINTS * 3
</td><td align="left"> Coordinates of points (P0x, P0y, P0z, P1x, P1y, P1z, ... P(n-1)x, P(n-1)y, P(n-1)z
</td></tr>
<tr>
<td align="left"> VERTICES
</td><td align="left"> uint32 * ((NPOINTS_0+1) + (NPOINTS_1+1) + (NPOINTS_(N-1)+1))
</td><td align="left"> array of vertices ((NPOINTS_0, index_0_0, index_0_1, ....), (NPOINTS_1, index_1_0, index_1_1, ....), ..., (NPOINTS_(N-1), index_(N-1)_0, index_(N-1)_1, ....))
</td></tr>
<tr>
<td align="left"> LINES
</td><td align="left"> uint32 * ((NPOINTS_0+1) + (NPOINTS_1+1) + (NPOINTS_(N-1)+1))
</td><td align="left"> array of vertices ((NPOINTS_0, index_0_0, index_0_1, ....), (NPOINTS_1, index_1_0, index_1_1, ....), ..., (NPOINTS_(N-1), index_(N-1)_0, index_(N-1)_1, ....))
</td></tr>
<tr>
<td align="left"> POLYGONS
</td><td align="left"> uint32 * ((NPOINTS_0+1) + (NPOINTS_1+1) + (NPOINTS_(N-1)+1))
</td><td align="left"> array of vertices ((NPOINTS_0, index_0_0, index_0_1, ....), (NPOINTS_1, index_1_0, index_1_1, ....), ..., (NPOINTS_(N-1), index_(N-1)_0, index_(N-1)_1, ....))
</td></tr>
<tr>
<td align="left"> TRIANGLE_STRIPS
</td><td align="left"> uint32 * ((NPOINTS_0+1) + (NPOINTS_1+1) + (NPOINTS_(N-1)+1))
</td><td align="left"> array of vertices ((NPOINTS_0, index_0_0, index_0_1, ....), (NPOINTS_1, index_1_0, index_1_1, ....), ..., (NPOINTS_(N-1), index_(N-1)_0, index_(N-1)_1, ....))
</td></tr>
<tr>
<td align="left"> TYPE_ATTRIBUTE0
</td><td align="left"> uint16
</td><td align="left"> Type of dataset attribute 0 (including number of components for scalar type)
</td></tr>
<tr>
<td align="left"> NATTRIBUTE0
</td><td align="left"> uint32
</td><td align="left"> Number of data for attribute 0
</td></tr>
<tr>
<td align="left"> TYPE_ATTRIBUTE1
</td><td align="left"> uint16
</td><td align="left"> Type of dataset attribute 1 (including number of components for scalar type)
</td></tr>
<tr>
<td align="left"> NATTRIBUTE1
</td><td align="left"> uint32
</td><td align="left"> Number of data for attribute1
</td></tr>
<tr>
<td align="left"> ...
</td><td align="left"> ...
</td><td align="left"> ...
</td></tr>
<tr>
<td align="left"> TYPE_ATTRIBUTE(NATTRIBUTES-1)
</td><td align="left"> uint16
</td><td align="left"> Type of dataset attribute 1 (including number of components for scalar type)
</td></tr>
<tr>
<td align="left"> NATTRIBUTE(NATTRIBUTES-1)
</td><td align="left"> uint32
</td><td align="left"> Number of data for attribute2
</td></tr>
<tr>
<td align="left"> NAME_ATTRIBUTE0
</td><td align="left"> char * (name length)
</td><td align="left"> Name of attribute 0
</td></tr>
<tr>
<td align="left"> (null)
</td><td align="left"> char
</td><td align="left"> (null)
</td></tr>
<tr>
<td align="left"> NAME_ATTRIBUTE1
</td><td align="left"> char * (name length)
</td><td align="left"> Name of attribute 1
</td></tr>
<tr>
<td align="left"> (null)
</td><td align="left"> char
</td><td align="left"> (null)
</td></tr>
<tr>
<td align="left"> ...
</td><td align="left"> ...
</td><td align="left"> ...
</td></tr>
<tr>
<td align="left"> NAME_ATTRIBUTE(NATTRIBUTES-1)
</td><td align="left"> char * (name length)
</td><td align="left"> Name of attribute (NATTRIBUTES-1)
</td></tr>
<tr>
<td align="left"> (null)
</td><td align="left"> char
</td><td align="left"> (null)
</td></tr>
<tr>
<td align="left"> (Padding)
</td><td align="left"> char or 0
</td><td align="left"> Padding (inserted if (NAMESIZE_ATTRIBUTE0+1+NAMESIZE_ATTRIBUTE1+1+...NAMESIZE_ATTRIBUTE(NATTRIBUTES-1)+1)&nbsp;% 2 == 1)
</td></tr>
<tr>
<td align="left"> ATTRIBUTE0
</td><td align="left"> SIZE_ATTRIBUTE0 * (number of components) * (float32)
</td><td align="left"> Actual attribute data
</td></tr>
<tr>
<td align="left"> ATTRIBUTE1
</td><td align="left"> NATTRIBUTE0 * (number of components) * (float32)
</td><td align="left"> Actual attribute data
</td></tr>
<tr>
<td align="left"> ...
</td><td align="left"> ...
</td><td align="left"> ...
</td></tr>
<tr>
<td align="left"> ATTRIBUTE(NATTRIBUTES-1)
</td><td align="left"> NATTRIBUTE(NATTRIBUTES-1) * (number of components) * (float32)
</td><td align="left"> Actual attribute data
</td></tr>
</tbody></table>


* Values for TYPE_ATTRIBUTE (16-bit)
  * 0-7 bit: Attribute Type:
    * 0x00: POINT_DATA / Scalars
    * 0x01: POINT_DATA / Vectors
    * 0x02: POINT_DATA / Normals
    * 0x03: POINT_DATA / Tensors
    * 0x04: POINT_DATA / RGBA
    * 0x10: CELL_DATA / Scalars
    * 0x11: CELL_DATA / Vectors
    * 0x12: CELL_DATA / Normals
    * 0x13: CELL_DATA / Tensors
    * 0x14: CELL_DATA / RGBA
  * 8-15 bit: Number of components -- must be 3 for Vectors and Normal, 9 for Tensor, 4 for RGBA.
* Maximum length for attribute name is 255
* If there are a pair of scalar and RGBA data with the same name and type (either POINT or CELL), those are used as scalar values and a look up table.
  * elements in RGBA data must be in the rage of \[0.0, 1.0\]

### GET_POLYDATA

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</tbody></table>

### STT_POLYDATA

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td align="left" style="background:#e0e0e0;"> Data
</td><td align="left" style="background:#e0e0e0;"> Type
</td><td align="left" style="background:#e0e0e0;"> Description
</td></tr>
</tbody></table>

or 

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td align="left" style="background:#e0e0e0;"> Data
</td><td align="left" style="background:#e0e0e0;"> Type
</td><td align="left" style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> RESOL
</td><td align="left"> uint64
</td><td align="left"> Minimum interval between message (ns). Same format as <a href="/Wiki/index.php/OpenIGTLink/Timestamp" title="OpenIGTLink/Timestamp">TimeStamp</a>
</td></tr>
</tbody></table>

### STP_POLYDATA

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
</tbody></table>


### RTS_POLYDATA


<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td style="background:#e0e0e0;"> Data
</td><td style="background:#e0e0e0;"> Type
</td><td style="background:#e0e0e0;"> Description
</td></tr>
<tr>
<td align="left"> Status
</td><td align="left"> 8 bit unsigned
</td><td align="left"> 0: Success 1: Error
</td></tr>
</tbody></table>

## Implementations

The POLYDATA message type is implemented in the following source code.

* [igtlPolyDataMessage.h](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlPolyDataMessage.h)
* [igtlPolyDataMessage.cxx](https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlPolyDataMessage.cxx)



## Contributors

* Junichi Tokuda









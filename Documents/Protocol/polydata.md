[Back to Index](/Documents/Protocol/index.md)

PolyData Message
================

- Protocol Version: 3.0
- Release Date: January 20, 2017

Summary
=======

POLYDATA is used to transfer 3D polygonal data. The message format is designed
based on the [POLY DATA format](http://www.vtk.org/VTK/img/file-formats.pdf)
defined in VTK file format and equivalent to
[VTK's vtkPolyData class](http://noodle.med.yale.edu/vtk5/classvtkPolyData.html).
The message also supports dataset attribute field in VTK, including scalars,
calar_scalars, vectors, vectors, normals, texture coordinates, and tensor, but
not lookup table.


Message Types
=============

POLYDATA
--------

 Data                 | Type         | Description
----------------------|--------------|------------------------------------------
 NPOINTS              | uint32       | Number of points
 NVERTICES            | uint32       | Number of vertices
 SIZE_VERTICES        | uint32       | Total size of vertices data
 NLINES               | uint32       | Number of lines
 SIZE_LINES           | uint32       | Total size of line data
 NPOLYGONS            | uint32       | Number of polygons
 SIZE_POLYGONS        | uint32       | Total size of polygon data
 NTRIANGLE_STRIPS     | uint32       | Number of triangle strips
 SIZE_TRIANGLE_STRIPS | uint32       | Total size of triangle strips data
 NATTRIBUTES          | uint32       | Number of dataset attributes
 POINTS               | POINTS       | Coordinates of point 0 - (NPOINTS-1)
 VERTICES             | STRUCT_ARRAY | Array of vertices
 LINES                | STRUCT_ARRAY | Array of lines
 POLYGONS             | STRUCT_ARRAY | Array of polygons
 TRIANGLE_STRIPS      | STRUCT_ARRAY | Array of triangle strips
 ATTRIBUTES           | ATTRIBUTES   | Attributes


- POINTS

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 P0X,P0Y,P0Z  | float32[3]    | Coordinates for point 0
 ...          | ...           | ...
 P(NPOINTS-1)X,P(NPOINTS-1)Y,P(NPOINTS-1)Z  | float32[3]    | Coordinates for point (NPOINTS-1)


- STRUCT_ARRAY

STRUCT_ARRAY contains an array of point indices that represent a geometric
structure, including vertices, lines, polygons, and triangle strips.
The number of structures (N_STRUCT) are specified by either NVERTICES, NLINES,
NPOLYGONS, or NTRIANGLE_STRIPS (see the table above).

 Data                     | Type         | Description
--------------------------|--------------|---------------------------------------
 STRUCT_0                 | POINT_INDICES| Point indices for 0th structure
 ...                      | ...          | ...
 STRUCT_(N_STRUCT-1)      | POINT_INDICES| Point indices for N_STRUCT-1 th structure

- POINT_INDICES

 Data                     | Type        | Description
--------------------------|-------------|---------------------------------------
 NINDICES                 | uint32      | Number of point indices
 POINT_INDEX_0            | uint32      | Index for point #0
 ...                      | ...         |
 POINT_INDEX_(NINDICES-1) | uint32      | Index for point # (NINDICES - 1)


- ATTRIBUTES

 Data                    | Type           | Description
-------------------------|----------------|----------------------------------------
 ATTRIBUTE_HEADER        |ATTRIBUTE_HEADER| Attribute header
 ATTRIBUTE_NAMES         | ATTRIBUTE_NAMES| List of attribute names
 ATTRIBUTE_0             | ATTRIBUTE_DATA | Data for attribute #0
 ...                     | ...            | ...
 ATTRIBUTE_(NATTRIBUTE-1)| ATTRIBUTE_DATA | Data for attribute #(NATTRIBUTE-1)


- ATTRIBUTE_HEADER

 Data                 | Type          | Description
----------------------|---------------|------------------------------------------
 TYPE_ATTRIBUTE_0     | uint16        | Type of dataset attribute #0 (including number of components for scalar type)
 NATTRIBUTE_0         | uint32        | Number of data for attribute #0
 ...                  | ...           | ...
 ...                  | ...           | ...
 TYPE_ATTRIBUTE(NATTRIBUTES-1)| uint16| Type of dataset attribute #(NATTRIBUTES-1)
 NATTRIBUTE(NATTRIBUTES-1)| uint32    | Number of data for attribute #(NATTRIBUTES-1)


- ATTRIBUTE_NAMES

 Data                 | Type          | Description
----------------------|---------------|------------------------------------------
 NAME_ATTRIBUTE_0     | char[]	      | Name of attribute 0
 (null)               | char          | (null)
 ...                  | ...           | ...
 ...                  | ...           | ...
 NAME_ATTRIBUTE_(NATTRIBUTES-1)| char[] | Name of attribute (NATTRIBUTES-1)
 (null)               | char          | (null)
 Padding              | (int8) NULL   | Padding (inserted to make the size of ATTRIBUTE_NAMES even)


- ATTRIBUTE_DATA

 Data | Type                          | Description
------|-------------------------------|------------------------------------------
 DATA | float32[NATTRIBUTE_0][NCOMP]  | Attribute data
 ...  | ...                           |
 DATA | float32[NATTRIBUTE_(NATTRIBUTES-1)][NCOMP] | Attribute data


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

GET_POLYDATA
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |


STT_POLYDATA
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |

or 

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 RESOL        | uint64        | Minimum interval between message (ns)*

See [Time Stamp](timestamp.md)


STP_POLYDATA
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
              |               |


RTS_POLYDATA
-------------------

 Data         | Type          | Description
--------------|---------------|-------------------------------------------------
 STATUS       | uint8         | 0: Success; 1: Error


Implementations
===================

* [igtlPolyDataMessage.h](/Source/igtlPolyDataMessage.h)
* [igtlPolyDataMessage.cxx](/Source/igtlPolyDataMessage.cxx)











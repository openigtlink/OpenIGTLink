/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    $HeadURL: $
  Language:  C
  Date:      $Date: 2010-07-21 14:15:29 -0400 (Wed, 21 Jul 2010) $
  Version:   $Revision: 6836 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <stdlib.h>
#include <string.h>

#include "igtl_polydata.h"
#include "igtl_util.h"


void igtl_export igtl_polydata_init_info(igtl_polydata_info * info)
{
  if (info)
    {
    info->header.npoints = 0;
    info->header.nvertices = 0;
    info->header.size_vertices = 0;
    info->header.nlines = 0;
    info->header.size_lines = 0;
    info->header.npolygons = 0;
    info->header.size_polygons = 0;
    info->header.ntriangle_strips = 0;
    info->header.size_triangle_strips = 0;
    info->points = NULL;
    info->vertices = NULL;
    info->lines = NULL;
    info->polygons = NULL;
    info->triangle_strips = NULL;

    info->nattributes = 0;
    info->attributes = NULL:
    }
}


int igtl_export igtl_polydata_alloc_info(igtl_polydata_info * info)
{
  size_t size;
  int    i;
  
  if (info == NULL)
    {
    return 0;
    }

  /** Points **/
  if (info->points)
    {
    free(info->points);
    info->points = NULL;
    }
  if (info->header.npoints > 0)
    {
    info->points = malloc(info->header.size_points);
    if (info->points == NULL)
      {
      return 0;
      }
    }

  /** Vertices **/
  if (info->vertices)
    {
    free(info->vertices);
    info->vertices = NULL;
    }
  if (info->header.nvertices > 0)
    {
    info->vertices = malloc(info->header.size_vertices);
    if (info->vertices == NULL)
      {
      return 0;
      }
    }

  /** Lines **/
  if (info->lines)
    {
    free(info->lines);
    info->lines = NULL;
    }
  if (info->header.nlines > 0)
    {    
    info->lines = malloc(info->header.size_lines);
    if (info->lines == NULL)
      {
      return 0;
      }
    }

  /** Polygons **/
  if (info->polygons)
    {
    free(info->polygons);
    }
  if (info->header.nlines > 0)
    {
    info->polygons = malloc(info->header.size_polygons);  
    if (info->polygons == NULL)
      {
      return 0;
      }
    }

  /** Triangle strips **/
  if (info->triangle_strips)
    {
    free(info->triangle_strips);
    }
  if (info->header.ntriangle_strips > 0)
    {
    info->triangle_strips = malloc(info->header.size_triangle_strips);
    if (info->triangle_strips == NULL)
      {
      return 0;
      }
    }

  /** Attributes **/
  if (info->attribute)
    {
    for (i = 0; i < info->nattributes; i ++)
      {
      if (info->attributes[i].name)
        {
        free(info->attributes[i].name);
        }
      if (info->attributes[i].data)
        {
        free(info->attributes[i].data);
        }
      }
    }
  if (info->header.nattributes > 0)
    {
    info->attributes = malloc(sizeof(igtl_polydata_attribute) * info->header.nattributes);
    if (info->attributes == NULL)
      {
      return 0;
      }
    }

  return 1;
  
}


int igtl_export igtl_polydata_free_info(igtl_polydata_info * info)
{

  if (info == NULL)
    {
    return 0;
    }

  /** Points **/
  if (info->points)
    {
    free(info->points);
    info->points = NULL;
    }

  /** Vertices **/
  if (info->vertices)
    {
    free(info->vertices);
    info->vertices = NULL;
    }

  /** Lines **/
  if (info->lines)
    {
    free(info->lines);
    info->lines = NULL;
    }

  /** Polygons **/
  if (info->polygons)
    {
    free(info->polygons);
    info->polygons = NULL;
    }

  /** Triangle strips **/
  if (info->triangle_strips)
    {
    free(info->triangle_strips);
    info->triangle_strips = NULL;
    }

  /** Attributes **/
  if (info->attribute)
    {
    for (i = 0; i < info->nattributes; i ++)
      {
      if (info->attributes[i].name)
        {
        free(info->attributes[i].name);
        }
      if (info->attributes[i].data)
        {
        free(info->attributes[i].data);
        }
      }
    }

  return 1;

}


int igtl_polydata_convert_byteorder_topology(igtl_uint32 * src, igtl_uint32 * dst, igtl_uint32 size)
{
  igtl_uint32 * ptr32_src;
  igtl_uint32 * ptr32_src_end;
  igtl_uint32 * ptr32_dst;

  if (!igtl_is_little_endian())
    {
    memcpy(src, dst, size);
    }
  else
    {
    ptr32_src = (igtl_uint32 *) src;
    ptr32_src_end = src + (size/sizeof(igtl_uint32));
    ptr32_dst = (igtl_uint32 *) dst;
    while (ptr32_src < ptr32_src_end)
      {
      *ptr32_dst = BYTE_SWAP_INT32(*ptr32_src);
      ptr32_dst ++;
      ptr32_src ++;
      }
    }

  return 1;
}


int igtl_export igtl_polydata_unpack(int type, void * byte_array, igtl_polydata_info * info, igtl_uint64 size)
{
  /* size = number of ponits (not number of bytes). In case of vertices, this is specfied 
     by size_vertices in igtl_polydata_header. */
  igtl_polydata_header * header;
  void * ptr;

  igtl_uint64 * ptr64_src;
  igtl_uint64 * ptr64_src_end;
  igtl_uint64 * ptr64_dst;

  igtl_polydata_attribute_header * att_header;
  igtl_polydata_attribute * att;

  int size;
  int name_length;
  char name_buf[IGTL_POLY_MAX_ATTR_NAME_LEN+1];
 

  if (byte_array == NULL || info == NULL | size == 0)
    {
    return 0;
    }

  /* POLYDATA header */
  header = (igtl_polydata_header *) byte_array;
  if (igtl_is_little_endian())
    {
    info->header.npoints              = BYTE_SWAP_INT32(header->npoints);
    info->header.nvertices            = BYTE_SWAP_INT32(header->nvertices);
    info->header.size_vertices        = BYTE_SWAP_INT32(header->size_vertices);
    info->header.nlines               = BYTE_SWAP_INT32(header->nlines);
    info->header.size_lines           = BYTE_SWAP_INT32(header->size_lines);
    info->header.npolygons            = BYTE_SWAP_INT32(header->npolygons);
    info->header.size_polygons        = BYTE_SWAP_INT32(header->size_polygons);
    info->header.ntriangle_strips     = BYTE_SWAP_INT32(header->ntriangle_strips);
    info->header.size_triangle_strips = BYTE_SWAP_INT32(header->size_triangle_strips);
    info->header.nattributes          = BYTE_SWAP_INT32(header->nattributes);
    }
  else
    {
    memcpy(info, header, sizeof(igtl_polydata_header));
    }
  
  /* Allocate memory to read data */
  /* TODO: compare the size of info before copying the header. */
  /* If the size doesn't change, avoid reallocation of memory. */
  if (igtl_polydata_alloc_info(info) == 0)
    {
    return 0;
    }
  
  /* POINT section */
  ptr = byte_array + sizeof(igtl_polydata_header);
  if (!igtl_is_little_endian())
    {
    memcpy(info->points, ptr, sizeof(igtl_float64)*info->header.npoints*3);
    }
  else 
    {
    ptr64_src = (igtl_uint64 *) ptr;
    ptr64_src_end = ptr64_src + info->header.npoints*3;
    ptr64_dst = (igtl_uint64 *) info->points;
    while (ptr64_src < ptr64_src_end)
      {
      *ptr64_dst = BYTE_SWAP_INT64(*ptr64_src);
      ptr64_dst ++;
      ptr64_src ++;
      }
    }

  ptr += sizeof(igtl_float64)*info->header.npoints*3;

  /* Check size parameters */
  if (info->header.size_vertices%sizeof(igtl_uint32) != 0 ||
      info->header.size_lines%sizeof(igtl_uint32) != 0 ||
      info->header.size_polygons%sizeof(igtl_uint32) != 0 ||
      info->header.size_triangle_strips%sizeof(igtl_uint32))
    {
    /* More than one of size parameters is multiples of 4 (size of 32-bit value) */
    return 0;
    }

  /* VERTICES section */
  igtl_polydata_convert_byteorder_topology(info->vertices, ptr, info->header.size_vertices);
  ptr += info->header.size_vertices;

  /* LINES section */
  igtl_polydata_convert_byteorder_topology(info->lines, ptr, info->header.size_lines);
  ptr += info->header.size_lines;

  /* POLYGONS section */
  igtl_polydata_convert_byteorder_topology(info->polygons, ptr, info->header.size_polygons);
  ptr += info->header.size_polygons;

  /* TRIANGLE_STRIPS section */
  igtl_polydata_convert_byteorder_topology(info->triangle_strips, ptr, info->header.size_triangle_strips);
  ptr += info->header.size_triangle_strips;

  /* Attribute header */
  for (i = 0; i < info->header.nattributes; i ++)
    {
    att = &(info->attributes[i]);
    att_header = (igtl_polydata_attribute_header *) ptr;
    if (igtl_is_little_endian())
      {
      att->type        = BYTE_SWAP_INT32(att_header->type);
      att->ncomponents = BYTE_SWAP_INT32(att_header->size);
      }
    else
      {
      att->type        = att_header->type;
      att->ncomponents = att_header->ncomponents;
      }
    ptr += sizeof(igtl_polydata_attribute_header);
    }
  
  /* Attribute names */
  size = 0;
  name_buf[IGTL_POLY_MAX_ATTR_NAME_LEN] = NULL;
  for (i = 0; i < info->header.nattributes; i ++)
    {
    name_length = strlen(ptr);
    if (name_length <= IGTL_POLY_MAX_ATTR_NAME_LEN)
      {
      info->attributes[i].name = malloc(name_length+1);
      strcpy(info->attributes[i].name, ptr);
      }
    else
      {
      /* invalid name length */
      return 0;
      }
    size += name_length;
    ptr += (name_length+1);
    }

  if (size % 2 > 0)
    {
    /* add padding */
    ptr ++;
    }

  /* Attributes */
  for (i = 0; i < info->header.nattributes; i ++)
    {
    if (info->attributres[i].type == IGTL_POLY_ATTR_TYPE_SCALAR)
      {
      n = info->attributres[i].ncomponents * info->attributres[i].size;
      size = n * sizeof(igtl_float64);
      }
    else if (info->attributes[i].type == IGTL_POLY_ATTR_TYPE_NORMAL)
      {
      n = 3 * info->attributres[i].size;
      size = n * sizeof(igtl_float64);
      }
    else if (info->attributes[i].type == IGTL_POLY_ATTR_TYPE_VECTOR)
      {
      n = 3 * info->attributres[i].size;
      size = n * sizeof(igtl_float64);
      }
    else /* TENSOR */
      {
      n = 9 * info->attributres[i].size;
      size = n * sizeof(igtl_float64);
      }
    info->attributes[i].data = (igtl_float64*)malloc(size);
    ptr64_dst = (igtl_uint64*)info->attributes[i].data;
    ptr64_src = (igtl_uint64*)ptr;
    ptr64_src_end = ptr64_src + n;
    while (ptr64_src < ptr64_src_end)
      {
      *ptr64_dst = BYTE_SWAP_INT64(*ptr64_src);
      ptr64_dst ++;
      ptr64_src ++;
      }
    ptr += size;
    }

  return 1;
}


int igtl_export igtl_polydata_pack(igtl_polydata_info * info, void * byte_array, int type)
{
  /* size = number of ponits (not number of bytes). In case of vertices, this is specfied 
     by size_vertices in igtl_polydata_header. */
  igtl_polydata_header * header;
  void * ptr;

  igtl_uint64 * ptr64_src;
  igtl_uint64 * ptr64_src_end;
  igtl_uint64 * ptr64_dst;

  if (byte_array == NULL || info == NULL)
    {
    return 0;
    }

  /* POLYDATA header */
  header = (igtl_polydata_header *) byte_array;
  if (igtl_is_little_endian())
    {
    header->npoints              = BYTE_SWAP_INT32(info->header.npoints);
    header->nvertices            = BYTE_SWAP_INT32(info->header.nvertices);
    header->size_vertices        = BYTE_SWAP_INT32(info->header.size_vertices);
    header->nlines               = BYTE_SWAP_INT32(info->header.nlines);
    header->size_lines           = BYTE_SWAP_INT32(info->header.size_lines);
    header->npolygons            = BYTE_SWAP_INT32(info->header.npolygons);
    header->size_polygons        = BYTE_SWAP_INT32(info->header.size_polygons);
    header->ntriangle_strips     = BYTE_SWAP_INT32(info->header.ntriangle_strips);
    header->size_triangle_strips = BYTE_SWAP_INT32(info->header.size_triangle_strips);
    header->nattributes          = BYTE_SWAP_INT32(info->header.nattributes);
    }
  else
    {
    memcpy(header, info, sizeof(igtl_polydata_header));
    }
  
  /* POINT serction */
  ptr = byte_array + sizeof(igtl_polydata_header);
  if (!igtl_is_little_endian())
    {
    memcpy(info->points, ptr, sizeof(igtl_float64)*info->header.npoints*3);
    }
  else
    {
    ptr64_src = (igtl_uint64 *) info->points;
    ptr64_src_end = info->header.npoints*3;
    ptr64_dst = (igtl_uint64 *) ptr;
    while (ptr64_src < ptr64_src_end)
      {
      *ptr64_dst = BYTE_SWAP_INT64(*ptr64_src);
      ptr64_dst ++;
      ptr64_src ++;
      }
    }

  ptr += sizeof(igtl_float64)*info->header.npoints*3;

  /* Check size parameters */
  if (info->header.size_vertices%sizeof(igtl_uint32) != 0 ||
      info->header.size_lines%sizeof(igtl_uint32) != 0 ||
      info->header.size_polygons%sizeof(igtl_uint32) != 0 ||
      info->header.size_triangle_strips%sizeof(igtl_uint32))
    {
    /* More than one of size parameters is multiples of 4 (size of 32-bit value) */
    return 0;
    }

  /* VERTICES section */
  igtl_polydata_convert_byteorder_topology(ptr, info->vertices,
                                           info->header.size_vertices/sizeof(igtl_uint32));
  ptr += info->header.size_vertices;

  /* LINES section */
  igtl_polydata_convert_byteorder_topology(ptr, info->lines,
                                           info->header.size_lines/sizeof(igtl_uint32));
  ptr += info->header.size_lines;

  /* POLYGONS section */
  igtl_polydata_convert_byteorder_topology(ptr, info->polygons,
                                           info->header.size_polygons/sizeof(igtl_uint32));
  ptr += info->header.size_polygons;

  /* TRIANGLE_STRIPS section */
  igtl_polydata_convert_byteorder_topology(ptr, info->triangle_strips,
                                           info->header.size_triangle_strips/sizeof(igtl_uint32));
  ptr += info->header.size_triangle_strips;

  /* Attribute header */
  for (i = 0; i < info->header.nattributes; i ++)
    {
    att = &(info->attributes[i]);
    att_header = (igtl_polydata_attribute_header *) ptr;
    if (igtl_is_little_endian())
      {
      att_header->type = BYTE_SWAP_INT32(att->type);
      att_header->size = BYTE_SWAP_INT32(att->ncomponents);
      }
    else
      {
      att_header->type        = att->type;
      att_header->ncomponents = att->ncomponents;
      }
    ptr += sizeof(igtl_polydata_attribute_header);
    }
  
  /* Attribute names */
  size = 0;
  for (i = 0; i < info->header.nattributes; i ++)
    {
    name_length = strlen(info->attributes[i].name);
    if (name_length > IGTL_POLY_MAX_ATTR_NAME_LEN)
      {
      return 0;
      }
    strcpy(ptr, info->attributes[i].name);
    size += name_length;
    ptr += (name_length+1);
    }

  if (size % 2 > 0)
    {
    /* add padding */
    *ptr = NULL:
    ptr ++;
    }

  /* Attributes */
  for (i = 0; i < info->header.nattributes; i ++)
    {
    if (info->attributres[i].type == IGTL_POLY_ATTR_TYPE_SCALAR)
      {
      n = info->attributres[i].ncomponents * info->attributres[i].size;
      size = n * sizeof(igtl_float64);
      }
    else if (info->attributes[i].type == IGTL_POLY_ATTR_TYPE_NORMAL)
      {
      n = 3 * info->attributres[i].size;
      size = n * sizeof(igtl_float64);
      }
    else if (info->attributes[i].type == IGTL_POLY_ATTR_TYPE_VECTOR)
      {
      n = 3 * info->attributres[i].size;
      size = n * sizeof(igtl_float64);
      }
    else /* TENSOR */
      {
      n = 9 * info->attributres[i].size;
      size = n * sizeof(igtl_float64);
      }
    ptr64_dst = (igtl_uint64*)ptr;
    ptr64_src = (igtl_uint64*)info->attributes[i].data;
    ptr64_src_end = ptr64_src + n;
    while (ptr64_src < ptr64_src_end)
      {
      *ptr64_dst = BYTE_SWAP_INT64(*ptr64_src);
      ptr64_dst ++;
      ptr64_src ++;
      }
    ptr += size;
    }

  return 1;
}


igtl_uint64 igtl_export igtl_polydata_get_size(igtl_polydata_info * info, int type)
{
  igtl_uint64 data_size;
  int name_len;
  int i;

  /* Header */
  data_size = sizeof(igtl_polydata_header);
  /* POINT section */
  data_size += sizeof(igtl_float64)*info->header.npoints*3;
  /* VERTICES */
  data_size += info->header.size_vertices;
  /* LINES */
  data_size += info->header.size_lines;
  /* POLYGONS */
  data_size += info->header.size_polygons;
  /* TRIANGLE_STRIPS */
  data_size += info->header.size_trinagle_strips;
  /* Attribute header */
  data_size += sizeof(igtl_polydata_attribute_header) * info->header.nattributes;
  /* Attribute names */
  for (i = 0; i < info->header.nattributes; i ++)
    {
    name_len = strlen(info->attributes[i].name);
    if (name_len > IGTL_POLY_MAX_ATTR_NAME_LEN)
      {
      /* Invaid name length */
      return 0;
      }
    data_size += name_len;
    }

  /* Attributes */
  for (i = 0; i < info->header.nattributes; i ++)
    {
    if (info->attributres[i].type == IGTL_POLY_ATTR_TYPE_SCALAR)
      {
      n = info->attributres[i].ncomponents * info->attributres[i].size;
      size = n * sizeof(igtl_float64);
      }
    else if (info->attributes[i].type == IGTL_POLY_ATTR_TYPE_NORMAL)
      {
      n = 3 * info->attributres[i].size;
      size = n * sizeof(igtl_float64);
      }
    else if (info->attributes[i].type == IGTL_POLY_ATTR_TYPE_VECTOR)
      {
      n = 3 * info->attributres[i].size;
      size = n * sizeof(igtl_float64);
      }
    else /* TENSOR */
      {
      n = 9 * info->attributres[i].size;
      size = n * sizeof(igtl_float64);
      }
    data_size += size;
    }

  return data_size;
}


igtl_uint64 igtl_export igtl_polydata_get_crc(igtl_polydata_info * info, int type, void* polydata_message)
{
  igtl_uint64   crc;
  igtl_uint64   polydata_length;
  igtl_uint16   i;
  igtl_uint16   nc;

  polydata_length = (igtl_uint32)igtl_polydata_get_size(info, type);
  crc = crc64(0, 0, 0);
  crc = crc64((unsigned char*) polydata_message, (int)polydata_length, crc);

  return crc;
}


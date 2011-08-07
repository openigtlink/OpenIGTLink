/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    $RCSfile: $
  Language:  C
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtl_types.h"
#include "igtl_header.h"
#include "igtl_polydata.h"
#include "igtl_util.h"
#include <string.h>
#include <stdio.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

/* Include serialized test data (gold standard) */
#include "igtl_test_data_polydata.h"

int main( int argc, char * argv [] )
{
  /*** Message structures and byte array ***/
  igtl_header header;
  void * body;
  
  igtl_polydata_info info;
  igtl_float64 * array;

  int rh; /* Comparison result for header */
  int rb; /* Comparison result for body */

  int i;
  igtl_float64 * ptr_f;
  igtl_uint32 *  ptr_i;
  
  static double points[8][3]={{0,0,0}, {1,0,0}, {1,1,0}, {0,1,0},
                        {0,0,1}, {1,0,1}, {1,1,1}, {0,1,1}};
  static igtl_uint32 poly[6][4]={{0,1,2,3}, {4,5,6,7}, {0,1,5,4},
                        {1,2,6,5}, {2,3,7,6}, {3,0,4,7}};

  /*** Generate test data ***/
  igtl_polydata_init_info(&info);
  info.header.npoints           = 8;
  info.header.nvertices         = 0;
  info.header.size_vertices     = 0;
  info.header.nlines            = 0;
  info.header.size_nlines       = 0;
  info.header.npolygon          = 6;
  info.header.size_npolygon     = 6 * 4 * sizeof(iglt_float64);
  info.header.ntriangle_striops = 0;
  info.header.ntriangle_striops = 0;
  info.nattributes              = 8;

  if (igtl_polydata_alloc_info(&info) == 0)
    {
    return EXIT_FAILURE;
    }

  /*** Substitute cube data ***/
  if (info.points)
    {
    ptr_f = info.points;
    for (i = 0; i < 8; i ++)
      {
      *(ptr_f++) = points[i][0];
      *(ptr_f++) = points[i][1];
      *(ptr_f++) = points[i][2];
      }
    }
  else
    {
    return EXIT_FAILURE;
    }

  /*** Substitute polygon data ***/
  if (info.polygons)
    {
    ptr_i = info.polygons;
    for (i = 0; i  < 6; i ++)
      {
      *(ptr_i++) = 4;  /* Number of points in the polygon */
      *(ptr_i++) = poly[i][0];
      *(ptr_i++) = poly[i][1];
      *(ptr_i++) = poly[i][2];
      *(ptr_i++) = poly[i][3];
      }
    }

  /** Allocate memory for pack **/
  body_size = igtl_polydata_get_size(&info, IGTL_TYPE_PREFIX_NONE);
  body = malloc(body_size);

  if (body == NULL)
    {
    igtl_polydata_free_info(&info);
    return EXIT_FAILURE;
    }

  igtl_polydata_pack(&info, body, IGTL_TYPE_PREFIX_NONE);

  /*** Set OpenIGTLink header ***/
  header.version = 1;
  strncpy( (char*)&(header.name), "POLYDATA", 12 );
  strncpy( (char*)&(header.device_name), "DeviceName", 20 );
  header.timestamp = 1234567890;
  header.body_size = body_size;
  header.crc = igtl_polydata_get_crc(&info, IGTL_TYPE_PREFIX_NONE, body);
  igtl_header_convert_byte_order( &(header) );

  /* Dumping data -- for testing */

  FILE *fp;
  fp = fopen("polydata.bin", "w");
  fwrite(&(header), IGTL_HEADER_SIZE, 1, fp);
  fwrite(body, body_size, 1, fp);
  fclose(fp);

  rh = memcmp((const void*)&header, (const void*)test_polydata_message_header, IGTL_HEADER_SIZE);
  rb = memcmp((const void*)body, (const void*)test_polydata_message_body, body_size);

  igtl_polydata_free_info(&info);
  free(body);

  if (rh == 0 && rb == 0)
    {
    return EXIT_SUCCESS;
    }
  else
    {
    /* Print first 256 bytes as HEX values in STDERR for debug */
    s = IGTL_HEADER_SIZE + body_size;
    if (s > 256)
      {
      s = 256;
      }

    fprintf(stdout, "\n===== First %d bytes of the test message =====\n", s);
    igtl_message_dump_hex(stdout, (const void*)&header, s);

    return EXIT_FAILURE;
    }

}







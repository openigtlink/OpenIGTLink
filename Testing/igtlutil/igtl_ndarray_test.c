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
#include "igtl_ndarray.h"
#include "igtl_util.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

/* Include serialized test data (gold standard) */
#include "igtl_test_data_ndarray.h"


int main( int argc, char * argv [] )
{
  /*** Message structures and byte array ***/
  igtl_header header;
  void * body;
  
  igtl_ndarray_info info;
  size_t body_size;
  igtl_uint16 size[3];

  int i, j, k;
  igtl_float64 * array;

  int rh; /* Comparison result for header */
  int rb; /* Comparison result for body */

  int s;

  /*** Generate test data ***/
  igtl_ndarray_init_info(&info);
  info.type = IGTL_NDARRAY_STYPE_TYPE_FLOAT64;
  /* Array size is 5x4x3 */
  info.dim  = 3;
  size[0] = 5;
  size[1] = 4;
  size[2] = 3;

  if (igtl_ndarray_alloc_info(&info, size) == 0)
    {
    return EXIT_FAILURE;
    }

  /* Generate dummy array */
  array = (igtl_float64 *) info.array;
  for (i = 0; i < 5; i ++)
    {
    for (j = 0; j < 4; j ++)
      {
      for (k = 0; k < 3; k ++)
        {
        array[i*(4*3) + j*3 + k] = (igtl_float64) (i*(4*3) + j*3 + k);
        }
      }
    }

  /** Allocate memory for pack **/
  body_size = (size_t)igtl_ndarray_get_size(&info, IGTL_TYPE_PREFIX_NONE);
  body = malloc(body_size);

  if (body == NULL)
    {
    igtl_ndarray_free_info(&info);
    return EXIT_FAILURE;
    }

  igtl_ndarray_pack(&info, body, IGTL_TYPE_PREFIX_NONE);

  /*** Set OpenIGTLink header ***/
  header.version = 1;
  strncpy( (char*)&(header.name), "NDARRAY", 12 );
  strncpy( (char*)&(header.device_name), "DeviceName", 20 );
  header.timestamp = 1234567890;
  header.body_size = body_size;
  header.crc = igtl_ndarray_get_crc(&info, IGTL_TYPE_PREFIX_NONE, body);
  igtl_header_convert_byte_order( &(header) );

  /* Dumping data -- for testing */
  /*
  FILE *fp;
  fp = fopen("ndarray.bin", "w");
  fwrite(&(header), IGTL_HEADER_SIZE, 1, fp);
  fwrite(body, body_size, 1, fp);
  fclose(fp);
  */

  rh = memcmp((const void*)&header, (const void*)test_ndarray_message_header, IGTL_HEADER_SIZE);
  rb = memcmp((const void*)body, (const void*)test_ndarray_message_body, body_size);

  igtl_ndarray_free_info(&info);
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







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

#include <stdio.h>
#include <string.h>

#include "igtl_types.h"
#include "igtl_header.h"
#include "igtl_colortable.h"
#include "igtl_util.h"

/* include test colortable data and serialized colortable message */
#include "igtl_test_data_colortable.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define TEST_COLORTABLE_SIZE 256

#pragma pack(1)
struct colortable_message {
  igtl_header            header;
  igtl_colortable_header cheader;     /* colortable header */
  igtl_uint8             table[TEST_COLORTABLE_SIZE];
};
#pragma pack()


int main( int argc, char * argv [] )
{

  struct colortable_message message;
  igtl_uint64 table_size;
  int r;
  int s;
  int i;

  /* Test structure size */
  if (sizeof(message) != IGTL_HEADER_SIZE+IGTL_COLORTABLE_HEADER_SIZE+TEST_COLORTABLE_SIZE)
    {
    fprintf(stdout, "Invalid size of colortable message structure.\n");
    return EXIT_FAILURE;
    }

  /* Set COLORTABLE message */
  message.cheader.indexType = IGTL_COLORTABLE_INDEX_UINT8;
  message.cheader.mapType   = IGTL_COLORTABLE_MAP_UINT8;
  for (i = 0; i < 256; i ++)
    {
    message.table[i] = i;
    }

  /* Get image data size -- note that this should be done before byte order swapping. */
  table_size = igtl_colortable_get_table_size(&(message.cheader));
  
  /* Swap byte order if necessary */
  igtl_colortable_convert_byte_order(&(message.cheader), (void*)&(message.table));

  /* Create OpenIGTLink header */
  message.header.header_version = 1;
  strncpy( (char*)&(message.header.name), "COLORTABLE", 12 );
  strncpy( (char*)&(message.header.device_name), "DeviceName", 20 );
  message.header.timestamp = 1234567892;
  message.header.body_size = IGTL_COLORTABLE_HEADER_SIZE + table_size;
  message.header.crc = igtl_colortable_get_crc(&(message.cheader), (void*)message.table);
  igtl_header_convert_byte_order( &(message.header) );

  /* Dumping data -- for debugging */
  /*
  FILE *fp;
  fp = fopen("colortable.bin", "w");
  fwrite(&(message), IGTL_HEADER_SIZE+IGTL_COLORTABLE_HEADER_SIZE+TEST_COLORTABLE_SIZE, 1, fp);
  fclose(fp);
  */

  /* Compare the serialized byte array with the gold standard */ 
  r = memcmp((const void*)&message, (const void*)test_colortable_message,
             (size_t)(IGTL_HEADER_SIZE+IGTL_COLORTABLE_HEADER_SIZE));

  if (r == 0)
    {
    return EXIT_SUCCESS;
    }
  else
    {
    /* Print first 256 bytes as HEX values in STDERR for debug */
      s = IGTL_HEADER_SIZE+IGTL_COLORTABLE_HEADER_SIZE+(int)table_size;
    if (s > 256)
      {
      s = 256;
      }

    fprintf(stdout, "\n===== First %d bytes of the test message =====\n", s);
    igtl_message_dump_hex(stdout, (const void*)&message, s);

    return EXIT_FAILURE;
    }
}

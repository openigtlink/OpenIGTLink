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
#include "igtl_capability.h"
#include "igtl_util.h"

/* include test capability data and serialized capability message */
#include "igtl_test_data_capability.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define TEST_CAPABILITY_SIZE 256

#pragma pack(1)
struct capability_message {
  igtl_header            header;
  unsigned char          body[IGTL_HEADER_TYPE_SIZE*4];
};
#pragma pack()


int main( int argc, char * argv [] )
{

  igtl_capability_info info;
  struct capability_message message;
  int r;
  int s;

  igtl_capability_alloc_info(&info, 4);

  /* Test structure size */
  if (sizeof(message) != IGTL_HEADER_SIZE+igtl_capability_get_length(&info))
    {
    fprintf(stdout, "Invalid size of capability message structure.\n");
    return EXIT_FAILURE;
    }

  strcpy((char*)info.typenames[0], "IMAGE");
  strcpy((char*)info.typenames[1], "GET_IMAGE");
  strcpy((char*)info.typenames[2], "TRANSFORM");
  strcpy((char*)info.typenames[3], "GET_TRANS");

  igtl_capability_pack(&info, message.body);

  /* Create OpenIGTLink header */
  message.header.header_version = 1;
  strncpy( (char*)&(message.header.name), "CAPABILITY", 12 );
  strncpy( (char*)&(message.header.device_name), "DeviceName", 20 );
  message.header.timestamp = 1234567892;
  message.header.body_size = igtl_capability_get_length(&info);
  message.header.crc = igtl_capability_get_crc(&info, message.body);
  igtl_header_convert_byte_order( &(message.header) );

  /* Dumping data -- for debugging */
  /*
  FILE *fp;
  fp = fopen("capability.bin", "w");
  fwrite(&(message), IGTL_HEADER_SIZE+igtl_capability_get_length(&info), 1, fp);
  fclose(fp);
  */

  /* Compare the serialized byte array with the gold standard */ 
  r = memcmp((const void*)&message, (const void*)test_capability_message,
             (size_t)(IGTL_HEADER_SIZE+igtl_capability_get_length(&info)));

  if (r == 0)
    {
    return EXIT_SUCCESS;
    }
  else
    {
    /* Print first 256 bytes as HEX values in STDERR for debug */
      s = IGTL_HEADER_SIZE+igtl_capability_get_length(&info);
    if (s > 256)
      {
      s = 256;
      }

    fprintf(stdout, "\n===== First %d bytes of the test message =====\n", s);
    igtl_message_dump_hex(stdout, (const void*)&message, s);

    return EXIT_FAILURE;
    }
}

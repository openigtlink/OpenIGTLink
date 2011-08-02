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
#include "igtl_status.h"
#include "igtl_util.h"
#include <string.h>
#include <stdio.h>


#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

/* Include serialized test data (gold standard) */
#include "igtl_test_data_status.h"

#define STR_ERROR_NAME     "ACTUATOR_DISABLED" /* within 20 characters */
#define STR_ERROR_MESSAGE  "Actuator A is disabled."

#pragma pack(1)
struct status_message {
  igtl_header        header;
  igtl_status_header status;
  char               err_msg[sizeof(STR_ERROR_MESSAGE)];
};
#pragma pack()

int main( int argc, char * argv [] )
{

  struct status_message message;
  /*igtl_uint64 crc;*/
  unsigned int msglen;
  int r;
  int s;

  // Test structure size
  if (sizeof(message) != IGTL_HEADER_SIZE+IGTL_STATUS_HEADER_SIZE+sizeof(STR_ERROR_MESSAGE))
    {
    fprintf(stdout, "Invalid size of image message structure.\n");
    return EXIT_FAILURE;
    }

  // Test binary
  msglen = sizeof(STR_ERROR_MESSAGE);

  /* Set dummy status */
  message.status.code = IGTL_STATUS_DISABLED;
  message.status.subcode = 0x0A;
  strcpy(message.err_msg, STR_ERROR_MESSAGE);
  strncpy(message.status.error_name, STR_ERROR_NAME, 20);

  igtl_status_convert_byte_order(&message.status);

  /* Set header */
  message.header.version = 1;
  strncpy( (char*)&(message.header.name), "STATUS", 12 );
  strncpy( (char*)&(message.header.device_name), "DeviceName", 20 );
  message.header.timestamp = 1234567890;
  message.header.body_size = IGTL_STATUS_HEADER_SIZE + msglen;
  message.header.crc = igtl_status_get_crc(&(message.status), msglen, message.err_msg);
  igtl_header_convert_byte_order( &(message.header) );

  /* Dumping data -- for testing */

  /*
  FILE *fp;
  fp = fopen("status.bin", "w");
  fwrite(&(message.header), IGTL_HEADER_SIZE+IGTL_STATUS_HEADER_SIZE + msglen, 1, fp);
  fclose(fp);
  */


  /* Compare the serialized byte array with the gold standard */ 
  r = memcmp((const void*)&message, (const void*)test_status_message,
                 IGTL_HEADER_SIZE+IGTL_STATUS_HEADER_SIZE+msglen);

  if (r == 0)
    {
    return EXIT_SUCCESS;
    }
  else
    {
    /* Print first 256 bytes as HEX values in STDERR for debug */
    s = IGTL_HEADER_SIZE+IGTL_STATUS_HEADER_SIZE+msglen;
    if (s > 256)
      {
      s = 256;
      }

    fprintf(stdout, "\n===== First %d bytes of the test message =====\n", s);
    igtl_message_dump_hex(stdout, (const void*)&message, s);

    return EXIT_FAILURE;
    }

}


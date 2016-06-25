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
#include "igtl_string.h"
#include "igtl_util.h"
#include <string.h>
#include <stdio.h>


#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

/* Include serialized test data (gold standard) */
#include "igtl_test_data_string.h"

#pragma pack(1)

#define IGTL_STRING_TEST_STRING "Welcome to OpenIGTLink"
#define IGTL_STRING_TEST_STRING_LEN 22

/* Test string message data (6-ch 64-bit float data, m/s^2) */
struct string_message {
  igtl_header        header;
  igtl_string_header string_header;
  igtl_uint8         string[IGTL_STRING_TEST_STRING_LEN];
};
#pragma pack()

int main( int argc, char * argv [] )
{

  struct string_message message;

  /*igtl_uint64 crc;*/
  int r;
  int s;

  // Test structure size
  if (sizeof(message) != IGTL_HEADER_SIZE+IGTL_STRING_HEADER_SIZE+IGTL_STRING_TEST_STRING_LEN)
    {
    fprintf(stdout, "Invalid size of image message structure.\n");
    return EXIT_FAILURE;
    }

  /* Set dummy string header and values */
  message.string_header.encoding = 3;
  message.string_header.length = IGTL_STRING_TEST_STRING_LEN;
  strncpy((char*)message.string, IGTL_STRING_TEST_STRING, IGTL_STRING_TEST_STRING_LEN);
  igtl_string_convert_byte_order(&(message.string_header));

  /* Set header */
  message.header.header_version = 1;
  strncpy( (char*)&(message.header.name), "STRING", 12 );
  strncpy( (char*)&(message.header.device_name), "DeviceName", 20 );
  message.header.timestamp = 1234567892;
  message.header.body_size = IGTL_STRING_HEADER_SIZE + IGTL_STRING_TEST_STRING_LEN;
  message.header.crc = igtl_string_get_crc(&(message.string_header), (void*) message.string);
  igtl_header_convert_byte_order( &(message.header) );

  /* Dumping data -- for testing */

  /*
  FILE *fp;
  fp = fopen("string.bin", "w");
  fwrite(&(message.header), IGTL_HEADER_SIZE+IGTL_STRING_HEADER_SIZE + IGTL_STRING_TEST_STRING_LEN, 1, fp);
  fclose(fp);
  */

  /* Compare the serialized byte array with the gold standard */ 

  r = memcmp((const void*)&message, (const void*)test_string_message,
                 IGTL_HEADER_SIZE+IGTL_STRING_HEADER_SIZE+IGTL_STRING_TEST_STRING_LEN);

  if (r == 0)
    {
    return EXIT_SUCCESS;
    }
  else
    {
    /* Print first 256 bytes as HEX values in STDERR for debug */
    s = IGTL_HEADER_SIZE+IGTL_STRING_HEADER_SIZE+IGTL_STRING_TEST_STRING_LEN;
    if (s > 256)
      {
      s = 256;
      }

    fprintf(stdout, "\n===== First %d bytes of the test message =====\n", s);
    igtl_message_dump_hex(stdout, (const void*)&message, s);

    return EXIT_FAILURE;
    }

}




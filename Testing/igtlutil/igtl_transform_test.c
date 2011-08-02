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
#include "igtl_transform.h"
#include "igtl_util.h"
#include <string.h>
#include <stdio.h>


#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

/* The decimal is rounded when it is converted to IEEE 754 floating point */

/* Include serialized test data (gold standard) */
#include "igtl_test_data_transform.h"

#pragma pack(1)
struct transform_message {
  igtl_header      header;
  igtl_float32     transform[12];
};
#pragma pack()

int main( int argc, char * argv [] )
{

  struct transform_message message;
  int r;

  /* Set dummy transform */
  message.transform[0] = -0.954892f;
  message.transform[1] = 0.196632f;
  message.transform[2] = -0.222525f;
  message.transform[3] = -0.196632f;
  message.transform[4] = 0.142857f;
  message.transform[5] = 0.970014f;
  message.transform[6] = 0.222525f;
  message.transform[7] = 0.970014f;
  message.transform[8] = -0.0977491f;
  message.transform[9] = 46.0531f;
  message.transform[10] = 19.4709f;
  message.transform[11] = 46.0531f;

  igtl_transform_convert_byte_order(message.transform);

  /* Set header */
  message.header.version = 1;
  strncpy( (char*)&(message.header.name), "TRANSFORM", 12 );
  strncpy( (char*)&(message.header.device_name), "DeviceName", 20 );
  message.header.timestamp = 1234567890;
  message.header.body_size = IGTL_TRANSFORM_SIZE;
  message.header.crc = igtl_transform_get_crc(message.transform);
  igtl_header_convert_byte_order( &(message.header) );

  /* Dumping data -- for testing */
  /*
  FILE *fp;
  fp = fopen("transform.bin", "w");
  fwrite(&(message.header), IGTL_HEADER_SIZE+IGTL_TRANSFORM_SIZE, 1, fp);
  fclose(fp);
  */

  /* Compare the serialized byte array with the gold standard */ 
  r = memcmp((const void*)&message, (const void*)test_transform_message,
                 IGTL_HEADER_SIZE+IGTL_TRANSFORM_SIZE);

  if (r == 0)
    {
    return EXIT_SUCCESS;
    }
  else
    {
    /* Print message as HEX values in STDERR for debug */
    fprintf(stdout, "\n===== First %d bytes of the test message =====\n", IGTL_HEADER_SIZE+IGTL_TRANSFORM_SIZE);
    igtl_message_dump_hex(stdout, (const void*)&message, IGTL_HEADER_SIZE+IGTL_TRANSFORM_SIZE);

    return EXIT_FAILURE;
    }

}




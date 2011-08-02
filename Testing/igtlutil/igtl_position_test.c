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
#include "igtl_position.h"
#include "igtl_util.h"
#include <string.h>
#include <stdio.h>


#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

/* The decimal is rounded when it is converted to IEEE 754 floating point */

/* Include serialized test data (gold standard) */
#include "igtl_test_data_position.h"

#pragma pack(1)
struct position_message {
  igtl_header      header;
  igtl_position    position;
};
#pragma pack()

int main( int argc, char * argv [] )
{

  struct position_message message;
  int r;

  // Test structure size
  if (sizeof(message) != IGTL_HEADER_SIZE+IGTL_POSITION_MESSAGE_DEFAULT_SIZE)
    {
    fprintf(stdout, "Invalid size of position message structure.\n");
    return EXIT_FAILURE;
    }

  /* Set dummy position */
  message.position.position[0] = 46.0531f;
  message.position.position[1] = 19.4709f;
  message.position.position[2] = 46.0531f;

  message.position.quaternion[0] = 0.0;
  message.position.quaternion[1] = 0.5773502691f;
  message.position.quaternion[2] = 0.5773502692f;
  message.position.quaternion[3] = 0.3333333333f;

  igtl_position_convert_byte_order(&(message.position));

  /* Set header */
  message.header.version = 1;
  strncpy( (char*)&(message.header.name), "POSITION", 12 );
  strncpy( (char*)&(message.header.device_name), "DeviceName", 20 );
  message.header.timestamp = 1234567890;
  message.header.body_size = IGTL_POSITION_MESSAGE_DEFAULT_SIZE;
  message.header.crc = igtl_position_get_crc(&(message.position));
  igtl_header_convert_byte_order( &(message.header) );

  /* Dumping data -- for testing */
  /*
  FILE *fp;
  fp = fopen("position.bin", "w");
  fwrite(&(message.header), IGTL_HEADER_SIZE+IGTL_POSITION_MESSAGE_DEFAULT_SIZE, 1, fp);
  fclose(fp);
  */

  /* Compare the serialized byte array with the gold standard */ 

  r = memcmp((const void*)&message, (const void*)test_position_message,
             IGTL_HEADER_SIZE+IGTL_POSITION_MESSAGE_DEFAULT_SIZE);

  if (r == 0)
    {
    return EXIT_SUCCESS;
    }
  else
    {
    /* Print message as HEX values in STDERR for debug */
    fprintf(stdout, "\n===== First %d bytes of the test message =====\n", IGTL_HEADER_SIZE+IGTL_POSITION_MESSAGE_DEFAULT_SIZE);
    igtl_message_dump_hex(stdout, (const void*)&message, IGTL_HEADER_SIZE+IGTL_POSITION_MESSAGE_DEFAULT_SIZE);

    return EXIT_FAILURE;
    }

}




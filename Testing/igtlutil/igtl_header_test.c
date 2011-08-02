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

#include <string.h>
#include "igtl_util.h"
#include "igtl_header.h"


#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

char barray[] = {
  0x00, 0x01,                                     /* Version number */
  0x54, 0x59, 0x50, 0x45, 0x4e, 0x41, 0x4d, 0x45,
  0x00, 0x00, 0x00, 0x00,                         /* TYPENAME */ 
  0x44, 0x65, 0x76, 0x69, 0x63, 0x65, 0x4e, 0x61,
  0x6d, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,                         /* Device name */
  0x00, 0x00, 0x00, 0x00, 0x49, 0x96, 0x02, 0xd2, /* Time stamp */
  0x00, 0x00, 0x00, 0x07, 0x50, 0x88, 0xFF, 0x06, /* Body size */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x7E, 0xA7, /* CRC */
};


int main( int argc, char * argv [] )
{

  igtl_header  header;

  // Test structure size
  if (sizeof(header) != IGTL_HEADER_SIZE)
    {
    fprintf(stdout, "Invalid size of header structure.\n");
    return EXIT_FAILURE;
    }

  // Test binary
  header.version = 1;
  strncpy( header.name, "TYPENAME", 12 );
  strncpy( header.device_name, "DeviceName", 20 );
  header.timestamp = 1234567890;
  header.body_size = 31415926534;
  header.crc = 1343143;

  igtl_header_convert_byte_order( &header );

  if (memcmp((const void*)&header, (const void*)barray, IGTL_HEADER_SIZE) == 0)
    {
    return EXIT_SUCCESS;
    }
  else
    {
    /* Print message as HEX values in STDERR for debug */
    fprintf(stdout, "\n===== First %d bytes of the test message =====\n", IGTL_HEADER_SIZE);
    igtl_message_dump_hex(stdout, (const void*)&header, IGTL_HEADER_SIZE);

    return EXIT_FAILURE;
    }

}

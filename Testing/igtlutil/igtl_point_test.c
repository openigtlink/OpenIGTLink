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
#include "igtl_point.h"
#include "igtl_util.h"

/* include test point data and serialized point message */
#include "igtl_test_data_point.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define TEST_POINT_NUM 3

#pragma pack(1)
struct point_message {
  igtl_header           header;
  igtl_point_element    pointlist[TEST_POINT_NUM];
};
#pragma pack()


int main( int argc, char * argv [] )
{

  struct point_message message;
  int r;
  int s;

  /* Test structure size */
  if (sizeof(message) != IGTL_HEADER_SIZE+IGTL_POINT_ELEMENT_SIZE*TEST_POINT_NUM)
    {
    fprintf(stdout, "Invalid size of point message structure.\n");
    return EXIT_FAILURE;
    }

  /* Point data 0 */
  strncpy((char*)&(message.pointlist[0].name), "POINT_DESCRIPTION_0", 64);
  strncpy((char*)&(message.pointlist[0].group_name), "Landmark", 32);
  message.pointlist[0].rgba[0]  = 255;
  message.pointlist[0].rgba[1]  = 0;
  message.pointlist[0].rgba[2]  = 0;
  message.pointlist[0].rgba[3]  = 255;
  message.pointlist[0].position[0]  = 10.0;
  message.pointlist[0].position[1]  = 15.0;
  message.pointlist[0].position[2]  = 20.0;
  message.pointlist[0].radius       = 5.0;
  strncpy((char*)&(message.pointlist[0].owner), "IMAGE_0", 20);

  /* Point data 1 */
  strncpy((char*)&(message.pointlist[1].name), "POINT_DESCRIPTION_1", 64);
  strncpy((char*)&(message.pointlist[1].group_name), "Landmark", 32);
  message.pointlist[1].rgba[0]  = 0;
  message.pointlist[1].rgba[1]  = 255;
  message.pointlist[1].rgba[2]  = 0;
  message.pointlist[1].rgba[3]  = 255;
  message.pointlist[1].position[0]  = 25.0;
  message.pointlist[1].position[1]  = 30.0;
  message.pointlist[1].position[2]  = 35.0;
  message.pointlist[1].radius       = 3.0;
  strncpy((char*)&(message.pointlist[1].owner), "IMAGE_0", 20);

  /* Point data 2 */
  strncpy((char*)&(message.pointlist[2].name), "POINT_DESCRIPTION_2", 64);
  strncpy((char*)&(message.pointlist[2].group_name), "Landmark", 32);
  message.pointlist[2].rgba[0]  = 0;
  message.pointlist[2].rgba[1]  = 0;
  message.pointlist[2].rgba[2]  = 255;
  message.pointlist[2].rgba[3]  = 255;
  message.pointlist[2].position[0]  = 40.0;
  message.pointlist[2].position[1]  = 45.0;
  message.pointlist[2].position[2]  = 50.0;
  message.pointlist[2].radius       = 1.0;
  strncpy((char*)&(message.pointlist[2].owner), "IMAGE_0", 20);

  /* Swap byte order if necessary */
  igtl_point_convert_byte_order(message.pointlist, TEST_POINT_NUM);

  /* Create OpenIGTLink header */
  message.header.header_version = 1;
  strncpy( (char*)&(message.header.name), "POINT", 12 );
  strncpy( (char*)&(message.header.device_name), "DeviceName", 20 );
  message.header.timestamp = 1234567892;
  message.header.body_size = IGTL_POINT_ELEMENT_SIZE*TEST_POINT_NUM;
  message.header.crc = igtl_point_get_crc(message.pointlist, TEST_POINT_NUM);
  igtl_header_convert_byte_order( &(message.header) );

  /* Dumping data -- for debugging */
  /*
  FILE *fp;
  fp = fopen("point.bin", "w");
  fwrite(&(message), IGTL_HEADER_SIZE+IGTL_POINT_ELEMENT_SIZE*TEST_POINT_NUM, 1, fp);
  fclose(fp);
  */

  /* Compare the serialized byte array with the gold standard */ 
  r = memcmp((const void*)&message, (const void*)test_point_message,
             (size_t)(IGTL_HEADER_SIZE+IGTL_POINT_ELEMENT_SIZE*TEST_POINT_NUM));

  if (r == 0)
    {
    return EXIT_SUCCESS;
    }
  else
    {
    /* Print first 256 bytes as HEX values in STDERR for debug */
    s = IGTL_HEADER_SIZE+IGTL_POINT_ELEMENT_SIZE*TEST_POINT_NUM;
    if (s > 256)
      {
      s = 256;
      }

    fprintf(stdout, "\n===== First %d bytes of the test message =====\n", s);
    igtl_message_dump_hex(stdout, (const void*)&message, s);
    
    return EXIT_FAILURE;
    }
}

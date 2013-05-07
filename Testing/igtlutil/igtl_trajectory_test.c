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
#include "igtl_trajectory.h"
#include "igtl_util.h"

/* include test trajectory data and serialized trajectory message */
#include "igtl_test_data_trajectory.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define TEST_TRAJECTORY_NUM 3

#pragma pack(1)
struct trajectory_message {
  igtl_header             header;
  igtl_trajectory_element tlist[TEST_TRAJECTORY_NUM];
};
#pragma pack()


int main( int argc, char * argv [] )
{

  struct trajectory_message message;
  int r;
  int s;

  // Test structure size
  if (sizeof(message) != IGTL_HEADER_SIZE+IGTL_TRAJECTORY_ELEMENT_SIZE*TEST_TRAJECTORY_NUM)
    {
    fprintf(stdout, "Invalid size of trajectory message structure.\n");
    return EXIT_FAILURE;
    }

  /* Trajectory data 0 */
  strncpy((char*)&(message.tlist[0].name), "TRAJECTORY_DESCRIPTION_0", 64);
  strncpy((char*)&(message.tlist[0].group_name), "TRAJECTORY", 32);
  message.tlist[0].type     = IGTL_TRAJECTORY_TYPE_ENTRY_TARGET;
  message.tlist[0].reserved = 0;
  message.tlist[0].rgba[0]  = 255;
  message.tlist[0].rgba[1]  = 0;
  message.tlist[0].rgba[2]  = 0;
  message.tlist[0].rgba[3]  = 255;
  message.tlist[0].entry_pos[0]   = 10.0;
  message.tlist[0].entry_pos[1]   = 15.0;
  message.tlist[0].entry_pos[2]   = 20.0;
  message.tlist[0].target_pos[0]  = 25.0;
  message.tlist[0].target_pos[1]  = 30.0;
  message.tlist[0].target_pos[2]  = 35.0;
  message.tlist[0].radius       = 5.0;
  strncpy((char*)&(message.tlist[0].owner_name), "IMAGE_0", 20);

  /* Trajectory data 1 */
  strncpy((char*)&(message.tlist[1].name), "TRAJECTORY_DESCRIPTION_1", 64);
  strncpy((char*)&(message.tlist[1].group_name), "TRAJECTORY", 32);
  message.tlist[1].type     = IGTL_TRAJECTORY_TYPE_ENTRY_TARGET;
  message.tlist[1].reserved = 0;
  message.tlist[1].rgba[0]  = 0;
  message.tlist[1].rgba[1]  = 255;
  message.tlist[1].rgba[2]  = 0;
  message.tlist[1].rgba[3]  = 255;
  message.tlist[1].entry_pos[0]   = 40.0;
  message.tlist[1].entry_pos[1]   = 45.0;
  message.tlist[1].entry_pos[2]   = 50.0;
  message.tlist[1].target_pos[0]  = 55.0;
  message.tlist[1].target_pos[1]  = 60.0;
  message.tlist[1].target_pos[2]  = 65.0;
  message.tlist[1].radius         = 2.5;
  strncpy((char*)&(message.tlist[1].owner_name), "IMAGE_0", 20);

  /* Trajectory data 1 */
  strncpy((char*)&(message.tlist[2].name), "TRAJECTORY_DESCRIPTION_2", 64);
  strncpy((char*)&(message.tlist[2].group_name), "TRAJECTORY", 32);
  message.tlist[2].type     = IGTL_TRAJECTORY_TYPE_ENTRY_TARGET;
  message.tlist[2].reserved = 0;
  message.tlist[2].rgba[0]  = 0;
  message.tlist[2].rgba[1]  = 0;
  message.tlist[2].rgba[2]  = 255;
  message.tlist[2].rgba[3]  = 255;
  message.tlist[2].entry_pos[0]   = 70.0;
  message.tlist[2].entry_pos[1]   = 75.0;
  message.tlist[2].entry_pos[2]   = 80.0;
  message.tlist[2].target_pos[0]  = 85.0;
  message.tlist[2].target_pos[1]  = 90.0;
  message.tlist[2].target_pos[2]  = 95.0;
  message.tlist[2].radius         = 0.0;
  strncpy((char*)&(message.tlist[2].owner_name), "IMAGE_0", 20);

  /* Swap byte order if necessary */
  igtl_trajectory_convert_byte_order(message.tlist, TEST_TRAJECTORY_NUM);

  /* Create OpenIGTLink header */
  message.header.version = 1;
  strncpy( (char*)&(message.header.name), "TRAJ", 12 );
  strncpy( (char*)&(message.header.device_name), "DeviceName", 20 );
  message.header.timestamp = 1234567890;
  message.header.body_size = IGTL_TRAJECTORY_ELEMENT_SIZE*TEST_TRAJECTORY_NUM;
  message.header.crc = igtl_trajectory_get_crc(message.tlist, TEST_TRAJECTORY_NUM);
  igtl_header_convert_byte_order( &(message.header) );

  /* Dumping data -- for debugging */
  /*
  FILE *fp;
  fp = fopen("trajectory.bin", "w");
  fwrite(&(message), IGTL_HEADER_SIZE+IGTL_TRAJECTORY_ELEMENT_SIZE*TEST_TRAJECTORY_NUM, 1, fp);
  fclose(fp);
  */

  /* Compare the serialized byte array with the gold standard */ 
  r = memcmp((const void*)&message, (const void*)test_trajectory_message,
             (size_t)(IGTL_HEADER_SIZE+IGTL_TRAJECTORY_ELEMENT_SIZE*TEST_TRAJECTORY_NUM));

  if (r == 0)
    {
    return EXIT_SUCCESS;
    }
  else
    {
    /* Print first 256 bytes as HEX values in STDERR for debug */
    s = IGTL_HEADER_SIZE+IGTL_TRAJECTORY_ELEMENT_SIZE*TEST_TRAJECTORY_NUM;
    if (s > 256)
      {
      s = 256;
      }

    fprintf(stdout, "\n===== First %d bytes of the test message =====\n", s);
    //igtl_message_dump_hex(stdout, (const void*)&message, s);

    return EXIT_FAILURE;
    }
}

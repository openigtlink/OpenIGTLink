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
#include <stdio.h>
#include "igtl_types.h"
#include "igtl_header.h"
#include "igtl_tdata.h"
#include "igtl_fulltdata.h"
#include "igtl_util.h"

/* include test fulltdata data and serialized fulltdata message */
#include "igtl_test_data_fulltdata.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define TEST_TDATA_NUM 3

#pragma pack(1)
struct fulltdata_message {
  igtl_header           header;
  igtl_fulltdata_element    tlist[TEST_TDATA_NUM];
};
#pragma pack()


int main( int argc, char * argv [] )
{

  struct fulltdata_message message;
  int r;
  int s;

  // Test structure size
  if (sizeof(message) != IGTL_HEADER_SIZE+IGTL_FULLTDATA_ELEMENT_SIZE*TEST_TDATA_NUM)
    {
    fprintf(stdout, "Invalid size of fulltdata message structure.\n");
    return EXIT_FAILURE;
    }

  /* Tracking data 0 */
  strncpy((char*)&(message.tlist[0].name), "Tracker0", 20);
  message.tlist[0].type     = IGTL_TDATA_TYPE_6D;
  message.tlist[0].reserved = 0;
  message.tlist[0].transform[0] = -0.954892f;
  message.tlist[0].transform[1] = 0.196632f;
  message.tlist[0].transform[2] = -0.222525f;
  message.tlist[0].transform[3] = 0.0f;
  message.tlist[0].transform[4] = -0.196632f;
  message.tlist[0].transform[5] = 0.142857f;
  message.tlist[0].transform[6] = 0.970014f;
  message.tlist[0].transform[7] = 0.0f;
  message.tlist[0].transform[8] = 0.222525f;
  message.tlist[0].transform[9] = 0.970014f;
  message.tlist[0].transform[10] = -0.0977491f;
  message.tlist[0].transform[11] = 0.0f;
  message.tlist[0].transform[12] = 46.0531f;
  message.tlist[0].transform[13] = 19.4709f;
  message.tlist[0].transform[14] = 46.0531f;
  message.tlist[0].transform[15] = 1.0f;

  message.tlist[0].error = 46.0531f;
  
  /* Tracking data 1 */
  strncpy((char*)&(message.tlist[1].name), "Tracker1", 20);
  message.tlist[1].type     = IGTL_TDATA_TYPE_6D;
  message.tlist[1].reserved = 0;
  message.tlist[1].transform[0] = -0.954892f;
  message.tlist[1].transform[1] = 0.196632f;
  message.tlist[1].transform[2] = -0.222525f;
  message.tlist[1].transform[3] = 0.0f;
  message.tlist[1].transform[4] = -0.196632f;
  message.tlist[1].transform[5] = 0.142857f;
  message.tlist[1].transform[6] = 0.970014f;
  message.tlist[1].transform[7] = 0.0f;
  message.tlist[1].transform[8] = 0.222525f;
  message.tlist[1].transform[9] = 0.970014f;
  message.tlist[1].transform[10] = -0.0977491f;
  message.tlist[1].transform[11] = 0.0f;
  message.tlist[1].transform[12] = 46.0531f;
  message.tlist[1].transform[13] = 19.4709f;
  message.tlist[1].transform[14] = 46.0531f;
  message.tlist[1].transform[15] = 1.0f;
  message.tlist[1].error = 46.0531f;

  /* Tracking data 2 */
  strncpy((char*)&(message.tlist[2].name), "Tracker2", 20);
  message.tlist[2].type     = IGTL_TDATA_TYPE_6D;
  message.tlist[2].reserved = 0;
  message.tlist[2].transform[0] = -0.954892f;
  message.tlist[2].transform[1] = 0.196632f;
  message.tlist[2].transform[2] = -0.222525f;
  message.tlist[2].transform[3] = 0.0f;
  message.tlist[2].transform[4] = -0.196632f;
  message.tlist[2].transform[5] = 0.142857f;
  message.tlist[2].transform[6] = 0.970014f;
  message.tlist[2].transform[7] = 0.0f;
  message.tlist[2].transform[8] = 0.222525f;
  message.tlist[2].transform[9] = 0.970014f;
  message.tlist[2].transform[10] = -0.0977491f;
  message.tlist[2].transform[11] = 0.0f;
  message.tlist[2].transform[12] = 46.0531f;
  message.tlist[2].transform[13] = 19.4709f;
  message.tlist[2].transform[14] = 46.0531f;
  message.tlist[2].transform[15] = 1.0f;
  message.tlist[2].error = 46.0531f;

  /* Swap byte order if necessary */
  igtl_fulltdata_convert_byte_order(message.tlist, TEST_TDATA_NUM);

  /* Create OpenIGTLink header */
  message.header.version = 1;
  strncpy( (char*)&(message.header.name), "FULLTDATA", 12 );
  strncpy( (char*)&(message.header.device_name), "DeviceName", 20 );
  message.header.timestamp = 1234567890;
  message.header.body_size = IGTL_FULLTDATA_ELEMENT_SIZE*TEST_TDATA_NUM;
  message.header.crc = igtl_fulltdata_get_crc(message.tlist, TEST_TDATA_NUM);
  igtl_header_convert_byte_order( &(message.header) );

  /* Dumping data -- for debugging */
  /*
  FILE *fp;
  fp = fopen("tdata.bin", "w");
  fwrite(&(message), IGTL_HEADER_SIZE+IGTL_TDATA_ELEMENT_SIZE*TEST_TDATA_NUM, 1, fp);
  fclose(fp);
  */


  /* Compare the serialized byte array with the gold standard */ 
  r = memcmp((const void*)&message, (const void*)test_fulltdata_message,
             (size_t)(IGTL_HEADER_SIZE+IGTL_FULLTDATA_ELEMENT_SIZE*TEST_TDATA_NUM));

  if (r == 0)
    {
    return EXIT_SUCCESS;
    }
  else
    {
    /* Print first 256 bytes as HEX values in STDERR for debug */
    s = IGTL_HEADER_SIZE+IGTL_FULLTDATA_ELEMENT_SIZE*TEST_TDATA_NUM;
    if (s > 256)
      {
     // s = 256;
      }

    fprintf(stdout, "\n===== First %d bytes of the test message =====\n", s);
    igtl_message_dump_hex(stdout, (const void*)&message, s);

    fprintf(stdout, "\n===== First %d bytes of the gs message =====\n", s);
    igtl_message_dump_hex(stdout, (const void*)&test_fulltdata_message, s);
    return EXIT_FAILURE;
    }
}

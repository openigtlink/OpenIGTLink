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
#include "igtl_imgmeta.h"
#include "igtl_util.h"

/* include test imgmeta data and serialized imgmeta message */
#include "igtl_test_data_imgmeta.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define TEST_IMGMETA_NUM 3

#pragma pack(1)
struct imgmeta_message {
  igtl_header            header;
  igtl_imgmeta_element   metalist[TEST_IMGMETA_NUM];
};
#pragma pack()


int main( int argc, char * argv [] )
{

  struct imgmeta_message message;
  int r;
  int s;

  /* Test structure size */
  if (sizeof(message) != IGTL_HEADER_SIZE+IGTL_IMGMETA_ELEMENT_SIZE*TEST_IMGMETA_NUM)
    {
    fprintf(stdout, "Invalid size of imgmeta message structure.\n");
    return EXIT_FAILURE;
    }

  /* Image meta data 0 */
  strncpy((char*)&(message.metalist[0].name), "IMAGE_DESCRIPTION_0", 64);
  strncpy((char*)&(message.metalist[0].device_name), "IMAGE_0", 20);
  strncpy((char*)&(message.metalist[0].modality), "CT", 32);
  strncpy((char*)&(message.metalist[0].patient_name), "PATIENT_0", 64);
  strncpy((char*)&(message.metalist[0].patient_id),   "PATIENT_ID_0", 64);
  message.metalist[0].timestamp = 1234567892;
  message.metalist[0].size[0]   = 512;
  message.metalist[0].size[1]   = 512;
  message.metalist[0].size[2]   = 64;
  message.metalist[0].scalar_type = IGTL_IMAGE_STYPE_TYPE_UINT16;
  message.metalist[0].reserved = 0;

  /* Image meta data 1 */
  strncpy((char*)&(message.metalist[1].name), "IMAGE_DESCRIPTION_1", 64);
  strncpy((char*)&(message.metalist[1].device_name), "IMAGE_1", 20);
  strncpy((char*)&(message.metalist[1].modality), "MRI", 32);
  strncpy((char*)&(message.metalist[1].patient_name), "PATIENT_1", 64);
  strncpy((char*)&(message.metalist[1].patient_id),   "PATIENT_ID_1", 64);
  message.metalist[1].timestamp = 1234567896;
  message.metalist[1].size[0]   = 256;
  message.metalist[1].size[1]   = 128;
  message.metalist[1].size[2]   = 32;
  message.metalist[1].scalar_type = IGTL_IMAGE_STYPE_TYPE_UINT16;
  message.metalist[1].reserved = 0;

  /* Image meta data 2 */
  strncpy((char*)&(message.metalist[2].name), "IMAGE_DESCRIPTION_2", 64);
  strncpy((char*)&(message.metalist[2].device_name), "IMAGE_2", 20);
  strncpy((char*)&(message.metalist[2].modality), "PET", 32);
  strncpy((char*)&(message.metalist[2].patient_name), "PATIENT_2", 64);
  strncpy((char*)&(message.metalist[2].patient_id),   "PATIENT_ID_2", 64);
  message.metalist[2].timestamp = 1234567900;
  message.metalist[2].size[0]   = 256;
  message.metalist[2].size[1]   = 256;
  message.metalist[2].size[2]   = 32;
  message.metalist[2].scalar_type = IGTL_IMAGE_STYPE_TYPE_UINT16;
  message.metalist[2].reserved = 0;
  
  /* Swap byte order if necessary */
  igtl_imgmeta_convert_byte_order(message.metalist, TEST_IMGMETA_NUM);

  /* Create OpenIGTLink header */
  message.header.header_version = 1;
  strncpy( (char*)&(message.header.name), "IMGMETA", 12 );
  strncpy( (char*)&(message.header.device_name), "DeviceName", 20 );
  message.header.timestamp = 1234567892;
  message.header.body_size = IGTL_IMGMETA_ELEMENT_SIZE*TEST_IMGMETA_NUM;
  message.header.crc = igtl_imgmeta_get_crc(message.metalist, TEST_IMGMETA_NUM);
  igtl_header_convert_byte_order( &(message.header) );

  /* Dumping data -- for debugging */
  /*
  FILE *fp;
  fp = fopen("imgmeta.bin", "w");
  fwrite(&(message), IGTL_HEADER_SIZE+IGTL_IMGMETA_ELEMENT_SIZE*TEST_IMGMETA_NUM, 1, fp);
  fclose(fp);
  */

  /* Compare the serialized byte array with the gold standard */ 
  r = memcmp((const void*)&message, (const void*)test_imgmeta_message,
             (size_t)(IGTL_HEADER_SIZE+IGTL_IMGMETA_ELEMENT_SIZE*TEST_IMGMETA_NUM));

  if (r == 0)
    {
    return EXIT_SUCCESS;
    }
  else
    {
    /* Print first 256 bytes as HEX values in STDERR for debug */
    s = IGTL_HEADER_SIZE+IGTL_IMGMETA_ELEMENT_SIZE*TEST_IMGMETA_NUM;
    if (s > 256)
      {
      s = 256;
      }

    fprintf(stdout, "\n===== First %d bytes of the test message =====\n", s);
    igtl_message_dump_hex(stdout, (const void*)&message, s);

    return EXIT_FAILURE;
    }
}

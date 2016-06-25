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
#include "igtl_image.h"
#include "igtl_util.h"

/* include test image data and serialized image message */
#include "igtl_test_data_image.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1


#pragma pack(1)
struct image_message {
  igtl_header        header;
  igtl_image_header  iheader;     /* image header */
  igtl_uint8         image[TEST_IMAGE_MESSAGE_SIZE];
};
#pragma pack()


int main( int argc, char * argv [] )
{

  struct image_message message;
  int r;
  igtl_uint64 image_size;
  int s;

  igtl_float32 spacing[] = {1.0f, 1.0f, 1.0f};
  igtl_float32 origin[]  = {46.0531f, 19.4709f, 46.0531f};
  igtl_float32 norm_i[]  = {-0.954892f, 0.196632f, -0.222525f};
  igtl_float32 norm_j[]  = {-0.196632f, 0.142857f, 0.970014f};
  igtl_float32 norm_k[]  = {0.222525f, 0.970014f, -0.0977491f};

  /* Test structure size */
  if (sizeof(message) != IGTL_HEADER_SIZE+IGTL_IMAGE_HEADER_SIZE+TEST_IMAGE_MESSAGE_SIZE)
    {
    fprintf(stdout, "Invalid size of image message structure.\n");
    return EXIT_FAILURE;
    }

  /* Set data */
  message.iheader.header_version     = 1;
  message.iheader.num_components = 1; /* Scalar */
  message.iheader.scalar_type = 3; /* uint8 */
  message.iheader.endian      = 2; /* Little endian */
  message.iheader.coord       = 1; /* RAS */
  message.iheader.size[0] = 50;
  message.iheader.size[1] = 50;
  message.iheader.size[2] = 1;

  /* Dimensions */
  message.iheader.subvol_offset[0] = 0;
  message.iheader.subvol_offset[1] = 0;
  message.iheader.subvol_offset[2] = 0;
  message.iheader.subvol_size[0] = 50;
  message.iheader.subvol_size[1] = 50;
  message.iheader.subvol_size[2] = 1;

  igtl_image_set_matrix(spacing, origin, norm_i, norm_j, norm_k, &(message.iheader));

  /* Copy image data */
  memcpy((void*)message.image, (void*)test_image, TEST_IMAGE_MESSAGE_SIZE);
  
  /* Get image data size -- note that this should be done before byte order swapping. */
  image_size = igtl_image_get_data_size(&(message.iheader));
  
  /* Swap byte order if necessary */
  igtl_image_convert_byte_order(&(message.iheader));

  /* Create OpenIGTLink header */
  message.header.header_version = 1;
  strncpy( (char*)&(message.header.name), "IMAGE", 12 );
  strncpy( (char*)&(message.header.device_name), "DeviceName", 20 );
  message.header.timestamp = 1234567892;
  message.header.body_size = IGTL_IMAGE_HEADER_SIZE + image_size;
  message.header.crc = igtl_image_get_crc(&(message.iheader), message.image);
  igtl_header_convert_byte_order( &(message.header) );

  /* Dumping data -- for debugging */
  /*
  FILE *fp;
  fp = fopen("image.bin", "w");
  fwrite(&(message), IGTL_HEADER_SIZE+IGTL_IMAGE_HEADER_SIZE+image_size, 1, fp);
  fclose(fp);
  */

  /* Compare the serialized byte array with the gold standard */ 
  r = memcmp((const void*)&message, (const void*)test_image_message,
                 (size_t)(IGTL_HEADER_SIZE+IGTL_IMAGE_HEADER_SIZE+image_size));

  if (r == 0)
    {
    return EXIT_SUCCESS;
    }
  else
    {
    /* Print first 256 bytes as HEX values in STDERR for debug */
    s = IGTL_HEADER_SIZE+IGTL_IMAGE_HEADER_SIZE+ (int)image_size;
    if (s > 256)
      {
      s = 256;
      }

    fprintf(stdout, "\n===== First %d bytes of the test message =====\n", s);
    igtl_message_dump_hex(stdout, (const void*)&message, s);

    return EXIT_FAILURE;
    }
}

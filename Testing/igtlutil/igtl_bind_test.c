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
#include "igtl_image.h"
#include "igtl_sensor.h"
#include "igtl_bind.h"
#include "igtl_util.h"
#include "igtl_transform.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

/* Include serialized test data (gold standard) */
#include "igtl_test_data_bind.h"
#include "igtl_test_data_transform.h"
#include "igtl_test_data_image.h"
#include "igtl_test_data_status.h"

#pragma pack(1)

/*
 * Structure for byte array of test message body.
 * Since sizes of all child messages in this test program are even,
 * there is no padding inserted at the end of each child message.
 * In general, a padding byte is added to the end of each message body,
 * if the message body size is odd.
 *
 */

typedef struct {
  igtl_float32     transform[12];
} transform_message_body;

typedef struct {
  igtl_image_header  iheader;     /* image header */
  igtl_uint8         image[TEST_IMAGE_MESSAGE_SIZE];
} image_message_body;

typedef struct {
  igtl_sensor_header sensor;
  igtl_float64       value[6];
} sensor_message_body;

typedef struct {
  transform_message_body transform;
  image_message_body     image;
  sensor_message_body    sensor;
} child_message_body;

#pragma pack()


void generate_transform_body(transform_message_body * body)
{
  body->transform[0] = -0.954892f;
  body->transform[1] = 0.196632f;
  body->transform[2] = -0.222525f;
  body->transform[3] = -0.196632f;
  body->transform[4] = 0.142857f;
  body->transform[5] = 0.970014f;
  body->transform[6] = 0.222525f;
  body->transform[7] = 0.970014f;
  body->transform[8] = -0.0977491f;
  body->transform[9] = 46.0531f;
  body->transform[10] = 19.4709f;
  body->transform[11] = 46.0531f;

  igtl_transform_convert_byte_order(body->transform);
}


igtl_uint64 generate_image_body(image_message_body * body)
{
  igtl_uint64 image_size;

  igtl_float32 spacing[] = {1.0f, 1.0f, 1.0f};
  igtl_float32 origin[]  = {46.0531f, 19.4709f, 46.0531f};
  igtl_float32 norm_i[]  = {-0.954892f, 0.196632f, -0.222525f};
  igtl_float32 norm_j[]  = {-0.196632f, 0.142857f, 0.970014f};
  igtl_float32 norm_k[]  = {0.222525f, 0.970014f, -0.0977491f};

  /* Set data */
  body->iheader.version     = 1;
  body->iheader.num_components = 1; /* Scalar */
  body->iheader.scalar_type = 3; /* uint8 */
  body->iheader.endian      = 2; /* Little endian */
  body->iheader.coord       = 1; /* RAS */
  body->iheader.size[0] = 50;
  body->iheader.size[1] = 50;
  body->iheader.size[2] = 1;

  /* Dimensions */
  body->iheader.subvol_offset[0] = 0;
  body->iheader.subvol_offset[1] = 0;
  body->iheader.subvol_offset[2] = 0;
  body->iheader.subvol_size[0] = 50;
  body->iheader.subvol_size[1] = 50;
  body->iheader.subvol_size[2] = 1;

  igtl_image_set_matrix(spacing, origin, norm_i, norm_j, norm_k, &(body->iheader));

  /* Copy image data */
  memcpy((void*)body->image, (void*)test_image, TEST_IMAGE_MESSAGE_SIZE);
  
  /* Get image data size -- note that this should be done before byte order swapping. */
  image_size = igtl_image_get_data_size(&(body->iheader));
  
  /* Swap byte order if necessary */
  igtl_image_convert_byte_order(&(body->iheader));

  return image_size;
}


unsigned int generate_sensor_body(sensor_message_body * body)
{
  igtl_unit_data unit_data;

  /*igtl_uint64 crc;*/
  unsigned int value_size;

  /* Create unit (m/s^2) */
  igtl_unit_init(&unit_data);
  unit_data.prefix = IGTL_UNIT_PREFIX_NONE;
  unit_data.unit[0] = IGTL_UNIT_SI_BASE_METER;
  unit_data.exp[0] = (igtl_int8) 1;
  unit_data.unit[1] = IGTL_UNIT_SI_BASE_SECOND;
  unit_data.exp[1] = (igtl_int8) -2;

  /* Set dummy sensor header and values */
  body->sensor.larray = 6;
  body->sensor.status = 0;
  body->sensor.unit = igtl_unit_pack(&(unit_data));
  
  body->value[0] = 123456.78;
  body->value[1] = 12345.678;
  body->value[2] = 1234.5678;
  body->value[3] = 123.45678;
  body->value[4] = 12.345678;
  body->value[5] = 1.2345678;

  value_size = igtl_sensor_get_data_size(&(body->sensor));
  igtl_sensor_convert_byte_order(&(body->sensor), body->value);

  return value_size;
}


int main( int argc, char * argv [] )
{
  /* Message structures and byte array */
  igtl_header header;
  void* bind_header;
  child_message_body child_body;
  
  igtl_bind_info bind_info;
  size_t bind_size;
  size_t child_body_size;
  int rh; /* Comparison result for header */
  int rb; /* Comparison result for BIND header section*/
  int rc; /* Comparison result for child body section */

  int s;

  igtl_bind_init_info(&bind_info);
  
  /* Generate transform message */
  generate_transform_body(&(child_body.transform));

  /* Generate image message */
  generate_image_body(&(child_body.image));

  /* Generate sensor message */
  generate_sensor_body(&(child_body.sensor));

  /* Set up BIND info structure */
  if (igtl_bind_alloc_info(&bind_info, 3) == 0)
    {
    return EXIT_FAILURE;
    }

  strncpy(bind_info.child_info_array[0].type, "TRANSFORM", IGTL_HEADER_TYPE_SIZE);
  strncpy(bind_info.child_info_array[0].name, "ChildTrans", IGTL_HEADER_NAME_SIZE);
  bind_info.child_info_array[0].size = sizeof(transform_message_body);
  bind_info.child_info_array[0].ptr = (void*)&child_body.transform;

  strncpy(bind_info.child_info_array[1].type, "IMAGE", IGTL_HEADER_TYPE_SIZE);
  strncpy(bind_info.child_info_array[1].name, "ChildImage", IGTL_HEADER_NAME_SIZE);
  bind_info.child_info_array[1].size = sizeof(image_message_body);
  bind_info.child_info_array[1].ptr = (void*)&child_body.image;

  strncpy(bind_info.child_info_array[2].type, "SENSOR", IGTL_HEADER_TYPE_SIZE);
  strncpy(bind_info.child_info_array[2].name, "ChildSensor", IGTL_HEADER_NAME_SIZE);
  bind_info.child_info_array[2].size = sizeof(sensor_message_body);
  bind_info.child_info_array[2].ptr = (void*)&child_body.sensor;

  bind_size = (size_t)igtl_bind_get_size(&bind_info, IGTL_TYPE_PREFIX_NONE);
  bind_header = malloc(bind_size);


  
  if (bind_header == NULL)
    {
    igtl_bind_free_info(&bind_info);
    return EXIT_FAILURE;
    }

  igtl_bind_pack(&bind_info, bind_header, IGTL_TYPE_PREFIX_NONE);

  /* Calculate the sum of child body size and paddings (0 in this test program) */
  child_body_size = (size_t)
    (bind_info.child_info_array[0].size +
    bind_info.child_info_array[1].size +
    bind_info.child_info_array[2].size);

  /* Set header */
  header.version = 1;
  strncpy( (char*)&(header.name), "BIND", 12 );
  strncpy( (char*)&(header.device_name), "DeviceName", 20 );
  header.timestamp = 1234567890;
  header.body_size = bind_size + sizeof(child_message_body);
  header.crc = igtl_bind_get_crc(&bind_info, IGTL_TYPE_PREFIX_NONE, bind_header);
  igtl_header_convert_byte_order( &(header) );


  /* Dumping data -- for testing */
  /*
  FILE *fp;
  fp = fopen("bind.bin", "w");
  fwrite(&(header), IGTL_HEADER_SIZE, 1, fp);
  fwrite(bind_header, bind_size, 1, fp);
  fwrite(&(child_body), child_body_size, 1, fp);
  fclose(fp);
  */


  /* Compare the serialized byte array with the gold standard */ 
  rh = memcmp((const void*)&header, (const void*)test_bind_message_header, IGTL_HEADER_SIZE);
  rb = memcmp((const void*)bind_header, (const void*)test_bind_message_bind_header, bind_size);
  rc = memcmp((const void*)&child_body, (const void*)test_bind_message_bind_body, child_body_size);

  igtl_bind_free_info(&bind_info);
  free(bind_header);

  if (rh == 0 && rb == 0 && rc == 0)
    {
    return EXIT_SUCCESS;
    }
  else
    {
    /* Print first 256 bytes as HEX values in STDERR for debug */
    s = IGTL_HEADER_SIZE+bind_size+child_body_size;
    if (s > 256)
      {
      s = 256;
      }

    fprintf(stdout, "\n===== First %d bytes of the test message =====\n", s);
    igtl_message_dump_hex(stdout, (const void*)&header, s);

    return EXIT_FAILURE;
    }

}







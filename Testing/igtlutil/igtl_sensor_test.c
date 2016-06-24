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
#include "igtl_sensor.h"
#include "igtl_util.h"
#include <string.h>
#include <stdio.h>


#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

/* Include serialized test data (gold standard) */
#include "igtl_test_data_sensor.h"

#pragma pack(1)

/* Test sensor message data (6-ch 64-bit float data, m/s^2) */
struct sensor_message {
  igtl_header        header;
  igtl_sensor_header sensor;
  igtl_float64       value[6];
};
#pragma pack()

int main( int argc, char * argv [] )
{

  struct sensor_message message;
  igtl_unit_data unit_data;

  /*igtl_uint64 crc;*/
  unsigned int value_size;
  int r;
  int s;

  // Test structure size
  if (sizeof(message) != IGTL_HEADER_SIZE+IGTL_SENSOR_HEADER_SIZE+sizeof(igtl_float64)*6)
    {
    fprintf(stdout, "Invalid size of sensor message structure.\n");
    return EXIT_FAILURE;
    }

  /* Create unit (m/s^2) */
  igtl_unit_init(&unit_data);
  unit_data.prefix = IGTL_UNIT_PREFIX_NONE;
  unit_data.unit[0] = IGTL_UNIT_SI_BASE_METER;
  unit_data.exp[0] = (igtl_int8) 1;
  unit_data.unit[1] = IGTL_UNIT_SI_BASE_SECOND;
  unit_data.exp[1] = (igtl_int8) -2;

  /* Set dummy sensor header and values */
  message.sensor.larray = 6;
  message.sensor.status = 0;
  message.sensor.unit = igtl_unit_pack(&(unit_data));
  
  message.value[0] = 123456.78;
  message.value[1] = 12345.678;
  message.value[2] = 1234.5678;
  message.value[3] = 123.45678;
  message.value[4] = 12.345678;
  message.value[5] = 1.2345678;

  value_size = igtl_sensor_get_data_size(&(message.sensor));
  igtl_sensor_convert_byte_order(&(message.sensor), message.value);

  /* Set header */
  message.header.header_version = 1;
  strncpy( (char*)&(message.header.name), "SENSOR", 12 );
  strncpy( (char*)&(message.header.device_name), "DeviceName", 20 );
  message.header.timestamp = 1234567892;
  message.header.body_size = IGTL_SENSOR_HEADER_SIZE + value_size;
  message.header.crc = igtl_sensor_get_crc(&(message.sensor), message.value);
  igtl_header_convert_byte_order( &(message.header) );

  /* Dumping data -- for testing */

  /*
  FILE *fp;
  fp = fopen("sensor.bin", "w");
  fwrite(&(message.header), IGTL_HEADER_SIZE+IGTL_SENSOR_HEADER_SIZE + value_size, 1, fp);
  fclose(fp);
  */


  /* Compare the serialized byte array with the gold standard */ 

  r = memcmp((const void*)&message, (const void*)test_sensor_message,
                 IGTL_HEADER_SIZE+IGTL_SENSOR_HEADER_SIZE+value_size);

  if (r == 0)
    {
    return EXIT_SUCCESS;
    }
  else
    {
    /* Print first 256 bytes as HEX values in STDERR for debug */
    s = IGTL_HEADER_SIZE+IGTL_SENSOR_HEADER_SIZE+value_size;
    if (s > 256)
      {
      s = 256;
      }

    fprintf(stdout, "\n===== First %d bytes of the test message =====\n", s);
    igtl_message_dump_hex(stdout, (const void*)&message, s);

    return EXIT_FAILURE;
    }

}




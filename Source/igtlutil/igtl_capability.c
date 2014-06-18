/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <stdlib.h>
#include <string.h>

#include "igtl_capability.h"
#include "igtl_util.h"
#include "igtl_header.h"

void igtl_export igtl_capability_init_info(igtl_capability_info * info)
{
  if (info)
    {
    info->ntypes = 0;
    info->typenames = NULL;
    }
}


int igtl_export igtl_capability_alloc_info(igtl_capability_info * info, int ntypes)
{
  unsigned int i;
  unsigned char * ptr;

  if (info == NULL)
    {
    return 0;
    }
  
  info->ntypes = ntypes;

  info->typenames = malloc(sizeof(unsigned char*) * info->ntypes);
  if (info->typenames == NULL)
    {
    /* failed to allocate memory */
    return 0;
    }

  ptr = malloc(sizeof(unsigned char) * (IGTL_HEADER_TYPE_SIZE+1) * info->ntypes);
  if (ptr == NULL)
    {
    free(info->typenames);
    info->ntypes = 0;
    return 0;
    }
  
  for (i = 0; i < info->ntypes; i ++)
    {
    info->typenames[i] = ptr;
    ptr += sizeof(unsigned char) * (IGTL_HEADER_TYPE_SIZE+1);
    }

  return 1;
}


int igtl_export igtl_capability_free_info(igtl_capability_info * info)
{

  if (info == NULL)
    {
    return 0;
    }

  if (info->typenames == NULL)
    {
    return 0;
    }

  free(info->typenames[0]);
  free(info->typenames);
  info->ntypes = 0;

  return 1;
}


igtl_uint32 igtl_export igtl_capability_get_length(igtl_capability_info * info)
{

  if (info == NULL)
    {
    return 0;
    }

  return (info->ntypes * IGTL_HEADER_TYPE_SIZE);
}


int igtl_export igtl_capability_unpack(void * byte_array, igtl_capability_info * info, igtl_uint64 pack_size)
{
  int ntypes;
  int i;
  unsigned char * ptr;

  if (pack_size % IGTL_HEADER_TYPE_SIZE > 0)
    {
    /* In valid body size */
    return 0;
    }

  if (info == NULL)
    {
    return 0;
    }

  ntypes = (int)((int)pack_size / IGTL_HEADER_TYPE_SIZE);

  /* Adjust the size of info->typenames */
  if (info->ntypes != ntypes)
    {
    igtl_capability_free_info(info);
    igtl_capability_alloc_info(info, ntypes);
    }

  ptr = byte_array;
  for (i = 0; i < ntypes; i ++)
    {
    strncpy((char*)info->typenames[i], (char*)ptr, IGTL_HEADER_TYPE_SIZE);
    info->typenames[i][IGTL_HEADER_TYPE_SIZE] = '\0';
    ptr += IGTL_HEADER_TYPE_SIZE;
    }

  return 1;

}


int igtl_export igtl_capability_pack(igtl_capability_info * info, void * byte_array)
{
  unsigned int i;
  unsigned char * ptr;
  
  if (info == NULL)
    {
    return 0;
    }
  
  if (byte_array == NULL)
    {
    return 0;
    }
  
  ptr = byte_array;
  
  for (i = 0; i < info->ntypes; i ++)
    {
    strncpy((char*)ptr, (char*)info->typenames[i], IGTL_HEADER_TYPE_SIZE);
    ptr += IGTL_HEADER_TYPE_SIZE;
    }
  
  return 1;
  
}

igtl_uint64 igtl_export igtl_capability_get_crc(igtl_capability_info * info, void* capability)
{
  igtl_uint64   crc;
  igtl_uint64   message_length;
  
  message_length = (igtl_uint32)igtl_capability_get_length(info);
  crc = crc64(0, 0, 0);
  crc = crc64((unsigned char*) capability, (int)message_length, crc);
  
  return crc;
}





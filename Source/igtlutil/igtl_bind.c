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

#include "igtl_bind.h"
#include "igtl_util.h"


void igtl_export igtl_bind_init_info(igtl_bind_info * bind_info)
{
  if (bind_info)
    {
    bind_info->ncmessages = 0;
    bind_info->child_info_array = NULL;
    bind_info->resol = 0;
    bind_info->request_all = 0;
    bind_info->status = 0;
    }
}


int igtl_export igtl_bind_alloc_info(igtl_bind_info * bind_info, igtl_uint16 ncmessages)
{
  size_t size;
  
  if (bind_info == NULL)
    {
    return 0;
    }

  size = (size_t) ncmessages * sizeof(igtl_bind_child_info);
  bind_info->child_info_array = malloc(size);

  if (bind_info->child_info_array != NULL)
    {
    bind_info->ncmessages = ncmessages;
    memset(bind_info->child_info_array, 0, size);
    return 1;
    }
  else
    {
    bind_info->ncmessages = 0;
    return 0;
    }
}


int igtl_export igtl_bind_free_info(igtl_bind_info * bind_info)
{

  if (bind_info == NULL)
    {
    return 0;
    }
  
  if (bind_info->ncmessages == 0 || bind_info->child_info_array == NULL)
    {
    bind_info->ncmessages = 0;
    return 1;
    }

  free(bind_info->child_info_array);
  return 1;

}


/*
 * Function to unpack BIND message
 */
int igtl_bind_unpack_normal(void * byte_array, igtl_bind_info * info)
{
  igtl_uint16 i;
  igtl_uint16 ncmessages;
  igtl_uint16 nametable_size;
  size_t      namelen;
  char * ptr;
  char * ptr2;
  igtl_uint64 tmp64;
  
  if (byte_array == NULL || info == NULL)
    {
    return 0;
    }
  
  /* Number of child messages */
  ptr = (char *) byte_array;
  if (igtl_is_little_endian())
    {
    ncmessages = BYTE_SWAP_INT16(*((igtl_uint16*)ptr));
    }
  else
    {
    ncmessages = *((igtl_uint16*)ptr);
    }
  
  /* Allocate an array of bind_info, if neccessary */
  if (ncmessages != info->ncmessages)
    {
    if (igtl_bind_alloc_info(info, ncmessages) == 0)
      {
      return 0;
      }
    }
  
  /* Pointer to the first element in the BIND header section */
  ptr += sizeof(igtl_uint16);
  
  /* Extract types and body sizes from the BIND header section */
  for (i = 0; i < ncmessages; i ++)
    {
    /* Type of child message */
    strncpy(info->child_info_array[i].type, (char*)ptr, IGTL_HEADER_TYPE_SIZE);
    info->child_info_array[i].type[IGTL_HEADER_TYPE_SIZE] = '\0';
    
    /* Body size of child message */
    ptr += IGTL_HEADER_TYPE_SIZE;
    if (igtl_is_little_endian())
      {
      memcpy(&tmp64, ptr, sizeof(igtl_uint64));
      info->child_info_array[i].size = BYTE_SWAP_INT64(tmp64);
      }
    else
      {
      memcpy(&(info->child_info_array[i].size), ptr, sizeof(igtl_uint64));
      }

    ptr += sizeof(igtl_uint64);
    }

  /* Name table size */
  if (igtl_is_little_endian())
    {
    nametable_size = BYTE_SWAP_INT16(*((igtl_uint16 *) ptr));
    }
  else
    {
    nametable_size = *((igtl_uint16 *) ptr);
    }

  /*
   * Check if the name table field is aligned to WORD (The length of
   * the field must be even. 
   */
  if (nametable_size % 2 != 0)
    {
    return 0;
    }
  
  ptr += sizeof(igtl_uint16);
  ptr2 = ptr;

  /* Extract device names from the name table section */
  if (nametable_size > 0)
    {
    for (i = 0; i < ncmessages; i ++)
      {
      strncpy(info->child_info_array[i].name, ptr, IGTL_HEADER_NAME_SIZE);
      info->child_info_array[i].name[IGTL_HEADER_NAME_SIZE] = '\0';
      namelen = strlen(info->child_info_array[i].name);
      ptr += namelen + 1;
      }
    }

  ptr = ptr2 + nametable_size;

  /* Set pointers to the child message bodies */
  for (i = 0; i < ncmessages; i ++)
    {
    info->child_info_array[i].ptr = ptr;
    /* Note: a padding byte is added, if the size of the child message body
       is odd. */
    ptr += info->child_info_array[i].size + (info->child_info_array[i].size % 2);
    }
    
  /** TODO: check the total size of the message? **/

  return 1;
}


/*
 * Function to unpack GET_BIND and STT_BIND messages
 */
int igtl_bind_unpack_request(void * byte_array, igtl_bind_info * info, igtl_uint64 size)
{
  igtl_uint16 i;
  igtl_uint16 ncmessages;
  igtl_uint16 nametable_size;
  size_t      namelen;
  char * ptr;
  char * ptr2;
  
  if (size == 0)
    {
    info->request_all = 1;
    return 1;
    }

  info->request_all = 0;

  if (byte_array == NULL || info == NULL)
    {
    return 0;
    }
  
  /* Number of child messages */
  ptr = (char *) byte_array;
  if (igtl_is_little_endian())
    {
    ncmessages = BYTE_SWAP_INT16(*((igtl_uint16*)ptr));
    }
  else
    {
    ncmessages = *((igtl_uint16*)ptr);
    }
  
  /* Allocate an array of bind_info, if neccessary */
  if (ncmessages != info->ncmessages)
    {
    if (igtl_bind_alloc_info(info, ncmessages) == 0)
      {
      return 0;
      }
    }
  
  /* Pointer to the first element in the BIND header section */
  ptr += sizeof(igtl_uint16);
  
  /* Extract types and body sizes from the BIND header section */
  for (i = 0; i < ncmessages; i ++)
    {
    /* Type of child message */
    strncpy(info->child_info_array[i].type, (char*)ptr, IGTL_HEADER_TYPE_SIZE);
    info->child_info_array[i].type[IGTL_HEADER_TYPE_SIZE] = '\0';
    ptr += IGTL_HEADER_TYPE_SIZE;
    }

  /* Name table size */
  if (igtl_is_little_endian())
    {
    nametable_size = BYTE_SWAP_INT16(*((igtl_uint16 *) ptr));
    }
  else
    {
    nametable_size = *((igtl_uint16 *) ptr);
    }

  /*
   * Check if the name table field is aligned to WORD (The length of
   * the field must be even. 
   */
  if (nametable_size % 2 != 0)
    {
    return 0;
    }
  
  ptr += sizeof(igtl_uint16);
  ptr2 = ptr;

  /* Extract device names from the name table section */
  if (nametable_size > 0)
    {
    for (i = 0; i < ncmessages; i ++)
      {
      strncpy(info->child_info_array[i].name, ptr, IGTL_HEADER_NAME_SIZE);
      info->child_info_array[i].name[IGTL_HEADER_NAME_SIZE] = '\0';
      namelen = strlen(info->child_info_array[i].name);
      ptr += namelen + 1;
      }
    }

  /** TODO: check the total size of the message? **/

  return 1;
}


int igtl_export igtl_bind_unpack(int type, void * byte_array, igtl_bind_info * info, igtl_uint64 size)
{
  char * ptr;

  switch (type)
    {

    case IGTL_TYPE_PREFIX_NONE:
      return igtl_bind_unpack_normal(byte_array, info);
      break;

    case IGTL_TYPE_PREFIX_GET:
      return igtl_bind_unpack_request(byte_array, info, size);
      break;

    case IGTL_TYPE_PREFIX_STT:
      /*
       * STT_BIND message has the same format as GET_BIND, except that it
       * has the time resolution field.
       */
      /* Obtain time resolution */
      ptr = byte_array;
      if (igtl_is_little_endian())
        {
        memcpy(&(info->resol), ptr, sizeof(igtl_uint64));
        info->resol = BYTE_SWAP_INT64(info->resol);
        }
      else
        {
        memcpy(&(info->resol), ptr, sizeof(igtl_uint64));
        }
      /* Unpack rest of the message */
      ptr += sizeof(igtl_uint64);
      return igtl_bind_unpack_request(ptr, info, size-sizeof(igtl_uint64));
      break;

    case IGTL_TYPE_PREFIX_STP:
      return 1;
      break;

    case IGTL_TYPE_PREFIX_RTS:
      info->status = * (igtl_uint8 *) byte_array;
      return 1;
      break;

    default:
      return 0;
      break;
    }
}


/*
 * Function to unpack BIND message
 */
int igtl_bind_pack_normal(igtl_bind_info * info, void * byte_array)
{
  char * ptr;
  igtl_uint32 i;
  igtl_uint32 nc;
  igtl_uint16 * nts;
  size_t wb;
  size_t len;
  igtl_uint16 tmp16;
  igtl_uint64 tmp64;

  ptr = (char *) byte_array;
  nc = info->ncmessages;

  /* Validate info */
  if (info->ncmessages == 0 || info->child_info_array == NULL)
    {
    return 0;
    }
  
  /* Number of child messages */
  if (igtl_is_little_endian())
    {
    /* *((igtl_uint16*) ptr) = BYTE_SWAP_INT16(info->ncmessages); */
    tmp16 = BYTE_SWAP_INT16(info->ncmessages);
    memcpy(ptr, &tmp16, sizeof(igtl_uint16));
    }
  else
    {
    /* *((igtl_uint16*) ptr) = info->ncmessages; */
    memcpy(ptr, &(info->ncmessages), sizeof(igtl_uint16));
    }
  ptr += sizeof(igtl_uint16);


  /* BIND header section */
  for (i = 0; i < nc; i ++)
    {
    /* Fill with 0 */
    memset(ptr, 0, IGTL_HEADER_TYPE_SIZE);
    strncpy((char*)ptr, info->child_info_array[i].type, IGTL_HEADER_TYPE_SIZE);
    ptr += IGTL_HEADER_TYPE_SIZE;

    /* 2011-04-29
     * Since ptr is not alined to double-word order at this point, 
     * "* ((igtl_uint64 *) ptr) = <value>" may fail in some archtecture.
     * In the following section, memcpy() is called instaed of using * ((igtl_uint64 *) ptr)
     * expression.
     */

    if (igtl_is_little_endian())
      {
      tmp64 = BYTE_SWAP_INT64(info->child_info_array[i].size);
      memcpy(ptr, &tmp64, sizeof(igtl_uint64));
      }
    else
      {
      memcpy(ptr, &(info->child_info_array[i].size), sizeof(igtl_uint64));
      }
    ptr += sizeof(igtl_uint64);
    }


  /* Name table section */
  nts = (igtl_uint16 *) ptr; /* save address for name table size field */
  ptr += sizeof(igtl_uint16);

  wb = 0;
  for (i = 0; i < nc; i ++)
    {
    len = strlen(info->child_info_array[i].name);
    if (len > IGTL_HEADER_NAME_SIZE)
      {
      return 0;
      }
    /* copy string + NULL-terminator */
    strncpy(ptr, info->child_info_array[i].name, len+1);
    ptr += (len + 1);
    wb += len + 1;
    }

  /* Add padding if the size of the name table section is not even */
  if (wb % 2 > 0)
    {
    *((igtl_uint8*)ptr) = 0;
    ptr ++;
    wb ++;
    }

  /* Substitute name table size */
  *nts = (igtl_uint16) wb;
  if (igtl_is_little_endian())
    {
    *nts = BYTE_SWAP_INT16(*nts);
    }

  return 1;

}


/*
 * Function to unpack STT_BIND and GET_BIND messages
 */
int igtl_bind_pack_request(igtl_bind_info * info, void * byte_array)
{
  char * ptr;
  igtl_uint32 i;
  igtl_uint32 nc;
  igtl_uint16 * nts;
  size_t wb;
  size_t len;
  igtl_uint16 tmp16;

  ptr = (char *) byte_array;
  nc = info->ncmessages;

  /* If requesting all available, no body is generated. */
  if (info->request_all == 1)
    {
    return 1;
    }

  /* Validate info */
  if (info->ncmessages == 0 || info->child_info_array == NULL)
    {
    return 0;
    }

  /* Number of child messages */
  if (igtl_is_little_endian())
    {
    /* *((igtl_uint16*) ptr) = BYTE_SWAP_INT16(info->ncmessages); */
    tmp16 = BYTE_SWAP_INT16(info->ncmessages);
    memcpy(ptr, &tmp16, sizeof(igtl_uint16));
    }
  else
    {
    /* *((igtl_uint16*) ptr) = info->ncmessages; */
    memcpy(ptr, &(info->ncmessages), sizeof(igtl_uint16));
    }
  ptr += sizeof(igtl_uint16);


  /* BIND header section */
  for (i = 0; i < nc; i ++)
    {
    /*memset(ptr, 0, IGTL_HEADER_TYPE_SIZE); */ /* Fill with 0 */
    strncpy((char*)ptr, info->child_info_array[i].type, IGTL_HEADER_TYPE_SIZE);
    ptr += IGTL_HEADER_TYPE_SIZE;
    }

  /* Name table section */
  nts = (igtl_uint16 *) ptr; /* save address for name table size field */
  ptr += sizeof(igtl_uint16);

  wb = 0;
  for (i = 0; i < nc; i ++)
    {
    len = strlen(info->child_info_array[i].name);
    if (len > IGTL_HEADER_NAME_SIZE)
      {
      return 0;
      }
    /* copy string + NULL-terminator */
    strncpy(ptr, info->child_info_array[i].name, len+1);
    ptr += (len + 1);
    wb += len + 1;
    }

  /* Substitute name table size */
  *nts = (igtl_uint16) wb;
  if (igtl_is_little_endian())
    {
    *nts = BYTE_SWAP_INT16(*nts);
    }

  return 1;
  
}


int igtl_export igtl_bind_pack(igtl_bind_info * info, void * byte_array, int type)
{
  char * ptr;
  igtl_uint64 tmp64;

  switch (type)
    {
    case IGTL_TYPE_PREFIX_NONE:
      return igtl_bind_pack_normal(info, byte_array);
      break;

    case IGTL_TYPE_PREFIX_GET:
      return igtl_bind_pack_request(info, byte_array);
      break;

    case IGTL_TYPE_PREFIX_STT:
      /*
       * STT_BIND message has the same format as GET_BIND, except that it
       * has the time resolution field.
       */
      ptr = (char *) byte_array;

      /* Get time resolution */ 
      if (igtl_is_little_endian())
        {
        memcpy(&tmp64, ptr, sizeof(igtl_uint64));
        info->resol = BYTE_SWAP_INT64(tmp64);
        }
      else
        {
        memcpy(&(info->resol), ptr, sizeof(igtl_uint64));
        }
      ptr += sizeof(igtl_uint64);

      /* Generate rest of the message */
      return igtl_bind_pack_request(info, ptr);
      break;
      
    case IGTL_TYPE_PREFIX_STP:
      return 1;
      break;

    case IGTL_TYPE_PREFIX_RTS:
      * (igtl_uint8 * ) byte_array = info->status;
      return 1;
      break;

    default:
      return 0;
      break;
    }
}


/*
 * Function to calculate size of BIND message header
 */
igtl_uint64 igtl_bind_get_size_normal(igtl_bind_info * info)
{
  igtl_uint64 size;
  igtl_uint32 ntable_size;
  igtl_uint16 i;
  igtl_uint16 nc;
  
  nc = info->ncmessages;

  /* Size of NCMESSAGES section */
  size = sizeof(igtl_uint16);

  /* Add size of BIND header section */
  size += (IGTL_HEADER_TYPE_SIZE+sizeof(igtl_uint64)) * nc;

  /* Add size of table size field*/
  size += sizeof(igtl_uint16);

  /* Calculate size of name table */
  ntable_size = 0;
  for (i = 0; i < nc; i ++)
    {
    /* string length + NULL separator */
    ntable_size += strlen(info->child_info_array[i].name) + 1;
    }
  if (ntable_size %2 > 0)
    {
    ntable_size += 1;
    }
  size += ntable_size;

  return size;
}


/*
 * Function to calculate size of GET_BIND/STT_BIND messages
 */
igtl_uint64 igtl_bind_get_size_request(igtl_bind_info * info)
{
  igtl_uint64 size;
  igtl_uint32 ntable_size;
  igtl_uint16 i;
  igtl_uint16 nc;
  
  nc = info->ncmessages;

  /* Size of NCMESSAGES section */
  size = sizeof(igtl_uint16);

  /* Add size of BIND header section */
  size += (IGTL_HEADER_TYPE_SIZE+sizeof(igtl_uint64)) * nc;

  /* Add size of table size field*/
  size += sizeof(igtl_uint16);

  /* Calculate size of name table */
  ntable_size = 0;
  for (i = 0; i < nc; i ++)
    {
    /* string length + NULL separator */
    ntable_size += strlen(info->child_info_array[i].name) + 1;
    }

  return size;

}


igtl_uint64 igtl_export igtl_bind_get_size(igtl_bind_info * info, int type)
{
  switch (type)
    {
    case IGTL_TYPE_PREFIX_NONE:
      return igtl_bind_get_size_normal(info);
      break;

    case IGTL_TYPE_PREFIX_GET:
      return igtl_bind_get_size_request(info);
      break;

    case IGTL_TYPE_PREFIX_STT:
      /*
       * STT_BIND message has the same format as GET_BIND, except that it
       * has the time resolution field.
       */
      return igtl_bind_get_size_request(info) + sizeof(igtl_uint64);
      break;

    case IGTL_TYPE_PREFIX_STP:
      return 0;
      break;

    case IGTL_TYPE_PREFIX_RTS:
      return sizeof(igtl_uint8);
      break;

    default:
      return 0;
      break;
    }
}


igtl_uint64 igtl_export igtl_bind_get_crc(igtl_bind_info * info, int type, void* bind_message)
{
  igtl_uint64   crc;
  igtl_uint64   bind_length;
  igtl_uint16   i;
  igtl_uint16   nc;

  bind_length = (igtl_uint64)igtl_bind_get_size(info, type);
  crc = crc64(0, 0, 0);
  crc = crc64((unsigned char*) bind_message, (int)bind_length, crc);

  if (type == IGTL_TYPE_PREFIX_NONE)
    {
    nc = info->ncmessages;
    for (i = 0; i < nc; i ++)
      {
      crc = crc64((unsigned char*) info->child_info_array[i].ptr, (int)info->child_info_array[i].size, crc);
      }
    }

  return crc;
}


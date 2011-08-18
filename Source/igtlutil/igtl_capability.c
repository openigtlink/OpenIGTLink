/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    $HeadURL: $
  Language:  C
  Date:      $Date: 2010-07-21 14:15:29 -0400 (Wed, 21 Jul 2010) $
  Version:   $Revision: 6836 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <stdlib.h>
#include <string.h>

#include "igtl_capability.h"
#include "igtl_util.h"

void igtl_export igtl_capability_init_info(igtl_capability_info * info)
{
  if (info)
    {
    info->nsize = 0;
    info->typenames = NULL;
    }
}


int igtl_export igtl_capability_alloc_info(igtl_capability_info * info, int ntypes)
{
  int i, j;

  if (info == NULL)
    {
    return 0;
    }

  info->typenames = malloc(sizeof(unsigned char*) * info->ntypes);
  if (info->typenames == NULL)
    {
    /* failed to allocate memory */
    return 0;
    }

  for (i = 0; i < info->ntypes; i ++)
    {
    info->typenames[i] = malloc(sizeof(unsigned char) * IGTL_HEADER_TYPE_SIZE);
    if (info->typenames[i] == NULL)
      {
      /* failed to allocate memory */
      /* -- free memories already allocated */ 
      for (j = 0; j < i; j ++)
        {
        free(info->typenames[j]);
        }
      free(info->typenames);
      return 0;
      }
    }

  return 1;
}


int igtl_export igtl_capability_free_info(igtl_capability_info * info)
{
}


igtl_uint32 igtl_export igtl_capability_get_capability_length(igtl_capability_info * info)
{
}


int igtl_export igtl_capability_unpack(int type, void * byte_array, igtl_capability_info * info, igtl_uint64 pack_size)
{
}


int igtl_export igtl_capability_pack(igtl_capability_info * info, void * byte_array, int type)
{
}


igtl_uint64 igtl_export igtl_capability_get_crc(igtl_capability_info, int type, void* capability)
{
}





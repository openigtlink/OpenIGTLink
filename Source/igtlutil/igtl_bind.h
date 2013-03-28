/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_BIND_H
#define __IGTL_BIND_H

#include "igtl_win32header.h"
#include "igtl_header.h"
#include "igtl_util.h"
#include "igtl_types.h"
#include "igtl_win32header.h"

#ifdef __cplusplus
extern "C" {
#endif


/** The igtl_bind_info structure holds information about a child message in an OpenIGTLink
 *  BIND message. The structure is used for functions defined in igtl_bind.h */
typedef struct {
  char             type[IGTL_HEADER_TYPE_SIZE+1]; /* Data type (OpenIGTLink Device Type string) */
  char             name[IGTL_HEADER_NAME_SIZE+1]; /* Device name */
  igtl_uint64      size;                          /* Data size */
  void*            ptr;                           /* Pointer to the child message */
} igtl_bind_child_info;

typedef struct {
  igtl_uint16             ncmessages;             /* Number of child message */
  igtl_bind_child_info *  child_info_array;       /* Array of igtl_bind_child_info */
  igtl_uint64             resol;                  /* Time resolution (used for STT_BIND) */
  igtl_uint8              request_all;            /* Flag to request all available data
                                                     (used for GET_BIND and STT_BIND) */
  igtl_uint8              status;                 /* Status for RTS message */
} igtl_bind_info;


/** Initializes igtl_bind_info */
void igtl_export igtl_bind_init_info(igtl_bind_info * bind_info);

/** Allocates an array of igtl_bind_child_info in bind_info with length of 'ncmessages.'
 *  Returns 1 if the array is successfully allocated/freed */
int igtl_export igtl_bind_alloc_info(igtl_bind_info * bind_info, igtl_uint16 ncmessages);

/** Frees an array of igtl_bind_child_info in bind_info with length of 'ncmessages.'
 *  Returns 1 if the array is successfully allocated/freed */
int igtl_export igtl_bind_free_info(igtl_bind_info * bind_info);

/** Unpacks BIND message. Extracts information about child messages in a byte array of
 *  BIND messages and store it in a igtl_bind_info structure.
 *  'type' argument specifies a message type prefix
 *  (none, GET_, STT_, STP_ or RTS_) by IGTL_TYPE_PREFIX_* macro.
 *  Returns 1 if success, otherwise 0. */
int igtl_export igtl_bind_unpack(int type, void * byte_array, igtl_bind_info * info, igtl_uint64 size);

/** Packs BIND message. Converts an igtl_bind_info structure to a byte array. 
 *  'byte_array' should be allocated prior to calling igtl_bind_pack() with memory size
 *  calculated by igtl_bind_get_size(). 'type' argument specifies a message type prefix
 *  (none, GET_, STT_, STP_ or RTS_) by IGTL_TYPE_PREFIX_* macro.
 *  Returns 1 if success, otherwise 0. */
int igtl_export igtl_bind_pack(igtl_bind_info * info, void * byte_array, int type);

/** igtl_bind_get_size() calculates the size of bind header, consisting of
 *  BIND hearder section (including number of child messages) and
 *  name table section based on a igtl_bind_header.
 *  The size returned from this function does not include size of child message data.
 *  'type' argument specifies a message type prefix
 *  (none, GET_, STT_, STP_ or RTS_) by IGTL_TYPE_PREFIX_* macro. */
igtl_uint64 igtl_export igtl_bind_get_size(igtl_bind_info * info, int type);

/** Calculates CRC of BIND message. Note that 'info' is used only for
 *  getting size of the message. */
igtl_uint64 igtl_export igtl_bind_get_crc(igtl_bind_info * info, int type, void* bind_message);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_BIND_H */



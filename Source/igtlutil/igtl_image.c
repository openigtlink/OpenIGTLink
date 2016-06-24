/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <string.h>
#include <math.h>

#include "igtl_image.h"
#include "igtl_util.h"

igtl_uint64 igtl_export igtl_image_get_data_size(igtl_image_header * header)
{
  igtl_uint64 si;
  igtl_uint64 sj;
  igtl_uint64 sk;
  igtl_uint64 sp;
  igtl_uint64 data_size;

  si = header->subvol_size[0];
  sj = header->subvol_size[1];
  sk = header->subvol_size[2];

  switch (header->scalar_type) {
  case IGTL_IMAGE_STYPE_TYPE_INT8:
  case IGTL_IMAGE_STYPE_TYPE_UINT8:
    sp = 1;
    break;
  case IGTL_IMAGE_STYPE_TYPE_INT16:
  case IGTL_IMAGE_STYPE_TYPE_UINT16:
    sp = 2;
    break;
  case IGTL_IMAGE_STYPE_TYPE_INT32:
  case IGTL_IMAGE_STYPE_TYPE_UINT32:
    sp = 4;
    break;
  default:
    sp = 0;
    break;
  }

  data_size = si*sj*sk*sp;
  return data_size;
}


void igtl_export igtl_image_set_matrix(float spacing[3], float origin[3],
                            float norm_i[3], float norm_j[3], float norm_k[3],
                            igtl_image_header * header)
{
  header->matrix[0]  = (igtl_float32) (norm_i[0] * spacing[0]);
  header->matrix[1]  = (igtl_float32) (norm_i[1] * spacing[0]);
  header->matrix[2]  = (igtl_float32) (norm_i[2] * spacing[0]);
  header->matrix[3]  = (igtl_float32) (norm_j[0] * spacing[1]);
  header->matrix[4]  = (igtl_float32) (norm_j[1] * spacing[1]);
  header->matrix[5]  = (igtl_float32) (norm_j[2] * spacing[1]);
  header->matrix[6]  = (igtl_float32) (norm_k[0] * spacing[2]);
  header->matrix[7]  = (igtl_float32) (norm_k[1] * spacing[2]);
  header->matrix[8]  = (igtl_float32) (norm_k[2] * spacing[2]);
  header->matrix[9]  = (igtl_float32) (origin[0]);
  header->matrix[10] = (igtl_float32) (origin[1]);
  header->matrix[11] = (igtl_float32) (origin[2]);
}

void igtl_export igtl_image_set_matrix_4x4(float _matrix [4][4], igtl_image_header * header)
{
  header->matrix[0]  = _matrix[0][0];
  header->matrix[1]  = _matrix[0][1];
  header->matrix[2]  = _matrix[0][2];
  header->matrix[3]  = _matrix[1][0];
  header->matrix[4]  = _matrix[1][1];
  header->matrix[5]  = _matrix[1][2];
  header->matrix[6]  = _matrix[2][0];
  header->matrix[7]  = _matrix[2][1];
  header->matrix[8]  = _matrix[2][2];
  header->matrix[9]  = _matrix[0][3];
  header->matrix[10] = _matrix[1][3];
  header->matrix[11] = _matrix[2][3];
}

void igtl_export igtl_image_get_matrix(float spacing[3], float origin[3],
                            float norm_i[3], float norm_j[3], float norm_k[3],
                            igtl_image_header * header)
{
  float tx;
  float ty;
  float tz;
  float sx;
  float sy;
  float sz;
  float nx;
  float ny;
  float nz;
  float px;
  float py;
  float pz;

  tx = (float) header->matrix[0];
  ty = (float) header->matrix[1];
  tz = (float) header->matrix[2];
  sx = (float) header->matrix[3];
  sy = (float) header->matrix[4];
  sz = (float) header->matrix[5];
  nx = (float) header->matrix[6];
  ny = (float) header->matrix[7];
  nz = (float) header->matrix[8];
  px = (float) header->matrix[9];
  py = (float) header->matrix[10];
  pz = (float) header->matrix[11];

  spacing[0] = sqrtf(tx*tx + ty*ty + tz*tz);
  spacing[1] = sqrtf(sx*sx + sy*sy + sz*sz);
  spacing[2] = sqrtf(nx*nx + ny*ny + nz*nz);

  if (spacing[0] != 0)
  {
    norm_i[0] = header->matrix[0]  / spacing[0];
    norm_i[1] = header->matrix[1]  / spacing[0];
    norm_i[2] = header->matrix[2]  / spacing[0];
  }
  if (spacing[1] != 0)
  {
    norm_j[0] = header->matrix[3] / spacing[1];
    norm_j[1] = header->matrix[4] / spacing[1];
    norm_j[2] = header->matrix[5] / spacing[1];
  }
  if (spacing[2] != 0)
  {
    norm_k[0] = header->matrix[6] / spacing[2];
    norm_k[1] = header->matrix[7] / spacing[2];
    norm_k[2] = header->matrix[8] / spacing[2];
  }

  origin[0] = header->matrix[9];
  origin[1] = header->matrix[10];
  origin[2] = header->matrix[11];

}


void igtl_export igtl_image_get_matrix_4x4(float _matrix[4][4], igtl_image_header * header)
{
  _matrix[0][0] = header->matrix[0];
  _matrix[0][1] = header->matrix[1];
  _matrix[0][2] = header->matrix[2];
  _matrix[1][0] = header->matrix[3];
  _matrix[1][1] = header->matrix[4];
  _matrix[1][2] = header->matrix[5];
  _matrix[2][0] = header->matrix[6];
  _matrix[2][1] = header->matrix[7];
  _matrix[2][2] = header->matrix[8];

  _matrix[0][3] = header->matrix[9];
  _matrix[1][3] = header->matrix[10];
  _matrix[2][3] = header->matrix[11];

  _matrix[3][0] = 0;
  _matrix[3][1] = 0;
  _matrix[3][2] = 0;
  _matrix[3][3] = 1;
}


void igtl_export igtl_image_convert_byte_order(igtl_image_header * header)
{
  int i;
  igtl_uint32 tmp[12];

  if (igtl_is_little_endian()) 
    {

    header->header_version = BYTE_SWAP_INT16(header->header_version);

    for (i = 0; i < 3; i ++) 
      {
      header->size[i] = BYTE_SWAP_INT16(header->size[i]);
      header->subvol_size[i] = BYTE_SWAP_INT16(header->subvol_size[i]);
      header->subvol_offset[i] = BYTE_SWAP_INT16(header->subvol_offset[i]);
      }

    memcpy(tmp, header->matrix, sizeof(igtl_uint32)*12);

    /* 
     * TODO: The following loop may cause segmentation fault, when it is compiled
     * with '-ftree-vectorize' optimization option on 64-bit Linux.
     * ('-ftree-vectorize' becomes active, when '-O3' optimization is specified.)
     * -- code has been updated on June 24 -- needs test
     */
    for (i = 0; i < 12; i ++) 
      {
      tmp[i] =  BYTE_SWAP_INT32(tmp[i]);
      }
    memcpy(header->matrix, tmp, sizeof(igtl_uint32)*12);

  }
}


igtl_uint64 igtl_export igtl_image_get_crc(igtl_image_header * header, void* image)
{
  igtl_uint64   crc;
  igtl_uint64   img_size;

  /* calculate image size (we do not call igtl_image_get_data_size()
   * because header has already been serialized.
   */
  igtl_uint64 si;
  igtl_uint64 sj;
  igtl_uint64 sk;
  igtl_uint64 sp;

  if (igtl_is_little_endian())
    {
    si = BYTE_SWAP_INT16(header->subvol_size[0]);
    sj = BYTE_SWAP_INT16(header->subvol_size[1]);
    sk = BYTE_SWAP_INT16(header->subvol_size[2]);
    }
  else
    {
    si = header->subvol_size[0];
    sj = header->subvol_size[1];
    sk = header->subvol_size[2];
    }
  switch (header->scalar_type) {
  case IGTL_IMAGE_STYPE_TYPE_INT8:
  case IGTL_IMAGE_STYPE_TYPE_UINT8:
    sp = 1;
    break;
  case IGTL_IMAGE_STYPE_TYPE_INT16:
  case IGTL_IMAGE_STYPE_TYPE_UINT16:
    sp = 2;
    break;
  case IGTL_IMAGE_STYPE_TYPE_INT32:
  case IGTL_IMAGE_STYPE_TYPE_UINT32:
    sp = 4;
    break;
  default:
    sp = 0;
    break;
  }
  
  img_size = si*sj*sk*sp;
  crc = crc64(0, 0, 0);
  crc = crc64((unsigned char*) header, IGTL_IMAGE_HEADER_SIZE, crc);
  crc = crc64((unsigned char*) image, (int)img_size, crc);

  return crc;
}

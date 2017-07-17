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

#include "igtl_videometa.h"
#include "igtl_util.h"

void igtl_export igtl_videometa_set_matrix(float spacing[3], float origin[3],
                                      float norm_i[3], float norm_j[3], float norm_k[3],
                                      igtl_videometa_element * element)
{
  element->matrix[0]  = (igtl_float32) (norm_i[0] * spacing[0]);
  element->matrix[1]  = (igtl_float32) (norm_i[1] * spacing[0]);
  element->matrix[2]  = (igtl_float32) (norm_i[2] * spacing[0]);
  element->matrix[3]  = (igtl_float32) (norm_j[0] * spacing[1]);
  element->matrix[4]  = (igtl_float32) (norm_j[1] * spacing[1]);
  element->matrix[5]  = (igtl_float32) (norm_j[2] * spacing[1]);
  element->matrix[6]  = (igtl_float32) (norm_k[0] * spacing[2]);
  element->matrix[7]  = (igtl_float32) (norm_k[1] * spacing[2]);
  element->matrix[8]  = (igtl_float32) (norm_k[2] * spacing[2]);
  element->matrix[9]  = (igtl_float32) (origin[0]);
  element->matrix[10] = (igtl_float32) (origin[1]);
  element->matrix[11] = (igtl_float32) (origin[2]);
}

void igtl_export igtl_videometa_set_matrix_4x4(float _matrix [4][4], igtl_videometa_element * element)
{
  element->matrix[0]  = _matrix[0][0];
  element->matrix[1]  = _matrix[0][1];
  element->matrix[2]  = _matrix[0][2];
  element->matrix[3]  = _matrix[1][0];
  element->matrix[4]  = _matrix[1][1];
  element->matrix[5]  = _matrix[1][2];
  element->matrix[6]  = _matrix[2][0];
  element->matrix[7]  = _matrix[2][1];
  element->matrix[8]  = _matrix[2][2];
  element->matrix[9]  = _matrix[0][3];
  element->matrix[10] = _matrix[1][3];
  element->matrix[11] = _matrix[2][3];
}

void igtl_export igtl_videometa_get_matrix(float spacing[3], float origin[3],
                                       float norm_i[3], float norm_j[3], float norm_k[3],
                                       igtl_videometa_element * element)
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
  
  tx = (float) element->matrix[0];
  ty = (float) element->matrix[1];
  tz = (float) element->matrix[2];
  sx = (float) element->matrix[3];
  sy = (float) element->matrix[4];
  sz = (float) element->matrix[5];
  nx = (float) element->matrix[6];
  ny = (float) element->matrix[7];
  nz = (float) element->matrix[8];
  px = (float) element->matrix[9];
  py = (float) element->matrix[10];
  pz = (float) element->matrix[11];
  
  spacing[0] = sqrtf(tx*tx + ty*ty + tz*tz);
  spacing[1] = sqrtf(sx*sx + sy*sy + sz*sz);
  spacing[2] = sqrtf(nx*nx + ny*ny + nz*nz);
  
  if (spacing[0] != 0)
    {
    norm_i[0] = element->matrix[0]  / spacing[0];
    norm_i[1] = element->matrix[1]  / spacing[0];
    norm_i[2] = element->matrix[2]  / spacing[0];
    }
  if (spacing[1] != 0)
    {
    norm_j[0] = element->matrix[3] / spacing[1];
    norm_j[1] = element->matrix[4] / spacing[1];
    norm_j[2] = element->matrix[5] / spacing[1];
    }
  if (spacing[2] != 0)
    {
    norm_k[0] = element->matrix[6] / spacing[2];
    norm_k[1] = element->matrix[7] / spacing[2];
    norm_k[2] = element->matrix[8] / spacing[2];
    }
  
  origin[0] = element->matrix[9];
  origin[1] = element->matrix[10];
  origin[2] = element->matrix[11];
  
}


void igtl_export igtl_videometa_get_matrix_4x4(float _matrix[4][4], igtl_videometa_element * element)
{
  _matrix[0][0] = element->matrix[0];
  _matrix[0][1] = element->matrix[1];
  _matrix[0][2] = element->matrix[2];
  _matrix[1][0] = element->matrix[3];
  _matrix[1][1] = element->matrix[4];
  _matrix[1][2] = element->matrix[5];
  _matrix[2][0] = element->matrix[6];
  _matrix[2][1] = element->matrix[7];
  _matrix[2][2] = element->matrix[8];
  
  _matrix[0][3] = element->matrix[9];
  _matrix[1][3] = element->matrix[10];
  _matrix[2][3] = element->matrix[11];
  
  _matrix[3][0] = 0;
  _matrix[3][1] = 0;
  _matrix[3][2] = 0;
  _matrix[3][3] = 1;
}


void igtl_export igtl_videometa_convert_byte_order(igtl_videometa_element * metalist, int nitem)
{
  igtl_videometa_element* elem;
  int item;
  int i;
  igtl_uint32 tmp[12];
  igtl_uint64 tmp_focal;
  for (item = 0; item < nitem; item ++)
    {
    elem = &(metalist[item]);
    
    if (igtl_is_little_endian())
      {
      elem->zoom_level = BYTE_SWAP_INT16(elem->zoom_level);
      memcpy(&tmp_focal, &elem->focal_length, sizeof(igtl_float64));
      tmp_focal = BYTE_SWAP_INT64(tmp_focal);
      memcpy(&elem->focal_length, &tmp_focal, sizeof(igtl_float64));
      
      for (i = 0; i < 3; i ++)
        {
        elem->size[i] = BYTE_SWAP_INT16(elem->size[i]);
        }
      
      memcpy(tmp, elem->matrix, sizeof(igtl_uint32)*12);
      
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
      memcpy(elem->matrix, tmp, sizeof(igtl_uint32)*12);
      
      }
    }
}

igtl_uint64 igtl_export igtl_videometa_get_crc(igtl_videometa_element* metalist, int nitem)
{
  igtl_videometa_element* elem;
  int i;
  igtl_uint64  crc;
  
  crc = crc64(0, 0, 0);
  for (i = 0; i < nitem; i ++)
    {
    elem = &(metalist[i]);
    crc = crc64((unsigned char*) elem, IGTL_VIDEOMETA_ELEMENT_SIZE, crc);
    }
  
  return crc;
}

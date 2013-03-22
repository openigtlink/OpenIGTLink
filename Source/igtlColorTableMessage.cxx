/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlColorTableMessage.h"

#include "igtl_header.h"
#include "igtl_colortable.h"

namespace igtl {


ColorTableMessage::ColorTableMessage():
  MessageBase()
{

  indexType = INDEX_UINT8;
  mapType   = MAP_UINT8;
  m_ColorTableHeader = NULL;
  m_ColorTable       = NULL;
  m_DefaultBodyType  = "COLORT";

}

ColorTableMessage::~ColorTableMessage()
{
}

void ColorTableMessage::AllocateTable()
{
  // Memory area to store image scalar is allocated with
  // message and image header, by using AllocatePack() implemented
  // in the parent class.
  AllocatePack();
  m_ColorTableHeader = m_Body;
  m_ColorTable       = &m_ColorTableHeader[IGTL_COLORTABLE_HEADER_SIZE];
}

void* ColorTableMessage::GetTablePointer()
{
  return (void*)m_ColorTable;
}

int ColorTableMessage::GetColorTableSize()
{
  igtl_colortable_header header;

  header.indexType = this->indexType;
  header.mapType   = this->mapType;

  return (int) igtl_colortable_get_table_size(&header);

}

int ColorTableMessage::GetBodyPackSize()
{
  return GetColorTableSize() + IGTL_COLORTABLE_HEADER_SIZE;
}

int ColorTableMessage::PackBody()
{
  igtl_colortable_header* colortable_header = (igtl_colortable_header*)m_ColorTableHeader;

  colortable_header->indexType = this->indexType;
  colortable_header->mapType   = this->mapType;

  igtl_colortable_convert_byte_order(colortable_header, (void*)m_ColorTable);

  return 1;

}


int ColorTableMessage::UnpackBody()
{

  this->m_ColorTableHeader = this->m_Body;
  this->m_ColorTable       = &(this->m_Body[IGTL_COLORTABLE_HEADER_SIZE]);

  igtl_colortable_header* colortable_header = (igtl_colortable_header*)this->m_ColorTableHeader;
  igtl_colortable_convert_byte_order(colortable_header, (void*)this->m_ColorTable);

  this->indexType = colortable_header->indexType;
  this->mapType   = colortable_header->mapType;

  return 1;
}


} // namespace igtl




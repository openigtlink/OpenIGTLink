/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlUnit.h"
#include "igtl_unit.h"

// Disable warning C4996 (strncpy() may be unsafe) in Windows. 
#define _CRT_SECURE_NO_WARNINGS

namespace igtl {

Unit::Unit():
  Object()
{
  Init();
}


Unit::~Unit()
{
}


void Unit::Init()
{
  this->m_Prefix = SI_PREFIX_NONE;
  this->m_NUnits = 0;

  for (int i = 0; i < 6; i ++)
    {
    this->m_Unit[i] = SI_BASE_NONE;
    this->m_Exp[i]  = 0;
    }
}


int Unit::SetPrefix(int prefix)
{
  if (prefix >= SI_PREFIX_NONE && prefix <= SI_PREFIX_FEMTO)
    {
    this->m_Prefix = prefix;
    return 1;
    }
  else
    {
    return 0;
    }
}


int Unit::Append(int unit, int exp)
{
  // Check number of units already appended
  if (this->m_NUnits >= 6)
    {
    return 0;
    }

  // Check range
  if (unit < SI_BASE_NONE || unit >= SI_NUM_UNIT_TYPES)
    {
    return 0;
    }
  if (exp < -7 || exp > 7)
    {
    return 0;
    }
  
  // Append
  this->m_Unit[this->m_NUnits] = (igtlUint8)unit;
  this->m_Exp[this->m_NUnits] =  (igtlInt8)exp;

  return 1;
}


igtlUnit Unit::Pack()
{
  igtl_unit_data data;

  igtl_unit_init(&data);

  data.prefix = this->m_Prefix;
  for (int i = 0; i < 6; i ++)
    {
    data.unit[i] = this->m_Unit[i];
    data.exp[i] = this->m_Exp[i];
    }
  
  return (igtlUnit) igtl_unit_pack(&data);
}


int Unit::Unpack(igtlUnit unit)
{
  igtl_unit_data data;

  if (igtl_unit_unpack((igtl_unit)unit, &data) == 0)
    {
    return 0;
    }

  Init();
    
  this->m_Prefix = data.prefix;
  for (int i = 0; i < 6; i ++)
    {
    this->m_Unit[i] = data.unit[i];
    this->m_Exp[i]  = data.exp[i];
    }

  return 1;
  
}


} // namespace igtl






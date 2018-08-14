/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlUnit_h
#define __igtlUnit_h

#include "igtlWin32Header.h"

#include "igtlMacro.h"
#include "igtlObject.h"
#include "igtlObjectFactory.h"
#include "igtlTypes.h"

namespace igtl
{

typedef igtlUint64 igtlUnit;

/// The Unit class provides a general way to describe unites defined in International System of Unites (SI)
/// in its 8-byte (or 64-bit) field. The field is designed to specifiy a unit consisting of SI-prefix
/// (e.g. milli, micro, kilo etc...) and combination of SI-base and/or SI-derived unites.
/// The bites in the field are assigned as follows:
/// 
///  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-...
///  |PREFIX |   UNIT0   |  EXP0 |   UNIT1   |  EXP1 |   UNIT2   | EXP2
///  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-...
///  0               1               2               3               4 
/// 
///  ...-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///     EXP2 |   UNIT3   |  EXP3 |   UNIT4   |  EXP4 |   UNIT5   |  EXP5 |
///  ...-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///         4               5               6               7
class IGTLCommon_EXPORT Unit : public Object
{
public:

  /// SI Prefix
  enum {
    SI_PREFIX_NONE  = 0x0, /* None */
    SI_PREFIX_DEKA  = 0x1, /* deka (deca) (1e1) */
    SI_PREFIX_HECTO = 0x2, /* hecto (1e2) */
    SI_PREFIX_KILO  = 0x3, /* kilo (1e3) */
    SI_PREFIX_MEGA  = 0x4, /* mega (1e6) */
    SI_PREFIX_GIGA  = 0x5, /* giga (1e9) */
    SI_PREFIX_TERA  = 0x6, /* tera (1e12) */
    SI_PREFIX_PETA  = 0x7, /* peta (1e15) */
    SI_PREFIX_DECI  = 0x9, /* deci (1e-1) */
    SI_PREFIX_CENTI = 0xA, /* centi (1e-2) */
    SI_PREFIX_MILLI = 0xB, /* milli (1e-3) */
    SI_PREFIX_MICRO = 0xC, /* micro (1e-6) */
    SI_PREFIX_NANO  = 0xD, /* nano (1e-9) */
    SI_PREFIX_PICO  = 0xE, /* pico (1e-12) */
    SI_PREFIX_FEMTO = 0xF, /* femto (1e-15) */
  };

  /// SI Units
  enum {
    // SI Base Units
    SI_BASE_NONE    = 0x00,
    SI_BASE_METER   = 0x01, /* meter */
    SI_BASE_GRAM    = 0x02, /* gram */
    SI_BASE_SECOND  = 0x03, /* second */
    SI_BASE_AMPERE  = 0x04, /* ampere */
    SI_BASE_KELVIN  = 0x05, /* kelvin */
    SI_BASE_MOLE    = 0x06, /* mole */
    SI_BASE_CANDELA = 0x07, /* candela */

    // SI Derived Units
    SI_DERIVED_RADIAN    = 0x08,  /* radian     meter/meter */
    SI_DERIVED_STERADIAN = 0x09,  /* steradian  meter^2/meter^2 */
    SI_DERIVED_HERTZ     = 0x0A,  /* hertz      /second */
    SI_DERIVED_NEWTON    = 0x0B,  /* newton     meter-kilogram/second^2 */
    SI_DERIVED_PASCAL    = 0x0C,  /* pascal     kilogram/meter-second^2 */
    SI_DERIVED_JOULE     = 0x0D,  /* joule      meter^2-kilogram/second^2 */
    SI_DERIVED_WATT      = 0x0E,  /* watt       meter^2-kilogram/second^3 */
    SI_DERIVED_COULOMB   = 0x0F,  /* coulomb    second-ampere */
    SI_DERIVED_VOLT      = 0x10,  /* volt       meter^2-kilogram/second^3-ampere */
    SI_DERIVED_FARAD     = 0x11,  /* farad      second^4-ampere^2/meter^2-kilogram */
    SI_DERIVED_OHM       = 0x12,  /* ohm        meter^2-kilogram/second^3-ampere^2 */
    SI_DERIVED_SIEMENS   = 0x13,  /* siemens    second^3-ampere^2/meter^2-kilogram */
    SI_DERIVED_WEBER     = 0x14,  /* weber      meter^2-kilogram/second^2-ampere */
    SI_DERIVED_TESLA     = 0x15,  /* tesla      kilogram/second^2-ampere */
    SI_DERIVED_HENRY     = 0x16,  /* henry      meter^2-kilogram/second^2-ampere^2 */
    SI_DERIVED_LUMEN     = 0x17,  /* lumen      candela-steradian */
    SI_DERIVED_LUX       = 0x18,  /* lux        candela-steradian/meter^2 */
    SI_DERIVED_BECQUEREL = 0x19,  /* becquerel  /second */
    SI_DERIVED_GRAY      = 0x1A,  /* gray       meter^2/second^2 */
    SI_DERIVED_SIEVERT   = 0x1B,  /* sievert    meter^2/second^2 */

    SI_NUM_UNIT_TYPES    = 0x1C,
  };



public:
  igtlTypeMacro(Unit, Object);
  igtlNewMacro(Self);

  /// Initializes the class.
  void     Init();

  /// Sets the SI prefix.
  int      SetPrefix(int prefix);

  /// Appends a unit with exponential
  int      Append(int unit, int exp);

  /// Packs (or serializes) the unit into a 64-bit value.
  igtlUnit Pack();

  /// Unpacks the unit from a 64-bit value.
  int      Unpack(igtlUnit unit);
  
protected:

  /// constructor
  Unit();

  /// destructor
  virtual ~Unit();

private:
  
  /// Prefix.
  igtlUint8       m_Prefix;

  /// Either SI-Base or SI-Derived.
  igtlUint8       m_Unit[6];

  /// Must be within [-7, 7].
  igtlInt8        m_Exp[6];

  /// Number of units appended.
  igtlInt32       m_NUnits;

};

}

#endif // __igtlUnit_h


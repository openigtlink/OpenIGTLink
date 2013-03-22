/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_UNIT_H
#define __IGTL_UNIT_H

#include "igtl_types.h"
#include "igtl_win32header.h"
#include "igtl_unit.h"


/* PREFIX */
#define IGTL_UNIT_PREFIX_NONE   0x0 /* None */
#define IGTL_UNIT_PREFIX_DEKA   0x1 /* deka (deca) (1e1) */
#define IGTL_UNIT_PREFIX_HECTO  0x2 /* hecto (1e2) */
#define IGTL_UNIT_PREFIX_KILO   0x3 /* kilo (1e3) */
#define IGTL_UNIT_PREFIX_MEGA   0x4 /* mega (1e6) */
#define IGTL_UNIT_PREFIX_GIGA   0x5 /* giga (1e9) */
#define IGTL_UNIT_PREFIX_TERA   0x6 /* tera (1e12) */
#define IGTL_UNIT_PREFIX_PETA   0x7 /* peta (1e15) */
#define IGTL_UNIT_PREFIX_DECI   0x9 /* deci (1e-1) */
#define IGTL_UNIT_PREFIX_CENTI  0xA /* centi (1e-2) */
#define IGTL_UNIT_PREFIX_MILLI  0xB /* milli (1e-3) */
#define IGTL_UNIT_PREFIX_MICRO  0xC /* micro (1e-6) */
#define IGTL_UNIT_PREFIX_NANO   0xD /* nano (1e-9) */
#define IGTL_UNIT_PREFIX_PICO   0xE /* pico (1e-12) */
#define IGTL_UNIT_PREFIX_FEMTO  0xF /* femto (1e-15) */

/* SI Base Units */
#define IGTL_UNIT_SI_BASE_NONE    0x00
#define IGTL_UNIT_SI_BASE_METER   0x01 /* meter */
#define IGTL_UNIT_SI_BASE_GRAM    0x02 /* gram */
#define IGTL_UNIT_SI_BASE_SECOND  0x03 /* second */
#define IGTL_UNIT_SI_BASE_AMPERE  0x04 /* ampere */
#define IGTL_UNIT_SI_BASE_KELVIN  0x05 /* kelvin */
#define IGTL_UNIT_SI_BASE_MOLE    0x06 /* mole */
#define IGTL_UNIT_SI_BASE_CANDELA 0x07 /* candela */

/* SI Derived Units */
#define IGTL_UNIT_SI_DERIVED_RADIAN    0x08  /* radian     meter/meter */
#define IGTL_UNIT_SI_DERIVED_STERADIAN 0x09  /* steradian  meter^2/meter^2 */
#define IGTL_UNIT_SI_DERIVED_HERTZ     0x0A  /* hertz      /second */
#define IGTL_UNIT_SI_DERIVED_NEWTON    0x0B  /* newton     meter-kilogram/second^2 */
#define IGTL_UNIT_SI_DERIVED_PASCAL    0x0C  /* pascal     kilogram/meter-second^2 */
#define IGTL_UNIT_SI_DERIVED_JOULE     0x0D  /* joule      meter^2-kilogram/second^2 */
#define IGTL_UNIT_SI_DERIVED_WATT      0x0E  /* watt       meter^2-kilogram/second^3 */
#define IGTL_UNIT_SI_DERIVED_COULOMB   0x0F  /* coulomb    second-ampere */
#define IGTL_UNIT_SI_DERIVED_VOLT      0x10  /* volt       meter^2-kilogram/second^3-ampere */
#define IGTL_UNIT_SI_DERIVED_FARAD     0x11  /* farad      second^4-ampere^2/meter^2-kilogram */
#define IGTL_UNIT_SI_DERIVED_OHM       0x12  /* ohm        meter^2-kilogram/second^3-ampere^2 */
#define IGTL_UNIT_SI_DERIVED_SIEMENS   0x13  /* siemens    second^3-ampere^2/meter^2-kilogram */
#define IGTL_UNIT_SI_DERIVED_WEBER     0x14  /* weber      meter^2-kilogram/second^2-ampere */
#define IGTL_UNIT_SI_DERIVED_TESLA     0x15  /* tesla      kilogram/second^2-ampere */
#define IGTL_UNIT_SI_DERIVED_HENRY     0x16  /* henry      meter^2-kilogram/second^2-ampere^2 */
#define IGTL_UNIT_SI_DERIVED_LUMEN     0x17  /* lumen      candela-steradian */
#define IGTL_UNIT_SI_DERIVED_LUX       0x18  /* lux        candela-steradian/meter^2 */
#define IGTL_UNIT_SI_DERIVED_BECQUEREL 0x19  /* becquerel  /second */
#define IGTL_UNIT_SI_DERIVED_GRAY      0x1A  /* gray       meter^2/second^2 */
#define IGTL_UNIT_SI_DERIVED_SIEVERT   0x1B  /* sievert    meter^2/second^2 */

typedef igtl_uint64 igtl_unit;


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  igtl_uint8     prefix;            /* Prefix */
  igtl_uint8     unit[6];           /* Either SI-Base or SI-Derived */
  igtl_int8      exp[6];            /* Must be within [-7, 7] */
} igtl_unit_data;

void igtl_export igtl_unit_init(igtl_unit_data* data);
igtl_unit igtl_export igtl_unit_pack(igtl_unit_data* data);
int igtl_export igtl_unit_unpack(igtl_unit pack, igtl_unit_data* data);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_UNIT_H */





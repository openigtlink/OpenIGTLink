/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_TYPES_H
#define __IGTL_TYPES_H

#include "igtl_typeconfig.h"

enum IANA_ENCODING_TYPE
{
  IANA_TYPE_US_ASCII = 3,
  IANA_TYPE_ISO_8859_1 = 4,
  IANA_TYPE_ISO_8859_2 = 5,
  IANA_TYPE_ISO_8859_3 = 6,
  IANA_TYPE_ISO_8859_4 = 7,
  IANA_TYPE_ISO_8859_5 = 8,
  IANA_TYPE_ISO_8859_6 = 9,
  IANA_TYPE_ISO_8859_7 = 10,
  IANA_TYPE_ISO_8859_8 = 11,
  IANA_TYPE_ISO_8859_9 = 12,
  IANA_TYPE_ISO_8859_10 = 13,
  IANA_TYPE_ISO_6937_2_add = 14,
  IANA_TYPE_JIS_X0201 = 15,
  IANA_TYPE_JIS_Encoding = 16,
  IANA_TYPE_Shift_JIS = 17,
  IANA_TYPE_EUC_JP = 18,
  IANA_TYPE_Extended_UNIX_Code_Fixed_Width_for_Japanese = 19,
  IANA_TYPE_BS_4730 = 20,
  IANA_TYPE_SEN_850200_C = 21,
  IANA_TYPE_IT = 22,
  IANA_TYPE_ES = 23,
  IANA_TYPE_DIN_66003 = 24,
  IANA_TYPE_NS_4551_1 = 25,
  IANA_TYPE_NF_Z_62_010 = 26,
  IANA_TYPE_ISO_10646_UTF_1 = 27,
  IANA_TYPE_ISO_646_basic_1983 = 28,
  IANA_TYPE_INVARIANT = 29,
  IANA_TYPE_ISO_646_irv_1983 = 30,
  IANA_TYPE_NATS_SEFI = 31,
  IANA_TYPE_NATS_SEFI_ADD = 32,
  IANA_TYPE_NATS_DANO = 33,
  IANA_TYPE_NATS_DANO_ADD = 34,
  IANA_TYPE_SEN_850200_B = 35,
  IANA_TYPE_KS_C_5601_1987 = 36,
  IANA_TYPE_ISO_2022_KR = 37,
  IANA_TYPE_EUC_KR = 38,
  IANA_TYPE_ISO_2022_JP = 39,
  IANA_TYPE_ISO_2022_JP_2 = 40,
  IANA_TYPE_JIS_C6220_1969_jp = 41,
  IANA_TYPE_JIS_C6220_1969_ro = 42,
  IANA_TYPE_PT = 43,
  IANA_TYPE_greek7_old = 44,
  IANA_TYPE_latin_greek = 45,
  IANA_TYPE_NF_Z_62_010_1973 = 46,
  IANA_TYPE_Latin_greek_1 = 47,
  IANA_TYPE_ISO_5427 = 48,
  IANA_TYPE_JIS_C6226_1978 = 49,
  IANA_TYPE_BS_viewdata = 50,
  IANA_TYPE_INIS = 51,
  IANA_TYPE_INIS_8 = 52,
  IANA_TYPE_INIS_cyrillic = 53,
  IANA_TYPE_ISO_5427_1981 = 54,
  IANA_TYPE_ISO_5428_1980 = 55,
  IANA_TYPE_GB_1988_80 = 56,
  IANA_TYPE_GB_2312_80 = 57,
  IANA_TYPE_NS_4551_2 = 58,
  IANA_TYPE_videotex_suppl = 59,
  IANA_TYPE_PT2 = 60,
  IANA_TYPE_ES2 = 61,
  IANA_TYPE_MSZ_7795_3 = 62,
  IANA_TYPE_JIS_C6226_1983 = 63,
  IANA_TYPE_greek7 = 64,
  IANA_TYPE_ASMO_449 = 65,
  IANA_TYPE_iso_ir_90 = 66,
  IANA_TYPE_JIS_C6229_1984_a = 67,
  IANA_TYPE_JIS_C6229_1984_b = 68,
  IANA_TYPE_JIS_C6229_1984_b_add = 69,
  IANA_TYPE_JIS_C6229_1984_hand = 70,
  IANA_TYPE_JIS_C6229_1984_hand_add = 71,
  IANA_TYPE_JIS_C6229_1984_kana = 72,
  IANA_TYPE_ISO_2033_1983 = 73,
  IANA_TYPE_ANSI_X3_110_1983 = 74,
  IANA_TYPE_T_61_7bit = 75,
  IANA_TYPE_T_61_8bit = 76,
  IANA_TYPE_ECMA_cyrillic = 77,
  IANA_TYPE_CSA_Z243_4_1985_1 = 78,
  IANA_TYPE_CSA_Z243_4_1985_2 = 79,
  IANA_TYPE_CSA_Z243_4_1985_gr = 80,
  IANA_TYPE_ISO_8859_6_E = 81,
  IANA_TYPE_ISO_8859_6_I = 82,
  IANA_TYPE_T_101_G2 = 83,
  IANA_TYPE_ISO_8859_8_E = 84,
  IANA_TYPE_ISO_8859_8_I = 85,
  IANA_TYPE_CSN_369103 = 86,
  IANA_TYPE_JUS_I_B1_002 = 87,
  IANA_TYPE_IEC_P27_1 = 88,
  IANA_TYPE_JUS_I_B1_003_serb = 89,
  IANA_TYPE_JUS_I_B1_003_mac = 90,
  IANA_TYPE_greek_ccitt = 91,
  IANA_TYPE_NC_NC00_10_81 = 92,
  IANA_TYPE_ISO_6937_2_25 = 93,
  IANA_TYPE_GOST_19768_74 = 94,
  IANA_TYPE_ISO_8859_supp = 95,
  IANA_TYPE_ISO_10367_box = 96,
  IANA_TYPE_latin_lap = 97,
  IANA_TYPE_JIS_X0212_1990 = 98,
  IANA_TYPE_DS_2089 = 99,
  IANA_TYPE_us_dk = 100,
  IANA_TYPE_dk_us = 101,
  IANA_TYPE_KSC5636 = 102,
  IANA_TYPE_UNICODE_1_1_UTF_7 = 103,
  IANA_TYPE_ISO_2022_CN = 104,
  IANA_TYPE_ISO_2022_CN_EXT = 105,
  IANA_TYPE_UTF_8 = 106,
  IANA_TYPE_ISO_8859_13 = 109,
  IANA_TYPE_ISO_8859_14 = 110,
  IANA_TYPE_ISO_8859_15 = 111,
  IANA_TYPE_ISO_8859_16 = 112,
  IANA_TYPE_GBK = 113,
  IANA_TYPE_GB18030 = 114,
  IANA_TYPE_OSD_EBCDIC_DF04_15 = 115,
  IANA_TYPE_OSD_EBCDIC_DF03_IRV = 116,
  IANA_TYPE_OSD_EBCDIC_DF04_1 = 117,
  IANA_TYPE_ISO_11548_1 = 118,
  IANA_TYPE_KZ_1048 = 119,
  IANA_TYPE_ISO_10646_UCS_2 = 1000,
  IANA_TYPE_ISO_10646_UCS_4 = 1001,
  IANA_TYPE_ISO_10646_UCS_Basic = 1002,
  IANA_TYPE_ISO_10646_Unicode_Latin1 = 1003,
  IANA_TYPE_ISO_10646_J_1 = 1004,
  IANA_TYPE_ISO_Unicode_IBM_1261 = 1005,
  IANA_TYPE_ISO_Unicode_IBM_1268 = 1006,
  IANA_TYPE_ISO_Unicode_IBM_1276 = 1007,
  IANA_TYPE_ISO_Unicode_IBM_1264 = 1008,
  IANA_TYPE_ISO_Unicode_IBM_1265 = 1009,
  IANA_TYPE_UNICODE_1_1 = 1010,
  IANA_TYPE_SCSU = 1011,
  IANA_TYPE_UTF_7 = 1012,
  IANA_TYPE_UTF_16BE = 1013,
  IANA_TYPE_UTF_16LE = 1014,
  IANA_TYPE_UTF_16 = 1015,
  IANA_TYPE_CESU_8 = 1016,
  IANA_TYPE_UTF_32 = 1017,
  IANA_TYPE_UTF_32BE = 1018,
  IANA_TYPE_UTF_32LE = 1019,
  IANA_TYPE_BOCU_1 = 1020,
  IANA_TYPE_ISO_8859_1_Windows_3_0_Latin_1 = 2000,
  IANA_TYPE_ISO_8859_1_Windows_3_1_Latin_1 = 2001,
  IANA_TYPE_ISO_8859_2_Windows_Latin_2 = 2002,
  IANA_TYPE_ISO_8859_9_Windows_Latin_5 = 2003,
  IANA_TYPE_hp_roman8 = 2004,
  IANA_TYPE_Adobe_Standard_Encoding = 2005,
  IANA_TYPE_Ventura_US = 2006,
  IANA_TYPE_Ventura_International = 2007,
  IANA_TYPE_DEC_MCS = 2008,
  IANA_TYPE_IBM850 = 2009,
  IANA_TYPE_PC8_Danish_Norwegian = 2012,
  IANA_TYPE_IBM862 = 2013,
  IANA_TYPE_PC8_Turkish = 2014,
  IANA_TYPE_IBM_Symbols = 2015,
  IANA_TYPE_IBM_Thai = 2016,
  IANA_TYPE_HP_Legal = 2017,
  IANA_TYPE_HP_Pi_font = 2018,
  IANA_TYPE_HP_Math8 = 2019,
  IANA_TYPE_Adobe_Symbol_Encoding = 2020,
  IANA_TYPE_HP_DeskTop = 2021,
  IANA_TYPE_Ventura_Math = 2022,
  IANA_TYPE_Microsoft_Publishing = 2023,
  IANA_TYPE_Windows_31J = 2024,
  IANA_TYPE_GB2312 = 2025,
  IANA_TYPE_Big5 = 2026,
  IANA_TYPE_macintosh = 2027,
  IANA_TYPE_IBM037 = 2028,
  IANA_TYPE_IBM038 = 2029,
  IANA_TYPE_IBM273 = 2030,
  IANA_TYPE_IBM274 = 2031,
  IANA_TYPE_IBM275 = 2032,
  IANA_TYPE_IBM277 = 2033,
  IANA_TYPE_IBM278 = 2034,
  IANA_TYPE_IBM280 = 2035,
  IANA_TYPE_IBM281 = 2036,
  IANA_TYPE_IBM284 = 2037,
  IANA_TYPE_IBM285 = 2038,
  IANA_TYPE_IBM290 = 2039,
  IANA_TYPE_IBM297 = 2040,
  IANA_TYPE_IBM420 = 2041,
  IANA_TYPE_IBM423 = 2042,
  IANA_TYPE_IBM424 = 2043,
  IANA_TYPE_IBM437 = 2011,
  IANA_TYPE_IBM500 = 2044,
  IANA_TYPE_IBM851 = 2045,
  IANA_TYPE_IBM852 = 2010,
  IANA_TYPE_IBM855 = 2046,
  IANA_TYPE_IBM857 = 2047,
  IANA_TYPE_IBM860 = 2048,
  IANA_TYPE_IBM861 = 2049,
  IANA_TYPE_IBM863 = 2050,
  IANA_TYPE_IBM864 = 2051,
  IANA_TYPE_IBM865 = 2052,
  IANA_TYPE_IBM868 = 2053,
  IANA_TYPE_IBM869 = 2054,
  IANA_TYPE_IBM870 = 2055,
  IANA_TYPE_IBM871 = 2056,
  IANA_TYPE_IBM880 = 2057,
  IANA_TYPE_IBM891 = 2058,
  IANA_TYPE_IBM903 = 2059,
  IANA_TYPE_IBM904 = 2060,
  IANA_TYPE_IBM905 = 2061,
  IANA_TYPE_IBM918 = 2062,
  IANA_TYPE_IBM1026 = 2063,
  IANA_TYPE_EBCDIC_AT_DE = 2064,
  IANA_TYPE_EBCDIC_AT_DE_A = 2065,
  IANA_TYPE_EBCDIC_CA_FR = 2066,
  IANA_TYPE_EBCDIC_DK_NO = 2067,
  IANA_TYPE_EBCDIC_DK_NO_A = 2068,
  IANA_TYPE_EBCDIC_FI_SE = 2069,
  IANA_TYPE_EBCDIC_FI_SE_A = 2070,
  IANA_TYPE_EBCDIC_FR = 2071,
  IANA_TYPE_EBCDIC_IT = 2072,
  IANA_TYPE_EBCDIC_PT = 2073,
  IANA_TYPE_EBCDIC_ES = 2074,
  IANA_TYPE_EBCDIC_ES_A = 2075,
  IANA_TYPE_EBCDIC_ES_S = 2076,
  IANA_TYPE_EBCDIC_UK = 2077,
  IANA_TYPE_EBCDIC_US = 2078,
  IANA_TYPE_UNKNOWN_8BIT = 2079,
  IANA_TYPE_MNEMONIC = 2080,
  IANA_TYPE_MNEM = 2081,
  IANA_TYPE_VISCII = 2082,
  IANA_TYPE_VIQR = 2083,
  IANA_TYPE_KOI8_R = 2084,
  IANA_TYPE_HZ_GB_2312 = 2085,
  IANA_TYPE_IBM866 = 2086,
  IANA_TYPE_IBM775 = 2087,
  IANA_TYPE_KOI8_U = 2088,
  IANA_TYPE_IBM00858 = 2089,
  IANA_TYPE_IBM00924 = 2090,
  IANA_TYPE_IBM01140 = 2091,
  IANA_TYPE_IBM01141 = 2092,
  IANA_TYPE_IBM01142 = 2093,
  IANA_TYPE_IBM01143 = 2094,
  IANA_TYPE_IBM01144 = 2095,
  IANA_TYPE_IBM01145 = 2096,
  IANA_TYPE_IBM01146 = 2097,
  IANA_TYPE_IBM01147 = 2098,
  IANA_TYPE_IBM01148 = 2099,
  IANA_TYPE_IBM01149 = 2100,
  IANA_TYPE_Big5_HKSCS = 2101,
  IANA_TYPE_IBM1047 = 2102,
  IANA_TYPE_PTCP154 = 2103,
  IANA_TYPE_Amiga_1251 = 2104,
  IANA_TYPE_KOI7_switched = 2105,
  IANA_TYPE_BRF = 2106,
  IANA_TYPE_TSCII = 2107,
  IANA_TYPE_CP51932 = 2108,
  IANA_TYPE_windows_874 = 2109,
  IANA_TYPE_windows_1250 = 2250,
  IANA_TYPE_windows_1251 = 2251,
  IANA_TYPE_windows_1252 = 2252,
  IANA_TYPE_windows_1253 = 2253,
  IANA_TYPE_windows_1254 = 2254,
  IANA_TYPE_windows_1255 = 2255,
  IANA_TYPE_windows_1256 = 2256,
  IANA_TYPE_windows_1257 = 2257,
  IANA_TYPE_windows_1258 = 2258,
  IANA_TYPE_TIS_620 = 2259,
  IANA_TYPE_CP50220 = 2260
};
/* 8-bit integer type */
#if IGTL_SIZEOF_CHAR == 1
typedef unsigned char igtl_uint8;
typedef signed char   igtl_int8;
#else
# error "No native data type can represent an 8-bit integer."
#endif

/* 16-bit integer type */
#if IGTL_SIZEOF_SHORT == 2
typedef unsigned short igtl_uint16;
typedef signed short   igtl_int16;
#elif IGTL_SIZEOF_INT == 2
typedef unsigned int   igtl_uint16;
typedef signed int     igtl_int16;
#else
# error "No native data type can represent a 16-bit integer."
#endif

/* 32-bit integer type */
#if IGTL_SIZEOF_INT == 4
typedef unsigned int   igtl_uint32;
typedef signed int     igtl_int32;
#elif IGTL_SIZEOF_LONG == 4
typedef unsigned long  igtl_uint32;
typedef signed long    igtl_int32;
#else
# error "No native data type can represent a 32-bit integer."
#endif

/* 64-bit integer type */
#if defined(IGTL_TYPE_USE_LONG_LONG) && IGTL_SIZEOF_LONG_LONG == 8
typedef unsigned long long igtl_uint64;
typedef signed long long   igtl_int64;
#elif IGTL_SIZEOF_INT == 8
typedef unsigned int       igtl_uint64;
typedef signed int         igtl_int64;
#elif IGTL_SIZEOF_LONG == 8
typedef unsigned long      igtl_uint64;
typedef signed long        igtl_int64;
#elif defined(IGTL_TYPE_USE___INT64) && IGTL_SIZEOF___INT64 == 8
typedef unsigned __int64   igtl_uint64;
typedef signed __int64     igtl_int64;
#elif defined(IGTL_TYPE_USE_INT64_T) && IGTL_SIZEOF_INT64_T == 8
typedef unsigned int64_t   igtl_uint64;
typedef signed int64_t     igtl_int64;
#else
# error "No native data type can represent a 64-bit integer."
#endif

/* 32-bit floating point type */
#if IGTL_SIZEOF_FLOAT == 4
typedef float              igtl_float32;
#else
# error "No native data type can represent a 32-bit floating point value."
#endif

/* 64-bit floating point type */
#if IGTL_SIZEOF_DOUBLE == 8
typedef double             igtl_float64;
#else
# error "No native data type can represent a 64-bit floating point value."
#endif

/* 128-bit complex type (64-bit real + 64-bit imaginal)*/
typedef double igtl_complex[2];


#endif /* __IGTL_TYPES_H */





/*=========================================================================

  Program:   OpenIGTLLink Library -- Dummy point data
  Module:    $RCSfile: $
  Language:  C
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_TEST_DATA_POLYDATA_H
#define __IGTL_TEST_DATA_POLYDATA_H

unsigned char test_polydata_message_header[] = {
  /*------- OpenIGTLink message header --------*/
  0x00, 0x01,                                     /* Version number */
  0x50, 0x4f, 0x4c, 0x59, 0x44, 0x41, 0x54, 0x41,
  0x00, 0x00, 0x00, 0x00,                         /* POLYDATA */
  0x44, 0x65, 0x76, 0x69, 0x63, 0x65, 0x4e, 0x61,
  0x6d, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,                         /* Device name */
  0x00, 0x00, 0x00, 0x00, 0x49, 0x96, 0x02, 0xd4, /* Time stamp */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x2c, /* Body size */
  0x73, 0xdd, 0xdf, 0xc8, 0x0c, 0x48, 0xc6, 0x2b, /* CRC */
};

unsigned char test_polydata_message_body[] = {
  /*---------- POLYDATA message header ------------*/
  0x00, 0x00, 0x00, 0x08,                         /* Number of points */
  0x00, 0x00, 0x00, 0x00,                         /* Number of vertices */
  0x00, 0x00, 0x00, 0x00,                         /* size of vertice data */
  0x00, 0x00, 0x00, 0x00,                         /* Number of lines */
  0x00, 0x00, 0x00, 0x00,                         /* size of line data */
  0x00, 0x00, 0x00, 0x06,                         /* Number of polygons */
  0x00, 0x00, 0x00, 0x78,                         /* size of polygon data */
  0x00, 0x00, 0x00, 0x00,                         /* Number of triangle strips */
  0x00, 0x00, 0x00, 0x00,                         /* size of triangle strip data */
  0x00, 0x00, 0x00, 0x01,                         /* number of attributes */

  /*---------- POLYDATA message body (geometry section) ------------*/
  0x00, 0x00, 0x00, 0x00,                         /* Point #0 - x*/
  0x00, 0x00, 0x00, 0x00,                         /* Point #0 - y*/
  0x00, 0x00, 0x00, 0x00,                         /* Point #0 - z*/

  0x3f, 0x80, 0x00, 0x00,                         /* Point #1 - x*/
  0x00, 0x00, 0x00, 0x00,                         /* Point #1 - y*/
  0x00, 0x00, 0x00, 0x00,                         /* Point #1 - z*/

  0x3f, 0x80, 0x00, 0x00,                         /* Point #2 - x*/
  0x3f, 0x80, 0x00, 0x00,                         /* Point #2 - y*/
  0x00, 0x00, 0x00, 0x00,                         /* Point #2 - z*/

  0x00, 0x00, 0x00, 0x00,                         /* Point #3 - x*/
  0x3f, 0x80, 0x00, 0x00,                         /* Point #3 - y*/
  0x00, 0x00, 0x00, 0x00,                         /* Point #3 - z*/

  0x00, 0x00, 0x00, 0x00,                         /* Point #4 - x*/
  0x00, 0x00, 0x00, 0x00,                         /* Point #4 - y*/
  0x3f, 0x80, 0x00, 0x00,                         /* Point #4 - z*/

  0x3f, 0x80, 0x00, 0x00,                         /* Point #5 - x*/
  0x00, 0x00, 0x00, 0x00,                         /* Point #5 - y*/
  0x3f, 0x80, 0x00, 0x00,                         /* Point #5 - z*/

  0x3f, 0x80, 0x00, 0x00,                         /* Point #6 - x*/
  0x3f, 0x80, 0x00, 0x00,                         /* Point #6 - y*/
  0x3f, 0x80, 0x00, 0x00,                         /* Point #6 - z*/

  0x00, 0x00, 0x00, 0x00,                         /* Point #7 - x*/
  0x3f, 0x80, 0x00, 0x00,                         /* Point #7 - y*/
  0x3f, 0x80, 0x00, 0x00,                         /* Point #7 - z*/

                                                  /* No vertices */

                                                  /* No lines */

  0x00, 0x00, 0x00, 0x04,                         /* Polygon #0 - number of data */
  0x00, 0x00, 0x00, 0x00,                         /* Polygon #0 - point #0 */
  0x00, 0x00, 0x00, 0x01,                         /* Polygon #0 - point #1 */
  0x00, 0x00, 0x00, 0x02,                         /* Polygon #0 - point #2 */
  0x00, 0x00, 0x00, 0x03,                         /* Polygon #0 - point #3 */

  0x00, 0x00, 0x00, 0x04,                         /* Polygon #1 - number of data */
  0x00, 0x00, 0x00, 0x04,                         /* Polygon #1 - point #0 */
  0x00, 0x00, 0x00, 0x05,                         /* Polygon #1 - point #1 */
  0x00, 0x00, 0x00, 0x06,                         /* Polygon #1 - point #2 */
  0x00, 0x00, 0x00, 0x07,                         /* Polygon #1 - point #3 */

  0x00, 0x00, 0x00, 0x04,                         /* Polygon #2 - number of data */
  0x00, 0x00, 0x00, 0x00,                         /* Polygon #2 - point #0 */
  0x00, 0x00, 0x00, 0x01,                         /* Polygon #2 - point #1 */
  0x00, 0x00, 0x00, 0x05,                         /* Polygon #2 - point #2 */
  0x00, 0x00, 0x00, 0x04,                         /* Polygon #2 - point #3 */

  0x00, 0x00, 0x00, 0x04,                         /* Polygon #3 - number of data */
  0x00, 0x00, 0x00, 0x01,                         /* Polygon #3 - point #0 */
  0x00, 0x00, 0x00, 0x02,                         /* Polygon #3 - point #1 */
  0x00, 0x00, 0x00, 0x06,                         /* Polygon #3 - point #2 */
  0x00, 0x00, 0x00, 0x05,                         /* Polygon #3 - point #3 */

  0x00, 0x00, 0x00, 0x04,                         /* Polygon #4 - number of data */
  0x00, 0x00, 0x00, 0x02,                         /* Polygon #4 - point #0 */
  0x00, 0x00, 0x00, 0x03,                         /* Polygon #4 - point #1 */
  0x00, 0x00, 0x00, 0x07,                         /* Polygon #4 - point #2 */
  0x00, 0x00, 0x00, 0x06,                         /* Polygon #4 - point #3 */

  0x00, 0x00, 0x00, 0x04,                         /* Polygon #5 - number of data */
  0x00, 0x00, 0x00, 0x03,                         /* Polygon #5 - point #0 */
  0x00, 0x00, 0x00, 0x00,                         /* Polygon #5 - point #1 */
  0x00, 0x00, 0x00, 0x04,                         /* Polygon #5 - point #2 */
  0x00, 0x00, 0x00, 0x07,                         /* Polygon #5 - point #3 */
  
  /*---------- POLYDATA message body (attribute section) ------------*/
  0x00,                                           /* Attribute #0 Type (upper 8bit) - type */
  0x01,                                           /* Attribute #0 Type (lower 8bit) - number of components */
  0x00, 0x00, 0x00, 0x08,                         /* Attribute #0 Size */

  0x61, 0x74, 0x74, 0x72,                         /* Attribute #0 - name */
  0x00,                                           /* Name string terminater */
  0x00,                                           /* Padding required */

  0x00, 0x00, 0x00, 0x00,                         /* Point #0 */
  0x3f, 0x80, 0x00, 0x00,                         /* Point #1 */
  0x40, 0x00, 0x00, 0x00,                         /* Point #2 */
  0x40, 0x40, 0x00, 0x00,                         /* Point #3 */
  0x40, 0x80, 0x00, 0x00,                         /* Point #4 */
  0x40, 0xa0, 0x00, 0x00,                         /* Point #5 */
  0x40, 0xc0, 0x00, 0x00,                         /* Point #6 */
  0x40, 0xe0, 0x00, 0x00,                         /* Point #7 */
 
};

#endif /* IGTL_TEST_DATA_POLYDATA_H */




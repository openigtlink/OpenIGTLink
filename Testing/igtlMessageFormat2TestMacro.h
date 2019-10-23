/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igltMessageFormat2TestMarco_h
#define __igltMessageFormat2TestMarco_h

#include <igtl_types.h>

#define EXTENDED_CONTENT_SIZE 69 // the extended content size variable sums up the size of the Extended header, meta data header and meta data.


#define igtlMetaDataAddElementMacro(object) \
  object->SetHeaderVersion(IGTL_HEADER_VERSION_2);\
  IANA_ENCODING_TYPE codingScheme = IANA_TYPE_US_ASCII; /* 3 corresponding to US-ASCII */ \
  object->SetMetaDataElement("First patient age", codingScheme, "22");\
  object->SetMetaDataElement("Second patient age",codingScheme, "25");\
  object->SetMessageID(1);

#define igtlMetaDataComparisonMacro(object) \
  {\
  std::vector<std::string> groundTruth(0); \
  groundTruth.push_back("First patient age");\
  groundTruth.push_back("Second patient age");\
  \
  std::vector<std::string> groundTruthAge(0);\
  groundTruthAge.push_back("22");\
  groundTruthAge.push_back("25");\
  \
  EXPECT_EQ(object->GetMessageID(),1);\
  \
  \
  int i = 0;\
  for (igtl::MessageBase::MetaDataMap::const_iterator it = object->GetMetaData().begin(); it != object->GetMetaData().end(); ++it, ++i)\
  {\
    EXPECT_STREQ(it->first.c_str(), groundTruth[i].c_str());\
    EXPECT_EQ(it->second.first, IANA_TYPE_US_ASCII);\
    EXPECT_STREQ(it->second.second.c_str(), groundTruthAge[i].c_str());\
  }\
  }


#endif // __igltMessageFormat2TestMarco_h



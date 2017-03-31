//
//  CodecCommonClasses.cpp
//  OpenIGTLink
//
//  Created by Longquan Chen on 3/30/17.
//
//

#include "igtlCodecCommonClasses.h"


ReadConfigFile::ReadConfigFile()
: configFile(NULL)
, configFileName ("")
, lineNums(0) {
}

ReadConfigFile::ReadConfigFile (const char* fileName)
: configFile (0)
, configFileName (fileName)
, lineNums (0) {
  if (strlen (fileName) > 0) {
    configFile = fopen (fileName, "r");
  }
}

ReadConfigFile::ReadConfigFile (const std::string& fileName)
: configFile (0)
, configFileName (fileName.c_str())
, lineNums (0) {
  if (strlen (fileName.c_str()) > 0) {
    configFile = fopen (fileName.c_str(), "r");
  }
}

ReadConfigFile::~ReadConfigFile() {
  if (configFile) {
    fclose (configFile);
    configFile = NULL;
  }
}

void ReadConfigFile::OpenFile (const char* strFile) {
  if (strFile != NULL && strlen (strFile) > 0) { // confirmed_safe_unsafe_usage
    configFileName = strFile;
    configFile = fopen (strFile, "r");
  }
}

long ReadConfigFile::ReadLine (std::string* var, const int valSize/* = 4*/) {
  if (configFile == NULL || var == NULL || valSize <= 1)
    return 0;
  
  std::string* strTags = &var[0];
  int nTagNum = 0, n = 0;
  bool bCommentFlag = false;
  
  while (n < valSize) {
    var[n] = "";
    ++ n;
  }
  do {
    const char singleChar = (char)fgetc (configFile);
    
    if (singleChar == '\n' || feof (configFile)) {
      ++ lineNums;
      break;
    }
    if (singleChar == '#')
      bCommentFlag = true;
    if (!bCommentFlag) {
      if (singleChar == '\t' || singleChar == ' ') {
        if (nTagNum >= valSize-1)
          break;
        if (! (*strTags).empty()) {
          ++ nTagNum;
          strTags = &var[nTagNum];
        }
      } else
        *strTags += singleChar;
    }
    
  } while (true);
  
  return 1 + nTagNum;
}

const bool ReadConfigFile::EndOfFile() {
  if (configFile == NULL)
    return true;
  return feof (configFile) ? true : false;
}

const int ReadConfigFile::GetLines() {
  return lineNums;
}

const bool ReadConfigFile::ExistFile() {
  return (configFile != NULL);
}

const std::string& ReadConfigFile::GetFileName() {
  return configFileName;
}

void GenericDecoder::Write2File (FILE* pFp, unsigned char* pData[], igtl_uint32 iDimensions[], igtl_uint32 iStride[])
{
  int i;
  unsigned char*  pPtr = NULL;
  igtl_uint32 iWidth = iDimensions[0];
  igtl_uint32 iHeight = iDimensions[1];
  pPtr = pData[0];
  for (i = 0; i < iHeight; i++) {
    fwrite (pPtr, 1, iWidth, pFp);
    pPtr += iStride[0];
  }
  
  iHeight = iHeight / 2;
  iWidth = iWidth / 2;
  pPtr = pData[1];
  for (i = 0; i < iHeight; i++) {
    fwrite (pPtr, 1, iWidth, pFp);
    pPtr += iStride[1];
  }
  
  pPtr = pData[2];
  for (i = 0; i < iHeight; i++) {
    fwrite (pPtr, 1, iWidth, pFp);
    pPtr += iStride[1];
  }
}

void GenericDecoder::ComposeByteSteam(igtl_uint8** inputData, int dimension[2], int iStride[2], igtl_uint8 *outputFrame)
{
  int iWidth = dimension[0];
  int iHeight = dimension[1];
#pragma omp parallel for default(none) shared(outputByteStream,inputData, iStride, iHeight, iWidth)
  for (int i = 0; i < iHeight; i++)
  {
    igtl_uint8* pPtr = inputData[0]+i*iStride[0];
    for (int j = 0; j < iWidth; j++)
    {
      outputFrame[i*iWidth + j] = pPtr[j];
    }
  }
#pragma omp parallel for default(none) shared(outputByteStream,inputData, iStride, iHeight, iWidth)
  for (int i = 0; i < iHeight/2; i++)
  {
    igtl_uint8* pPtr = inputData[1]+i*iStride[1];
    for (int j = 0; j < iWidth/2; j++)
    {
      outputFrame[i*iWidth/2 + j + iHeight*iWidth] = pPtr[j];
    }
  }
#pragma omp parallel for default(none) shared(outputByteStream, inputData, iStride, iHeight, iWidth)
  for (int i = 0; i < iHeight/2; i++)
  {
    igtl_uint8* pPtr = inputData[2]+i*iStride[1];
    for (int j = 0; j < iWidth/2; j++)
    {
      outputFrame[i*iWidth/2 + j + iHeight*iWidth*5/4] = pPtr[j];
    }
  }
}

igtl_int64 GenericDecoder::getCurrentTime()
{
#if defined(_WIN32)
  SYSTEMTIME sysTime = {0};
  GetLocalTime(&sysTime);
  return sysTime.wMilliseconds;
#else
  struct timeval tv_date;
  gettimeofday(&tv_date, NULL);
  return ((igtl_int64)tv_date.tv_sec * 1000000 + (igtl_int64)tv_date.tv_usec);
#endif
  
}
//
//  CodecCommonClasses.cpp
//  OpenIGTLink
//
//  Created by Longquan Chen on 3/30/17.
//
//

#include "igtlCodecCommonClasses.h"

namespace igtl {

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
  igtl_uint32 i;
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

int GenericDecoder::UnpackUncompressedData(igtl::VideoMessage* videoMessage, SourcePicture* decodedPic)
{
  igtl_int32 iWidth = videoMessage->GetWidth();
  igtl_int32 iHeight = videoMessage->GetHeight();
  if ((iWidth*iHeight*3>>1) != videoMessage->GetBitStreamSize())
    {
    return -1;
    }
  igtl_uint16 frameType = videoMessage->GetFrameType();
  isGrayImage = false;
  if (frameType>0X00FF)
    {
    frameType= frameType>>8;
    isGrayImage = true;
    }
  decodedPic->picWidth = iWidth;
  decodedPic->picHeight = iHeight;
  decodedPic->data[1]= decodedPic->data[0] + iWidth*iHeight;
  decodedPic->data[2]= decodedPic->data[1] + iWidth*iHeight/4;
  decodedPic->stride[0] = iWidth;
  decodedPic->stride[1] = decodedPic->stride[2] = iWidth>>1;
  decodedPic->stride[3] = 0;
  memcpy(decodedPic->data[0], videoMessage->GetPackFragmentPointer(2), iWidth*iHeight*3>>1);
  return 1;
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

int GenericDecoder::ConvertYUVToRGB(igtl_uint8 *YUVFrame, igtl_uint8* RGBFrame, int iHeight, int iWidth)
{
  int componentLength = iHeight*iWidth;
  const igtl_uint8 *srcY = YUVFrame;
  const igtl_uint8 *srcU = YUVFrame+componentLength;
  const igtl_uint8 *srcV = YUVFrame+componentLength*5/4;
  igtl_uint8 * YUV444 = new igtl_uint8[componentLength * 3];
  igtl_uint8 *dstY = YUV444;
  igtl_uint8 *dstU = dstY + componentLength;
  igtl_uint8 *dstV = dstU + componentLength;
  
  memcpy(dstY, srcY, componentLength);
  const int halfHeight = iHeight/2;
  const int halfWidth = iWidth/2;
  
#pragma omp parallel for default(none) shared(dstV,dstU,srcV,srcU,iWidth)
  for (int y = 0; y < halfHeight; y++)
    {
    for (int x = 0; x < halfWidth; x++)
      {
      dstU[2 * x + 2 * y*iWidth] = dstU[2 * x + 1 + 2 * y*iWidth] = srcU[x + y*iWidth/2];
      dstV[2 * x + 2 * y*iWidth] = dstV[2 * x + 1 + 2 * y*iWidth] = srcV[x + y*iWidth/2];
      }
    memcpy(&dstU[(2 * y + 1)*iWidth], &dstU[(2 * y)*iWidth], iWidth);
    memcpy(&dstV[(2 * y + 1)*iWidth], &dstV[(2 * y)*iWidth], iWidth);
    }
  
  
  const int yOffset = 16;
  const int cZero = 128;
  int yMult, rvMult, guMult, gvMult, buMult;
  yMult =   298;
  rvMult = 409;
  guMult = -100;
  gvMult = -208;
  buMult = 517;
  
  static unsigned char clp_buf[384+256+384];
  static unsigned char *clip_buf = clp_buf+384;
  
  // initialize clipping table
  memset(clp_buf, 0, 384);
  
  for (int i = 0; i < 256; i++)
    {
    clp_buf[384+i] = i;
    }
  memset(clp_buf+384+256, 255, 384);
  
  
#pragma omp parallel for default(none) shared(dstY,dstU,dstV,RGBFrame,yMult,rvMult,guMult,gvMult,buMult,clip_buf,componentLength)// num_threads(2)
  for (int i = 0; i < componentLength; ++i)
    {
    const int Y_tmp = ((int)dstY[i] - yOffset) * yMult;
    const int U_tmp = (int)dstU[i] - cZero;
    const int V_tmp = (int)dstV[i] - cZero;
    
    const int R_tmp = (Y_tmp                  + V_tmp * rvMult ) >> 8;//32 to 16 bit conversion by left shifting
    const int G_tmp = (Y_tmp + U_tmp * guMult + V_tmp * gvMult ) >> 8;
    const int B_tmp = (Y_tmp + U_tmp * buMult                  ) >> 8;
    
    RGBFrame[3*i]   = clip_buf[R_tmp];
    RGBFrame[3*i+1] = clip_buf[G_tmp];
    RGBFrame[3*i+2] = clip_buf[B_tmp];
    }
  delete [] YUV444;
  YUV444 = NULL;
  dstY = NULL;
  dstU = NULL;
  dstV = NULL;
  return 1;
}

int GenericDecoder::ConvertYUVToGrayImage(igtl_uint8 * YUV420Frame, igtl_uint8 *GrayFrame, int iHeight, int iWidth)
{
  int componentLength = iHeight*iWidth;
  for (int i = 0,j= 0; i < 3*componentLength; i=i+3,j++) {
    GrayFrame[i] = YUV420Frame[j];
    GrayFrame[i+1] = YUV420Frame[j];
    GrayFrame[i+2] = YUV420Frame[j];
  }
  return 1;
}

void GenericEncoder::ConvertRGBToYUV(igtlUint8 *rgb, igtlUint8 *destination, unsigned int width, unsigned int height)
{
  size_t image_size = width * height;
  size_t upos = image_size;
  size_t vpos = upos + upos / 4;
  size_t i = 0;
  
  for (size_t line = 0; line < height; ++line)
    {
    if (!(line % 2))
      {
      for (size_t x = 0; x < width; x += 2)
        {
        igtlUint8 r = rgb[3 * i];
        igtlUint8 g = rgb[3 * i + 1];
        igtlUint8 b = rgb[3 * i + 2];
        
        destination[i++] = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
        
        destination[upos++] = ((-38 * r - 74 * g + 112 * b) >> 8) + 128;
        destination[vpos++] = ((112 * r - 94 * g -18 * b) >> 8) + 128;
        
        r = rgb[3 * i];
        g = rgb[3 * i + 1];
        b = rgb[3 * i + 2];
        
        destination[i++] = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
        }
      }
    else
      {
      for (size_t x = 0; x < width; x += 1)
        {
        igtlUint8 r = rgb[3 * i];
        igtlUint8 g = rgb[3 * i + 1];
        igtlUint8 b = rgb[3 * i + 2];
        
        destination[i++] = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
        }
      }
    }
}

int GenericEncoder::PackUncompressedData(SourcePicture* pSrcPic, igtl::VideoMessage* videoMessage, bool isGrayImage)
{
  int iSourceWidth = pSrcPic->picWidth;
  int iSourceHeight = pSrcPic->picHeight;
  long kiPicResSize = iSourceWidth*iSourceHeight * 3 >> 1;
  videoMessage->SetBitStreamSize(kiPicResSize);
  videoMessage->AllocateScalars();
  int endian = (igtl_is_little_endian() == 1 ? IGTL_VIDEO_ENDIAN_LITTLE : IGTL_VIDEO_ENDIAN_BIG);
  videoMessage->SetEndian(endian); //little endian is 2 big endian is 1
  videoMessage->SetWidth(pSrcPic->picWidth);
  videoMessage->SetHeight(pSrcPic->picHeight);
  encodedFrameType = FrameTypeKey;
  if (isGrayImage)
    {
    encodedFrameType = FrameTypeKey << 8;
    }
  videoMessage->SetFrameType(encodedFrameType);
  memcpy(videoMessage->GetPackFragmentPointer(2), pSrcPic->data[0], kiPicResSize);
  videoMessage->Pack();
  return 0;
}

} //namespace igtl
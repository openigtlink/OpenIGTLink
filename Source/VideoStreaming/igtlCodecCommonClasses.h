//
//  CodecCommonClasses.h
//  OpenIGTLink
//
//  Created by Longquan Chen on 3/30/17.
//
//

#ifndef igtlCodecCommonClasses_h
#define igtlCodecCommonClasses_h

#include <time.h>
#if defined(_WIN32) /*&& defined(_DEBUG)*/
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#else
#include <sys/time.h>
#endif
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "igtlVideoMessage.h"


namespace igtl {

/**
 * @brief Enumerate the type of video format,  Only YV12, I420, I422, I444 images are supported in the codec. RGB format are not supported yet.
 */
typedef enum {
  FormatI420       = 1,     ////  YUV420
  FormatI444        = 2     //// I444,
} VideoFormatType;

typedef enum {
  FrameTypeInvalid,    ///< encoder not ready or parameters are invalidate
  FrameTypeKey,        ///< Key Frame
  FrameTypeIntraPrediction,        ///< Intra Prediction Frame
  FrameTypeInterPrediction,        ///< Inter Prediction Frame
  FrameTypeSkip,       ///< skip the frame based encoder kernel
  FrameTypeUnKnown     ///
} VideoFrameType;

/**
 * @brief Enumerate  return type
 */
typedef enum {
  ResultSuccess,          ///< successful
  InitParaError,          ///< parameters are invalid
  UnknownReason,
  MallocMemeError,        ///< malloc a memory error
  InitExpected,           ///< initial action is expected
  UnsupportedData
} cmRETURN;

typedef struct {
  int       colorFormat;          ///< color space type
  int       stride[4];            ///< stride for each plane pData
  unsigned char*  data[4];        ///< plane pData
  int       picWidth;             ///< luma picture width in x coordinate
  int       picHeight;            ///< luma picture height in y coordinate
  long long timeStamp;           ///< timestamp of the source picture, unit: millisecond
} SourcePicture;

class IGTLCommon_EXPORT ReadConfigFile {
public:
  ReadConfigFile();
  ReadConfigFile (const char* pConfigFileName);
  ReadConfigFile (const std::string& pConfigFileName);
  ~ReadConfigFile();
  
  void OpenFile (const char* strFile);
  long ReadLine (std::string* strVal, const int iValSize = 4);
  const bool EndOfFile();
  const int GetLines();
  const bool ExistFile();
  const std::string& GetFileName();
  
private:
  FILE*             configFile;
  std::string       configFileName;
  unsigned int      lineNums;
};

class IGTLCommon_EXPORT GenericEncoder : public Object
{
public:
  igtlTypeMacro(GenericEncoder, Object);

  GenericEncoder(){
    this->configFile = std::string("");
    
    this->encodedFrameType = -1;

    this->isLossLessLink  = true;
    
    this->initializationDone = false;
    
    this->picWidth = 0;
    
    this->picHeight = 0;
    
    this->codecSpeed = 0;
  };
  virtual ~GenericEncoder(){};
  //void UpdateHashFromFrame (SFrameBSInfo& info, SHA1Context* ctx);
  
  //bool CompareHash (const unsigned char* digest, const char* hashStr);
  
  /**
   Parse the configuration file to initialize the encoder and server.
   */
  virtual int InitializeEncoder() = 0;
  
  
  virtual void SetConfigurationFile(std::string configFile){this->configFile = std::string(configFile);};
  
  virtual int FillSpecificParameters(){return -1;};
  
  /**
   Encode a frame, for performance issue, before encode the frame, make sure the frame pointer is updated with a new frame.
   Otherwize, the old frame will be encoded.
   */
  virtual int EncodeSingleFrameIntoVideoMSG(SourcePicture* pSrcPic, igtl::VideoMessage* videoMessage, bool isGrayImage = false ){return -1;};
  
  /**
   Get the encoder and server initialization status.
   */
  virtual bool GetInitializationStatus(){return initializationDone;};
  
  /**
   Get the type of encoded frame
   */
  
  /**
   Convert the generic image format to local specific data format of the codec
   */
  virtual int ConvertToLocalImageFormat(SourcePicture* pSrcPic){return 0;};
  
  virtual int GetVideoFrameType(){return encodedFrameType;};
  
  virtual unsigned int GetPicWidth(){return this->picWidth;};
  
  virtual unsigned int GetPicHeight(){return this->picHeight;};
  
  virtual int SetLosslessLink(bool linkMethod){this->isLossLessLink = linkMethod; return 0;};
  
  virtual bool GetLosslessLink(){return this->isLossLessLink;};
  
  virtual int SetSpeed(int speed){return -1;};
  
  virtual int SetRCMode(int value){return -1;};
  
  virtual int SetKeyFrameDistance(int frameNum){return -1;};
  
  virtual int SetQP(int maxQP, int minQP){return -1;};
  
  virtual int SetRCTaregetBitRate(unsigned int bitRate){return -1;};
  
  virtual int SetPicWidthAndHeight(unsigned int Width, unsigned int Height){return -1;};
  
  static void ConvertRGBToYUV(igtlUint8 *rgb, igtlUint8 *destination, unsigned int width, unsigned int height);
   
  int PackUncompressedData(SourcePicture* pSrcPic, igtl::VideoMessage* videoMessage, bool isGrayImage);

protected:
  
  unsigned int picWidth;
  
  unsigned int picHeight;
  
  int encodedFrameType;
  
  ReadConfigFile cRdCfg;
    
  std::string configFile;
  
  bool initializationDone;
  
  bool isLossLessLink;
  
  int codecSpeed;
  
};

class IGTLCommon_EXPORT GenericDecoder : public Object
{
public:
  igtlTypeMacro(igtl::GenericDecoder, igtl::Object);

  GenericDecoder(){deviceName = ""; isGrayImage = false;};
  virtual ~GenericDecoder(){};
  
  virtual int DecodeBitStreamIntoFrame(unsigned char* bitStream,igtl_uint8* outputFrame,igtl_uint32 iDimensions[], igtl_uint64 &iStreamSize) = 0;
  
  static void Write2File (FILE* pFp, unsigned char* pData[], igtl_uint32 iDimensions[], igtl_uint32 iStride[]);
  
  virtual int DecodeVideoMSGIntoSingleFrame(igtl::VideoMessage* videoMessage, SourcePicture* decodedPic){return -1;};
  
  virtual igtl_int64 getCurrentTime();
  
  virtual std::string GetDeviceName()
  {
    return this->deviceName;
  };
  
  virtual void SetDeviceName(std::string name)
  {
    this->deviceName = std::string(name);
  };
  
  virtual bool GetIsGrayImage()
  {
    return this->isGrayImage;
  };
  
  virtual void SetIsGrayImage(bool grayImage)
  {
    this->isGrayImage = grayImage;
  };
  
  /**
   The conversion equations between RGB and YUV is Keith Jack's book "Video Demystified" (ISBN 1-878707-09-4). The equations are:
   RGB to YUV Conversion
   Y  = (0.257 * R) + (0.504 * G) + (0.098 * B) + 16
   
   U = -(0.148 * R) - (0.291 * G) + (0.439 * B) + 128
   
   V =  (0.439 * R) - (0.368 * G) - (0.071 * B) + 128
   
   YUV to RGB Conversion
   R = 1.164(Y - 16)                  + 1.596(V - 128)
   
   G = 1.164(Y - 16) - 0.391(U - 128) - 0.813(V - 128)
   
   B = 1.164(Y - 16) + 2.018(U - 128)
   
   To speed up the computation, float point multiplication is achieved via bitwise operator
   RGB to YUV Conversion
   Y  = (16843 * R + 33030 * G + 6423 * B) >> 16 + 16
   
   U = -(9699 * R - 19071 * G + 28770 * B) >> 16 + 128
   
   V =  (28770 * R - 24117 * G - 4653 * B) >> 16 + 128
   
   Or
   
   Y  = (66 * R + 129 * G + 25 * B) >> 8 + 16
   
   U = (-38 * R - 74 * G + 112 * B) >> 8 + 128
   
   V =  (112 * R - 94 * G - 18 * B) >> 8 + 128
   
   YUV to RGB Conversion
   R = (76284 * (Y - 16) + 104595 * (V - 128)) >> 16
   
   G = (76284 * (Y - 16) - 25625 * (U - 128) - 53280 * (V - 128)) >> 16
   
   B = (76284 * (Y - 16) + 132252 * (U - 128)) >> 16
   
   Or
   
   R = (298 * (Y - 16) + 409 * (V - 128)) >> 8
   
   G = (298 * (Y - 16) - 100 * (U - 128) - 208 * (V - 128)) >> 8
   
   B = (298 * (Y - 16) + 517 * (U - 128)) >> 8
   
   To do, use the latest conversion Scheme from ITU. 
   https://www.itu.int/dms_pubrec/itu-r/rec/bt/R-REC-BT.2020-2-201510-I!!PDF-E.pdf
   */
  static int ConvertYUVToRGB(igtl_uint8 *YUVFrame, igtl_uint8* RGBFrame, int iHeight, int iWidth);
  
  static int ConvertYUVToGrayImage(igtl_uint8 * YUV420Frame, igtl_uint8 *GrayFrame, int iHeight, int iWidth);
  
  int UnpackUncompressedData(igtl::VideoMessage* videoMessage, SourcePicture* decodedPic);
  
protected:
  
  virtual void ComposeByteSteam(igtl_uint8** inputData, int dimension[2], int iStride[2], igtl_uint8 *outputFrame){};
  
  std::string deviceName;
  
  bool isGrayImage;
  
};

} // namespace igtl
#endif

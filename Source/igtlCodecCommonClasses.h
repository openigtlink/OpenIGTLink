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

/**
 * @brief Enumerate the type of video format
 */
typedef enum {
  FormatI420       = 1     //// only YUV420 is supported now
} VideoFormatType;

typedef enum {
  FrameTypeInvalid,    ///< encoder not ready or parameters are invalidate
  FrameTypeKey,        ///< Key Frame
  FrameTypeSkip,       ///< skip the frame based encoder kernel
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

class ReadConfigFile {
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

class GenericEncoder
{
public:
  GenericEncoder(){
    this->configFile = std::string("");
    
    this->encodedFrameType = -1;
    
    this->useCompress = true;
    
    this->isLossLessLink  = true;
    
    this->initializationDone = false;
    
    this->picWidth = 0;
    
    this->picHeight = 0;
    
    this->codecSpeed = 0;
  };
  ~GenericEncoder(){};
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
  
  virtual void SetUseCompression(bool useCompression){
    this->useCompress = useCompression;
  };
  
  virtual bool GetUseCompression(){return useCompress;};
  
  virtual int SetLosslessLink(bool linkMethod){this->isLossLessLink = linkMethod; return 0;};
  
  virtual bool GetLosslessLink(){return this->isLossLessLink;};
  
  virtual int SetSpeed(int speed){return -1;};
  
  virtual int SetRCMode(int value){return -1;};
  
  virtual int SetKeyFrameDistance(int frameNum){return -1;};
  
  virtual int SetQP(int maxQP, int minQP){return -1;};
  
  virtual int SetRCTaregetBitRate(unsigned int bitRate){return -1;};
  
  virtual int SetPicWidthAndHeight(unsigned int Width, unsigned int Height){return -1;};
   
  int PackUncompressedData(SourcePicture* pSrcPic, igtl::VideoMessage* videoMessage, bool isGrayImage);

protected:
  unsigned int picWidth;
  
  unsigned int picHeight;
  
  int encodedFrameType;
  
  ReadConfigFile cRdCfg;
  
  bool  useCompress;
  
  std::string configFile;
  
  bool initializationDone;
  
  bool isLossLessLink;
  
  int codecSpeed;
  
};

class GenericDecoder
{
public:
  GenericDecoder(){deviceName = ""; isGrayImage = false;};
  ~GenericDecoder(){};
  
  virtual int DecodeBitStreamIntoFrame(unsigned char* bitStream,igtl_uint8* outputFrame,igtl_uint32 iDimensions[], igtl_uint64 &iStreamSize) = 0;
  
  virtual void Write2File (FILE* pFp, unsigned char* pData[], igtl_uint32 iDimensions[], igtl_uint32 iStride[]);
  
  virtual int DecodeVideoMSGIntoSingleFrame(igtl::VideoMessage* videoMessage, SourcePicture* decodedPic){return 0;};
  
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
  
  int ConvertYUVToRGB(igtl_uint8 *YUVFrame, igtl_uint8* RGBFrame, int iHeight, int iWidth);
  
  int ConvertYUVToGrayImage(igtl_uint8 * YUV420Frame, igtl_uint8 *GrayFrame, int iHeight, int iWidth);
  
protected:
  
  virtual void ComposeByteSteam(igtl_uint8** inputData, int dimension[2], int iStride[2], igtl_uint8 *outputFrame){};
  
  std::string deviceName;
  
  bool isGrayImage;
  
};




float* SSIMCalculate();

#endif

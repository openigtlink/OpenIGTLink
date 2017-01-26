#include <time.h>
#if defined(_WIN32) /*&& defined(_DEBUG)*/
  #include <windows.h>
  #include <stdio.h>
  #include <stdarg.h>
  #include <sys/types.h>
#else
  #include <sys/time.h>
#endif
#include <vector>


#include "api/svc/codec_api.h"
#include "api/svc/codec_app_def.h"

#define NO_DELAY_DECODING
class H264Decode
{
public: 
  H264Decode();
  void Write2File (FILE* pFp, unsigned char* pData[3], int iStride[2], int iWidth, int iHeight);

  int Process (void* pDst[3], SBufferInfo* pInfo, FILE* pFp);
  
  void ComposeByteSteam(uint8_t** inputData, SBufferInfo bufInfo, uint8_t *outputByteStream,  int iWidth, int iHeight);
  
  int DecodeSingleNal (ISVCDecoder* pDecoder, unsigned char* kpH264BitStream,uint8_t* outputByteStream, const char* kpOuputFileName,
                                            int32_t& iWidth, int32_t& iHeight, int32_t& iStreamSize, const char* pOptionFileName);
};
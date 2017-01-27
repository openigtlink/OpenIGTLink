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
#include "igtl_types.h"

#define NO_DELAY_DECODING
class H264Decode
{
public: 
  H264Decode();
  void Write2File (FILE* pFp, unsigned char* pData[3], int iStride[2], int iWidth, int iHeight);

  int Process (void* pDst[3], SBufferInfo* pInfo, FILE* pFp);
  
  void ComposeByteSteam(igtl_uint8** inputData, SBufferInfo bufInfo, igtl_uint8 *outputByteStream,  int iWidth, int iHeight);
  
  int DecodeSingleNal (ISVCDecoder* pDecoder, unsigned char* kpH264BitStream,igtl_uint8* outputByteStream, const char* kpOuputFileName,
                                            igtl_int32& iWidth, igtl_int32& iHeight, igtl_int32& iStreamSize, const char* pOptionFileName);
};
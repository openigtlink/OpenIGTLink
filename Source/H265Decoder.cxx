/*=========================================================================
 
 Program:   H265Encoder
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "H265Decoder.h"

static void GetNALUnitFromByteStream(
                                     InputByteStreamNoFile& bs,
                                     vector<uint8_t>& nalUnit)
{
  while ((bs.eofBeforeNBytes(24/8) || bs.peekBytes(24/8) != 0x000001)
         &&     (bs.eofBeforeNBytes(32/8) || bs.peekBytes(32/8) != 0x00000001))
  {
    uint8_t leading_zero_8bits = bs.readByte();
    assert(leading_zero_8bits == 0);
  }
  if (bs.peekBytes(24/8) != 0x000001)
  {
    uint8_t zero_byte = bs.readByte();
    assert(zero_byte == 0);
  }
  uint32_t start_code_prefix_one_3bytes = bs.readBytes(24/8);
  assert(start_code_prefix_one_3bytes == 0x000001);
  while (bs.eofBeforeNBytes(24/8) || bs.peekBytes(24/8) > 2)
  {
    nalUnit.push_back(bs.readByte());
  }
  while ((bs.eofBeforeNBytes(24/8) || bs.peekBytes(24/8) != 0x000001)
         &&     (bs.eofBeforeNBytes(32/8) || bs.peekBytes(32/8) != 0x00000001))
  {
    uint8_t trailing_zero_8bits = bs.readByte();
    assert(trailing_zero_8bits == 0);
  }
}
  H265Decoder::H265Decoder()
  {
    this->pDecoder = new TDecTop();
    this->pDecoder->create();
    this->pDecoder->init();
    pDecoder->setDecodedPictureHashSEIEnabled(0);
    this->deviceName = "";
    pcListPic = NULL;
    bytestream = new InputByteStreamNoFile();
    previousStream = new igtl_uint8[0];
    previousStreamSize = 0;
    VPSReceived = false;
    SPSReceived = false;
    PPSReceived = false;
  }

  H265Decoder::~H265Decoder()
  {
    pDecoder->deletePicBuffer();
    pDecoder->destroy();
    if (pcListPic)
    {
      pcListPic->clear();
      pcListPic = NULL;
    }
    delete bytestream;
    delete previousStream;
    previousStream = NULL;
    pDecoder = NULL;
  }

  igtl_int32 iFrameCountTotal = 0;


  void H265Decoder::ComposeByteSteam(igtl_uint8** inputData, int dimension[2], int iStride[2], igtl_uint8 *outputFrame)
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


  int H265Decoder::DecodeVideoMSGIntoSingleFrame(igtl::VideoMessage* videoMessage, SourcePicture* pDecodedPic)
  {
    if(videoMessage->GetBitStreamSize())
    {
      igtl_int32 iWidth = videoMessage->GetWidth();
      igtl_int32 iHeight = videoMessage->GetHeight();
      igtl_uint64 iStreamSize = videoMessage->GetBitStreamSize();
      igtl_uint16 frameType = videoMessage->GetFrameType();
      isGrayImage = false;
      if (frameType>0X00FF)
      {
        frameType= frameType>>8;
        isGrayImage = true;
      }
      pDecodedPic->picWidth = iWidth;
      pDecodedPic->picHeight = iHeight;
      pDecodedPic->data[1]= pDecodedPic->data[0] + iWidth*iHeight;
      pDecodedPic->data[2]= pDecodedPic->data[1] + iWidth*iHeight/4;
      pDecodedPic->stride[0] = iWidth;
      pDecodedPic->stride[1] = pDecodedPic->stride[2] = iWidth>>1;
      pDecodedPic->stride[3] = 0;
      igtl_uint32 dimensions[2] = {iWidth, iHeight};
      int iRet = this->DecodeBitStreamIntoFrame(videoMessage->GetPackFragmentPointer(2), pDecodedPic->data[0], dimensions, iStreamSize);
      return iRet;
    }
    return -1;
  }

  int H265Decoder::DecodeBitStreamIntoFrame(unsigned char* kpH265BitStream,igtl_uint8* outputFrame, igtl_uint32 dimensions[2], igtl_uint64& iStreamSize) {
    int m_iSkipFrame = 0;
    int m_iPOCLastDisplay = m_iSkipFrame;
    if(previousStreamSize)
    {
      bytestream->PrepareStreamWithProcessedNal(kpH265BitStream, &(previousStream[0]), iStreamSize, previousStreamSize);
    }
    else
    {
      bytestream->PrepareStreamWithProcessedNal(kpH265BitStream, NULL, iStreamSize, 0);
    }
    Bool iRet = false;
    while(bytestream->GetPos()<(iStreamSize+previousStreamSize))
    {
      InputNALUnit nalu;
      GetNALUnitFromByteStream(*bytestream, nalu.getBitstream().getFifo());
      read(nalu);   
      if(nalu.m_nalUnitType == NAL_UNIT_VPS)
      {
        this->VPSReceived = true;
      }
      if(nalu.m_nalUnitType == NAL_UNIT_SPS)
      {
        this->SPSReceived = true;
      }
      if(nalu.m_nalUnitType == NAL_UNIT_PPS)
      {
        this->PPSReceived = true;
      }
      Bool iRet = false;
      if (nalu.m_nalUnitType == NAL_UNIT_VPS || nalu.m_nalUnitType == NAL_UNIT_SPS || nalu.m_nalUnitType == NAL_UNIT_PPS)
      {
        iRet = pDecoder->decode(nalu, m_iSkipFrame, m_iPOCLastDisplay);
      }
      else
      {
        if(this->VPSReceived && this->SPSReceived  && this->PPSReceived)
        {
          iRet = pDecoder->decode(nalu, m_iSkipFrame, m_iPOCLastDisplay);
        }
      }
      if (iRet)
      {
        int poc;
        pDecoder->executeLoopFilters(poc, pcListPic);
        iRet = ReconstructDecodedPic(*pcListPic->begin(), outputFrame);
        previousStreamSize = nalu.getBitstream().getFifo().size();
        previousStream = new igtl_uint8[previousStreamSize];
        bytestream->resetFutureBytes();
        memcpy(&previousStream[0], &nalu.getBitstream().getFifo()[0],  previousStreamSize);
        if(pcListPic->size()>=5)
        {
          pcListPic->popFront();
        }
        return iRet;
      }
    }
    if(bytestream->GetPos()>=iStreamSize && !iRet)
    {
      delete previousStream;
      previousStream = NULL;
      previousStreamSize = 0;
      return -1;
    }
    return -1;
  }

  int H265Decoder::ReconstructDecodedPic(TComPic* picTop, igtl_uint8* outputFrame)
  {
    Bool retval = true;
    const Window &conf    = picTop->getConformanceWindow();
    TComPicYuv*   picTopYUV = picTop->getPicYuvRec();
    Bool is16bit = false;
    const Int  stride420 = picTopYUV->getStride(COMPONENT_Y);
    const UInt width420  = picTopYUV->getWidth(COMPONENT_Y) - conf.getWindowLeftOffset() - conf.getWindowRightOffset();
    const UInt height420 = picTopYUV->getHeight(COMPONENT_Y) - conf.getWindowTopOffset() - conf.getWindowBottomOffset();
    int outputFramePos = 0;
    for(UInt comp=0; retval && comp<picTopYUV->getNumberValidComponents(); comp++)
    {
      const ComponentID compID = ComponentID(comp);
      const UInt csx = picTop->getComponentScaleX(compID);
      const UInt csy = picTop->getComponentScaleY(compID);
      const Int planeOffset =  (conf.getWindowLeftOffset()>>csx) + (conf.getWindowTopOffset()>>csy) * picTopYUV->getStride(compID);
      const UInt csx_file =getComponentScaleX(compID, CHROMA_420);
      const UInt csy_file =getComponentScaleY(compID, CHROMA_420);
      const UInt csx_src  =getComponentScaleX(compID, CHROMA_420);
      const UInt csy_src  =getComponentScaleY(compID, CHROMA_420);
      
      const UInt stride_src      = stride420>>csx_src;
      
      const UInt stride_file      = (width420 * (is16bit ? 2 : 1)) >> csx_file;
      const UInt width_file       = width420 >>csx_file;
      
      std::vector<unsigned char> bufVec(stride_file);
      Pel* src = picTopYUV->getAddr(compID) + planeOffset;
      UChar *buf=&(bufVec[0]);
      const UInt mask_y_file=(1<<csy_file)-1;
      const UInt mask_y_src =(1<<csy_src )-1;
      
      for(UInt y420=0; y420<height420; y420++)
      {
        if ((y420&mask_y_file)==0)
        {
          const UInt sx=csx_file-csx_src;
          if (!is16bit)
          {
            for (UInt x = 0; x < width_file; x++)
            {
              buf[x] = (UChar)(src[x<<sx]);
            }
          }
          else
          {
            for (UInt x = 0; x < width_file; x++)
            {
              buf[2*x  ] = (src[x<<sx]>>0) & 0xff;
              buf[2*x+1] = (src[x<<sx]>>8) & 0xff;
            }
          }
          memcpy(outputFrame+outputFramePos, buf,stride_file);
          outputFramePos += stride_file;
        }
        
        if ((y420&mask_y_src)==0)
        {
          src += stride_src;
        }
        
      }
    }
    return true;
  }

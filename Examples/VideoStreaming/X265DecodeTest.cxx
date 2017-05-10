//
//  main.c
//  libavHEVC
//
//  Created by Mickaël Raulet on 11/10/12.
//
//

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string.h>
#include "Lib/TLibDecoder/AnnexBread.h"
#include "Lib/TLibDecoder/NALread.h"


#include "TLibVideoIO/TVideoIOYuv.h"
#include "TLibCommon/TComList.h"
#include "TLibCommon/TComPicYuv.h"
#include "TLibDecoder/TDecTop.h"
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


/*bool isNaluWithinTargetDecLayerIdSet( InputNALUnit* nalu )
{
  if ( m_targetDecLayerIdSet.size() == 0 ) // By default, the set is empty, meaning all LayerIds are allowed
  {
    return true;
  }
  for (std::vector<Int>::iterator it = m_targetDecLayerIdSet.begin(); it != m_targetDecLayerIdSet.end(); it++)
  {
    if ( nalu->m_nuhLayerId == (*it) )
    {
      return true;
    }
  }
  return false;
}*/


void video_decode_example(const char* fileName)
{
  Int                 poc;
  TComList<TComPic*>* pcListPic = NULL;
  ifstream bitstreamFile(fileName, ifstream::in | ifstream::binary);
  if (!bitstreamFile)
  {
    fprintf(stderr, "\nfailed to open bitstream file `%s' for reading\n", fileName);
    exit(EXIT_FAILURE);
  }
  
  InputByteStream bytestream(bitstreamFile);
  InputByteStreamNoFile bytestreamLocal = *(new InputByteStreamNoFile());
  FILE* inputFile = fopen(fileName,"rb");
  igtl_uint8 *bitStream = new igtl_uint8[300000];
  fread(bitStream,1,300000,inputFile);
  bytestreamLocal.SetByteStream(bitStream, 300000);
  std::string   m_outputDecodedSEIMessagesFilename;
  std::ofstream                   m_seiMessageFileStream;
  if (!m_outputDecodedSEIMessagesFilename.empty() && m_outputDecodedSEIMessagesFilename!="-")
  {
    m_seiMessageFileStream.open(m_outputDecodedSEIMessagesFilename.c_str(), std::ios::out);
    if (!m_seiMessageFileStream.is_open() || !m_seiMessageFileStream.good())
    {
      fprintf(stderr, "\nUnable to open file `%s' for writing decoded SEI messages\n", m_outputDecodedSEIMessagesFilename.c_str());
      exit(EXIT_FAILURE);
    }
  }
  
  // create & initialize internal classes
  TDecTop   m_cTDecTop;
  m_cTDecTop.create();
  m_cTDecTop.init();
  m_cTDecTop.setDecodedPictureHashSEIEnabled(0);
  int m_iSkipFrame = 0;
  int m_iPOCLastDisplay = m_iSkipFrame;      // set the last displayed POC correctly for skip forward.
  int m_iMaxTemporalLayer = 1;
  // main decoder loop
  Bool loopFiltered = false;
  
  while (!!bitstreamFile)
  {
    /* location serves to work around a design fault in the decoder, whereby
     * the process of reading a new slice that is the first slice of a new frame
     * requires the TDecTop::decode() method to be called again with the same
     * nal unit. */
#if RExt__DECODER_DEBUG_BIT_STATISTICS
    TComCodingStatistics::TComCodingStatisticsData backupStats(TComCodingStatistics::GetStatistics());
    streampos location = bitstreamFile.tellg() - streampos(bytestream.GetNumBufferedBytes());
#else
    streampos location = bitstreamFile.tellg();
#endif
    AnnexBStats stats = AnnexBStats();
    
    InputNALUnit nalu;
    InputNALUnit nalu2;
    byteStreamNALUnit(bytestream, nalu.getBitstream().getFifo(), stats);
    GetNALUnitFromByteStream(bytestreamLocal, nalu2.getBitstream().getFifo());
    
    // call actual decoding function
    Bool bNewPicture = false;
    if (nalu.getBitstream().getFifo().empty())
    {
      /* this can happen if the following occur:
       *  - empty input file
       *  - two back-to-back start_code_prefixes
       *  - start_code_prefix immediately followed by EOF
       */
      fprintf(stderr, "Warning: Attempt to decode an empty NAL unit\n");
    }
    else
    {
      read(nalu);
      read(nalu2);
      std::cerr<<(int)bitstreamFile.tellg()-location<<" "<<nalu.getBitstream().getFifo().size()<<" "<<bytestreamLocal.GetPos()<<std::endl;
      if( (m_iMaxTemporalLayer >= 0 && nalu.m_temporalId > m_iMaxTemporalLayer) /*|| !isNaluWithinTargetDecLayerIdSet(&nalu)  */)
      {
        bNewPicture = false;
      }
      else
      {
        bNewPicture = m_cTDecTop.decode(nalu2, m_iSkipFrame, m_iPOCLastDisplay);
        if (bNewPicture)
        {
          streampos locationtest = bitstreamFile.tellg();
          bitstreamFile.clear();
          /* location points to the current nalunit payload[1] due to the
           * need for the annexB parser to read three extra bytes.
           * [1] except for the first NAL unit in the file
           *     (but bNewPicture doesn't happen then) */
#if RExt__DECODER_DEBUG_BIT_STATISTICS
          bitstreamFile.seekg(location);
          bytestream.reset();
          TComCodingStatistics::SetStatistics(backupStats);
#else
          bitstreamFile.seekg(location-streamoff(3));
          bytestreamLocal.SetPos((int)location-3);
          bytestreamLocal.resetFutureBytes();
          bytestream.reset();
#endif
        }
      }
    }
    
    if ( (bNewPicture || !bitstreamFile || nalu.m_nalUnitType == NAL_UNIT_EOS) &&
        !m_cTDecTop.getFirstSliceInSequence () )
    {
      if (!loopFiltered || bitstreamFile)
      {
        m_cTDecTop.executeLoopFilters(poc, pcListPic);
      }
      loopFiltered = (nalu.m_nalUnitType == NAL_UNIT_EOS);
      if (nalu.m_nalUnitType == NAL_UNIT_EOS)
      {
        m_cTDecTop.setFirstSliceInSequence(true);
      }
    }
    else if ( (bNewPicture || !bitstreamFile || nalu.m_nalUnitType == NAL_UNIT_EOS ) &&
             m_cTDecTop.getFirstSliceInSequence () )
    {
      m_cTDecTop.setFirstSliceInPicture (true);
    }
    
    if( pcListPic )
    {
      // write reconstruction to file
      if( bNewPicture )
      {
        //m_cTDecTop.xWriteOutput( pcListPic, nalu.m_temporalId );
      }
      if ( (bNewPicture || nalu.m_nalUnitType == NAL_UNIT_CODED_SLICE_CRA) && m_cTDecTop.getNoOutputPriorPicsFlag() )
      {
        m_cTDecTop.checkNoOutputPriorPics( pcListPic );
        m_cTDecTop.setNoOutputPriorPicsFlag (false);
      }
      if ( bNewPicture &&
          (   nalu.m_nalUnitType == NAL_UNIT_CODED_SLICE_IDR_W_RADL
           || nalu.m_nalUnitType == NAL_UNIT_CODED_SLICE_IDR_N_LP
           || nalu.m_nalUnitType == NAL_UNIT_CODED_SLICE_BLA_N_LP
           || nalu.m_nalUnitType == NAL_UNIT_CODED_SLICE_BLA_W_RADL
           || nalu.m_nalUnitType == NAL_UNIT_CODED_SLICE_BLA_W_LP ) )
      {
        //xFlushOutput( pcListPic );
      }
      if (nalu.m_nalUnitType == NAL_UNIT_EOS)
      {
        //xWriteOutput( pcListPic, nalu.m_temporalId );
        m_cTDecTop.setFirstSliceInPicture (false);
      }
      // write reconstruction to file -- for additional bumping as defined in C.5.2.3
      if(!bNewPicture && nalu.m_nalUnitType >= NAL_UNIT_CODED_SLICE_TRAIL_N && nalu.m_nalUnitType <= NAL_UNIT_RESERVED_VCL31)
      {
        //xWriteOutput( pcListPic, nalu.m_temporalId );
      }
    }
  }
  
  //xFlushOutput( pcListPic );
  // delete buffers
  m_cTDecTop.deletePicBuffer();
  
  // destroy internal classes
  m_cTDecTop.destroy();
}

int main(int argc, char *argv[]) {
  //init_main(argc, argv);
  std::string input_file = "/Users/longquanchen/Desktop/Github/Slicer-build/OpenIGTLink-XcodeBuild/bin/Debug/Test.265";
  video_decode_example(input_file.c_str());
  return 0;
}


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

void xWriteOutput( TComList<TComPic*>* pcListPic, UInt tId )
{
  if (pcListPic->empty())
  {
    return;
  }
  TComList<TComPic*>::iterator iterPic   = pcListPic->begin();
  int m_iPOCLastDisplay = -MAX_INT;
  int numPicsNotYetDisplayed = 0;
  Int dpbFullness = 0;
  const TComSPS* activeSPS = &(pcListPic->front()->getPicSym()->getSPS());
  UInt numReorderPicsHighestTid;
  UInt maxDecPicBufferingHighestTid;
  UInt maxNrSublayers = activeSPS->getMaxTLayers();
  numReorderPicsHighestTid = activeSPS->getNumReorderPics(maxNrSublayers-1);
  maxDecPicBufferingHighestTid =  activeSPS->getMaxDecPicBuffering(maxNrSublayers-1);
  
  while (iterPic != pcListPic->end())
  {
    TComPic* pcPic = *(iterPic);
    if(pcPic->getOutputMark() && pcPic->getPOC() > m_iPOCLastDisplay)
    {
      numPicsNotYetDisplayed++;
      dpbFullness++;
    }
    else if(pcPic->getSlice( 0 )->isReferenced())
    {
      dpbFullness++;
    }
    iterPic++;
  }
  
  iterPic = pcListPic->begin();
  
  if (numPicsNotYetDisplayed>2)
  {
    iterPic++;
  }
  
  TComPic* pcPic = *(iterPic);
  if (numPicsNotYetDisplayed>2 && pcPic->isField()) //Field Decoding
  {
    TComList<TComPic*>::iterator endPic   = pcListPic->end();
    endPic--;
    iterPic   = pcListPic->begin();
    while (iterPic != endPic)
    {
      TComPic* pcPicTop = *(iterPic);
      iterPic++;
      TComPic* pcPicBottom = *(iterPic);
      
      if ( pcPicTop->getOutputMark() && pcPicBottom->getOutputMark() &&
          (numPicsNotYetDisplayed >  numReorderPicsHighestTid || dpbFullness > maxDecPicBufferingHighestTid) &&
          (!(pcPicTop->getPOC()%2) && pcPicBottom->getPOC() == pcPicTop->getPOC()+1) &&
          (pcPicTop->getPOC() == m_iPOCLastDisplay+1 || m_iPOCLastDisplay < 0))
      {
        // write to file
        numPicsNotYetDisplayed = numPicsNotYetDisplayed-2;
        // update POC of display order
        m_iPOCLastDisplay = pcPicBottom->getPOC();
        
        // erase non-referenced picture in the reference picture list after display
        if ( !pcPicTop->getSlice(0)->isReferenced() && pcPicTop->getReconMark() == true )
        {
          pcPicTop->setReconMark(false);
          
          // mark it should be extended later
          pcPicTop->getPicYuvRec()->setBorderExtension( false );
        }
        if ( !pcPicBottom->getSlice(0)->isReferenced() && pcPicBottom->getReconMark() == true )
        {
          pcPicBottom->setReconMark(false);
          
          // mark it should be extended later
          pcPicBottom->getPicYuvRec()->setBorderExtension( false );
        }
        pcPicTop->setOutputMark(false);
        pcPicBottom->setOutputMark(false);
      }
    }
  }
  else if (!pcPic->isField()) //Frame Decoding
  {
    iterPic = pcListPic->begin();
    
    while (iterPic != pcListPic->end())
    {
      pcPic = *(iterPic);
      
      if(pcPic->getOutputMark() && pcPic->getPOC() > m_iPOCLastDisplay &&
         (numPicsNotYetDisplayed >  numReorderPicsHighestTid || dpbFullness > maxDecPicBufferingHighestTid))
      {
        // write to file
        numPicsNotYetDisplayed--;
        if(pcPic->getSlice(0)->isReferenced() == false)
        {
          dpbFullness--;
        }
        
        // update POC of display order
        m_iPOCLastDisplay = pcPic->getPOC();
        
        // erase non-referenced picture in the reference picture list after display
        if ( !pcPic->getSlice(0)->isReferenced() && pcPic->getReconMark() == true )
        {
          pcPic->setReconMark(false);
          
          // mark it should be extended later
          pcPic->getPicYuvRec()->setBorderExtension( false );
        }
        pcPic->setOutputMark(false);
      }
      
      iterPic++;
    }
  }
}

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
  //TComList<TComPic*>* pcListPic = NULL;
  ifstream bitstreamFile(fileName, ifstream::in | ifstream::binary);
  if (!bitstreamFile)
  {
    fprintf(stderr, "\nfailed to open bitstream file `%s' for reading\n", fileName);
    exit(EXIT_FAILURE);
  }
  
  InputByteStream bytestream(bitstreamFile);
  //InputByteStreamNoFile bytestreamLocal = *(new InputByteStreamNoFile());
  //FILE* inputFile = fopen(fileName,"rb");
  //igtl_uint8 *bitStream = new igtl_uint8[300000];
  //fread(bitStream,1,300000,inputFile);
  //bytestreamLocal.SetByteStream(bitStream, 300000);
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
  H265Decoder* decoder = new H265Decoder();
  int m_iSkipFrame = 0;
  int m_iPOCLastDisplay = -MAX_INT;      // set the last displayed POC correctly for skip forward.
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
    //GetNALUnitFromByteStream(bytestreamLocal, nalu2.getBitstream().getFifo());
    
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
      //read(nalu2);
      std::cerr<<(int)bitstreamFile.tellg()-location<<" "<<nalu.getBitstream().getFifo().size()<<std::endl;
      if( (m_iMaxTemporalLayer >= 0 && nalu.m_temporalId > m_iMaxTemporalLayer) /*|| !isNaluWithinTargetDecLayerIdSet(&nalu)  */)
      {
        bNewPicture = false;
      }
      else
      {
        //bNewPicture = m_cTDecTop.decode(nalu2, m_iSkipFrame, m_iPOCLastDisplay);
        bNewPicture = decoder->pDecoder->decode(nalu, m_iSkipFrame, m_iPOCLastDisplay);
        if (bNewPicture)
        {
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
          //bytestreamLocal.SetPos((int)location-3);
          //bytestreamLocal.resetFutureBytes();
          bytestream.reset();
#endif
        }
      }
    }
    
    if ( (bNewPicture || !bitstreamFile || nalu.m_nalUnitType == NAL_UNIT_EOS) &&
        !decoder->pDecoder->getFirstSliceInSequence () )
    {
      if (!loopFiltered || bitstreamFile)
      {
        decoder->pDecoder->executeLoopFilters(poc, decoder->pcListPic);
      }
    }
    
    if( decoder->pcListPic )
    {
      // write reconstruction to file
      if( bNewPicture )
      {
        decoder->xWriteOutput( decoder->pcListPic, nalu.m_temporalId );
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


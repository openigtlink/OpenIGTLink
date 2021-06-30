/*=========================================================================

  Program:   OpenIGTLink -- Example for Label Meta Data Client
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <math.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "igtlOSUtil.h"
#include "igtlImageMessage.h"
#include "igtlLabelMetaMessage.h"
#include "igtlClientSocket.h"


int ReceiveLabelMeta(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header);


int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 3) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <hostname> <port>"    << std::endl;
    std::cerr << "    <hostname> : IP or host name"                    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    exit(0);
    }

  char*  hostname = argv[1];
  int    port     = atoi(argv[2]);

  //------------------------------------------------------------
  // Establish Connection
  igtl::ClientSocket::Pointer socket;
  socket = igtl::ClientSocket::New();
  int r = socket->ConnectToServer(hostname, port);

  if (r != 0)
    {
    std::cerr << "Cannot connect to the server." << std::endl;
    exit(0);
    }

  //------------------------------------------------------------
  // loop
  for (int i = 0; i < 10; i ++)
    {
    //------------------------------------------------------------
    // Send request data
    igtl::GetLabelMetaMessage::Pointer getLabelMetaMsg;
    getLabelMetaMsg = igtl::GetLabelMetaMessage::New();
    getLabelMetaMsg->SetDeviceName("Client");
    getLabelMetaMsg->Pack();
    socket->Send(getLabelMetaMsg->GetPackPointer(), getLabelMetaMsg->GetPackSize());
    
    //------------------------------------------------------------
    // Wait for a reply
    igtl::MessageHeader::Pointer headerMsg;
    headerMsg = igtl::MessageHeader::New();
    headerMsg->InitPack();
    bool timeout(false);
    igtlUint64 rs = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize(), timeout);
    if (rs == 0)
      {
      std::cerr << "Connection closed." << std::endl;
      socket->CloseSocket();
      exit(0);
      }
    if (rs != headerMsg->GetPackSize())
      {
      std::cerr << "Message size information and actual data size don't match." << std::endl; 
      exit(0);
      }

    headerMsg->Unpack();
    if (strcmp(headerMsg->GetDeviceType(), "LBMETA") == 0)
      {
      ReceiveLabelMeta(socket, headerMsg);
      }
    else
      {
      std::cerr << "Invalid response from the server:" << headerMsg->GetDeviceName() << std::endl; 
      exit(0);
      }

    igtl::Sleep(500); // wait
    }
    
  //------------------------------------------------------------
  // Close connection
  socket->CloseSocket();

}


int ReceiveLabelMeta(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{

  std::cerr << "Receiving LBMETA data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::LabelMetaMessage::Pointer lbMeta;
  lbMeta = igtl::LabelMetaMessage::New();
  lbMeta->SetMessageHeader(header);
  lbMeta->AllocatePack();
  
  // Receive transform data from the socket
  bool timeout(false);
  socket->Receive(lbMeta->GetPackBodyPointer(), lbMeta->GetPackBodySize(), timeout);
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = lbMeta->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    int nElements = lbMeta->GetNumberOfLabelMetaElement();
    for (int i = 0; i < nElements; i ++)
      {
      igtl::LabelMetaElement::Pointer lbMetaElement;
      lbMeta->GetLabelMetaElement(i, lbMetaElement);

      igtlUint8 rgba[4];
      lbMetaElement->GetRGBA(rgba);

      igtlUint16 size[3];
      lbMetaElement->GetSize(size);

      std::cerr << "========== Element #" << i << " ==========" << std::endl;
      std::cerr << " Name       : " << lbMetaElement->GetName() << std::endl;
      std::cerr << " DeviceName : " << lbMetaElement->GetDeviceName() << std::endl;
      std::cerr << " Label      : " << (int) lbMetaElement->GetLabel() << std::endl;
      std::cerr << " RGBA       : ( " << (int)rgba[0] << ", " << (int)rgba[1] << ", " << (int)rgba[2] << ", " << (int)rgba[3] << " )" << std::endl;
      std::cerr << " Size       : ( " << size[0] << ", " << size[1] << ", " << size[2] << ")" << std::endl;
      std::cerr << " Owner      : " << lbMetaElement->GetOwner() << std::endl;
      std::cerr << "================================" << std::endl;
      }
    return 1;
    }

  return 0;

}




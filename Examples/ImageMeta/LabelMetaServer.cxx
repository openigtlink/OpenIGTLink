/*=========================================================================

  Program:   OpenIGTLink -- Example for Label Meta Data Server
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include <iostream>
#include <math.h>
#include <cstdlib>
#include <cstring>

#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlImageMessage.h"
#include "igtlServerSocket.h"
#include "igtlLabelMetaMessage.h"


int SendLabelMeta(igtl::Socket::Pointer& socket);

int main(int argc, char* argv[])
{

  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 2) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <port>"    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    exit(0);
    }

  int    port     = atoi(argv[1]);

  igtl::ServerSocket::Pointer serverSocket;
  serverSocket = igtl::ServerSocket::New();
  int r = serverSocket->CreateServer(port);

  if (r < 0)
    {
    std::cerr << "Cannot create a server socket." << std::endl;
    exit(0);
    }

  igtl::Socket::Pointer socket;
  
  while (1)
    {
    //------------------------------------------------------------
    // Waiting for Connection
    socket = serverSocket->WaitForConnection(1000);
    
    if (socket.IsNotNull()) // if client connected
      {
      std::cerr << "A client is connected." << std::endl;

      // Create a message buffer to receive header
      igtl::MessageHeader::Pointer headerMsg;
      headerMsg = igtl::MessageHeader::New();

      //------------------------------------------------------------
      // loop
      for (int i = 0; i < 100; i ++)
        {

        // Initialize receive buffer
        headerMsg->InitPack();

        // Receive generic header from the socket
        int rs = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
        if (rs == 0)
          {
          socket->CloseSocket();
          }
        if (rs != headerMsg->GetPackSize())
          {
          continue;
          }

        // Deserialize the header
        headerMsg->Unpack();

        // Check data type and receive data body
        if (strcmp(headerMsg->GetDeviceType(), "GET_LBMETA") == 0)
          {
          std::cerr << "Received a GET_LBMETA message." << std::endl;
          //socket->Skip(headerMsg->GetBodySizeToRead(), 0);
          SendLabelMeta(socket);
          }
        else
          {
          // if the data type is unknown, skip reading.
          std::cerr << "Receiving : " << headerMsg->GetDeviceType() << std::endl;
          socket->Skip(headerMsg->GetBodySizeToRead(), 0);
          }
        }
      }
    }
    
  //------------------------------------------------------------
  // Close connection (The example code never reaches to this section ...)
  
  socket->CloseSocket();

}


int SendLabelMeta(igtl::Socket::Pointer& socket)
{

  //------------------------------------------------------------
  // Allocate Status Message Class

  igtl::LabelMetaMessage::Pointer lbMetaMsg;
  lbMetaMsg = igtl::LabelMetaMessage::New();
  lbMetaMsg->SetDeviceName("MetaServer");

  //---------------------------
  // Create 1st meta data
  igtl::LabelMetaElement::Pointer lbMeta0;
  lbMeta0 = igtl::LabelMetaElement::New();
  lbMeta0->SetName("LABEL_DESCRIPTION_0");
  lbMeta0->SetDeviceName("LABEL_0");
  lbMeta0->SetLabel(1);
  lbMeta0->SetRGBA(0xFF, 0x00, 0x00, 0xFF);
  lbMeta0->SetSize(512, 512, 64);
  lbMeta0->SetOwner("IMAGE_0");

  //---------------------------
  // Create 2nd meta data
  igtl::LabelMetaElement::Pointer lbMeta1;
  lbMeta1 = igtl::LabelMetaElement::New();
  lbMeta1->SetName("LABEL_DESCRIPTION_1");
  lbMeta1->SetDeviceName("LABEL_1");
  lbMeta1->SetLabel(2);
  lbMeta1->SetRGBA(0x00, 0xFF, 0, 0xFF);
  lbMeta1->SetSize(256, 128, 32);
  lbMeta1->SetOwner("IMAGE_1");

  //---------------------------
  // Create 3rd meta data
  igtl::LabelMetaElement::Pointer lbMeta2;
  lbMeta2 = igtl::LabelMetaElement::New();
  lbMeta2->SetName("LABEL_DESCRIPTION_2");
  lbMeta2->SetDeviceName("LABEL_2");
  lbMeta2->SetLabel(3);
  lbMeta2->SetRGBA(0, 0, 0xFF, 0xFF);
  lbMeta2->SetSize(256, 256, 32);
  lbMeta2->SetOwner("IMAGE_2");

  lbMetaMsg->AddLabelMetaElement(lbMeta0);
  lbMetaMsg->AddLabelMetaElement(lbMeta1);
  lbMetaMsg->AddLabelMetaElement(lbMeta2);

  lbMetaMsg->Pack();
  std::cerr << "Size of pack: " << lbMetaMsg->GetPackSize() << std::endl;
  std::cerr << "Name of type: " << lbMetaMsg->GetDeviceType() << std::endl;
  std::cerr << "Sending a LBMETA message..." << std::endl;

  socket->Send(lbMetaMsg->GetPackPointer(), lbMetaMsg->GetPackSize());

  return 1;
}









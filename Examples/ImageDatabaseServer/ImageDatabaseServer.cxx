/*=========================================================================

  Program:   OpenIGTLink -- Example for Image Meta Data Server
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include <iostream>
#include <math.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlImageMessage.h"
#include "igtlImageMetaMessage.h"
#include "igtlLabelMetaMessage.h"
#include "igtlServerSocket.h"

int SendImageMeta(igtl::Socket::Pointer& socket, const char* name);
int SendLabelMeta(igtl::Socket::Pointer& socket, const char* name);
int SendImage(igtl::Socket::Pointer& socket, const char* name, const char* filedir);
int SendLabel(igtl::Socket::Pointer& socket, const char* name, const char* filedir);
int GetTestImage(igtl::ImageMessage::Pointer& msg, const char* dir, int i);

int main(int argc, char* argv[])
{

  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 3) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <port>"    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    std::cerr << "    <imgdir>   : file directory, where \"igtlTestImage[1-3].raw\" are placed." << std::endl;
    exit(0);
    }

  int    port     = atoi(argv[1]);
  char*  filedir  = argv[2];

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
        bool timeout(false);
        // Receive generic header from the socket
        int rs = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize(), timeout);
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

        std::cerr << "Receiving a message: " << std::endl;
        std::cerr << "    Device Type: \"" << headerMsg->GetDeviceType() << "\"" << std::endl;
        std::cerr << "    Device Name: \"" << headerMsg->GetDeviceName() << "\"" << std::endl;

        // Check data type and receive data body
        if (strcmp(headerMsg->GetDeviceType(), "GET_IMGMETA") == 0)
          {
          //socket->Skip(headerMsg->GetBodySizeToRead(), 0);
          SendImageMeta(socket, headerMsg->GetDeviceName());
          }
        else if (strcmp(headerMsg->GetDeviceType(), "GET_LBMETA") == 0)
          {
          SendLabelMeta(socket, headerMsg->GetDeviceName());
          }
        else if (strcmp(headerMsg->GetDeviceType(), "GET_IMAGE") == 0)
          {
          SendImage(socket, headerMsg->GetDeviceName(), filedir);
          }
        else
          {
          // if the data type is unknown, skip reading.
          socket->Skip(headerMsg->GetBodySizeToRead(), 0);
          }
        }
      }
    }
    
  //------------------------------------------------------------
  // Close connection (The example code never reaches to this section ...)
  
  socket->CloseSocket();

}


int SendImageMeta(igtl::Socket::Pointer& socket, const char* name)
{
  //------------------------------------------------------------
  // Allocate Status Message Class

  igtl::ImageMetaMessage::Pointer imgMetaMsg;
  imgMetaMsg = igtl::ImageMetaMessage::New();
  // NOTE: the server should send a message with the same device name
  // as the received query message.
  imgMetaMsg->SetDeviceName(name);

  //---------------------------
  // Create 1st meta data
  igtl::ImageMetaElement::Pointer imgMeta0;
  imgMeta0 = igtl::ImageMetaElement::New();
  imgMeta0->SetName("IMAGE_DESCRIPTION_0");
  imgMeta0->SetDeviceName("IMAGE_0");
  imgMeta0->SetModality("CT");
  imgMeta0->SetPatientName("PATIENT_0");
  imgMeta0->SetPatientID("PATIENT_ID_0");
  
  igtl::TimeStamp::Pointer ts0;
  ts0 = igtl::TimeStamp::New();
  ts0->SetTime(1291739370.2345);

  imgMeta0->SetTimeStamp(ts0);
  imgMeta0->SetSize(512, 512, 64);
  imgMeta0->SetScalarType(igtl::ImageMessage::TYPE_UINT16);

  //---------------------------
  // Create 2nd meta data
  igtl::ImageMetaElement::Pointer imgMeta1;
  imgMeta1 = igtl::ImageMetaElement::New();
  imgMeta1->SetName("IMAGE_DESCRIPTION_1");
  imgMeta1->SetDeviceName("IMAGE_1");
  imgMeta1->SetModality("MRI");
  imgMeta1->SetPatientName("PATIENT_1");
  imgMeta1->SetPatientID("PATIENT_ID_1");
  
  igtl::TimeStamp::Pointer ts1;
  ts1 = igtl::TimeStamp::New();
  ts1->SetTime(1291739380.3456);

  imgMeta1->SetTimeStamp(ts1);
  imgMeta1->SetSize(256, 128, 32);
  imgMeta1->SetScalarType(igtl::ImageMessage::TYPE_UINT16);

  //---------------------------
  // Create 3rd meta data
  igtl::ImageMetaElement::Pointer imgMeta2;
  imgMeta2 = igtl::ImageMetaElement::New();
  imgMeta2->SetName("IMAGE_DESCRIPTION_2");
  imgMeta2->SetDeviceName("IMAGE_2");
  imgMeta2->SetModality("PET");
  imgMeta2->SetPatientName("PATIENT_2");
  imgMeta2->SetPatientID("PATIENT_ID_2");
  
  igtl::TimeStamp::Pointer ts2;
  ts2 = igtl::TimeStamp::New();
  ts2->SetTime(1291739390.4567);

  imgMeta2->SetTimeStamp(ts2);
  imgMeta2->SetSize(256, 256, 32);
  imgMeta2->SetScalarType(igtl::ImageMessage::TYPE_UINT16);

  imgMetaMsg->AddImageMetaElement(imgMeta0);
  imgMetaMsg->AddImageMetaElement(imgMeta1);
  imgMetaMsg->AddImageMetaElement(imgMeta2);

  imgMetaMsg->Pack();
  std::cerr << "Size of pack: " << imgMetaMsg->GetPackSize() << std::endl;
  std::cerr << "Name of type: " << imgMetaMsg->GetDeviceType() << std::endl;
  std::cerr << "Sending a IMGMETA message..." << std::endl;

  socket->Send(imgMetaMsg->GetPackPointer(), imgMetaMsg->GetPackSize());

  return 1;

}


int SendLabelMeta(igtl::Socket::Pointer& socket, const char* name)
{
  //------------------------------------------------------------
  // Allocate Status Message Class

  igtl::LabelMetaMessage::Pointer lbMetaMsg;
  lbMetaMsg = igtl::LabelMetaMessage::New();
  // NOTE: the server should send a message with the same device name
  // as the received query message.
  lbMetaMsg->SetDeviceName(name);

  //---------------------------
  // Create 1st meta data
  igtl::LabelMetaElement::Pointer lbMeta0;
  lbMeta0 = igtl::LabelMetaElement::New();
  lbMeta0->SetName("LABEL_DESCRIPTION_0");
  lbMeta0->SetDeviceName("LABEL_0");
  lbMeta0->SetOwner("IMAGE_0");
  lbMeta0->SetSize(512, 512, 64);

  //---------------------------
  // Create 2nd meta data
  igtl::LabelMetaElement::Pointer lbMeta1;
  lbMeta1 = igtl::LabelMetaElement::New();
  lbMeta1->SetName("LABEL_DESCRIPTION_1");
  lbMeta1->SetDeviceName("LABEL_1");
  lbMeta1->SetOwner("IMAGE_1");
  
  lbMeta1->SetSize(256, 128, 32);

  //---------------------------
  // Create 3rd meta data
  igtl::LabelMetaElement::Pointer lbMeta2;
  lbMeta2 = igtl::LabelMetaElement::New();
  lbMeta2->SetName("LABEL_DESCRIPTION_2");
  lbMeta2->SetDeviceName("LABEL_2");
  lbMeta2->SetOwner("IMAGE_2");
  lbMeta2->SetSize(256, 256, 32);

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



int SendImage(igtl::Socket::Pointer& socket, const char* name, const char* filedir)
{
  int index = 0;

  if (strcmp(name, "IMAGE_0") == 0)
    {
    index = 1;
    }
  else if (strcmp(name, "IMAGE_1") == 0)
    {
    index = 2;
    }
  else if (strcmp(name, "IMAGE_2") == 0)
    {
    index = 3;
    }
  if (strcmp(name, "LABEL_0") == 0)
    {
    index = 4;
    }
  else if (strcmp(name, "LABEL_1") == 0)
    {
    index = 5;
    }
  else if (strcmp(name, "LABEL_2") == 0)
    {
    index = 6;
    }

  if (index > 0)
    {
    int   size[]     = {256, 256, 1};       // image dimension
    float spacing[]  = {1.0, 1.0, 5.0};     // spacing (mm/pixel)
    int   svsize[]   = {256, 256, 1};       // sub-volume size
    int   svoffset[] = {0, 0, 0};           // sub-volume offset
    int   scalarType = igtl::ImageMessage::TYPE_UINT8;// scalar type
    
    igtl::ImageMessage::Pointer imgMsg = igtl::ImageMessage::New();
    imgMsg->SetDimensions(size);
    imgMsg->SetSpacing(spacing);
    imgMsg->SetScalarType(scalarType);
    imgMsg->SetDeviceName(name);
    imgMsg->SetSubVolume(svsize, svoffset);
    imgMsg->AllocateScalars();
    
    // Following line may be called in case of 16-, 32-, and 64-bit scalar types.
    // imgMsg->SetEndian(igtl::ImageMessage::ENDIAN_BIG);

    //------------------------------------------------------------
    // Set image data (See GetTestImage() bellow for the details)
    GetTestImage(imgMsg, filedir, index);
    
    igtl::Matrix4x4 matrix;
    igtl::IdentityMatrix(matrix);
    imgMsg->SetMatrix(matrix);
    
    //------------------------------------------------------------
    // Pack (serialize) and send
    imgMsg->Pack();
    socket->Send(imgMsg->GetPackPointer(), imgMsg->GetPackSize());
    }
  else
    {
    //------------------------------------------------------------
    // Return RTS_IMAGE message with error code

    // TODO

    }

  return 1;
}


//------------------------------------------------------------
// Function to read test image data
int GetTestImage(igtl::ImageMessage::Pointer& msg, const char* dir, int i)
{

  //------------------------------------------------------------
  // Check if image index is in the range
  if (i < 0 || i >= 7) 
    {
    std::cerr << "Image index is invalid." << std::endl;
    return 0;
    }

  //------------------------------------------------------------
  // Generate path to the raw image file
  char filename[128];
  sprintf(filename, "%s/igtlTestImage%d.raw", dir, i+1);
  std::cerr << "Reading " << filename << "...";

  //------------------------------------------------------------
  // Load raw data from the file
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL)
    {
    std::cerr << "File opeining error: " << filename << std::endl;
    return 0;
    }
  int fsize = msg->GetImageSize();
  //size_t b = fread(msg->GetScalarPointer(), 1, fsize, fp);
  fread(msg->GetScalarPointer(), 1, fsize, fp);

  fclose(fp);

  std::cerr << "done." << std::endl;

  return 1;
}





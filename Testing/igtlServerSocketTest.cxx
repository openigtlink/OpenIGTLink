/*=========================================================================

  Program:   OpenIGTLink Library
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

  =========================================================================*/

#include "igtlServerSocket.h"
#include "igtlImageMessage2.h"
#include "igtlutil/igtl_test_data_image.h"
#include "igtlClientSocket.h"
#include "igtlMultiThreader.h"
#include "igtl_types.h"
#include "igtl_header.h"
#include "igtl_image.h"
#include "igtl_util.h"
#include "igtlOSUtil.h"
#include "igtlTestConfig.h"
#include "string.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Invoke;
using igtl::ServerSocket;
using ::testing::IsNull;

class ServerSocketMock {
public:
  //typedef igtl::SmartPointer<ServerSocketMock>  Pointer;
  //ServerSocketMock(){};
  ServerSocketMock(ServerSocket::Pointer pointer) {
    real_ = pointer;
    // By default, all calls are delegated to the real object.
    ON_CALL(*this, CreateServer(_))
      .WillByDefault(Invoke(real_.GetPointer(), &ServerSocket::CreateServer));
    ON_CALL(*this, GetServerPort())
      .WillByDefault(Invoke(real_.GetPointer(), &ServerSocket::GetServerPort));
    ON_CALL(*this, WaitForConnection(_))
      .WillByDefault(Invoke(real_.GetPointer(), &ServerSocket::WaitForConnection));
    ON_CALL(*this, Send(_,_))
      .WillByDefault(Invoke(real_.GetPointer(), &ServerSocket::Send));
  }
  ~ServerSocketMock(){real_.~SmartPointer();};
  ServerSocket::Pointer getPointer(){return real_;};
  void setPointer(ServerSocket::Pointer server){real_ = server;};
  MOCK_METHOD0(GetServerPort, int());
  MOCK_METHOD1(CreateServer, int(int port));
  MOCK_METHOD1(WaitForConnection, igtl::ClientSocket::Pointer(unsigned long msec));
  MOCK_METHOD2(Send, int(const void* data, int length));
private:
  ServerSocket::Pointer real_;
  
};
void* ThreadFunction(void* ptr);
int ReceiveImageData(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header);
class StopImageSendingMessage: public igtl::MessageBase
{
public:
  igtlTypeMacro(StopImageSendingMessage, igtl::MessageBase);
  igtlNewMacro(StopImageSendingMessage);
  
protected:
  StopImageSendingMessage() : MessageBase() { this->m_SendMessageType  = "STP_IMAGE"; };
  ~StopImageSendingMessage() {};
  
protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };
  
};

class StartImageSendingMessage: public igtl::MessageBase
{
public:
  typedef StartImageSendingMessage  Self;
  typedef MessageBase                        Superclass;
  typedef igtl::SmartPointer<Self>                 Pointer;
  typedef igtl::SmartPointer<const Self>           ConstPointer;
  
  igtlTypeMacro(StartImageSendingMessage, igtl::MessageBase);
  igtlNewMacro(StartImageSendingMessage);
  
protected:
  StartImageSendingMessage() : MessageBase() { this->m_SendMessageType  = "STT_IMAGE"; };
  ~StartImageSendingMessage() {};
  
protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };
  
};


igtl::ImageMessage2::Pointer imageMessage2Test = igtl::ImageMessage2::New();
long interval = 200;
int threadID;
int port = 18944;
int   size[]     = {50, 50, 1};       // image dimension
float spacing[]  = {1.0, 1.0, 5.0};     // spacing (mm/pixel)
int   svsize[]   = {50, 50, 1};       // sub-volume size
int   svoffset[] = {0, 0, 0};           // sub-volume offset
int   scalarType = igtl::ImageMessage2::TYPE_UINT8;// scalar type
void setupTest()
{
  //Initialization of a image message
  imageMessage2Test->SetDimensions(size);
  imageMessage2Test->SetSpacing(spacing);
  imageMessage2Test->SetScalarType(scalarType);
  imageMessage2Test->SetDeviceName("Image");
  imageMessage2Test->SetSubVolume(svsize, svoffset);
  imageMessage2Test->AllocateBuffer(IGTL_IMAGE_HEADER_SIZE+TEST_IMAGE_MESSAGE_SIZE);
  imageMessage2Test->AllocateScalars();
  memcpy((void*)imageMessage2Test->GetPackBodyPointer(), test_image_message+IGTL_HEADER_SIZE, IGTL_IMAGE_HEADER_SIZE+TEST_IMAGE_MESSAGE_SIZE);//here m_Body is set.
  imageMessage2Test->SetScalarPointer((void*)test_image);
  imageMessage2Test->Pack();
}

TEST(ServerSocketTest, ConnectionAndSending)
{
  igtl::ServerSocket::Pointer tempServerSocket = igtl::ServerSocket::New();
  ServerSocketMock mockServerSocket(tempServerSocket);
  EXPECT_CALL(mockServerSocket, CreateServer(port)).Times(1);
  int waitingTime = 3000;
  //EXPECT_CALL(*mockServerSocket, GetServerPort()).Times(1);
  //EXPECT_CALL(*mockServerSocket, WaitForConnection(_)).Times(2);
  //EXPECT_CALL(*mockServerSocket, Send(_,_)).Times(0);
  //mockServerSocket.GetServerPort();
  int bCreation = mockServerSocket.CreateServer(port);
  EXPECT_EQ(bCreation,0);
  if(1==0)
    {
    igtl::ClientSocket::Pointer returnSocket = mockServerSocket.WaitForConnection(waitingTime);
    /**Connecting to a established server, however, time out happened, return NULL */
    EXPECT_TRUE(returnSocket.IsNull());
    //EXPECT_EQ(mockServerSocket->CreateServer(port),-1);
    //-----------------------
    //Use multi threader to create a subprocess, which connects to the server.
    igtl::MultiThreader::Pointer threader = igtl::MultiThreader::New();
    igtl::MutexLock::Pointer glock = igtl::MutexLock::New();
    int loop = 0;
    threadID  = threader->SpawnThread((igtl::ThreadFunctionType) &ThreadFunction, &mockServerSocket);
    igtl::ClientSocket::Pointer clientSocket;
    clientSocket = igtl::ClientSocket::New();
    bool notConnected = true;
    while(notConnected)
      {
      int r = clientSocket->ConnectToServer("localhost", port);
      if(r!=0)
        {
        continue;
        }
      else
        {
        notConnected=false;
        }
      }
    StartImageSendingMessage::Pointer StartImageSending;
    StartImageSending = StartImageSendingMessage::New();
    StartImageSending->Pack();
    clientSocket->Send(StartImageSending->GetPackPointer(), StartImageSending->GetPackSize());

    while(1)
      {
      igtl::MessageHeader::Pointer headerMsg;
      headerMsg = igtl::MessageHeader::New();
      headerMsg->InitPack();
      int rs = clientSocket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
      if (rs == 0)
        {
        std::cerr << "Connection closed." << std::endl;
        clientSocket->CloseSocket();
        exit(0);
        }
      if (rs != headerMsg->GetPackSize())
        {
        continue;
        }
      
      headerMsg->Unpack();
      if (strcmp(headerMsg->GetDeviceName(),"Image")== 0)
        {
        ReceiveImageData(clientSocket, headerMsg);
        }
      else
        {
        EXPECT_TRUE(false)<< "Receiving : " << headerMsg->GetDeviceName() << std::endl;
        clientSocket->Skip(headerMsg->GetBodySizeToRead(), 0);
        }
      if (++loop >= 10) // if received 10 times
        {
        StopImageSendingMessage::Pointer StopImageSending;
        StopImageSending = StopImageSendingMessage::New();
        StopImageSending->Pack();
        clientSocket->Send(StopImageSending->GetPackPointer(), StopImageSending->GetPackSize());
        threadID = -1;
        break;
        //----------------------------------------
        }
      }
    }
  else
    {
    mockServerSocket.getPointer()->CloseSocket();
    }
  //delete mockServerSocket;
  //clientSocket::send
}


int ReceiveImageData(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{
  std::cerr << "Receiving QTDATA data type." << std::endl;
  
  igtl::ImageMessage2::Pointer imageData;
  imageData = igtl::ImageMessage2::New();
  imageData->SetDimensions(size);
  imageData->SetSpacing(spacing);
  imageData->SetScalarType(scalarType);
  imageData->SetSubVolume(svsize, svoffset);
  imageData->AllocateBuffer(IGTL_IMAGE_HEADER_SIZE+TEST_IMAGE_MESSAGE_SIZE);
  imageData->AllocateScalars();
  // Receive imageHeader from the socket
  socket->Receive(imageData->GetPackBodyPointer(), imageData->GetPackBodySize());
  
  // Deserialize position and quaternion (orientation) data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = imageData->Unpack();
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    std::cout << "================================" << std::endl;
    std::cout << " Name       : " << imageData->GetDeviceType();
    std::cout << "================================" << std::endl;
    }
  return 1;
}


void* ThreadFunction(void* ptr)
{
  //------------------------------------------------------------
  // Get the server socket
  igtl::MultiThreader::ThreadInfo* info =
    static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);
  ServerSocketMock* mockServerSocket = static_cast<ServerSocketMock*>(info->UserData);
  
  //------------------------------------------------------------
  // Get user data
  std::cout << "Interval = " << interval << " (ms)" << std::endl;
  igtl::MutexLock::Pointer glock = igtl::MutexLock::New();
  //------------------------------------------------------------
  // Loop
  igtl::Socket::Pointer socket;
  igtl::MultiThreader::Pointer threader = igtl::MultiThreader::New();
  socket = mockServerSocket->WaitForConnection(10000);
  if (socket.IsNotNull())
    {
    igtl::MessageHeader::Pointer headerMsg;
    headerMsg = igtl::MessageHeader::New();
    for (;;)
      {
      headerMsg->InitPack();
      int rs = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
      if (rs == 0)
        {
        EXPECT_TRUE(false) << "Disconnecting the client.";
        socket->CloseSocket();
        break;
        }
      if (rs != headerMsg->GetPackSize())
        {
        continue;
        }
      
      // Deserialize the header
      headerMsg->Unpack();
      if (strcmp(headerMsg->GetDeviceType(), "StartSending") == 0)
        {
        while(!(threadID==-1))
          {
          glock->Lock();
          for (int i = 0; i < imageMessage2Test->GetNumberOfPackFragments(); i ++)
            {
            socket->Send(imageMessage2Test->GetPackFragmentPointer(i), imageMessage2Test->GetPackFragmentSize(i));
            }
          glock->Unlock();
          igtl::Sleep(interval);
          }
        }
      else if (strcmp(headerMsg->GetDeviceType(), "StopSending") == 0)
        {
        std::cerr << "Received a stop message." << std::endl;
        socket->CloseSocket();
        break;
        }
      }
    }
  else
    {
    EXPECT_TRUE(false) << "No client connected.";
    }
  delete mockServerSocket;
  return NULL;
}

int main(int argc, char **argv)
{
  setupTest();
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


/*=========================================================================
 
  Program:   OpenIGTLink Library
  Language:  C++
 
  Copyright (c) Insight Software Consortium. All rights reserved.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.
 
  =========================================================================*/

#include "igtlServerSocket.h"
#include "igtlSocket.h"
#include "igtlMessageHeader.h"
#include "igtlClientSocket.h"
#include "igtlMultiThreader.h"
#include "igtlQuaternionTrackingDataMessage.h"

#include "igtl_types.h"
#include "igtl_header.h"
#include "igtl_util.h"
#include "igtlOSUtil.h"
#include "igtlTestConfig.h"
#include "string.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Invoke;
using igtl::ServerSocket;
using igtl::ClientSocket;
using igtl::Socket;
using ::testing::IsNull;


class SocketMock {
public:
  SocketMock(Socket::Pointer pointer) {
    real_ = pointer;
    // By default, all calls are delegated to the real object.
    ON_CALL(*this, GetConnected())
      .WillByDefault(Invoke(real_.GetPointer(), &Socket::GetConnected));
    ON_CALL(*this, Send(_,_))
      .WillByDefault(Invoke(real_.GetPointer(), &Socket::Send));
    ON_CALL(*this, Receive(_,_,_))
      .WillByDefault(Invoke(real_.GetPointer(), &Socket::Receive));
    ON_CALL(*this, SetTimeout(_))
      .WillByDefault(Invoke(real_.GetPointer(), &Socket::SetTimeout));
    ON_CALL(*this, SetReceiveTimeout(_))
      .WillByDefault(Invoke(real_.GetPointer(), &Socket::SetReceiveTimeout));
    ON_CALL(*this, SetSendTimeout(_))
      .WillByDefault(Invoke(real_.GetPointer(), &Socket::SetSendTimeout));
    ON_CALL(*this, SetReceiveBlocking(_))
      .WillByDefault(Invoke(real_.GetPointer(), &Socket::SetReceiveBlocking));
    ON_CALL(*this, SetSendBlocking(_))
      .WillByDefault(Invoke(real_.GetPointer(), &Socket::SetSendBlocking));
    ON_CALL(*this, GetSocketAddressAndPort(_,_))
      .WillByDefault(Invoke(real_.GetPointer(), &Socket::GetSocketAddressAndPort));
    ON_CALL(*this, Skip(_,_))
      .WillByDefault(Invoke(real_.GetPointer(), &Socket::Skip));
  }
  ~SocketMock(){real_.~SmartPointer();};
  Socket::Pointer getPointer(){return real_;};
  
  MOCK_METHOD0(GetConnected, int());
  MOCK_METHOD1(SetTimeout, int(int timeout));
  MOCK_METHOD1(SetReceiveTimeout, int(int timeout));
  MOCK_METHOD1(SetSendTimeout, int(int timeout));
  MOCK_METHOD1(SetReceiveBlocking, int(int sw));
  MOCK_METHOD1(SetSendBlocking, int(int sw));
  MOCK_METHOD2(Send, int(const void* data, int length));
  MOCK_METHOD2(GetSocketAddressAndPort, int(std::string& address, int& port));
  MOCK_METHOD2(Skip, int(int length, int skipFully));
  MOCK_METHOD3(Receive, int(void* data, int length, int readFully));
  
private:
  Socket::Pointer real_;
  
};
void* TestThreadFunction(void* ptr);
int ReceiveQuaternionTrackingData(ClientSocket::Pointer socket, igtl::MessageHeader::Pointer& header);
void clientServerProcess();

long interval = 200;
int threadID;
int port = 18944;
bool threadrunning = false;
bool msgReceived = true;
TEST(SocketTest, GeneralTest)
{
  igtl::MultiThreader::Pointer threader = igtl::MultiThreader::New();
  threadID  = threader->SpawnThread((igtl::ThreadFunctionType) &TestThreadFunction, NULL);
  clientServerProcess();
  while(threadrunning)
    {
    igtl::Sleep(interval);
    };
}

void* TestThreadFunction(void* ptr)
{
  threadrunning = true;
  igtl::MutexLock::Pointer glock = igtl::MutexLock::New();
  int waitingTime = 3000;
  ServerSocket::Pointer serverSocket = ServerSocket::New();
  serverSocket->CreateServer(port);
  SocketMock socketMock(static_cast<Socket::Pointer>(serverSocket->WaitForConnection(10*waitingTime)));
  EXPECT_CALL(socketMock, GetConnected()).Times(1);
  //The send command is called 10 times, it keep on sending at an interval(200ms) until a stop command is received.
  EXPECT_CALL(socketMock, Send(_,_)).Times(10);
  // Receive function should be called three times, the start command header, the start command body, and the stop command
  EXPECT_CALL(socketMock, Receive(_,_,_)).Times(3);
  EXPECT_CALL(socketMock, SetTimeout(_)).Times(0);
  EXPECT_CALL(socketMock, SetSendTimeout(_)).Times(0);
  if (socketMock.getPointer().IsNotNull())
    {
    EXPECT_GT(socketMock.GetConnected(),0);
    igtl::MessageHeader::Pointer headerMsg;
    headerMsg = igtl::MessageHeader::New();
    for (;;)
      {
      headerMsg->InitPack();
      int rs = socketMock.Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize(), 0);
      if (rs != headerMsg->GetPackSize())
        {
        continue;
        }
      
      // Deserialize the header
      headerMsg->Unpack();
      if (strcmp(headerMsg->GetDeviceType(), "STT_QTDATA") == 0)
        {
        std::cerr << "Received a STT_QTDATA message." << std::endl;
        
        igtl::StartQuaternionTrackingDataMessage::Pointer startQuaternionTracking;
        startQuaternionTracking = igtl::StartQuaternionTrackingDataMessage::New();
        startQuaternionTracking->SetMessageHeader(headerMsg);
        startQuaternionTracking->AllocatePack();
        
        socketMock.Receive(startQuaternionTracking->GetPackBodyPointer(), startQuaternionTracking->GetPackBodySize(),0);
        int c = startQuaternionTracking->Unpack(1);
        if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
          {
          igtl::QuaternionTrackingDataMessage::Pointer quaternionTrackingMsg;
          quaternionTrackingMsg = igtl::QuaternionTrackingDataMessage::New();
          quaternionTrackingMsg->SetDeviceName("Tracker");
          igtl::QuaternionTrackingDataElement::Pointer quaternionTrackElement0;
          quaternionTrackElement0 = igtl::QuaternionTrackingDataElement::New();
          quaternionTrackElement0->SetName("Channel 0");
          quaternionTrackElement0->SetType(igtl::QuaternionTrackingDataElement::TYPE_TRACKER);
          quaternionTrackingMsg->AddQuaternionTrackingDataElement(quaternionTrackElement0);
          while(!(threadID==-1))
            {
            if(msgReceived)
              {
              glock->Lock();
              quaternionTrackingMsg->Pack();
              socketMock.Send(quaternionTrackingMsg->GetPackPointer(), quaternionTrackingMsg->GetPackSize());
              glock->Unlock();
              msgReceived = false;
              }
            igtl::Sleep(interval);
            }
          }
        }
      else if (strcmp(headerMsg->GetDeviceType(), "STP_QTDATA") == 0)
        {
        std::cerr << "Received a stop message." << std::endl;
        socketMock.getPointer()->CloseSocket();
        threadrunning = false;
        break;
        }
      }
    }
  else
    {
    EXPECT_TRUE(false) << "No client connected.";
    }
  return NULL;
}

void clientServerProcess()
{
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
  //socketMock.setPointer(clientSocket);
  igtl::StartQuaternionTrackingDataMessage::Pointer startQuaternionTrackingMsg;
  startQuaternionTrackingMsg = igtl::StartQuaternionTrackingDataMessage::New();
  startQuaternionTrackingMsg->SetDeviceName("QTDataClient");
  startQuaternionTrackingMsg->SetResolution(interval);
  startQuaternionTrackingMsg->SetCoordinateName("Patient");
  startQuaternionTrackingMsg->Pack();
  clientSocket->Send(startQuaternionTrackingMsg->GetPackPointer(), startQuaternionTrackingMsg->GetPackSize());
  int loop = 0;
  while (1)
    {
    //------------------------------------------------------------
    // Wait for a reply
    igtl::Sleep(interval);
    igtl::MessageHeader::Pointer headerMsg;
    headerMsg = igtl::MessageHeader::New();
    headerMsg->InitPack();
    int rs = clientSocket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize(),0);
    if (rs == 0)
      {
      std::cerr << "Connection closed." << std::endl;
      clientSocket->CloseSocket();
      exit(0);
      }
    headerMsg->Unpack();
    if (strcmp(headerMsg->GetDeviceType(), "QTDATA") == 0)
      {
      ReceiveQuaternionTrackingData(clientSocket, headerMsg);
      msgReceived = true;
      }
    else
      {
      std::cerr << "Receiving : " << headerMsg->GetDeviceType() << std::endl;
      clientSocket->Skip(headerMsg->GetBodySizeToRead(), 0);
      }
    if (++loop >= 10) // if received 11 times
      {
      //------------------------------------------------------------
      // Ask the server to stop pushing quaternion tracking data
      std::cerr << "Sending STP_QTDATA message....." << std::endl;
      igtl::StopQuaternionTrackingDataMessage::Pointer stopQuaternionTrackingMsg;
      stopQuaternionTrackingMsg = igtl::StopQuaternionTrackingDataMessage::New();
      stopQuaternionTrackingMsg->SetDeviceName("QTDataClient");
      stopQuaternionTrackingMsg->Pack();
      clientSocket->Send(stopQuaternionTrackingMsg->GetPackPointer(), stopQuaternionTrackingMsg->GetPackSize());
      threadID = -1;
      break;
      }
    }
}

int ReceiveQuaternionTrackingData(ClientSocket::Pointer socket, igtl::MessageHeader::Pointer& header)
{
  std::cerr << "Receiving QTDATA data type." << std::endl;
  
  //------------------------------------------------------------
  // Allocate QuaternionTrackingData Message Class
  
  igtl::QuaternionTrackingDataMessage::Pointer quaternionTrackingData;
  quaternionTrackingData = igtl::QuaternionTrackingDataMessage::New();
  quaternionTrackingData->SetMessageHeader(header);
  quaternionTrackingData->AllocatePack();
  
  // Receive body from the socket
  socket->Receive(quaternionTrackingData->GetPackBodyPointer(), quaternionTrackingData->GetPackBodySize(),0);
  
  // Deserialize position and quaternion (orientation) data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = quaternionTrackingData->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    int nElements = quaternionTrackingData->GetNumberOfQuaternionTrackingDataElements();
    EXPECT_EQ(nElements, 1);
    return 1;
    }
  return 0;
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


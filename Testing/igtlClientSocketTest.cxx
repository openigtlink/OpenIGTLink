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
#include "igtl_types.h"
#include "igtl_header.h"
#include "igtlMessageHeader.h"
#include "igtl_util.h"
#include "igtlOSUtil.h"
#include "igtlClientSocket.h"
#include "igtlMultiThreader.h"
#include "igtlQuaternionTrackingDataMessage.h"

#include "igtlTestConfig.h"
#include "string.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Invoke;
using ::testing::IsNull;

using igtl::ServerSocket;
using igtl::ClientSocket;
using igtl::Socket;


class ClientSocketMock {
public:
  ClientSocketMock(ClientSocket::Pointer pointer) {
    real_ = pointer;
    // By default, all calls are delegated to the real object.
    ON_CALL(*this, ConnectToServer(_,_,_))
      .WillByDefault(Invoke(real_.GetPointer(), &ClientSocket::ConnectToServer));
  }
  ClientSocketMock(){real_.~SmartPointer();};
  
  MOCK_METHOD3(ConnectToServer, int(const char* hostName, int port, bool logErrorIfServerConnectionFailed));
  ClientSocket::Pointer getPointer(){return real_;};
  
private:
  ClientSocket::Pointer real_;
  
};

void* ThreadFunction(void* ptr);

int port = 18944;

TEST(ClientSocketTest, ConnectToServerTest)
{
  
  igtl::MultiThreader::Pointer threader = igtl::MultiThreader::New();
  igtl::ClientSocket::Pointer clientSocket;
  clientSocket = igtl::ClientSocket::New();
  ClientSocketMock clientSocketMock(clientSocket);
  EXPECT_CALL(clientSocketMock, ConnectToServer(_,_,_)).Times(2);
  EXPECT_EQ(clientSocketMock.ConnectToServer("localhost", port,0), -1);
  threader->SpawnThread((igtl::ThreadFunctionType) &ThreadFunction, NULL);
  igtl::Sleep(1000);
  EXPECT_EQ(clientSocketMock.ConnectToServer("localhost", port,1), 0);
  clientSocketMock.getPointer()->CloseSocket();
}

void* ThreadFunction(void* ptr)
{
  int waitingTime = 10000;
  ServerSocket::Pointer serverSocket = ServerSocket::New();
  serverSocket->CreateServer(port);
  serverSocket->WaitForConnection(waitingTime);
  serverSocket->CloseSocket();
  return NULL;
}
  
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    $RCSfile: itkImage.h,v $
  Language:  C++
  Date:      $Date: 2008/01/13 19:48:38 $
  Version:   $Revision: 1.142 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlServerSocket.h"
#include "igtlImageMessage.h"
#include "igtl_types.h"
#include "igtl_header.h"
#include "igtl_image.h"
#include "igtl_util.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "igtlClientSocket.h"
#include "igtlutil/igtl_test_data_image.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Invoke;
using igtl::ServerSocket;
using ::testing::IsNull;
class ServerSocketMock {
public:
  //typedef SmartPointer<ServerSocketMock>  Pointer;
  ServerSocketMock(ServerSocket::Pointer pointer) {
    real_ = pointer;
    // By default, all calls are delegated to the real object.
    ON_CALL(*this, CreateServer(_))
        .WillByDefault(Invoke(real_.GetPointer(), &ServerSocket::CreateServer));
    ON_CALL(*this, GetServerPort())
        .WillByDefault(Invoke(real_.GetPointer(), &ServerSocket::GetServerPort));
    ON_CALL(*this, WaitForConnection(_))
      .WillByDefault(Invoke(real_.GetPointer(), &ServerSocket::WaitForConnection));
  }
  ~ServerSocketMock(){real_.~SmartPointer();}; 
  MOCK_METHOD0(GetServerPort, int());
  MOCK_METHOD1(CreateServer, int(int port));
  MOCK_METHOD1(WaitForConnection, igtl::ClientSocket::Pointer(unsigned long msec));
 private:
   ServerSocket::Pointer real_;
  
};

TEST(ServerSocketTest, connection)
{
  ServerSocketMock mockServerSocket(ServerSocket::New());
  int port = 18944;
  int waitingTime = 5000;
  EXPECT_CALL(mockServerSocket, CreateServer(port)).Times(2);
  EXPECT_CALL(mockServerSocket, GetServerPort()).Times(1);
  EXPECT_CALL(mockServerSocket, WaitForConnection(_)).Times(1);
  int socketDescriptor = mockServerSocket.GetServerPort();
  EXPECT_EQ(mockServerSocket.CreateServer(port),0);
  igtl::ClientSocket::Pointer returnSocket = mockServerSocket.WaitForConnection(waitingTime);
   /**Connecting to a established server, however, time out happened, return NULL */
  EXPECT_TRUE(returnSocket.IsNull());
  EXPECT_EQ(mockServerSocket.CreateServer(port),-1);
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


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
#include "igtlutil/igtl_test_data_image.h"
using ::testing::_;
using ::testing::AtLeast;
using ::testing::Invoke;
using igtl::ServerSocket;
class ServerSocketMock :public ServerSocket {
public:
  ServerSocketMock(ServerSocket::Pointer pointer) {
    real_ = pointer;
    // By default, all calls are delegated to the real object.
    ON_CALL(*this, CreateServer(_))
        .WillByDefault(Invoke(real_.GetPointer(), &ServerSocket::CreateServer));
    ON_CALL(*this, GetServerPort())
        .WillByDefault(Invoke(real_.GetPointer(), &ServerSocket::GetServerPort));
  }
  ~ServerSocketMock(){}; 
  MOCK_METHOD1(CreateServer, int(int port));
  MOCK_METHOD0(GetServerPort, int());
 protected:
   ServerSocketMock();
 private:
   ServerSocket::Pointer real_;
  
};

TEST(ServerSocketTest, connection)
{
  ServerSocketMock mockServerSocket(ServerSocket::New());
  const char*  hostname = "test";
  int port = 5;
  EXPECT_CALL(mockServerSocket, CreateServer(port)).Times(2);
  EXPECT_CALL(mockServerSocket, GetServerPort()).Times(1);
  mockServerSocket.GetServerPort();
  EXPECT_EQ(mockServerSocket.CreateServer(port),0);
  EXPECT_EQ(mockServerSocket.CreateServer(port),0);
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


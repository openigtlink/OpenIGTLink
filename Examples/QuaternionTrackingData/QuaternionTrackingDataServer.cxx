/*=========================================================================

  Program:   OpenIGTLink -- Example for Quaternion Tracking Data Server
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
#include "igtlServerSocket.h"
#include "igtlQuaternionTrackingDataMessage.h"
#include "igtlMultiThreader.h"


void* ThreadFunction(void* ptr);
int   SendQuaternionTrackingData(igtl::Socket::Pointer& socket, igtl::QuaternionTrackingDataMessage::Pointer& quaternionTrackingMsg);
void  GetRandomTestPositionAndQuaternion(float position[3], float quaternion[4], float phi, float theta);

typedef struct {
  int   nloop;
  igtl::MutexLock::Pointer glock;
  igtl::Socket::Pointer socket;
  int   interval;
  int   stop;
} ThreadData;


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

  igtl::MultiThreader::Pointer threader = igtl::MultiThreader::New();
  igtl::MutexLock::Pointer glock = igtl::MutexLock::New();
  ThreadData td;

  while (1)
    {
    //------------------------------------------------------------
    // Waiting for Connection
    int threadID = -1;
    igtl::Socket::Pointer socket;
    socket = serverSocket->WaitForConnection(1000);
    
    if (socket.IsNotNull()) // if client connected
      {
      std::cerr << "A client is connected." << std::endl;

      // Create a message buffer to receive header
      igtl::MessageHeader::Pointer headerMsg;
      headerMsg = igtl::MessageHeader::New();
      //------------------------------------------------------------
      // loop
      for (;;)
        {
        // Initialize receive buffer
        headerMsg->InitPack();

        // Receive generic header from the socket
        int rs = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
        if (rs == 0)
          {
          if (threadID >= 0)
            {
            td.stop = 1;
            threader->TerminateThread(threadID);
            threadID = -1;
            }
          std::cerr << "Disconnecting the client." << std::endl;
          td.socket = NULL;  // VERY IMPORTANT. Completely remove the instance.
          socket->CloseSocket();
          break;
          }
        if (rs != headerMsg->GetPackSize())
          {
          continue;
          }

        // Deserialize the header
        headerMsg->Unpack();

        // Check data type and receive data body
        if (strcmp(headerMsg->GetDeviceType(), "STT_QTDATA") == 0)
          {
          std::cerr << "Received a STT_QTDATA message." << std::endl;
        
          igtl::StartQuaternionTrackingDataMessage::Pointer startQuaternionTracking;
          startQuaternionTracking = igtl::StartQuaternionTrackingDataMessage::New();
          startQuaternionTracking->SetMessageHeader(headerMsg);
          startQuaternionTracking->AllocatePack();
        
          int r2 = socket->Receive(startQuaternionTracking->GetPackBodyPointer(), startQuaternionTracking->GetPackBodySize());
          int c = startQuaternionTracking->Unpack(1);
          if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
            {
            td.interval = startQuaternionTracking->GetResolution();
            td.glock    = glock;
            td.socket   = socket;
            td.stop     = 0;
            threadID    = threader->SpawnThread((igtl::ThreadFunctionType) &ThreadFunction, &td);
            }
          }
        else if (strcmp(headerMsg->GetDeviceType(), "STP_QTDATA") == 0)
          {
          socket->Skip(headerMsg->GetBodySizeToRead(), 0);
          std::cerr << "Received a STP_QTDATA message." << std::endl;
          if (threadID >= 0)
            {
            td.stop  = 1;
            threader->TerminateThread(threadID);
            threadID = -1;
            std::cerr << "Disconnecting the client." << std::endl;
            td.socket = NULL;  // VERY IMPORTANT. Completely remove the instance.
            socket->CloseSocket();
            }
          break;
          }
        else
          {
          std::cerr << "Receiving : " << headerMsg->GetDeviceType() << std::endl;
          socket->Skip(headerMsg->GetBodySizeToRead(), 0);
          }
        }
      }
    }
    
  //------------------------------------------------------------
  // Close connection (The example code never reaches to this section ...)
  serverSocket->CloseSocket();

}


void* ThreadFunction(void* ptr)
{
  //------------------------------------------------------------
  // Get thread information
  igtl::MultiThreader::ThreadInfo* info = 
    static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);

  //int id      = info->ThreadID;
  //int nThread = info->NumberOfThreads;
  ThreadData* td = static_cast<ThreadData*>(info->UserData);

  //------------------------------------------------------------
  // Get user data
  igtl::MutexLock::Pointer glock = td->glock;
  long interval = td->interval;
  std::cerr << "Interval = " << interval << " (ms)" << std::endl;
  //long interval = 1000;
  //long interval = (id + 1) * 100; // (ms)

  igtl::Socket::Pointer& socket = td->socket;

  //------------------------------------------------------------
  // Allocate QuaternionTrackingData Message Class
  //
  // NOTE: QuaternionTrackingDataElement class instances are
  //       allocated before the loop starts to avoid
  //       reallocation in each image transfer.

  igtl::QuaternionTrackingDataMessage::Pointer quaternionTrackingMsg;
  quaternionTrackingMsg = igtl::QuaternionTrackingDataMessage::New();

  igtl::QuaternionTrackingDataElement::Pointer quaternionTrackElement0;
  quaternionTrackElement0 = igtl::QuaternionTrackingDataElement::New();
  quaternionTrackElement0->SetName("Channel 0");
  quaternionTrackElement0->SetType(igtl::QuaternionTrackingDataElement::TYPE_TRACKER);

  igtl::QuaternionTrackingDataElement::Pointer quaternionTrackElement1;
  quaternionTrackElement1 = igtl::QuaternionTrackingDataElement::New();
  quaternionTrackElement1->SetName("Channel 1");
  quaternionTrackElement1->SetType(igtl::QuaternionTrackingDataElement::TYPE_6D);

  igtl::QuaternionTrackingDataElement::Pointer quaternionTrackElement2;
  quaternionTrackElement2 = igtl::QuaternionTrackingDataElement::New();
  quaternionTrackElement2->SetName("Channel 2");
  quaternionTrackElement2->SetType(igtl::QuaternionTrackingDataElement::TYPE_5D);

  quaternionTrackingMsg->AddQuaternionTrackingDataElement(quaternionTrackElement0);
  quaternionTrackingMsg->AddQuaternionTrackingDataElement(quaternionTrackElement1);
  quaternionTrackingMsg->AddQuaternionTrackingDataElement(quaternionTrackElement2);

  //------------------------------------------------------------
  // Loop
  while (!td->stop)
    {
    quaternionTrackingMsg->SetDeviceName("Tracker");
    glock->Lock();
    SendQuaternionTrackingData(socket, quaternionTrackingMsg);
    glock->Unlock();
    igtl::Sleep(interval);
    }

  //glock->Lock();
  //std::cerr << "Thread #" << id << ": end." << std::endl;
  //glock->Unlock();
  
  return NULL;
}


int SendQuaternionTrackingData(igtl::Socket::Pointer& socket, igtl::QuaternionTrackingDataMessage::Pointer& quaternionTrackingMsg)
{

  static float phi0   = 0.0;
  static float theta0 = 0.0;
  static float phi1   = 0.0;
  static float theta1 = 0.0;
  static float phi2   = 0.0;
  static float theta2 = 0.0;

  float position[3];
  float quaternion[4];
  igtl::QuaternionTrackingDataElement::Pointer ptr;

  // Channel 0
  quaternionTrackingMsg->GetQuaternionTrackingDataElement(0, ptr);
  GetRandomTestPositionAndQuaternion(position, quaternion, phi0, theta0);
  ptr->SetPosition(position);
  ptr->SetQuaternion(quaternion);
  
  // Channel 1
  quaternionTrackingMsg->GetQuaternionTrackingDataElement(1, ptr);
  GetRandomTestPositionAndQuaternion(position, quaternion, phi1, theta1);
  ptr->SetPosition(position);
  ptr->SetQuaternion(quaternion);
  
  // Channel 2
  quaternionTrackingMsg->GetQuaternionTrackingDataElement(2, ptr);
  GetRandomTestPositionAndQuaternion(position, quaternion, phi2, theta2);
  ptr->SetPosition(position);
  ptr->SetQuaternion(quaternion);

  quaternionTrackingMsg->Pack();
  socket->Send(quaternionTrackingMsg->GetPackPointer(), quaternionTrackingMsg->GetPackSize());
  
  phi0 += 0.1;
  phi1 += 0.2;
  phi2 += 0.3;
  theta0 += 0.2;
  theta1 += 0.1;
  theta2 += 0.05;

  return 0;
}



//------------------------------------------------------------
// Function to generate random position and orientation (quaternion).
void GetRandomTestPositionAndQuaternion(float position[3], float quaternion[4], float phi, float theta)
{
  // random position
  position[0] = 50.0 * cos(phi);
  position[1] = 50.0 * sin(phi);
  position[2] = 50.0 * cos(phi);
  phi = phi + 0.2;

  // random orientation
  quaternion[0]=0.0;
  quaternion[1]=0.6666666666*cos(theta);
  quaternion[2]=0.577350269189626;
  quaternion[3]=0.6666666666*sin(theta);
  theta = theta + 0.1;

  //igtl::PrintVector3(position);
  //igtl::PrintVector4(quaternion);
}







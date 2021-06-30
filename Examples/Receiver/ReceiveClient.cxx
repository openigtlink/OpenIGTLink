/*=========================================================================

  Program:   OpenIGTLink -- Example for Data Receiving Client Program
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <iomanip>
#include <math.h>
#include <cstdlib>
#include <cstring>

#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlTransformMessage.h"
#include "igtlPositionMessage.h"
#include "igtlImageMessage.h"
#include "igtlClientSocket.h"
#include "igtlStatusMessage.h"

#if OpenIGTLink_PROTOCOL_VERSION >= 2
#include "igtlSensorMessage.h"
#include "igtlPointMessage.h"
#include "igtlTrajectoryMessage.h"
#include "igtlStringMessage.h"
#include "igtlTrackingDataMessage.h"
#include "igtlQuaternionTrackingDataMessage.h"
#include "igtlCapabilityMessage.h"
#endif // OpenIGTLink_PROTOCOL_VERSION >= 2

int ReceiveTransform(igtl::Socket * socket, igtl::MessageHeader::Pointer& header);
int ReceivePosition(igtl::Socket * socket, igtl::MessageHeader::Pointer& header);
int ReceiveImage(igtl::Socket * socket, igtl::MessageHeader::Pointer& header);
int ReceiveStatus(igtl::Socket * socket, igtl::MessageHeader::Pointer& header);

#if OpenIGTLink_PROTOCOL_VERSION >= 2
  int ReceiveSensor(igtl::Socket * socket, igtl::MessageHeader::Pointer& header);
  int ReceivePoint(igtl::Socket * socket, igtl::MessageHeader::Pointer& header);
  int ReceiveTrajectory(igtl::Socket * socket, igtl::MessageHeader::Pointer& header);
  int ReceiveString(igtl::Socket * socket, igtl::MessageHeader::Pointer& header);
  int ReceiveTrackingData(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header);
  int ReceiveQuaternionTrackingData(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header);
  int ReceiveCapability(igtl::Socket * socket, igtl::MessageHeader * header);
#endif //OpenIGTLink_PROTOCOL_VERSION >= 2

int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 3) // check number of arguments
    {
    // If not correct, print usage
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
  // Create a message buffer to receive header
  igtl::MessageHeader::Pointer headerMsg;
  headerMsg = igtl::MessageHeader::New();

  //------------------------------------------------------------
  // Allocate a time stamp
  igtl::TimeStamp::Pointer ts;
  ts = igtl::TimeStamp::New();


  while (1)
    {
    //------------------------------------------------------------
    // loop
    for (int i = 0; i < 100; i ++)
      {

      // Initialize receive buffer
      headerMsg->InitPack();

      // Receive generic header from the socket
      bool timeout(false);
      igtlUint64 r = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize(), timeout);
      if (r == 0)
        {
        socket->CloseSocket();
        exit(0);
        }
      if (r != headerMsg->GetPackSize())
        {
        continue;
        }

      // Deserialize the header
      headerMsg->Unpack();

      // Get time stamp
      igtlUint32 sec;
      igtlUint32 nanosec;

      headerMsg->GetTimeStamp(ts);
      ts->GetTimeStamp(&sec, &nanosec);

      std::cerr << "Name: " << headerMsg->GetDeviceName() << std::endl;
      std::cerr << "Time stamp: "
                << sec << "." << std::setw(9) << std::setfill('0')
                << nanosec << std::endl;

      // Check data type and receive data body
      if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM") == 0)
        {
        ReceiveTransform(socket, headerMsg);
        }
      else if (strcmp(headerMsg->GetDeviceType(), "POSITION") == 0)
        {
          ReceivePosition(socket, headerMsg);
        }
      else if (strcmp(headerMsg->GetDeviceType(), "IMAGE") == 0)
        {
        ReceiveImage(socket, headerMsg);
        }
      else if (strcmp(headerMsg->GetDeviceType(), "STATUS") == 0)
        {
        ReceiveStatus(socket, headerMsg);
        }
#if OpenIGTLink_PROTOCOL_VERSION >= 2
      else if (strcmp(headerMsg->GetDeviceType(), "SENSOR") == 0)
        {
          ReceiveSensor(socket, headerMsg);
        }
      else if (strcmp(headerMsg->GetDeviceType(), "POINT") == 0)
        {
        ReceivePoint(socket, headerMsg);
        }
      else if (strcmp(headerMsg->GetDeviceType(), "TRAJ") == 0)
        {
        ReceiveTrajectory(socket, headerMsg);
        }
      else if (strcmp(headerMsg->GetDeviceType(), "STRING") == 0)
        {
        ReceiveString(socket, headerMsg);
        }
      else if (strcmp(headerMsg->GetDeviceType(), "TDATA") == 0)
        {
        ReceiveTrackingData(socket, headerMsg);
        }
      else if (strcmp(headerMsg->GetDeviceType(), "QTDATA") == 0)
        {
        ReceiveQuaternionTrackingData(socket, headerMsg);
        }
      else if (strcmp(headerMsg->GetDeviceType(), "CAPABILITY") == 0)
        {
        ReceiveCapability(socket, headerMsg);;
        }
#endif //OpenIGTLink_PROTOCOL_VERSION >= 2
      else
        {
        std::cerr << "Receiving : " << headerMsg->GetDeviceType() << std::endl;
        socket->Skip(headerMsg->GetBodySizeToRead(), 0);
        }
      }
    }

  //------------------------------------------------------------
  // Close connection (The example code never reaches this section ...)

  socket->CloseSocket();

}


int ReceiveTransform(igtl::Socket * socket, igtl::MessageHeader::Pointer& header)
{
  std::cerr << "Receiving TRANSFORM data type." << std::endl;

  // Create a message buffer to receive transform datag
  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();
  transMsg->SetMessageHeader(header);
  transMsg->AllocatePack();

  // Receive transform data from the socket
  bool timeout(false);
  socket->Receive(transMsg->GetPackBodyPointer(), transMsg->GetPackBodySize(), timeout);

  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = transMsg->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    // Retrive the transform data
    igtl::Matrix4x4 matrix;
    transMsg->GetMatrix(matrix);
    igtl::PrintMatrix(matrix);
    std::cerr << std::endl;
    return 1;
    }

  return 0;
}

int ReceiveSensor(igtl::Socket * socket, igtl::MessageHeader::Pointer& header)
{
  std::cerr << "Receiving SENSOR data type." << std::endl;

  // Create a message buffer to receive transform datag
  igtl::SensorMessage::Pointer sensMsg;
  sensMsg = igtl::SensorMessage::New();
  sensMsg->SetMessageHeader(header);
  sensMsg->AllocatePack();

  // Receive transform data from the socket
  bool timeout(false);
  socket->Receive(sensMsg->GetPackBodyPointer(), sensMsg->GetPackBodySize(), timeout);

  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = sensMsg->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    for (size_t i = 0; i < sensMsg->GetLength(); ++i) {
	std::cerr << "v[" << i << "]: " << sensMsg->GetValue(i) << " ";
    }
    std::cerr << std::endl;
    return 1;
  }

  return 0;
}

int ReceivePosition(igtl::Socket * socket, igtl::MessageHeader::Pointer& header)
{
  std::cerr << "Receiving POSITION data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::PositionMessage::Pointer positionMsg;
  positionMsg = igtl::PositionMessage::New();
  positionMsg->SetMessageHeader(header);
  positionMsg->AllocatePack();

  // Receive position position data from the socket
  bool timeout(false);
  socket->Receive(positionMsg->GetPackBodyPointer(), positionMsg->GetPackBodySize(), timeout);

  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = positionMsg->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    // Retrive the transform data
    float position[3];
    float quaternion[4];

    positionMsg->GetPosition(position);
    positionMsg->GetQuaternion(quaternion);

    std::cerr << "position   = (" << position[0] << ", " << position[1] << ", " << position[2] << ")" << std::endl;
    std::cerr << "quaternion = (" << quaternion[0] << ", " << quaternion[1] << ", "
              << quaternion[2] << ", " << quaternion[3] << ")" << std::endl << std::endl;

    return 1;
    }

  return 0;
}

int ReceiveImage(igtl::Socket * socket, igtl::MessageHeader::Pointer& header)
{
  std::cerr << "Receiving IMAGE data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::ImageMessage::Pointer imgMsg;
  imgMsg = igtl::ImageMessage::New();
  imgMsg->SetMessageHeader(header);
  imgMsg->AllocatePack();

  // Receive transform data from the socket
  bool timeout(false);
  socket->Receive(imgMsg->GetPackBodyPointer(), imgMsg->GetPackBodySize(), timeout);

  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = imgMsg->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    // Retrive the image data
    int   size[3];          // image dimension
    float spacing[3];       // spacing (mm/pixel)
    int   svsize[3];        // sub-volume size
    int   svoffset[3];      // sub-volume offset
    int   scalarType;       // scalar type
    int   endian;           // endian

    scalarType = imgMsg->GetScalarType();
    endian = imgMsg->GetEndian();
    imgMsg->GetDimensions(size);
    imgMsg->GetSpacing(spacing);
    imgMsg->GetSubVolume(svsize, svoffset);


    std::cerr << "Device Name           : " << imgMsg->GetDeviceName() << std::endl;
    std::cerr << "Scalar Type           : " << scalarType << std::endl;
    std::cerr << "Endian                : " << endian << std::endl;
    std::cerr << "Dimensions            : ("
              << size[0] << ", " << size[1] << ", " << size[2] << ")" << std::endl;
    std::cerr << "Spacing               : ("
              << spacing[0] << ", " << spacing[1] << ", " << spacing[2] << ")" << std::endl;
    std::cerr << "Sub-Volume dimensions : ("
              << svsize[0] << ", " << svsize[1] << ", " << svsize[2] << ")" << std::endl;
    std::cerr << "Sub-Volume offset     : ("
              << svoffset[0] << ", " << svoffset[1] << ", " << svoffset[2] << ")" << std::endl << std::endl;
    return 1;
    }

  return 0;

}


int ReceiveStatus(igtl::Socket * socket, igtl::MessageHeader::Pointer& header)
{

  std::cerr << "Receiving STATUS data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::StatusMessage::Pointer statusMsg;
  statusMsg = igtl::StatusMessage::New();
  statusMsg->SetMessageHeader(header);
  statusMsg->AllocatePack();

  // Receive transform data from the socket
  bool timeout(false);
  socket->Receive(statusMsg->GetPackBodyPointer(), statusMsg->GetPackBodySize(), timeout);

  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = statusMsg->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    std::cerr << "========== STATUS ==========" << std::endl;
    std::cerr << " Code      : " << statusMsg->GetCode() << std::endl;
    std::cerr << " SubCode   : " << statusMsg->GetSubCode() << std::endl;
    std::cerr << " Error Name: " << statusMsg->GetErrorName() << std::endl;
    std::cerr << " Status    : " << statusMsg->GetStatusString() << std::endl;
    std::cerr << "============================" << std::endl << std::endl;
    }

  return 0;

}

#if OpenIGTLink_PROTOCOL_VERSION >= 2
int ReceivePoint(igtl::Socket * socket, igtl::MessageHeader::Pointer& header)
{

  std::cerr << "Receiving POINT data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::PointMessage::Pointer pointMsg;
  pointMsg = igtl::PointMessage::New();
  pointMsg->SetMessageHeader(header);
  pointMsg->AllocatePack();

  // Receive transform data from the socket
  bool timeout(false);
  socket->Receive(pointMsg->GetPackBodyPointer(), pointMsg->GetPackBodySize(), timeout);

  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = pointMsg->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    int nElements = pointMsg->GetNumberOfPointElement();
    for (int i = 0; i < nElements; i ++)
      {
      igtl::PointElement::Pointer pointElement;
      pointMsg->GetPointElement(i, pointElement);

      igtlUint8 rgba[4];
      pointElement->GetRGBA(rgba);

      igtlFloat32 pos[3];
      pointElement->GetPosition(pos);

      std::cerr << "========== Element #" << i << " ==========" << std::endl;
      std::cerr << " Name      : " << pointElement->GetName() << std::endl;
      std::cerr << " GroupName : " << pointElement->GetGroupName() << std::endl;
      std::cerr << " RGBA      : ( " << (int)rgba[0] << ", " << (int)rgba[1] << ", " << (int)rgba[2] << ", " << (int)rgba[3] << " )" << std::endl;
      std::cerr << " Position  : ( " << std::fixed << pos[0] << ", " << pos[1] << ", " << pos[2] << " )" << std::endl;
      std::cerr << " Radius    : " << std::fixed << pointElement->GetRadius() << std::endl;
      std::cerr << " Owner     : " << pointElement->GetOwner() << std::endl;
      std::cerr << "================================" << std::endl << std::endl;
      }
    }

  return 1;
}

int ReceiveTrajectory(igtl::Socket * socket, igtl::MessageHeader::Pointer& header)
{

  std::cerr << "Receiving TRAJECTORY data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::TrajectoryMessage::Pointer trajectoryMsg;
  trajectoryMsg = igtl::TrajectoryMessage::New();
  trajectoryMsg->SetMessageHeader(header);
  trajectoryMsg->AllocatePack();

  // Receive transform data from the socket
  bool timeout(false);
  socket->Receive(trajectoryMsg->GetPackBodyPointer(), trajectoryMsg->GetPackBodySize(), timeout);

  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = trajectoryMsg->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    int nElements = trajectoryMsg->GetNumberOfTrajectoryElement();
    for (int i = 0; i < nElements; i ++)
      {
      igtl::TrajectoryElement::Pointer trajectoryElement;
      trajectoryMsg->GetTrajectoryElement(i, trajectoryElement);

      igtlUint8 rgba[4];
      trajectoryElement->GetRGBA(rgba);

      igtlFloat32 entry[3];
      igtlFloat32 target[3];
      trajectoryElement->GetEntryPosition(entry);
      trajectoryElement->GetTargetPosition(target);

      std::cerr << "========== Element #" << i << " ==========" << std::endl;
      std::cerr << " Name      : " << trajectoryElement->GetName() << std::endl;
      std::cerr << " GroupName : " << trajectoryElement->GetGroupName() << std::endl;
      std::cerr << " RGBA      : ( " << (int)rgba[0] << ", " << (int)rgba[1] << ", " << (int)rgba[2] << ", " << (int)rgba[3] << " )" << std::endl;
      std::cerr << " Entry Pt  : ( " << std::fixed << entry[0] << ", " << entry[1] << ", " << entry[2] << " )" << std::endl;
      std::cerr << " Target Pt : ( " << std::fixed << target[0] << ", " << target[1] << ", " << target[2] << " )" << std::endl;
      std::cerr << " Radius    : " << std::fixed << trajectoryElement->GetRadius() << std::endl;
      std::cerr << " Owner     : " << trajectoryElement->GetOwner() << std::endl;
      std::cerr << "================================" << std::endl << std::endl;
      }
    }

  return 1;
}

int ReceiveString(igtl::Socket * socket, igtl::MessageHeader::Pointer& header)
{

  std::cerr << "Receiving STRING data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::StringMessage::Pointer stringMsg;
  stringMsg = igtl::StringMessage::New();
  stringMsg->SetMessageHeader(header);
  stringMsg->AllocatePack();

  // Receive transform data from the socket
  bool timeout(false);
  socket->Receive(stringMsg->GetPackBodyPointer(), stringMsg->GetPackBodySize(), timeout);

  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = stringMsg->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    std::cerr << "Encoding: " << stringMsg->GetEncoding() << "; "
              << "String: " << stringMsg->GetString() << std::endl << std::endl;
    }

  return 1;
}

int ReceiveTrackingData(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{
  std::cerr << "Receiving TDATA data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::TrackingDataMessage::Pointer trackingData;
  trackingData = igtl::TrackingDataMessage::New();
  trackingData->SetMessageHeader(header);
  trackingData->AllocatePack();

  // Receive body from the socket
  bool timeout(false);
  socket->Receive(trackingData->GetPackBodyPointer(), trackingData->GetPackBodySize(), timeout);

  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = trackingData->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    int nElements = trackingData->GetNumberOfTrackingDataElements();
    for (int i = 0; i < nElements; i ++)
      {
      igtl::TrackingDataElement::Pointer trackingElement;
      trackingData->GetTrackingDataElement(i, trackingElement);

      igtl::Matrix4x4 matrix;
      trackingElement->GetMatrix(matrix);


      std::cerr << "========== Element #" << i << " ==========" << std::endl;
      std::cerr << " Name       : " << trackingElement->GetName() << std::endl;
      std::cerr << " Type       : " << (int) trackingElement->GetType() << std::endl;
      std::cerr << " Matrix : " << std::endl;
      igtl::PrintMatrix(matrix);
      std::cerr << "================================" << std::endl << std::endl;
      }
    return 1;
    }
  return 0;
}

int ReceiveQuaternionTrackingData(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{
  std::cerr << "Receiving QTDATA data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::QuaternionTrackingDataMessage::Pointer quaternionTrackingData;
  quaternionTrackingData = igtl::QuaternionTrackingDataMessage::New();
  quaternionTrackingData->SetMessageHeader(header);
  quaternionTrackingData->AllocatePack();

  // Receive body from the socket
  bool timeout(false);
  socket->Receive(quaternionTrackingData->GetPackBodyPointer(), quaternionTrackingData->GetPackBodySize(), timeout);

  // Deserialize position and quaternion (orientation) data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = quaternionTrackingData->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    int nElements = quaternionTrackingData->GetNumberOfQuaternionTrackingDataElements();
    for (int i = 0; i < nElements; i ++)
      {
      igtl::QuaternionTrackingDataElement::Pointer quaternionTrackingElement;
      quaternionTrackingData->GetQuaternionTrackingDataElement(i, quaternionTrackingElement);

      float position[3];
      float quaternion[4];
      quaternionTrackingElement->GetPosition(position);
      quaternionTrackingElement->GetQuaternion(quaternion);

      std::cerr << "========== Element #" << i << " ==========" << std::endl;
      std::cerr << " Name       : " << quaternionTrackingElement->GetName() << std::endl;
      std::cerr << " Type       : " << (int) quaternionTrackingElement->GetType() << std::endl;
      std::cerr << " Position   : "; igtl::PrintVector3(position);
      std::cerr << " Quaternion : "; igtl::PrintVector4(quaternion);
      std::cerr << "================================" << std::endl << std::endl;
      }
    return 1;
    }
  return 0;
}

int ReceiveCapability(igtl::Socket * socket, igtl::MessageHeader * header)
{

  std::cerr << "Receiving CAPABILITY data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::CapabilityMessage::Pointer capabilMsg;
  capabilMsg = igtl::CapabilityMessage::New();
  capabilMsg->SetMessageHeader(header);
  capabilMsg->AllocatePack();

  // Receive transform data from the socket
  bool timeout(false);
  socket->Receive(capabilMsg->GetPackBodyPointer(), capabilMsg->GetPackBodySize(), timeout);

  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = capabilMsg->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    int nTypes = capabilMsg->GetNumberOfTypes();
    for (int i = 0; i < nTypes; i ++)
      {
      std::cerr << "Typename #" << i << ": " << capabilMsg->GetType(i) << std::endl;
      }
    }

  return 1;

}

#endif //OpenIGTLink_PROTOCOL_VERSION >= 2

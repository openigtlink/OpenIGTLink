/*=========================================================================

Program:   The OpenIGTLink Library
Language:  C++
Web page:  http://openigtlink.org/

Copyright (c) Insight Software Consortium. All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlMessageFactory.h"

#include "igtlTransformMessage.h"
#include "igtlPositionMessage.h"
#include "igtlImageMessage.h"
#include "igtlClientSocket.h"
#include "igtlStatusMessage.h"
#include "igtlCapabilityMessage.h"

#if OpenIGTLink_PROTOCOL_VERSION >= 2
#include "igtlPointMessage.h"
#include "igtlPolyDataMessage.h"
#include "igtlQuaternionTrackingDataMessage.h"
#include "igtlStringMessage.h"
#include "igtlTrackingDataMessage.h"
#include "igtlTrajectoryMessage.h"
#include "igtlImageMetaMessage.h"
#include "igtlLabelMetaMessage.h"
#endif // OpenIGTLink_PROTOCOL_VERSION >= 2

#if OpenIGTLink_HEADER_VERSION >= 2
#include "igtlCommandMessage.h"
#endif // OpenIGTLink_PROTOCOL_VERSION >= 3

#if defined(OpenIGTLink_ENABLE_VIDEOSTREAMING)
#include "igtlVideoMessage.h"
#endif 

#include "igtl_header.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <sstream>
#include <stdexcept>

namespace igtl
{

//-----------------------------------------------------------------------------
MessageFactory::MessageFactory()
{
  this->AddMessageType("TRANSFORM", (PointerToMessageBaseNew)&igtl::TransformMessage::New);
  this->AddMessageType("GET_TRANS", (PointerToMessageBaseNew)&igtl::GetTransformMessage::New);
  this->AddMessageType("POSITION", (PointerToMessageBaseNew)&igtl::PositionMessage::New);
  this->AddMessageType("IMAGE", (PointerToMessageBaseNew)&igtl::ImageMessage::New);
  this->AddMessageType("GET_IMAGE", (PointerToMessageBaseNew)&igtl::GetImageMessage::New);
  this->AddMessageType("STATUS", (PointerToMessageBaseNew)&igtl::StatusMessage::New);
  this->AddMessageType("GET_STATUS", (PointerToMessageBaseNew)&igtl::GetStatusMessage::New);
  this->AddMessageType("CAPABILITY", (PointerToMessageBaseNew)&igtl::CapabilityMessage::New);
#if OpenIGTLink_PROTOCOL_VERSION >= 2
  this->AddMessageType("POINT", (PointerToMessageBaseNew)&igtl::PointMessage::New);
  this->AddMessageType("GET_POINT", (PointerToMessageBaseNew)&igtl::GetPointMessage::New);
  this->AddMessageType("TRAJ", (PointerToMessageBaseNew)&igtl::TrajectoryMessage::New);
  this->AddMessageType("GET_TRAJ", (PointerToMessageBaseNew)&igtl::GetTrajectoryMessage::New);
  this->AddMessageType("STRING", (PointerToMessageBaseNew)&igtl::StringMessage::New);
  this->AddMessageType("TDATA", (PointerToMessageBaseNew)&igtl::TrackingDataMessage::New);
  this->AddMessageType("POLYDATA", (PointerToMessageBaseNew)&igtl::PolyDataMessage::New);
  this->AddMessageType("GET_POLYDATA", (PointerToMessageBaseNew)&igtl::GetPolyDataMessage::New);
  this->AddMessageType("RTS_POLYDATA", (PointerToMessageBaseNew)&igtl::RTSPolyDataMessage::New);
  this->AddMessageType("STT_POLYDATA", (PointerToMessageBaseNew)&igtl::StartPolyDataMessage::New);
  this->AddMessageType("STP_POLYDATA", (PointerToMessageBaseNew)&igtl::StopPolyDataMessage::New);
  this->AddMessageType("RTS_TDATA", (PointerToMessageBaseNew)&igtl::RTSTrackingDataMessage::New);
  this->AddMessageType("STT_TDATA", (PointerToMessageBaseNew)&igtl::StartTrackingDataMessage::New);
  this->AddMessageType("STP_TDATA", (PointerToMessageBaseNew)&igtl::StopTrackingDataMessage::New);
  this->AddMessageType("QTDATA", (PointerToMessageBaseNew)&igtl::QuaternionTrackingDataMessage::New);
  this->AddMessageType("RTS_QTDATA", (PointerToMessageBaseNew)&igtl::RTSQuaternionTrackingDataMessage::New);
  this->AddMessageType("STT_QTDATA", (PointerToMessageBaseNew)&igtl::StartQuaternionTrackingDataMessage::New);
  this->AddMessageType("STP_QTDATA", (PointerToMessageBaseNew)&igtl::StopQuaternionTrackingDataMessage::New);
  this->AddMessageType("GET_IMGMETA", (PointerToMessageBaseNew)&igtl::GetImageMetaMessage::New);
  this->AddMessageType("IMGMETA", (PointerToMessageBaseNew)&igtl::ImageMetaMessage::New);
  this->AddMessageType("GET_LBMETA", (PointerToMessageBaseNew)&igtl::GetLabelMetaMessage::New);
  this->AddMessageType("LBMETA", (PointerToMessageBaseNew)&igtl::LabelMetaMessage::New);
#endif

#if OpenIGTLink_PROTOCOL_VERSION >= 3
  this->AddMessageType("COMMAND", (PointerToMessageBaseNew)&igtl::CommandMessage::New);
  this->AddMessageType("RTS_COMMAND", (PointerToMessageBaseNew)&igtl::RTSCommandMessage::New);
#endif

#if defined(OpenIGTLink_ENABLE_VIDEOSTREAMING)
  this->AddMessageType("VIDEO", (PointerToMessageBaseNew)&igtl::VideoMessage::New);
#endif 
}


//-----------------------------------------------------------------------------
MessageFactory::~MessageFactory()
{
}

//-----------------------------------------------------------------------------
void MessageFactory::AddMessageType(const std::string& messageTypeName, MessageFactory::PointerToMessageBaseNew messageTypeNewPointer )
{
  this->IgtlMessageTypes[messageTypeName] = messageTypeNewPointer; 
}

//----------------------------------------------------------------------------
MessageFactory::PointerToMessageBaseNew MessageFactory::GetMessageTypeNewPointer(const std::string& messageTypeName) const
{
  if ( this->IgtlMessageTypes.find(messageTypeName) != this->IgtlMessageTypes.end() )
    {
    return this->IgtlMessageTypes.find(messageTypeName)->second;
    }

  return NULL; 
}

//-----------------------------------------------------------------------------
bool MessageFactory::IsValid(igtl::MessageHeader::Pointer headerMsg)
{
  bool result = false;

#if OpenIGTLink_HEADER_VERSION >= 2
  if (headerMsg.IsNotNull() && IgtlMessageTypes.find(headerMsg->GetMessageType()) != IgtlMessageTypes.end() )
#else
    if (headerMsg.IsNotNull() && IgtlMessageTypes.find(headerMsg->GetDeviceType()) != IgtlMessageTypes.end())
#endif
      {
      result = true;
      }
  return result;
}

//----------------------------------------------------------------------------
bool MessageFactory::IsValid(igtl::MessageHeader::Pointer headerMsg) const
{
  bool result = false;

#if OpenIGTLink_HEADER_VERSION >= 2
  if (headerMsg.IsNotNull() && IgtlMessageTypes.find(headerMsg->GetMessageType()) != IgtlMessageTypes.end() )
#else
    if (headerMsg.IsNotNull() && IgtlMessageTypes.find(headerMsg->GetDeviceType()) != IgtlMessageTypes.end())
#endif
      {
      result = true;
      }
  return result;
}

//-----------------------------------------------------------------------------
igtl::MessageBase::Pointer MessageFactory::GetMessage(igtl::MessageHeader::Pointer headerMsg)
{
  if (headerMsg.IsNull())
    {
    return NULL;
    }

#if OpenIGTLink_HEADER_VERSION >= 2
  std::string messageType(headerMsg->GetMessageType());
#else
  std::string messageType(headerMsg->GetDeviceType());
#endif

  if (!this->IsValid(headerMsg))
    {
    return NULL;
    }

  std::string messageTypeUpper(messageType);
  std::transform(messageTypeUpper.begin(), messageTypeUpper.end(), messageTypeUpper.begin(), ::toupper);

  igtl::MessageBase::Pointer result = NULL;
  if( GetMessageTypeNewPointer(messageTypeUpper) == NULL )
    {
    return NULL;
    }
  result = GetMessageTypeNewPointer(messageTypeUpper)();

  // Must have a valid message at this point, otherwise its a programming bug.
  assert(result.IsNotNull());

  result->SetMessageHeader(headerMsg);
  result->InitBuffer();

  return result;
}

//----------------------------------------------------------------------------
igtl::MessageHeader::Pointer MessageFactory::CreateHeaderMessage(int headerVersion) const
{
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->SetHeaderVersion(headerVersion);
  headerMsg->InitBuffer();

  return headerMsg;
}

//----------------------------------------------------------------------------
igtl::MessageBase::Pointer MessageFactory::CreateReceiveMessage(igtl::MessageHeader::Pointer headerMsg) const
{
  if (headerMsg.IsNull())
    {
    return NULL;
    }

#if OpenIGTLink_HEADER_VERSION >= 2
  std::string messageType(headerMsg->GetMessageType());
#else
  std::string messageType(headerMsg->GetDeviceType());
#endif
  std::string messageTypeUpper(messageType);
  std::transform(messageTypeUpper.begin(), messageTypeUpper.end(), messageTypeUpper.begin(), ::toupper);

  if (!this->IsValid(headerMsg))
    {
    return NULL;
    }

  igtl::MessageBase::Pointer result = NULL;
  if( GetMessageTypeNewPointer(messageTypeUpper) == NULL )
    {
    return NULL;
    }
  result = GetMessageTypeNewPointer(messageTypeUpper)();

  // Must have a valid message at this point, otherwise its a programming bug.
  assert(result.IsNotNull());

  result->SetMessageHeader(headerMsg);
  result->AllocateBuffer();

  return result;
}

//----------------------------------------------------------------------------
igtl::MessageBase::Pointer MessageFactory::CreateSendMessage(const std::string& messageType, int headerVersion) const
{
  if (messageType.empty())
    {
    return NULL;
    }

  std::string messageTypeUpper(messageType);
  std::transform(messageTypeUpper.begin(), messageTypeUpper.end(), messageTypeUpper.begin(), ::toupper);

  igtl::MessageBase::Pointer result = NULL;
  if( GetMessageTypeNewPointer(messageTypeUpper) == NULL )
    {
    return NULL;
    }
  result = GetMessageTypeNewPointer(messageTypeUpper)();

  // Must have a valid message at this point, otherwise its a programming bug.
  assert(result.IsNotNull());

  result->SetDeviceType(messageTypeUpper);
  result->SetHeaderVersion(headerVersion);
  result->InitBuffer();
  return result;
}

//----------------------------------------------------------------------------
void MessageFactory::GetAvailableMessageTypes(std::vector<std::string>& types) const
{
  types.clear();
  for( std::map<std::string, PointerToMessageBaseNew>::const_iterator it = IgtlMessageTypes.begin(); it != IgtlMessageTypes.end(); ++it )
    {
    types.push_back(it->first);
    }

  return;
}

} // end namespace

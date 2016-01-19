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

#if OpenIGTLink_PROTOCOL_VERSION >= 2
#include "igtlPointMessage.h"
#include "igtlTrajectoryMessage.h"
#include "igtlStringMessage.h"
#include "igtlTrackingDataMessage.h"
#include "igtlQuaternionTrackingDataMessage.h"
#include "igtlCapabilityMessage.h"
#endif // OpenIGTLink_PROTOCOL_VERSION >= 2

#if OpenIGTLink_PROTOCOL_VERSION >= 3
#include "igtlCommandMessage.h"
#endif // OpenIGTLink_PROTOCOL_VERSION >= 3

#include "igtl_header.h"

#include <stdexcept>
#include <sstream>
#include <cassert>
#include <cstring>

namespace igtl
{

//-----------------------------------------------------------------------------
MessageFactory::MessageFactory()
{
  this->AddMessageType("TRANSFORM", (PointerToMessageBaseNew)&igtl::TransformMessage::New);
  this->AddMessageType("POSITION", (PointerToMessageBaseNew)&igtl::PositionMessage::New);
  this->AddMessageType("IMAGE", (PointerToMessageBaseNew)&igtl::ImageMessage::New);
  this->AddMessageType("STATUS", (PointerToMessageBaseNew)&igtl::StatusMessage::New);
#if OpenIGTLink_PROTOCOL_VERSION >= 2
  this->AddMessageType("POINT", (PointerToMessageBaseNew)&igtl::PointMessage::New);
  this->AddMessageType("TRAJ", (PointerToMessageBaseNew)&igtl::TrajectoryMessage::New);
  this->AddMessageType("STRING", (PointerToMessageBaseNew)&igtl::StringMessage::New);
  this->AddMessageType("TDATA", (PointerToMessageBaseNew)&igtl::TrackingDataMessage::New);
  this->AddMessageType("QTDATA", (PointerToMessageBaseNew)&igtl::QuaternionTrackingDataMessage::New);
  this->AddMessageType("CAPABILITY", (PointerToMessageBaseNew)&igtl::CapabilityMessage::New);
#endif //OpenIGTLink_PROTOCOL_VERSION >= 2
#if OpenIGTLink_PROTOCOL_VERSION >= 3
  this->AddMessageType("COMMAND", (PointerToMessageBaseNew)&igtl::CommandMessage::New);
#endif //OpenIGTLink_PROTOCOL_VERSION >= 3
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
MessageFactory::PointerToMessageBaseNew MessageFactory::GetMessageTypeNewPointer(const std::string& messageTypeName)
{
  if ( this->IgtlMessageTypes.find(messageTypeName) != this->IgtlMessageTypes.end() )
  {
    return IgtlMessageTypes[messageTypeName];
  }

  std::ostringstream oss;
  oss << "Error: In " __FILE__ ", line " << __LINE__ << ", unknown message type constructor requested." << std::endl;
  throw std::invalid_argument(oss.str());

  return NULL; 
}

//-----------------------------------------------------------------------------
bool MessageFactory::IsValid(igtl::MessageHeader::Pointer headerMsg)
{
  bool result = false;

#if OpenIGTLink_PROTOCOL_VERSION >= 3
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

#if OpenIGTLink_PROTOCOL_VERSION >= 3
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
    std::ostringstream oss;
    oss << "Error: In " __FILE__ ", line " << __LINE__ << ", header is NULL." << std::endl;
    throw std::invalid_argument(oss.str());
  }

  if (!this->IsValid(headerMsg))
  {
    std::ostringstream oss;
#if OpenIGTLink_PROTOCOL_VERSION >= 3
    oss << "Error: In " __FILE__ ", line " << __LINE__ << ", receiving message of type:" << headerMsg->GetMessageType() << std::endl;
#else
    oss << "Error: In " __FILE__ ", line " << __LINE__ << ", receiving message of type:" << headerMsg->GetDeviceType() << std::endl;
#endif
    
    throw std::invalid_argument(oss.str());
  }

  igtl::MessageBase::Pointer result = NULL;

  #if OpenIGTLink_PROTOCOL_VERSION >= 3
    if ( headerMsg->GetMessageType().compare("TRANSFORM") == 0 )
  #else
    if ( strcmp(headerMsg->GetDeviceType(), "TRANSFORM") == 0 )
  #endif
  {
    result = igtl::TransformMessage::New();
  }
  #if OpenIGTLink_PROTOCOL_VERSION >= 3
    else if ( headerMsg->GetMessageType().compare(std::string("POSITION")) == 0 )
  #else
    else if ( strcmp(headerMsg->GetDeviceType(), "POSITION") == 0 )
  #endif
  {
    result = igtl::PositionMessage::New();
  }
  #if OpenIGTLink_PROTOCOL_VERSION >= 3
    else if ( headerMsg->GetMessageType().compare(std::string("IMAGE")) == 0 )
  #else
    else if ( strcmp(headerMsg->GetDeviceType(), "IMAGE") == 0 )
  #endif
  {
    result = igtl::ImageMessage::New();
  }
  #if OpenIGTLink_PROTOCOL_VERSION >= 3
    else if ( headerMsg->GetMessageType().compare(std::string("STATUS")) == 0 )
  #else
    else if ( strcmp(headerMsg->GetDeviceType(), "STATUS") == 0 )
  #endif
  {
    result = igtl::StatusMessage::New();
  }
#if OpenIGTLink_PROTOCOL_VERSION >= 2

  #if OpenIGTLink_PROTOCOL_VERSION >= 3
    else if ( headerMsg->GetMessageType().compare(std::string("POINT")) == 0 )
  #else
    else if ( strcmp(headerMsg->GetDeviceType(), "POINT") == 0 )
  #endif
  {
    result = igtl::PointMessage::New();
  }
  #if OpenIGTLink_PROTOCOL_VERSION >= 3
    else if ( headerMsg->GetMessageType().compare(std::string("TRAJ")) == 0 )
  #else
    else if ( strcmp(headerMsg->GetDeviceType(), "TRAJ") == 0 )
  #endif
  {
    result = igtl::TrajectoryMessage::New();
  }
  #if OpenIGTLink_PROTOCOL_VERSION >= 3
    else if ( headerMsg->GetMessageType().compare(std::string("STRING")) == 0 )
  #else
    else if ( strcmp(headerMsg->GetDeviceType(), "STRING") == 0 )
  #endif
  {
    result = igtl::StringMessage::New();
  }
  #if OpenIGTLink_PROTOCOL_VERSION >= 3
    else if ( headerMsg->GetMessageType().compare(std::string("TDATA")) == 0 )
  #else
    else if ( strcmp(headerMsg->GetDeviceType(), "TDATA") == 0 )
  #endif
  {
    result = igtl::TrackingDataMessage::New();
  }
  #if OpenIGTLink_PROTOCOL_VERSION >= 3
    else if ( headerMsg->GetMessageType().compare(std::string("QTDATA")) == 0 )
  #else
    else if ( strcmp(headerMsg->GetDeviceType(), "QTDATA") == 0 )
  #endif
  {
    result = igtl::QuaternionTrackingDataMessage::New();
  }
  #if OpenIGTLink_PROTOCOL_VERSION >= 3
    else if ( headerMsg->GetMessageType().compare(std::string("CAPABILITY")) == 0 )
  #else
    else if ( strcmp(headerMsg->GetDeviceType(), "CAPABILITY") == 0 )
  #endif
  {
    result = igtl::CapabilityMessage::New();
  }

#endif //OpenIGTLink_PROTOCOL_VERSION >= 2
#if OpenIGTLink_PROTOCOL_VERSION >= 3

  else if ( headerMsg->GetMessageType().compare(std::string("COMMAND")) == 0 )
  {
    result = igtl::CommandMessage::New();
  }

#endif //OpenIGTLink_PROTOCOL_VERSION >= 3

  // Must have a valid message at this point, otherwise its a programming bug.
  assert(result.IsNotNull());

  result->SetMessageHeader(headerMsg);
  result->AllocatePack();

  return result;
}

#if OpenIGTLink_PROTOCOL_VERSION >= 3
//----------------------------------------------------------------------------
igtl::MessageBase::Pointer MessageFactory::CreateReceiveMessage(igtl::MessageHeader::Pointer headerMsg) const
{
  if (headerMsg.IsNull())
  {
    std::ostringstream oss;
    oss << "Error: In " __FILE__ ", line " << __LINE__ << ", header is NULL." << std::endl;
    throw std::invalid_argument(oss.str());
  }

  if (!this->IsValid(headerMsg))
  {
    std::ostringstream oss;
#if OpenIGTLink_PROTOCOL_VERSION >= 3
    oss << "Error: In " __FILE__ ", line " << __LINE__ << ", receiving message of type:" << headerMsg->GetMessageType() << std::endl;
#else
    oss << "Error: In " __FILE__ ", line " << __LINE__ << ", receiving message of type:" << headerMsg->GetDeviceType() << std::endl;
#endif

    throw std::invalid_argument(oss.str());
  }

  igtl::MessageBase::Pointer result = NULL;

#if OpenIGTLink_PROTOCOL_VERSION >= 3
  if ( headerMsg->GetMessageType().compare("TRANSFORM") == 0 )
#else
  if ( strcmp(headerMsg->GetDeviceType(), "TRANSFORM") == 0 )
#endif
  {
    result = igtl::TransformMessage::New();
  }
#if OpenIGTLink_PROTOCOL_VERSION >= 3
  else if ( headerMsg->GetMessageType().compare(std::string("POSITION")) == 0 )
#else
  else if ( strcmp(headerMsg->GetDeviceType(), "POSITION") == 0 )
#endif
  {
    result = igtl::PositionMessage::New();
  }
#if OpenIGTLink_PROTOCOL_VERSION >= 3
  else if ( headerMsg->GetMessageType().compare(std::string("IMAGE")) == 0 )
#else
  else if ( strcmp(headerMsg->GetDeviceType(), "IMAGE") == 0 )
#endif
  {
    result = igtl::ImageMessage::New();
  }
#if OpenIGTLink_PROTOCOL_VERSION >= 3
  else if ( headerMsg->GetMessageType().compare(std::string("STATUS")) == 0 )
#else
  else if ( strcmp(headerMsg->GetDeviceType(), "STATUS") == 0 )
#endif
  {
    result = igtl::StatusMessage::New();
  }
#if OpenIGTLink_PROTOCOL_VERSION >= 2

#if OpenIGTLink_PROTOCOL_VERSION >= 3
  else if ( headerMsg->GetMessageType().compare(std::string("POINT")) == 0 )
#else
  else if ( strcmp(headerMsg->GetDeviceType(), "POINT") == 0 )
#endif
  {
    result = igtl::PointMessage::New();
  }
#if OpenIGTLink_PROTOCOL_VERSION >= 3
  else if ( headerMsg->GetMessageType().compare(std::string("TRAJ")) == 0 )
#else
  else if ( strcmp(headerMsg->GetDeviceType(), "TRAJ") == 0 )
#endif
  {
    result = igtl::TrajectoryMessage::New();
  }
#if OpenIGTLink_PROTOCOL_VERSION >= 3
  else if ( headerMsg->GetMessageType().compare(std::string("STRING")) == 0 )
#else
  else if ( strcmp(headerMsg->GetDeviceType(), "STRING") == 0 )
#endif
  {
    result = igtl::StringMessage::New();
  }
#if OpenIGTLink_PROTOCOL_VERSION >= 3
  else if ( headerMsg->GetMessageType().compare(std::string("TDATA")) == 0 )
#else
  else if ( strcmp(headerMsg->GetDeviceType(), "TDATA") == 0 )
#endif
  {
    result = igtl::TrackingDataMessage::New();
  }
#if OpenIGTLink_PROTOCOL_VERSION >= 3
  else if ( headerMsg->GetMessageType().compare(std::string("QTDATA")) == 0 )
#else
  else if ( strcmp(headerMsg->GetDeviceType(), "QTDATA") == 0 )
#endif
  {
    result = igtl::QuaternionTrackingDataMessage::New();
  }
#if OpenIGTLink_PROTOCOL_VERSION >= 3
  else if ( headerMsg->GetMessageType().compare(std::string("CAPABILITY")) == 0 )
#else
  else if ( strcmp(headerMsg->GetDeviceType(), "CAPABILITY") == 0 )
#endif
  {
    result = igtl::CapabilityMessage::New();
  }

#endif //OpenIGTLink_PROTOCOL_VERSION >= 2
#if OpenIGTLink_PROTOCOL_VERSION >= 3

  else if ( headerMsg->GetMessageType().compare(std::string("COMMAND")) == 0 )
  {
    result = igtl::CommandMessage::New();
  }

#endif //OpenIGTLink_PROTOCOL_VERSION >= 3

  // Must have a valid message at this point, otherwise its a programming bug.
  assert(result.IsNotNull());

  result->SetMessageHeader(headerMsg);
  result->AllocatePack();

  return result;
}

//----------------------------------------------------------------------------
igtl::MessageBase::Pointer MessageFactory::CreateSendMessage(const std::string& messageType) const
{
  if (messageType.empty())
  {
    std::ostringstream oss;
    oss << "Error: In " __FILE__ ", line " << __LINE__ << ", message type is undefined." << std::endl;
    throw std::invalid_argument(oss.str());
  }

  igtl::MessageBase::Pointer result = NULL;

  if ( messageType.compare("TRANSFORM") == 0 )
  {
    result = igtl::TransformMessage::New();
  }
  else if ( messageType.compare(std::string("POSITION")) == 0 )
  {
    result = igtl::PositionMessage::New();
  }
  else if ( messageType.compare(std::string("IMAGE")) == 0 )
  {
    result = igtl::ImageMessage::New();
  }
  else if ( messageType.compare(std::string("STATUS")) == 0 )
  {
    result = igtl::StatusMessage::New();
  }
  else if ( messageType.compare(std::string("POINT")) == 0 )
  {
    result = igtl::PointMessage::New();
  }
  else if ( messageType.compare(std::string("TRAJ")) == 0 )
  {
    result = igtl::TrajectoryMessage::New();
  }
  else if ( messageType.compare(std::string("STRING")) == 0 )
  {
    result = igtl::StringMessage::New();
  }
  else if ( messageType.compare(std::string("TDATA")) == 0 )
  {
    result = igtl::TrackingDataMessage::New();
  }
  else if ( messageType.compare(std::string("QTDATA")) == 0 )
  {
    result = igtl::QuaternionTrackingDataMessage::New();
  }
  else if ( messageType.compare(std::string("CAPABILITY")) == 0 )
  {
    result = igtl::CapabilityMessage::New();
  }
  else if ( messageType.compare(std::string("COMMAND")) == 0 )
  {
    result = igtl::CommandMessage::New();
  }

  // Must have a valid message at this point, otherwise its a programming bug.
  assert(result.IsNotNull());

  result->SetDeviceType(messageType);
  result->SetVersion(IGTL_HEADER_VERSION_3);
  result->AllocatePack();

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

#endif

} // end namespace

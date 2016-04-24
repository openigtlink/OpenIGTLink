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
    this->AddMessageType("RTS_TDATA", (PointerToMessageBaseNew)&igtl::RTSTrackingDataMessage::New);
    this->AddMessageType("STT_TDATA", (PointerToMessageBaseNew)&igtl::StartTrackingDataMessage::New);
    this->AddMessageType("STP_TDATA", (PointerToMessageBaseNew)&igtl::StopTrackingDataMessage::New);
    this->AddMessageType("QTDATA", (PointerToMessageBaseNew)&igtl::QuaternionTrackingDataMessage::New);
    this->AddMessageType("RTS_QTDATA", (PointerToMessageBaseNew)&igtl::RTSQuaternionTrackingDataMessage::New);
    this->AddMessageType("STT_QTDATA", (PointerToMessageBaseNew)&igtl::StartQuaternionTrackingDataMessage::New);
    this->AddMessageType("STP_QTDATA", (PointerToMessageBaseNew)&igtl::StopQuaternionTrackingDataMessage::New);
#endif //OpenIGTLink_PROTOCOL_VERSION >= 2
#if OpenIGTLink_PROTOCOL_VERSION >= 3
    this->AddMessageType("COMMAND", (PointerToMessageBaseNew)&igtl::CommandMessage::New);
    this->AddMessageType("RTS_COMMAND", (PointerToMessageBaseNew)&igtl::RTSCommandMessage::New);
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
  MessageFactory::PointerToMessageBaseNew MessageFactory::GetMessageTypeNewPointer(const std::string& messageTypeName) const
  {
    if ( this->IgtlMessageTypes.find(messageTypeName) != this->IgtlMessageTypes.end() )
    {
      return this->IgtlMessageTypes.find(messageTypeName)->second;
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

#if OpenIGTLink_PROTOCOL_VERSION >= 3
    std::string messageType(headerMsg->GetMessageType());
#else
    std::string messageType(headerMsg->GetDeviceType());
#endif

    if (!this->IsValid(headerMsg))
    {
      std::ostringstream oss;
      oss << "Error: In " __FILE__ ", line " << __LINE__ << ", receiving message of type:" << messageType << std::endl;
      throw std::invalid_argument(oss.str());
    }

    std::string messageTypeUpper(messageType);
    std::transform(messageTypeUpper.begin(), messageTypeUpper.end(), messageTypeUpper.begin(), ::toupper);

    igtl::MessageBase::Pointer result = NULL;
    if( GetMessageTypeNewPointer(messageTypeUpper) == NULL )
    {
      std::ostringstream oss;
      oss << "Error: In " __FILE__ ", line " << __LINE__ << ", unable to create message of type:" << messageTypeUpper << std::endl;
      throw std::invalid_argument(oss.str());
    }
    result = GetMessageTypeNewPointer(messageTypeUpper)();

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

#if OpenIGTLink_PROTOCOL_VERSION >= 3
    std::string messageType(headerMsg->GetMessageType());
#else
    std::string messageType(headerMsg->GetDeviceType());
#endif
    std::string messageTypeUpper(messageType);
    std::transform(messageTypeUpper.begin(), messageTypeUpper.end(), messageTypeUpper.begin(), ::toupper);

    if (!this->IsValid(headerMsg))
    {
      std::ostringstream oss;
      oss << "Error: In " __FILE__ ", line " << __LINE__ << ", receiving message of type:" << messageTypeUpper << std::endl;
      throw std::invalid_argument(oss.str());
    }

    igtl::MessageBase::Pointer result = NULL;
    if( GetMessageTypeNewPointer(messageTypeUpper) == NULL )
    {
      std::ostringstream oss;
      oss << "Error: In " __FILE__ ", line " << __LINE__ << ", unable to create message of type:" << messageTypeUpper << std::endl;
      throw std::invalid_argument(oss.str());
    }
    result = GetMessageTypeNewPointer(messageTypeUpper)();

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

    std::string messageTypeUpper(messageType);
    std::transform(messageTypeUpper.begin(), messageTypeUpper.end(), messageTypeUpper.begin(), ::toupper);

    igtl::MessageBase::Pointer result = NULL;
    if( GetMessageTypeNewPointer(messageTypeUpper) == NULL )
    {
      std::ostringstream oss;
      oss << "Error: In " __FILE__ ", line " << __LINE__ << ", unable to create message of type:" << messageTypeUpper << std::endl;
      throw std::invalid_argument(oss.str());
    }
    result = GetMessageTypeNewPointer(messageTypeUpper)();

    // Must have a valid message at this point, otherwise its a programming bug.
    assert(result.IsNotNull());

    result->SetDeviceType(messageTypeUpper);
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

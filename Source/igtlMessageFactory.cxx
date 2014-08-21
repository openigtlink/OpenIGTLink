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

#include <stdexcept>
#include <sstream>
#include <cassert>
#include <cstring>

namespace igtl
{

//-----------------------------------------------------------------------------
MessageFactory::MessageFactory()
{
  m_DeviceTypes.insert("TRANSFORM");
  m_DeviceTypes.insert("POSITION");
  m_DeviceTypes.insert("IMAGE");
  m_DeviceTypes.insert("STATUS");
#if OpenIGTLink_PROTOCOL_VERSION >= 2
  m_DeviceTypes.insert("POINT");
  m_DeviceTypes.insert("TRAJ");
  m_DeviceTypes.insert("STRING");
  m_DeviceTypes.insert("TDATA");
  m_DeviceTypes.insert("QTDATA");
  m_DeviceTypes.insert("CAPABILITY");
#endif //OpenIGTLink_PROTOCOL_VERSION >= 2
}


//-----------------------------------------------------------------------------
MessageFactory::~MessageFactory()
{
}


//-----------------------------------------------------------------------------
bool MessageFactory::IsValid(igtl::MessageHeader::Pointer headerMsg)
{
  bool result = false;

  if (headerMsg.IsNotNull() && m_DeviceTypes.find(headerMsg->GetDeviceType()) != m_DeviceTypes.end())
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
    oss << "Error: In " __FILE__ ", line " << __LINE__ << ", receiving message of type:" << headerMsg->GetDeviceType() << std::endl;
    throw std::invalid_argument(oss.str());
  }

  igtl::MessageBase::Pointer result = NULL;

  if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM") == 0)
  {
    result = igtl::TransformMessage::New();
  }
  else if (strcmp(headerMsg->GetDeviceType(), "POSITION") == 0)
  {
    result = igtl::PositionMessage::New();
  }
  else if (strcmp(headerMsg->GetDeviceType(), "IMAGE") == 0)
  {
    result = igtl::ImageMessage::New();
  }
  else if (strcmp(headerMsg->GetDeviceType(), "STATUS") == 0)
  {
    result = igtl::StatusMessage::New();
  }
#if OpenIGTLink_PROTOCOL_VERSION >= 2
  else if (strcmp(headerMsg->GetDeviceType(), "POINT") == 0)
  {
    result = igtl::PointMessage::New();
  }
  else if (strcmp(headerMsg->GetDeviceType(), "TRAJ") == 0)
  {
    result = igtl::TrajectoryMessage::New();
  }
  else if (strcmp(headerMsg->GetDeviceType(), "STRING") == 0)
  {
    result = igtl::StringMessage::New();
  }
  else if (strcmp(headerMsg->GetDeviceType(), "TDATA") == 0)
  {
    result = igtl::TrackingDataMessage::New();
  }
  else if (strcmp(headerMsg->GetDeviceType(), "QTDATA") == 0)
  {
    result = igtl::QuaternionTrackingDataMessage::New();
  }
  else if (strcmp(headerMsg->GetDeviceType(), "CAPABILITY") == 0)
  {
    result = igtl::CapabilityMessage::New();
  }
#endif //OpenIGTLink_PROTOCOL_VERSION >= 2

  // Must have a valid message at this point, otherwise its a programming bug.
  assert(result.IsNotNull());

  result->SetMessageHeader(headerMsg);
  result->AllocatePack();

  return result;
}

} // end namespace

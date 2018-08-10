/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igtlMessageFactory_h
#define __igtlMessageFactory_h

#include "igtlMacro.h"
#include "igtlMessageBase.h"
#include "igtlMessageHeader.h"
#include "igtlObject.h"
#include "igtl_header.h"

#include <map>

namespace igtl
{

class IGTLCommon_EXPORT MessageFactory: public Object
{
public:
  igtlTypeMacro(MessageFactory, Object)
  igtlNewMacro(MessageFactory);

  /*! Function pointer for storing New() static methods of igtl::MessageBase classes */ 
  typedef igtl::MessageBase::Pointer (*PointerToMessageBaseNew)(); 

  /// Add message type name and pointer to IGTL message new function 
  /// Usage: AddMessageType( "IMAGE", (PointerToMessageBaseNew)&igtl::ImageMessage::New );  
  /// \param messageTypeName The name of the message type
  /// \param messageTypeNewPointer Function pointer to the message type new function (e.g. (PointerToMessageBaseNew)&igtl::ImageMessage::New )
  virtual void AddMessageType(const std::string& messageTypeName, MessageFactory::PointerToMessageBaseNew messageTypeNewPointer);

  /// Get pointer to message type new function, or NULL if the message type not registered 
  /// Usage: igtl::MessageBase::Pointer message = GetMessageTypeNewPointer("IMAGE")(); 
  /// Returns NULL if message type is not found
  virtual MessageFactory::PointerToMessageBaseNew GetMessageTypeNewPointer(const std::string& messageTypeName) const; 

  /// Checks that headerMsg is not null and the headerMsg->GetDeviceType() refers to a valid type, returning true if valid, and false otherwise.
  bool IsValid(igtl::MessageHeader::Pointer headerMsg);

  /// Checks that headerMsg is not null and the headerMsg->GetDeviceType() refers to a valid type, returning true if valid, and false otherwise.
  bool IsValid(igtl::MessageHeader::Pointer headerMsg) const;

  /// LEGACY method, use CreateReceiveMessage instead
  /// Constructs a message from the given header.
  /// Returns NULL if headerMsg is NULL.
  /// Returns NULL if this->IsValid(headerMsg) returns false.
  /// Creates message, sets header onto message and calls AllocateBuffer() on the message.
  igtl::MessageBase::Pointer GetMessage(igtl::MessageHeader::Pointer headerMsg);

  /// Constructs a message header.
  /// Returns NULL if headerMsg is NULL.
  /// Returns NULL if this->IsValid(headerMsg) returns false.
  /// Creates message, calls InitBuffer()
  igtl::MessageHeader::Pointer CreateHeaderMessage(int headerVersion) const;

  /// Constructs a message from the given populated header.
  /// Returns NULL if headerMsg is NULL.
  /// Returns NULL if this->IsValid(headerMsg) returns false.
  /// Creates message, sets header onto message and calls AllocatePack() on the message.
  igtl::MessageBase::Pointer CreateReceiveMessage(igtl::MessageHeader::Pointer headerMsg) const;

  /// Constructs an empty message from the given message type.
  /// Returns NULL if messageType is empty.
  /// Creates message, sets header onto message and calls AllocatePack() on the message.
  igtl::MessageBase::Pointer CreateSendMessage(const std::string& messageType, int headerVersion) const;

  /// Return the list of known message types
  void GetAvailableMessageTypes(std::vector<std::string>& types) const;

protected:
  MessageFactory();
  ~MessageFactory();

private:
  /*! Map igt message types and the New() static methods of igtl::MessageBase classes */ 
  std::map<std::string, PointerToMessageBaseNew> IgtlMessageTypes; 

}; // end class

} // end namespace

#endif // __igtlMessageFactory_h

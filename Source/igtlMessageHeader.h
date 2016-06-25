/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlMessageHeader_h
#define __igtlMessageHeader_h

namespace igtl
{
/// The MessageHeader class is used to receive and parse general headers
/// to prepare for receiving body data. The class is currently just the alias
/// of MessageBase class. Please refer igtlMessageBase.h for more details and
/// the implementation of the class.
///
/// The following is the typical unpacking (deserialization) procedures
/// using igtl::MessssageHeader class:
///
///     // Create instance and set Device Name
///     igtl::MessageBase::Pointer headerMsg;
///     headerMsg = igtl::MessageBase::New();
///
///     // Initialize receive buffer
///     // Set up memory area to and receive the general header and unpack
///     headerMsg->InitPack();
///
///     socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
///     headerMsg->Unpack();
///
///     // Check data type string
///     if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM"))
///       {
///         igtl::TransformMessage::Pointer transMsg;
///         transMsg = igtl::TransformMessage::New();
///         transMsg->SetMessageHeader(headerMsg);
///         transMsg->AllocatePack();
///
///         // Receive transform data from the socket//
///         socket->Receive(transMsg->GetPackBodyPointer(), transMsg->GetPackBodySize());
///
///         // Deserialize the transform data
///         transMsg->Unpack();
///
///         // Retrive the transform data
///         igtl::Matrix4x4 matrix;
///         transMsg->GetMatrix(matrix);
///
///         ....
///
///       }
///     else if (strcmp(headerMsg->GetDeviceType(), "IMAGE"))
///       {
///         igtl::ImageMessage::Pointer imageMsg;
///         imageMsg = igtl::ImageMessage::New();
///         transMsg->SetMessageHeader(headerMsg);
///         imageMsg->AllocatePack();
///         socket->Receive(imageMsg->GetPackBodyPointer(), imageMsg->GetPackBodySize());
///         imageMsg->Unpack();
///       }
///     else if (...)
///       {
///          ...
///       }
  class MessageBase;
  typedef class MessageBase MessageHeader;

};

#include "igtlMessageBase.h"


#endif //__igtlMessageHeader_h

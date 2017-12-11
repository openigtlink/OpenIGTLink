/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <igtlMessageFactory.h>
#include <igtlMessageHeader.h>
#include <igtlTransformMessage.h>

#include <stdexcept>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

namespace igtl
{
  // Create a nonsense message type that won't exist in main library.
  igtlCreateDefaultQueryMessageClass(BananaMessage, "BANANA")
}

/**
 * Author: Matt Clarkson (m.clarkson@ucl.ac.uk).
 */
int main(int , char * [] )
{

  igtl::MessageFactory::Pointer factory = igtl::MessageFactory::New();

  igtl::MessageHeader::Pointer header = NULL;

  if (factory->IsValid(header))
    {
    std::cerr << "A null header is not valid." << std::endl;
    return EXIT_FAILURE;
    }

  header = igtl::MessageHeader::New();

  if (factory->IsValid(header))
    {
    std::cerr << "A header without a DeviceType e.g. STRING, TRANSFORM is invalid." << std::endl;
    return EXIT_FAILURE;
    }

  igtl::TransformMessage::Pointer transformMessage = igtl::TransformMessage::New();

  if (!factory->IsValid(transformMessage.GetPointer()))
    {
    std::cerr << "The IsValid method should check for not null, and a valid DeviceType. TRANSFORM should be valid." << std::endl;
    return EXIT_FAILURE;
    }

  // Test with an invalid message.
  igtl::BananaMessage::Pointer bananaMessage = igtl::BananaMessage::New();

  // Check firstly its not valid.
  if (factory->IsValid(bananaMessage.GetPointer()))
    {
    std::cerr << "The IsValid method should fail for BANANA messages." << std::endl;
    return EXIT_FAILURE;
    }
  
  header->SetDeviceType("BANANA");
  header->SetHeaderVersion(IGTL_HEADER_VERSION_1);

  if (factory->GetMessage(header))
    {
    std::cerr << "The GetMessage method should return NULL for BANANA messages." << std::endl;
    return EXIT_FAILURE;
    }
  
  if (factory->CreateReceiveMessage(header))
    {
    std::cerr << "The CreateReceiveMessage method should return NULL for BANANA messages." << std::endl;
    return EXIT_FAILURE;
    }
  
  if (factory->CreateSendMessage("BANANA", IGTL_HEADER_VERSION_1))
    {
    std::cerr << "The CreateSendMessage method should return NULL for BANANA messages." << std::endl;
    return EXIT_FAILURE;
    }
  
  if (factory->GetMessageTypeNewPointer("BANANA"))
    {
    std::cerr << "The CreateSendMessage method should return NULL for BANANA messages." << std::endl;
    return EXIT_FAILURE;
    }
  
  return EXIT_SUCCESS;
}


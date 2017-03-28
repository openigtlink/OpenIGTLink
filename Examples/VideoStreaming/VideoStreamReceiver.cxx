/*=========================================================================
 
 Program:   Video Streaming Receiver
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "VideoStreamIGTLinkReceiver.h"

#if defined(ANDROID_NDK) || defined(APPLE_IOS) || defined (WINDOWS_PHONE)
extern "C" int EncMain (int argc, char** argv)
#else
int main (int argc, char** argv)
#endif
{
  //char * cfgFileName = argv[1];
  if (argc != 2) // check number of arguments
  {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <configurationfile>"    << std::endl;
    std::cerr << "    <configurationfile> : file name "  << std::endl;
    exit(0);
  }
  VideoStreamIGTLinkReceiver receiver= VideoStreamIGTLinkReceiver(argv[1]);
  if (receiver.InitializeClient())
  {
    if (receiver.GetTransportMethod() == receiver.RunOnTCP)
    {
      receiver.RunOnTCPSocket();
    }
    else if(receiver.GetTransportMethod() == receiver.RunOnUDP)
    {
      receiver.RunOnUDPSocket();
    }
  }
  return 0;
}

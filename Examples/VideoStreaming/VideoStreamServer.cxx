/*=========================================================================

  Program:   Video Streaming server
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "VideoStreamIGTLinkServer.h"

#if defined(ANDROID_NDK) || defined(APPLE_IOS) || defined (WINDOWS_PHONE)
extern "C" int EncMain (int argc, char** argv)
#else
int main (int argc, char** argv)
#endif
{
  if (argc != 2) // check number of arguments
  {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <configurationfile>"    << std::endl;
    std::cerr << "    <configurationfile> : file name "  << std::endl;
    exit(0);
  }
  VideoStreamIGTLinkServer server(argv[1]);
  server.InitializeServer();
  server.SetWaitSTTCommand(true);
  if(server.transportMethod==server.UseTCP)
  {
    server.StartTCPServer();
  }
  else if(server.transportMethod==server.UseUDP)
  {
    server.StartUDPServer();
  }
  server.StartSendPacketThread();
  server.StartReadFrameThread(30);
  while(1)
  {
    igtl::Sleep(10);
    if(server.transportMethod==server.UseTCP)
    {
      if(server.GetServerConnectStatus())
      {
        if (!server.GetInitializationStatus())
        {
          server.InitializeServer();
        }
        if(server.useCompress == 1)
        {
          server.EncodeFile();
        }
        else
        {
          server.SendOriginalData();
        }
      }
    }
    else if(server.transportMethod==server.UseUDP)
    {
      if (!server.GetInitializationStatus())
      {
        server.InitializeServer();
      }
      if(server.useCompress == 1)
      {
        server.EncodeFile();
      }
      else
      {
        server.SendOriginalData();
      }
    }
  }
  server.Stop();
  return 0;
}

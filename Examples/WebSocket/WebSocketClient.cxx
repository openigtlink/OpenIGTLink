
#include "igtlWebClientSocket.h"

#include <iostream>
#include <math.h>
#include <cstdlib>
#include <cstring>

#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlImageMessage.h"

#include "igtlTrackingDataMessage.h"
#include "igtlMultiThreader.h"

#include <fstream>
#include <iostream>
#include <set>
#include <streambuf>
#include <string>

/**
 * The telemetry server accepts connections and sends a message every second to
 * each client containing an integer count. This example can be used as the
 * basis for programs that expose a stream of telemetry data for logging,
 * dashboards, etc.
 *
 * This example uses the timer based concurrency method and is self contained
 * and singled threaded. Refer to telemetry client for an example of a similar
 * telemetry setup using threads rather than timers.
 *
 * This example also includes an example simple HTTP server that serves a web
 * dashboard displaying the count. This simple design is suitable for use
 * delivering a small number of files to a small number of clients. It is ideal
 * for cases like embedded dashboards that don't want the complexity of an extra
 * HTTP server to serve static files.
 *
 * This design *will* fall over under high traffic or DoS conditions. In such
 * cases you are much better off proxying to a real HTTP server for the http
 * requests.
 */

int ReceiveTrackingData(igtl::TrackingDataMessage::Pointer trackingData)
{
  std::cerr << "Receiving TDATA data type." << std::endl;
  
  //------------------------------------------------------------
  // Allocate TrackingData Message Class
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
      std::cerr << "================================" << std::endl;
      }
    return 1;
    }
  return 0;
}

int main(int argc, char* argv[]) {
  webSocketClient s;
  
  if (argc == 1) {
    std::cerr << "Usage: " << argv[0] << " <hostname> <port> <fps>"    << std::endl;
    std::cerr << "    <hostname> : IP or host name"                    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    return 1;
  }
  
  const char*  hostname = argv[1];
  int    port     = atoi(argv[2]);
  // Create a thread to recevie the message
  igtl::TrackingDataMessage::Pointer trackingData;
  websocketpp::lib::thread client_thread(websocketpp::lib::bind(&webSocketClient::ConnectToServer, &s, hostname, port));
  
  void* data;
  int length;
  while(1)
    {
    length = IGTL_HEADER_SIZE;
    data = malloc(length);
    igtl::MessageHeader::Pointer headerMsg;
    headerMsg = igtl::MessageHeader::New();
    headerMsg->InitPack();
    s.Receive(headerMsg->GetPackPointer(), length);
    headerMsg->Unpack();
    trackingData = igtl::TrackingDataMessage::New();
    trackingData->SetMessageHeader(headerMsg);
    trackingData->AllocatePack();
    length = trackingData->GetPackBodySize();
    s.Receive(trackingData->GetPackBodyPointer(), length);
    ReceiveTrackingData(trackingData);
    }
  return 0;
}


#include "igtlWebServerSocket.h"
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <cstring>

#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlImageMessage.h"
#include "igtlServerSocket.h"
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

void GetRandomTestMatrix(igtl::Matrix4x4& matrix, float phi, float theta);


int PackTrackingData(igtl::TrackingDataMessage::Pointer trackingMsg)
{
  
  static float phi0   = 0.0;
  static float theta0 = 0.0;
  static float phi1   = 0.0;
  static float theta1 = 0.0;
  static float phi2   = 0.0;
  static float theta2 = 0.0;
  
  igtl::TrackingDataElement::Pointer trackElement0;
  trackElement0 = igtl::TrackingDataElement::New();
  trackElement0->SetName("Channel 0");
  trackElement0->SetType(igtl::TrackingDataElement::TYPE_TRACKER);
  
  igtl::TrackingDataElement::Pointer trackElement1;
  trackElement1 = igtl::TrackingDataElement::New();
  trackElement1->SetName("Channel 1");
  trackElement1->SetType(igtl::TrackingDataElement::TYPE_6D);
  
  igtl::TrackingDataElement::Pointer trackElement2;
  trackElement2 = igtl::TrackingDataElement::New();
  trackElement2->SetName("Channel 2");
  trackElement2->SetType(igtl::TrackingDataElement::TYPE_5D);
  
  trackingMsg->AddTrackingDataElement(trackElement0);
  trackingMsg->AddTrackingDataElement(trackElement1);
  trackingMsg->AddTrackingDataElement(trackElement2);
  
  igtl::Matrix4x4 matrix;
  igtl::TrackingDataElement::Pointer ptr;
  
  // Channel 0
  trackingMsg->GetTrackingDataElement(0, ptr);
  GetRandomTestMatrix(matrix, phi0, theta0);
  ptr->SetMatrix(matrix);
  
  // Channel 1
  trackingMsg->GetTrackingDataElement(1, ptr);
  GetRandomTestMatrix(matrix, phi1, theta1);
  ptr->SetMatrix(matrix);
  
  // Channel 2
  trackingMsg->GetTrackingDataElement(2, ptr);
  GetRandomTestMatrix(matrix, phi2, theta2);
  ptr->SetMatrix(matrix);
  
  trackingMsg->Pack();
  
  phi0 += 0.1;
  phi1 += 0.2;
  phi2 += 0.3;
  theta0 += 0.2;
  theta1 += 0.1;
  theta2 += 0.05;
  
  return 0;
}
//------------------------------------------------------------
// Function to generate random matrix.
void GetRandomTestMatrix(igtl::Matrix4x4& matrix, float phi, float theta)
{
  float position[3];
  float orientation[4];
  
  // random position
  position[0] = 50.0 * cos(phi);
  position[1] = 50.0 * sin(phi);
  position[2] = 50.0 * cos(phi);
  phi = phi + 0.2;
  
  // random orientation
  orientation[0]=0.0;
  orientation[1]=0.6666666666*cos(theta);
  orientation[2]=0.577350269189626;
  orientation[3]=0.6666666666*sin(theta);
  theta = theta + 0.1;
  
  //igtl::Matrix4x4 matrix;
  igtl::QuaternionToMatrix(orientation, matrix);
  
  matrix[0][3] = position[0];
  matrix[1][3] = position[1];
  matrix[2][3] = position[2];
  
  //igtl::PrintMatrix(matrix);
}


int main(int argc, char* argv[]) {
  
  
  std::string docroot;
  uint16_t port = 9002;
  
  if (argc == 1) {
    std::cout << "Usage: telemetry_server [documentroot] [port]" << std::endl;
    return 1;
  }
  
  if (argc >= 2) {
    docroot = std::string(argv[1]);
  }
  
  if (argc >= 3) {
    int i = atoi(argv[2]);
    if (i <= 0 || i > 65535) {
      std::cout << "invalid port" << std::endl;
      return 1;
    }
    
    port = uint16_t(i);
  }
  
  if(strcmp(&docroot.back(),"/") != 0)
    {
    docroot.append("/");
    }
  
  //s.CreateHTTPServer(docroot, port); // 100 ms interval
  webSocketServer s;
  s.m_docroot = docroot;
  s.port = port;
  try
  {
  websocketpp::lib::thread t(websocketpp::lib::bind(&webSocketServer::CreateHTTPServer, &s, docroot, port));
  while (1)
    {
    igtl::TrackingDataMessage::Pointer trackingMsg;
    trackingMsg = igtl::TrackingDataMessage::New();
    trackingMsg->SetDeviceName("Tracker");
    PackTrackingData(trackingMsg);
    s.Send(trackingMsg->GetPackPointer(), trackingMsg->GetPackSize());
    igtl::Sleep(200);
    }
  t.join();
  }
  catch (websocketpp::exception const & e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}

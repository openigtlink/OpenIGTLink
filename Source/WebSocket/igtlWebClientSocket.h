//
//  igtlWebSocket.hpp
//  OpenIGTLink
//
//  Created by Longquan Chen on 1/20/17.
//
//

#ifndef igtlWebClientSocket_h
#define igtlWebClientSocket_h


#include "websocketpp/config/asio_no_tls.hpp"
#include <websocketpp/config/asio_no_tls_client.hpp>

#include "websocketpp/client.hpp"

#include "igtlOSUtil.h"
#include <websocketpp/common/thread.hpp>
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::connection_hdl;
using websocketpp::lib::bind;
using websocketpp::lib::mutex;
using websocketpp::lib::lock_guard;
using websocketpp::lib::unique_lock;
using websocketpp::lib::condition_variable;

#include <fstream>
#include <iostream>
#include <set>
#include <streambuf>
#include <string>

class webSocketClient {
public:
  typedef websocketpp::client<websocketpp::config::asio_client> client;
  typedef websocketpp::lib::lock_guard<websocketpp::lib::mutex> scoped_lock;
  
  webSocketClient() ;
  
  // The open handler will signal that we are ready to start sending data
  void on_open(websocketpp::connection_hdl);
  
  
  // The close handler will signal that we should stop sending data
  void on_close(websocketpp::connection_hdl) ;
  
  // The fail handler will signal that we should stop sending data
  void on_fail(websocketpp::connection_hdl) ;
  
  int ConnectToServer(const char* hostName, int port);
  
  int Send(const void* data, int length);
  
  int Receive(void* data, int& length);
  
  void on_message(websocketpp::connection_hdl hdl, message_ptr msg);
  
private:
  client m_client;
  websocketpp::connection_hdl m_hdl;
  websocketpp::lib::mutex m_lock;
  bool m_open;
  bool m_done;
  std::queue<std::string> m_messages;
  
  mutex m_action_lock;
  mutex m_connection_lock;
  condition_variable m_action_cond;
};



#endif /* igtlWebClientSocket_h */

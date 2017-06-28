//
//  igtlWebServerSocket.hpp
//  OpenIGTLink
//
//  Created by Longquan Chen on 1/20/17.
//
//

#ifndef igtlWebServerSocket_h
#define igtlWebServerSocket_h


#include "websocketpp/config/asio_no_tls.hpp"

#include "websocketpp/server.hpp"

#include "igtlOSUtil.h"
#include <websocketpp/common/thread.hpp>
#include <websocketpp/server.hpp>
typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

using websocketpp::lib::thread;
using websocketpp::lib::mutex;
using websocketpp::lib::lock_guard;
using websocketpp::lib::unique_lock;
using websocketpp::lib::condition_variable;

#include <fstream>
#include <iostream>
#include <set>
#include <streambuf>
#include <string>


enum action_type {
  SUBSCRIBE,
  UNSUBSCRIBE,
  MESSAGE,
  SEVERCREATED
};

struct action {
  action(action_type t, connection_hdl h) : type(t), hdl(h) {}
  action(action_type t, connection_hdl h, std::string  m)
  : type(t), hdl(h), msg(m) {}
  
  action(action_type t):type(t){};
  action(action_type t, std::string  m):type(t), msg(m){};
  action_type type;
  websocketpp::connection_hdl hdl;
  std::string msg;
};

/**
 */

class webSocketServer{
  public:
    typedef websocketpp::connection_hdl connection_hdl;
    typedef websocketpp::server<websocketpp::config::asio> server;
    webSocketServer();
    int CreateServer(uint16_t port);
    int CreateHTTPServer(std::string docroot, uint16_t port);
  
    void Send(void * inputMessage, size_t len);
    void on_http(connection_hdl hdl);
    void on_open(connection_hdl hdl);
    void on_close(connection_hdl hdl);
    webSocketServer* WaitForConnection(unsigned long msec);
    void SetTimeInterval(unsigned int time);
    void on_timer();
    void set_timer();
    server m_endpoint;
    std::string m_docroot;
    bool serverCreated;
    uint16_t port;
  private:
    typedef std::set<connection_hdl,std::owner_less<connection_hdl> > con_list;
    int run(std::string docroot, uint16_t port);
  
    con_list m_connections;
    server::timer_ptr m_timer;
  
  
    int status;
  
    unsigned int m_timeInterval;
    // data count
    uint64_t m_count;
    std::queue<action> m_actions;
    
    mutex m_action_lock;
    mutex m_connection_lock;
    condition_variable m_action_cond;
};


#endif /* igtlWebServerSocket_h */

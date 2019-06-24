//
//  igtlWebClientSocket.cpp
//  OpenIGTLink
//
//  Created by Longquan Chen on 1/20/17.
//
//

#include "igtlWebClientSocket.h"

webSocketClient::webSocketClient() : m_open(false),m_done(false) {
  // set up access channels to only log interesting things
  m_client.clear_access_channels(websocketpp::log::alevel::all);
  m_client.set_access_channels(websocketpp::log::alevel::connect);
  m_client.set_access_channels(websocketpp::log::alevel::disconnect);
  m_client.set_access_channels(websocketpp::log::alevel::app);
  
  // Initialize the Asio transport policy
  m_client.init_asio();
  
  // Bind the handlers we are using
  using websocketpp::lib::placeholders::_1;
  using websocketpp::lib::placeholders::_2;
  using websocketpp::lib::bind;
  m_client.set_open_handler(bind(&webSocketClient::on_open,this,_1));
  m_client.set_close_handler(bind(&webSocketClient::on_close,this,_1));
  m_client.set_fail_handler(bind(&webSocketClient::on_fail,this,_1));
  m_client.set_message_handler(bind(&webSocketClient::on_message,this, _1, _2));
}

int webSocketClient::ConnectToServer(const char* hostName, int port)
{
  websocketpp::lib::error_code ec;
  std::string uri;
  uri.append("http://");
  uri.append(hostName);
  uri.append(":");
  uri.append(std::to_string(port));
  client::connection_ptr con = m_client.get_connection(uri, ec);
  if (ec)
    {
    m_client.get_alog().write(websocketpp::log::alevel::app,
                              "Get Connection Error: "+ec.message());
    return -1;
    }
  
  // Grab a handle for this connection so we can talk to it in a thread
  // safe manor after the event loop starts.
  m_hdl = con->get_handle();
  
  // Queue the connection. No DNS queries or network connections will be
  // made until the io_service event loop is run.
  m_client.connect(con);
  
  m_client.run();
  
  return 0;
}

// The open handler will signal that we are ready to start sending telemetry
void webSocketClient::on_open(websocketpp::connection_hdl) {
  m_client.get_alog().write(websocketpp::log::alevel::app,
                            "Connection opened, starting telemetry!");
  
  scoped_lock guard(m_lock);
  m_open = true;
}

// The close handler will signal that we should stop sending telemetry
void webSocketClient::on_close(websocketpp::connection_hdl) {
  m_client.get_alog().write(websocketpp::log::alevel::app,
                            "Connection closed, stopping telemetry!");
  
  scoped_lock guard(m_lock);
  m_done = true;
}

// The fail handler will signal that we should stop sending telemetry
void webSocketClient::on_fail(websocketpp::connection_hdl) {
  m_client.get_alog().write(websocketpp::log::alevel::app,
                            "Connection failed, stopping telemetry!");
  
  scoped_lock guard(m_lock);
  m_done = true;
}

void webSocketClient::on_message(websocketpp::connection_hdl hdl, message_ptr msg) {
  {
  unique_lock<mutex> lock(m_action_lock);
  std::string str(msg->get_raw_payload());
  m_messages.push(str);
  }
  m_action_cond.notify_one();
}


int webSocketClient::Receive(void* data, int& length)
{
  unique_lock<mutex> lock(m_action_lock);
  
  while(m_messages.empty())
    {
    m_action_cond.wait(lock);
    }
  std::string a = m_messages.front();
  int messageLen = a.length();
  if(length && messageLen>=length)
    {
    memcpy(data, a.c_str(), length);
    }
  if (messageLen == length)
    {
    m_messages.pop();
    }
  else
    {
    a.erase(a.begin(), a.begin()+length);
    messageLen = a.length();
    m_messages.front()= a;
    }
  lock.unlock();
  return length;
}

int webSocketClient::Send(const void* data, int length)
{
  std::stringstream val;
  websocketpp::lib::error_code ec;
  m_client.send(m_hdl,val.str(),websocketpp::frame::opcode::binary,ec);
  
  // The most likely error that we will get is that the connection is
  // not in the right state. Usually this means we tried to send a
  // message to a connection that was closed or in the process of
  // closing. While many errors here can be easily recovered from,
  // in this simple example, we'll stop the telemetry loop.
  if (ec)
    {
    m_client.get_alog().write(websocketpp::log::alevel::app,
                              "Send Error: "+ec.message());
    return 0;
    }
  return 1;
}
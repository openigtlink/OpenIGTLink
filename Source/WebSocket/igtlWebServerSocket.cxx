//
//  igtlWebServerSocket.cpp
//  OpenIGTLink
//
//  Created by Longquan Chen on 1/20/17.
//
//

#include "igtlWebServerSocket.h"

webSocketServer::webSocketServer() : m_count(0) {
  // set up access channels to only log interesting things
  m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
  m_endpoint.set_access_channels(websocketpp::log::alevel::access_core);
  m_endpoint.set_access_channels(websocketpp::log::alevel::app);
  
  // Initialize the Asio transport policy
  m_endpoint.init_asio();
  
  // Bind the handlers we are using
  using websocketpp::lib::placeholders::_1;
  m_endpoint.set_open_handler(bind(&webSocketServer::on_open,this,_1));
  m_endpoint.set_close_handler(bind(&webSocketServer::on_close,this,_1));
  m_endpoint.set_http_handler(bind(&webSocketServer::on_http,this,_1));
  m_timeInterval = 1;
}

void webSocketServer::SetTimeInterval(unsigned int time)
{
  this->m_timeInterval = time;
}

int webSocketServer::CreateServer(uint16_t port)
{
  return this->run("",port);
}

int webSocketServer::CreateHTTPServer(std::string docroot, uint16_t port)
{
  m_docroot = docroot;
  this->port = port;
  return this->run(docroot,port);
}

webSocketServer* webSocketServer::WaitForConnection(unsigned long msec )
{
  if(m_connections.size())
    {
    return this;
    }
  else
    {
    igtl::Sleep(msec);
    return NULL;
    }
}

int webSocketServer::run(std::string docroot, uint16_t port) {
  m_docroot = docroot;
  serverCreated = false;
  //------------------------------------------------------------
  // Get thread information
  //igtl::MultiThreader::ThreadInfo* info =
  //static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);
  std::stringstream ss;
  ss << "Running server on port "<< this->port <<" using docroot=" << this->m_docroot;
  this->m_endpoint.get_alog().write(websocketpp::log::alevel::app,ss.str());
  
  
  // listen on specified port
  this->m_endpoint.listen(this->port);
  
  // Start the server accept loop
  this->m_endpoint.start_accept();
  
  set_timer();
  // Start the ASIO io_service run loop
  try
  {
  this->m_endpoint.run();
  }
  catch (websocketpp::exception const & e)
  {
  std::cout << e.what() << std::endl;
  this->status = -1;
  }
  this->status = 0;
  return status;
}

void webSocketServer::Send(void * inputMessage, size_t len)
{
  {
  unique_lock<mutex> lock(m_action_lock);
  char * str = new char[len];
  memcpy(str, inputMessage, len);
  std::string msg(str, len);
  m_actions.push(action(MESSAGE,msg));
  delete [] str;
  }
  m_action_cond.notify_one();
  m_count++;
}

void webSocketServer::set_timer() {
  m_timer = m_endpoint.set_timer(this->m_timeInterval,
                                 websocketpp::lib::bind( &webSocketServer::on_timer, this)
                                 );
}

void webSocketServer::on_timer() {
  // Broadcast count to all connections
  unique_lock<mutex> lock(m_action_lock);
  
  while(m_actions.empty()) {
    m_action_cond.wait(lock);
  }
  action a = m_actions.front();
  m_actions.pop();
  lock.unlock();
  
  con_list::iterator it;
  for (it = m_connections.begin(); it != m_connections.end(); ++it) {
    if (a.msg.length())
      {
      m_endpoint.send(*it,a.msg,websocketpp::frame::opcode::binary);
      }
  }
  // set timer for next telemetry check
  set_timer();
}

void webSocketServer::on_http(connection_hdl hdl) {
  // Upgrade our connection handle to a full connection_ptr
  server::connection_ptr con = m_endpoint.get_con_from_hdl(hdl);
  
  std::ifstream file;
  std::string filename = con->get_resource();
  std::string response;
  
  //m_endpoint.get_alog().write(websocketpp::log::alevel::app, "http request1: "+filename);
  
  if (filename == "/")
    {
    filename = m_docroot+"index.html";
    }
  else
    {
    filename = m_docroot+filename.substr(1);
    }
  
  //m_endpoint.get_alog().write(websocketpp::log::alevel::app, "http request2: "+filename);
  
  file.open(filename.c_str(), std::ios::in);
  if (!file)
    {
    // 404 error
    std::stringstream ss;
    
    ss << "<!doctype html><html><head>"
    << "<title>Error 404 (Resource not found)</title><body>"
    << "<h1>Error 404</h1>"
    << "<p>The requested URL " << filename << " was not found on this server.</p>"
    << "</body></head></html>";
    
    con->set_body(ss.str());
    con->set_status(websocketpp::http::status_code::not_found);
    return;
    }
  
  file.seekg(0, std::ios::end);
  response.reserve(file.tellg());
  file.seekg(0, std::ios::beg);
  
  response.assign((std::istreambuf_iterator<char>(file)),
                  std::istreambuf_iterator<char>());
  
  con->set_body(response);
  con->set_status(websocketpp::http::status_code::ok);
}

void webSocketServer::on_open(connection_hdl hdl) {
  m_connections.insert(hdl);
}

void webSocketServer::on_close(connection_hdl hdl) {
  m_connections.erase(hdl);
}

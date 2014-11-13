/*
 *  JsonRpc-Cpp - JSON-RPC implementation.
 *  Copyright (C) 2008-2011 Sebastien Vincent <sebastien.vincent@cppextrem.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file jsonrpc_tcpserver.cpp
 * \brief JSON-RPC TCP server.
 * \author Sebastien Vincent
 */

#include <stdexcept>



#include "jsonrpc_websockserver.h"

#include "netstring.h"

#include <cstring>
#include <cerrno>



namespace Json 
{

  namespace Rpc
  {


    WebsockServer::WebsockServer(const std::string& address, uint16_t port) : Server(address, port)
    {
      m_server = new WebSockServer();
    }

    WebsockServer::~WebsockServer()
    {
       
    }
    void WebsockServer::on_open( websocketpp::connection_hdl hdl) 
    {

        std::cout << "WebsockServer::on_open websocket server" << std::endl;

        m_connections[hdl] = -1;

    }
     void WebsockServer::on_close( websocketpp::connection_hdl hdl) 
    {
        map_connection::iterator itor;
        
        itor = m_connections.find(hdl);

        if(itor != m_connections.end())
        {
          m_connections.erase(itor);
        }

        std::cout << "Receive Data in JSON-RPC websocket server" << std::endl;
        
    } 
    void WebsockServer::on_message(websocketpp::connection_hdl hdl, message_ptr msg) 
    {

        std::cout << "Receive Data in JSON-RPC websocket server" << std::endl;

        std::cout << msg->get_payload() << std::endl;

        WsRecv(hdl,msg);
        
    } 

    ssize_t WebsockServer::Send(int fd, const std::string& data)
    {
      fd = 0;
      std::string strTemp = data;
      //data = std::string("");
      return 0;
    }

    bool WebsockServer::Recv(int fd)
    {
      /*this function is not used within websocket*/
      fd = 0; 
        return false;
    }

    void WebsockServer::WaitMessage(uint32_t ms)
    {
      ms = 0;
    }

    bool WebsockServer::Listen() const
    {
      try 
      {
        m_server->listen(GetPort());
        m_server->start_accept();
        m_server->run();

      } 
      catch (const std::exception & e) 
      {
        return false;
      } 
      catch (websocketpp::lib::error_code e) 
      {
        return false;
      }
      catch (...) 
      {
        return false;
      }
      return true;
    }

    bool WebsockServer::Accept()
    {
      
      return true;
    }

    void WebsockServer::Close()
    {
      /* close all client sockets */
        m_server->stop();

    }
    bool WebsockServer::WsRecv(websocketpp::connection_hdl hdl, message_ptr msg)
    {
        Json::Value response;

        std::string strData  = msg->get_payload();

        m_jsonHandler.Process(strData,response);

        if(response != Json::Value::null)
        {

          std::string rep = m_jsonHandler.GetString(response);
         
          if(GetEncapsulatedFormat() == Json::Rpc::NETSTRING)
          {
              rep = netstring::encode(rep);
          }

          try 
          {
            m_server->send(hdl, rep, msg->get_opcode());
          }
          catch (const websocketpp::lib::error_code& e) 
          {
            return false;
          }

        }

        return true;
    }
    bool WebsockServer::Bind()
    {
      

      m_server->set_access_channels(websocketpp::log::alevel::all);

      m_server->clear_access_channels(websocketpp::log::alevel::frame_payload);

      m_server->init_asio();

      m_server->set_message_handler(bind(&WebsockServer::on_message,this,::_1,::_2));

      m_server->set_close_handler(bind(&WebsockServer::on_close,this,::_1));

      m_server->set_open_handler(bind(&WebsockServer::on_open,this,::_1));
      return true;
    }
    
    const std::list<int> WebsockServer::GetClients() const
    {
      return m_clients;
    }

  } /* namespace Rpc */

} /* namespace Json */


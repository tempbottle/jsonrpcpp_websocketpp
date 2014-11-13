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
 * \file jsonrpc_WebsockServer.h
 * \brief JSON-RPC TCP server.
 * \author Sebastien Vincent
 */

#ifndef JSONRPC_WEBSOCKSERVER_H
#define JSONRPC_WEBSOCKSERVER_H

#include <list>

#include "jsonrpc_common.h"
#include "jsonrpc_server.h"
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>


typedef websocketpp::server<websocketpp::config::asio> WebSockServer;

typedef WebSockServer::message_ptr message_ptr;

typedef int connection_div;

typedef std::map<websocketpp::connection_hdl,connection_div> map_connection;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

namespace Json
{

  namespace Rpc
  {
    /**
     * \class WebsockServer
     * \brief JSON-RPC TCP server implementation.
     */
    class WebsockServer : public Server
    {
      public:
        /**
         * \brief Constructor.
         * \param address network address or FQDN to bind
         * \param port local port to bind
         */
        WebsockServer(const std::string& address, uint16_t port);
 
        /**
         * \brief Destructor.
         */
        virtual ~WebsockServer();

        /**
         * \brief Receive data from the network and process it.
         * \param fd socket descriptor to receive data
         * \return true if message has been correctly received, processed and
         * response sent, false otherwise (mainly send/recv error)
         * \note This method will blocked until data comes.
         */
        virtual bool Recv(int fd);

        /**
         * \brief Send data.
         * \param fd file descriptor of the client TCP socket
         * \param data data to send
         * \return number of bytes sent or -1 if error
         */
        virtual ssize_t Send(int fd, const std::string& data);

        /**
         * \brief Wait message.
         *
         * This function do a select() on the socket and Process() immediately 
         * the JSON-RPC message.
         * \param ms millisecond to wait (0 means infinite)
         */
        virtual void WaitMessage(uint32_t ms);

        /**
         * \brief Put the TCP socket in LISTEN state.
         */
        bool Listen() const;

        /**
         * \brief Accept a new client socket.
         * \return -1 if error, 0 otherwise
         */
        bool Accept();
        
        /**
         * \brief Close listen socket and all client sockets.
         */
        void Close();

        /**
         * \brief Get the list of clients.
         * \return list of clients
         */
        const std::list<int> GetClients() const;

          /**
         * \brief Bind the websocket++.
         * \return true if success, false otherwise
         */
        bool Bind();

        /**
         * \brief get websocket++ server pointer
         * \return void* websocket pointer
         */
        WebSockServer* GetWebSocketServer(){return m_server;}

        void on_close( websocketpp::connection_hdl hdl) ;
        void on_message(websocketpp::connection_hdl hdl, message_ptr msg) ;
        void on_open( websocketpp::connection_hdl hdl) ;
        /**
         * \brief get websocket++ receive function
         * \return bool websocket receive Handler
         */
         bool WsRecv(websocketpp::connection_hdl hdl, message_ptr msg);

      private:

        /**
         * \brief List of Websocket Server C++.
         */
        WebSockServer* m_server;
       
       
        map_connection m_connections;

        std::list<int> m_clients;
    };

  } /* namespace Rpc */

} /* namespace Json */

#endif /* JSONRPC_WebsockServer_H */


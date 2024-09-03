#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "webserv.hpp"
#include "Socket.hpp"
#include "Response.hpp"
#include "Server.hpp"

class Client
{
	private:
		Socket 					*_socket;
		Server					*_server;
		std::vector<Response *>	_responseQueue;
		Request					*_request;

	public:
		Client();
		Client(Socket *socket, Server *server);
		Client(const Client &client);
		Client &operator=(const Client &client);
		~Client();

		Socket &getSocket();
		void setSocket(Socket *socket);
		std::vector<Response *> &getResponseQueue();
		Server &getServer();
		Request *getRequest();
		void setRequest(Request *request);
};

#endif

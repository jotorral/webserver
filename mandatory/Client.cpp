#include "../includes/Client.hpp"

Client::Client(): _socket(NULL), _server(NULL), _request(NULL) {}

Client::Client(Socket *socket, Server *server):
	_socket(socket), _server(server), _request(NULL) {}

Client::Client(const Client &client)
{
	*this = client;
}

Client &Client::operator=(const Client &client)
{
	if (this != &client)
	{
		this->_socket = client._socket;
		this->_server = client._server;
		this->_responseQueue = client._responseQueue;
		this->_request = client._request;
	}
	return (*this);
}

Client::~Client()
{
	delete this->_socket;
	for (size_t i = 0; i < this->_responseQueue.size(); i++)
	{
		delete this->_responseQueue[i];
	}
	if (this->_request)
		delete this->_request;
}

Socket &Client::getSocket()
{
	return (*this->_socket);
}

void Client::setSocket(Socket *socket)
{
	this->_socket = socket;
}

std::vector<Response *> &Client::getResponseQueue()
{
	return (this->_responseQueue);
}

Server &Client::getServer()
{
	return (*this->_server);
}

Request *Client::getRequest()
{
	return (this->_request);
}

void Client::setRequest(Request *request)
{
	this->_request = request;
}

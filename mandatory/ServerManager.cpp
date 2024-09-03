#include "../includes/ServerManager.hpp"

ServerManager::ServerManager(): _isRunning(false) {}

ServerManager::ServerManager(const ServerManager &serverManager)
{
	*this = serverManager;
}

ServerManager &ServerManager::operator=(const ServerManager &serverManager)
{
	if (this != &serverManager)
	{
		this->_servers = serverManager._servers;
		this->_clients = serverManager._clients;
		this->_epoll = serverManager._epoll;
	}
	return (*this);
}

ServerManager::~ServerManager()
{
	std::map<int, Client *>::iterator it = this->_clients.begin();
	while (it != this->_clients.end())
	{
		delete it->second;
		it++;
	}
}

ServerManager &ServerManager::getInstance()
{
	static ServerManager serverManager;
	return (serverManager);
}

void ServerManager::stop(bool isError)
{
	Logger::logInfo("⏻ Server terminating sequence started", 1);
	this->_isRunning = false;
	this->_isError = isError;
}

bool ServerManager::serve()
{
	this->initServerMasterSockets();
	this->initEpoll();
	this->epollLoop();
	return (this->_isError);
}

void ServerManager::initServerMasterSockets()
{
	for (size_t i = 0; i < this->_servers.size(); i++)
	{
		this->_servers[i].initMasterSocket();
	}
}

void ServerManager::initEpoll()
{
	this->_epoll.init();
	for (size_t i = 0; i < this->_servers.size(); i++)
	{
		_epoll.addMasterSocket(this->_servers[i].getSocket());
	}
}

void ServerManager::epollLoop()
{
	this->logServerListening();
	this->_isRunning = true;
	while (this->_isRunning)
	{
		this->handleEpollEvents(this->_epoll.waitForEvents());
	}
}

void ServerManager::logServerListening()
{
	for (size_t i = 0; i < this->_servers.size(); i++)
	{
		Logger::logInfo(
			(std::ostringstream &)(std::ostringstream().flush()
				<< "👂 Server listening on port " << this->_servers[i].getPort()
			), 1);
	}
}

void ServerManager::handleEpollEvents(std::vector<EpollEvent> events)
{
	for (size_t i = 0; i < events.size() && this->_isRunning; i++)
	{
		this->handleEpollEvent(events[i]);
	}
}

void ServerManager::handleEpollEvent(EpollEvent &event)
{
	try
	{
		Server *server = this->findServerByFd(event.data.fd);
		if (server != NULL)
			this->handleNewConnection(*server);
		else
			this->handleClientEvent(event);
	}
	catch (IOException &e)
	{
		this->closeClientConnection(this->_clients[event.data.fd]);
		Logger::logError(e.what());
	}
	catch (Epoll::EpollInitializationFailedException &e)
	{
		this->closeClientConnection(this->_clients[event.data.fd]);
		Logger::logError(e.what());
	}
	catch (std::exception &e)
	{
		Logger::logError(e.what());
	}
}

Server *ServerManager::findServerByFd(int fd)
{
	for (size_t i = 0; i < this->_servers.size(); i++)
	{
		if (fd == this->_servers[i].getSocket().getFd())
			return (&this->_servers[i]);
	}
	return (NULL);
}

void ServerManager::handleNewConnection(Server &server)
{
	Socket *socket = server.getSocket().acceptConnection();
	this->_clients[socket->getFd()] = new Client(socket, &server);
	this->_epoll.addClientSocket(*socket);
	this->logNewConnection(socket->getFd());
}

void ServerManager::logNewConnection(int fd)
{
	Logger::logInfo(
		(std::ostringstream &)(std::ostringstream().flush()
			<< "✅ New client connection with fd " << fd
		), 1);
}

void ServerManager::handleClientEvent(EpollEvent &event)
{
	Client *client;

	if (this->_clients.find(event.data.fd) == this->_clients.end())
		return;
	client = this->_clients[event.data.fd];
	if (event.events & EPOLLIN)
		this->handleClientRequest(event, client);
	else if (this->_isRunning && event.events & EPOLLOUT)
		this->sendResponseToClient(event, client);
}

void ServerManager::handleClientRequest(EpollEvent &event, Client *client)
{
	try
	{
		std::string rawRequest = this->getRawRequestFromEpollEvent(event);
		if (rawRequest.size() == 0)
		{
			this->closeClientConnection(client);
			return;
		}
		RequestParser requestParser(rawRequest, client);
		Request &request = requestParser.parseRequest(this->_servers);
		if (request.getIsComplete())
		{
			this->_epoll.setSocketOnWriteMode(client->getSocket());
			this->logRequestReceived(request, event.data.fd);
			Response *response = request.getServer()->handleRequest(request);
			client->getResponseQueue().push_back(response);
			client->setRequest(NULL);
			delete &request;
		}
		else
		{
			if (client->getRequest() == NULL)
				client->setRequest(&request);
			Logger::logInfo((std::ostringstream &)(std::ostringstream().flush()
				<< "✉️  Data received from client " << client->getSocket().getFd()
				<< " but request is not complete, waiting for more data."));
		}
	}
	catch (RequestParser::RequestParseErrorException &e)
	{
		this->_epoll.setSocketOnWriteMode(client->getSocket());
		client->getResponseQueue().push_back(new Response(400, client->getServer()));
		Logger::logError(e.what());
	}
	catch (RequestParser::HTTPVersionNotSupportedException &e)
	{
		this->_epoll.setSocketOnWriteMode(client->getSocket());
		client->getResponseQueue().push_back(new Response(505, client->getServer()));
		Logger::logError(e.what());
	}
	catch (RequestParser::RequestBodySizeExceededException &e)
	{
		this->_epoll.setSocketOnWriteMode(client->getSocket());
		client->getResponseQueue().push_back(new Response(413, *e.getServer()));
		Logger::logError(e.what());
	}
	catch (CgiHandler::CGIChildProcessErrorException &e)
	{
		this->stop(true);
	}
	catch (IOException &e)
	{
		throw IOException();
	}
	catch (std::exception &e)
	{
		this->_epoll.setSocketOnWriteMode(client->getSocket());
		client->getResponseQueue().push_back(new Response(500, client->getServer()));
		Logger::logError(e.what());
	}
}

std::string ServerManager::getRawRequestFromEpollEvent(EpollEvent &event)
{
	std::string buffer;
	char bufferTmp[1024];
	ssize_t bytes;

	do
	{
		bytes = read(event.data.fd, bufferTmp, sizeof(bufferTmp) - 1);
		if (bytes < 0)
			throw IOException();
		bufferTmp[bytes] = 0;
		std::string tmp(bufferTmp, bytes);
		buffer.append(tmp);
	}
	while (bytes == sizeof(bufferTmp) - 1);
	return (buffer);
}

void ServerManager::sendResponseToClient(EpollEvent &event, Client *client)
{
	for (size_t i = 0; i < client->getResponseQueue().size(); i++)
	{
		Response *response = client->getResponseQueue()[i];
		if (send(event.data.fd, response->getRaw().c_str(), response->getRaw().size(), 0) == -1)
			throw IOException();
		this->logResponseSent(*response, event.data.fd);
		delete response;
	}
	client->getResponseQueue().clear();
	this->_epoll.setSocketOnReadMode(client->getSocket());
}

void ServerManager::logRequestReceived(Request &request, int fd) const
{
	Logger::logInfo((std::ostringstream &)(std::ostringstream().flush()
		<< "🡇  Request received from client " << fd << ": "
		<< Request::getMethodName(request.getMethod()) << " "
		<< request.getUri()));
}

void ServerManager::logResponseSent(Response &response, int fd) const
{
	Logger::logInfo((std::ostringstream &)(std::ostringstream().flush()
		<< "🡅  Response sent to client " << fd << ": "
		<< response.getStatusCode() << " " << response.getStatusCodeMessage()));
}

void ServerManager::closeClientConnection(Client *client)
{
	int fd = client->getSocket().getFd();
	this->_epoll.deleteClientSocket(client->getSocket());
	this->_clients.erase(fd);
	delete client;
	this->logClosedConnection(fd);
}

void ServerManager::logClosedConnection(int fd)
{
	Logger::logInfo(
		(std::ostringstream &)(std::ostringstream().flush()
			<< "❎ Closed connection with client " << fd
		), 1);
}

void ServerManager::setServers(const std::vector<Server> &servers)
{
	this->_servers = servers;
}

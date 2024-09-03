#ifndef SERVER_MANAGER
#define SERVER_MANAGER

#include "Server.hpp"
#include "Epoll.hpp"
#include "RequestFactory.hpp"
#include "Response.hpp"
#include "RequestParser.hpp"
#include "Logger.hpp"
#include "Client.hpp"
#include "CGIHandler.hpp"

class ServerManager
{
	private:
		std::vector<Server> 	_servers;
		std::map<int, Client *>	_clients;
		Epoll					_epoll;
		bool					_isRunning;
		bool					_isError;

		void initServerMasterSockets();
		void initEpoll();
		void epollLoop();
		void handleEpollEvents(std::vector<EpollEvent> events);
		void handleEpollEvent(EpollEvent &event);
		Server *findServerByFd(int fd);
		void handleNewConnection(Server &server);
		void handleClientEvent(EpollEvent &event);
		void handleClientRequest(EpollEvent &event, Client *client);
		void sendResponseToClient(EpollEvent &event, Client *client);
		void closeClientConnection(Client *client);
		std::string getRawRequestFromEpollEvent(EpollEvent &event);
		void logServerListening();
		void logNewConnection(int fd);
		void logClosedConnection(int fd);
		void logRequestReceived(Request &request, int fd) const;
		void logResponseSent(Response &response, int fd) const;

		ServerManager();
		ServerManager(const ServerManager &serverManager);
		ServerManager &operator=(const ServerManager &serverManager);

	public:
		~ServerManager();

		bool serve();
		void stop(bool isError = false);
		static ServerManager &getInstance();

		void setServers(const std::vector<Server> &servers);

		class IOException : public std::exception
		{
			public:
				const char *what() const throw()
				{
					return ("ERROR: An unexpected IO error occurred.");
				}
		};
	
};

#endif

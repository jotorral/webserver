#ifndef EPOLL_HPP
#define EPOLL_HPP

#include "webserv.hpp"
#include "Logger.hpp"

class Socket;

class Epoll
{
	private:
		int _fd;

	public:
		Epoll();
		Epoll(const Epoll &epoll);
		Epoll &operator=(const Epoll &epoll);
		~Epoll();

		void init();
		void addMasterSocket(Socket &socket);
		void addClientSocket(Socket &socket);
		void deleteClientSocket(Socket &socket);
		std::vector<EpollEvent> waitForEvents();
		void setSocketOnReadMode(Socket &socket);
		void setSocketOnWriteMode(Socket &socket);

		int getFd() const;

		class EpollInitializationFailedException : public std::exception
		{
			// TODO: Cerrar el _fd cuando se atrapa la excepción
			public:
				const char *what() const throw()
				{
					return ("ERROR: Epoll could not be initialized due to an unexpected error.");
				}
		};

		class EpollCtlException : public std::exception
		{
			public:
				const char *what() const throw()
				{
					return ("ERROR: Epoll control interface caused an unexpected error.");
				}
		};
		
};

#endif

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "webserv.hpp"

class Epoll;

class Socket
{
	private:
		int 				_fd;
		struct sockaddr_in	*_address;
		socklen_t			*_addressLen;
		EpollEvent			_epollEvent;
		int					_reuseAddressAndPort;

		void createSocket();
		void setUpSocket();
		void bindSocket(in_addr_t host, uint16_t port);
		void listenForConnections();

	public:
		Socket();
		Socket(const Socket &socket);
		Socket &operator=(const Socket &socket);
		~Socket();

		void initAsMasterSocket(in_addr_t host, uint16_t port);
		Socket *acceptConnection() const;
		void setNonBlockingFd();

		int getFd() const;
		void setFd(int fd);
		struct sockaddr_in *getAddress() const;
		socklen_t *getAddressLen() const;
		void setEpollEvent(EpollEvent &event);
		struct epoll_event &getEpollEvent();

		class SocketInitializationFailedException : public std::exception
		{
			public:
				const char *what() const throw()
				{
					return ("ERROR: The socket could not be initialized due to an unexpected error.");
				}
		};
};

#endif

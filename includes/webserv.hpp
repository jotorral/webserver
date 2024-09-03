#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <stdint.h>
#include <climits>
#include <exception>
#include <cstring>
#include <string>
#include <fcntl.h>
#include <dirent.h>
#include <csignal>
#include <ctime>

/*NETWORK*/
#include <netinet/in.h>

/*STL CONTAINERS*/
#include <vector>
#include <map>

/*System*/
#include <sys/stat.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define SOCKET_MAX_CONN 1000
#define MAX_EPOLL_EVENTS 1000
#define MAX_CONTENT_LENGTH 30000000

enum Methods {
	UNKNOWN = 0,
	GET = 1,
	POST,
	DELETE,
	PUT,
	HEAD
};

typedef struct epoll_event EpollEvent;

#endif

#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include "webserv.hpp"
#include "Request.hpp"
#include "Utils.hpp"
#include "Client.hpp"

class RequestParser
{
	private:
		Request 	*_request;
		in_addr_t 	_clientSockHost;

		RequestParser();
		RequestParser(const RequestParser &parser);

		void parseRequestLine(std::string &rawRequest);
		void parseUri(std::string uri);
		void decodeUri(std::string &uri);
		void parseArgs(std::string args);
		void parseArg(std::string arg);
		void parseHeaders(std::string &rawRequest);
		void parseContent();
		void parseContentWithContentLength(std::string &rawBody, std::map<std::string, std::string> &headers);
		void parseContentWithChunkedEncoding(std::string &rawBody);
		void parseContentMultipartFormData(std::string &rawBody, std::map<std::string, std::string> &headers);
		void setRequestServer(std::vector<Server> &servers);
		void setRequestLocations();
		void setPathInfo();

	public:
		RequestParser(std::string &raw, Client *client);
		RequestParser &operator=(const RequestParser &parser);
		~RequestParser();

		Request &parseRequest(std::vector<Server> &servers);

		class RequestParseErrorException : public std::exception
		{
			public:
				const char *what() const throw()
				{
					return ("The request format is invalid.");
				}
		};

		class HTTPVersionNotSupportedException : public std::exception
		{
			public:
				const char *what() const throw()
				{
					return ("The HTTP version requested by the client is not supported.");
				}
		};

		class RequestBodySizeExceededException : public std::exception
		{
			private:
				Server *server;
			public:
				RequestBodySizeExceededException(Server *server): server(server) {}

				~RequestBodySizeExceededException() throw() {}

				const char *what() const throw()
				{
					return ("The body size of the request was exceeded by the client.");
				}

				Server *getServer()
				{
					return (server);
				}
		};
};

#endif

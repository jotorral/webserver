#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "webserv.hpp"
#include "Location.hpp"

class Server;

class Request
{
	private:
		static std::map<Methods, std::string> _methodDict;
		std::string _raw;
		std::map<std::string, std::string> _headers;
		std::map<std::string, std::string> _args;
		Methods 	_method;
		std::string _uri;
		std::string _queryString;
		std::string _protocol;
		std::string _protocolVersion;
		std::string _content;
		std::string _pathInfo;
		Server 		*_server;
		Location 	*_location;
		bool		_isComplete;
		bool		_isRequestLineProcessed;
		bool		_areHeadersProcessed;
		std::string _uploadFileName;
		std::string _uploadFileContent;

		static void initMethodDict();

	public:
		Request();
		Request(const Request &request);
		Request &operator=(const Request &request);
		~Request();

		static Methods getMethodEnumValue(const std::string &methodName);
		static std::string getMethodName(const Methods &method);

		friend std::ostream &operator<<(std::ostream &os, const Request &request);

		std::string &getRaw();
		void setRaw(const std::string &raw);
		std::map<std::string, std::string> &getHeaders();
		void setHeaders(std::map<std::string, std::string> &headers);
		std::map<std::string, std::string> &getArgs();
		void setArgs(std::map<std::string, std::string> &args);
		Methods &getMethod();
		void setMethod(const Methods &method);
		std::string &getUri();
		void setUri(const std::string &uri);
		std::string &getPathInfo();
		void setPathInfo(const std::string &pathInfo);
		std::string &getProtocol();
		void setProtocol(const std::string &protocol);
		std::string &getProtocolVersion();
		void setProtocolVersion(const std::string &protocolVersion);
		std::string &getContent();
		void setContent(const std::string &content);
		Server *getServer();
		void setServer(Server &server);
		Location *getLocation();
		void setLocation(Location &location);
		std::string &getQueryString();
		void setQueryString(std::string queryString);
		bool getIsComplete();
		void setIsComplete(bool isComplete);
		bool getIsRequestLineProcessed();
		void setIsRequestLineProcessed(bool isProcessed);
		bool getAreHeadersProcessed();
		void setAreHeadersProcessed(bool areProcessed);
		std::string &getUploadFileName();
		void setUploadFileName(const std::string &fileName);
		std::string &getUploadFileContent();
		void setUploadFileContent(const std::string &fileContent);
};

#endif

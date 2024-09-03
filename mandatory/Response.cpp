#include "../includes/Response.hpp"
#include "../includes/Server.hpp"

static void insertHtmlEnd(std::string & indexHtml)
{
	indexHtml.append("</table>\n");
	indexHtml.append("<hr>\n");
	indexHtml.append("</body>\n");
	indexHtml.append("</html>\n");
}

static void insertHtmlLoop(DIR *dir, std::string & dirName, std::string & indexHtml)
{
	struct stat objStats;
	struct dirent *objInfo;
	std::string path;

	while ((objInfo = readdir(dir)) != NULL)
	{
		if (strcmp(objInfo->d_name, ".") == 0)
			continue ;
		path = dirName + "/" + objInfo->d_name;
		stat(path.c_str(), &objStats);
		indexHtml.append("<tr>\n");
		indexHtml.append("<td>\n");
		indexHtml.append("<a href=\"");
		indexHtml.append(objInfo->d_name);
		if (S_ISDIR(objStats.st_mode))
			indexHtml.append("/");
		indexHtml.append("\">");
		indexHtml.append(objInfo->d_name);
		if (S_ISDIR(objStats.st_mode))
			indexHtml.append("/");
		indexHtml.append("</a>\n");
		indexHtml.append("</td>\n");
		indexHtml.append("<td>\n");
		indexHtml.append(ctime(&objStats.st_mtime));
		indexHtml.append("</td>\n");
		indexHtml.append("<td>\n");
		if (!S_ISDIR(objStats.st_mode))
			indexHtml.append(Utils::intToString(objStats.st_size));
		indexHtml.append("</td>\n");
		indexHtml.append("</tr>\n");
	}
}

static void insertHtmlMain(std::string & indexHtml, std::string & dirName)
{
	indexHtml.append("<html>\n");
	indexHtml.append("<head>\n");
	indexHtml.append("<title> Index of");
	indexHtml.append(dirName);
	indexHtml.append("</title>\n");
	indexHtml.append("</head>\n");
	indexHtml.append("<body>\n");
	indexHtml.append("<h1> Index of " + dirName + "</h1>\n");
	indexHtml.append("<table style=\"width:80%; font-size: 15px\">\n");
    indexHtml.append("<hr>\n");
    indexHtml.append("<th style=\"text-align:left\"> File Name </th>\n");
    indexHtml.append("<th style=\"text-align:left\"> Last Modification  </th>\n");
    indexHtml.append("<th style=\"text-align:left\"> File Size </th>\n");
}

int Response::buildHtmlIndex(Request & request)
{
	std::string indexHtml = "";
	std::string dirName = request.getServer()->getRoot() + request.getUri();

	DIR *dir = opendir(dirName.c_str());
	if (dir == NULL)
	{
		perror("Error: opendir : ");
		return (500);
	}
	insertHtmlMain(indexHtml, dirName);
	insertHtmlLoop(dir, dirName, indexHtml);
	insertHtmlEnd(indexHtml);
	setContent(indexHtml);
	free(dir);
	return (0);
}

void Response::contentErrorPage(std::string & path, const int statusCode)
{
	std::string content = Utils::codeStatus(statusCode);
	this->setStatusCodeMessage(content);
	std::ifstream ErrorPageOpen(path.c_str());
	std::stringstream ErrorPageContent;
	ErrorPageContent << ErrorPageOpen.rdbuf();
	content = ErrorPageContent.str();
	this->setContent(content);
	this->setFile(path);
}

void Response::contentForNoErrorPage(const int statusCode)
{
	std::string content = Utils::codeStatus(statusCode);
	std::ostringstream strStatusCode;
	strStatusCode << statusCode;
	std::string htmlContent = "<html lang=\"es\">\n<head>\n    <meta charset=\"UTF-8\">\n    <title>" + strStatusCode.str() + " " 
	+ Utils::codeStatus(statusCode) + "</title>\n</head>\n<body>\n    <center><b style=\"font-size:24px;\">" + strStatusCode.str() + " " 
	+ Utils::codeStatus(statusCode) + "</b></center>\n</body>\n</html>\n";
	this->setContent(htmlContent);
	this->setStatusCodeMessage(content);
	std::string path = "default";
	this->setFile(path);
}

void Response::errorResponseContentRoutine(Server &server)
{
	std::map<int, std::string> errorPages = server.getErrorPages();
	std::map<int, std::string>::iterator it;

	for (it = errorPages.begin(); it != errorPages.end(); it++)
	{
		if (it->first == this->getStatusCode())
		{
			if (it->second.empty())
				contentForNoErrorPage(it->first);
			else
			{
				std::string path = (server.getRoot() + it->second);
				if (access(path.c_str(), F_OK) == -1 && access(path.c_str(), R_OK) == -1)
					contentForNoErrorPage(it->first);
				else
					contentErrorPage(path, it->first);
			}
			break ;
		}
	}
	if (this->getContent().size() == 0)
		contentForNoErrorPage(this->getStatusCode());
}

static void ResponseContentType(Response & response)
{
	std::map<std::string, std::string> ContentType;
	std::string ext = response.getFile();
	size_t pos = ext.rfind('.', std::string::npos);
	if (pos == std::string::npos)
	{
		std::string extToFind = "default";
		response.getHeaders().insert(std::make_pair("Content-type:",
		response.getFileExt(extToFind)));
	}
	else
	{
		std::string extToFind = ext.substr(pos);
		response.getHeaders().insert(std::make_pair("Content-type:",
		response.getFileExt(extToFind)));
	}
}

static void ResponseContentLength(Response & response)
{
	std::string lengthTostring = Utils::intToString((int)response.getContent().size());
	response.getHeaders().insert(std::make_pair("Content-Length:", lengthTostring));
}

static void ResponseConnectionType(Request *request, Response & response)
{
	if (request && request->getHeaders()["connection"] == "keep-alive")
		response.getHeaders().insert(std::make_pair("Connection:", "keep-alive"));
	else
		response.getHeaders().insert(std::make_pair("Connection:", "close"));
}

static void ResponseServer(Response & response)
{
	response.getHeaders().insert(std::make_pair("Server:", "WebServer42"));
}

static void ResponseDate(Response & response)
{
	char date[1024];
	time_t actual = time(0);
	struct tm *GMTtime = gmtime(&actual);
	strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S %Z", GMTtime);
	response.getHeaders().insert(std::make_pair("Date:", date));
}

/*
static void ResponseLocationForError(Response & response)
{
	if (response.getFile() == "default")
		response.getHeaders().insert(std::make_pair("Location:", "/"));
	else
		response.getHeaders().insert(std::make_pair("Location:", response.getFile()));
}*/

void Response::ResponseLocation(Response & response, Request *request)
{
	if (request)
	{
		std::string pathToResource = request->getServer()->getRoot() + request->getUri();
		if (request->getLocation())
		{
			pathToResource = request->getLocation()->getLocationRoot() + request->getUri();
			std::string &returnLocation = request->getLocation()->getReturnLocation();
			if (!returnLocation.empty())
			{
				std::string reddir = request->getUri();
				if (reddir == request->getLocation()->getLocationPath())
					response.getHeaders().insert(std::make_pair("Location:", returnLocation));
				else if (Utils::typeOfFile(request->getLocation()->getLocationRoot() + returnLocation) == 1)
					response.getHeaders().insert(std::make_pair("Location:", returnLocation));
				else
				{
					std::string returnTmp = returnLocation;
					if (request->getLocation()->getLocationPath() == "/")
						returnTmp = returnTmp + "/";
					std::string toFind = request->getLocation()->getLocationPath();
					size_t pos = reddir.find(toFind, 0);
					reddir.replace(pos, toFind.length(), returnTmp);
					response.getHeaders().insert(std::make_pair("Location:", reddir));
				}
			}
			else
			{
				int typeOfResource = Utils::typeOfFile(pathToResource);
				/*if (typeOfResource == 1)
					response.getHeaders().insert(std::make_pair("Location:", request->getLocation()->getLocationPath()));*/
				if (typeOfResource == 2)
				{
					std::string reddir = request->getUri();
					size_t pos = reddir.size();
					if (reddir[pos - 1] != '/')
					{
						reddir = request->getUri() + "/";
						response.getHeaders().insert(std::make_pair("Location:", reddir));
					}
				}
			}
		}
		else
		{
			int typeOfResource = Utils::typeOfFile(pathToResource);
			if (typeOfResource == 2)
			{
				std::string reddir = request->getUri();
				size_t pos = reddir.size();
				if (reddir[pos - 1] != '/')
				{
					reddir = request->getUri() + "/";
					response.getHeaders().insert(std::make_pair("Location:", reddir));
				}
			}
		}
	}
}

void Response::ResponseHeaderRoutine(Response &response, Request *request)
{
	ResponseContentType(response); //cuidado si metemos algun archivo con extension no contemplada
	ResponseContentLength(response);
	ResponseConnectionType(request, response);
	ResponseServer(response);
	ResponseLocation(response, request);
	ResponseDate(response);
}

void Response::parseProtocolandVersion()
{
	std::string protocol = this->getProtocol();
	size_t pos = protocol.find('\r');
	if (pos != std::string::npos)
		protocol.erase(pos, 1);
	std::string pVersion = this->getProtocolVersion();
	pos = pVersion.find('\r');
	if (pos != std::string::npos)
		pVersion.erase(pos, 1);
	this->setProtocol(protocol);
	this->setProtocolVersion(pVersion);
}

void Response::ResponseRawRoutine()
{
	std::string raw = "";
	parseProtocolandVersion();
	raw.append(getProtocol() + "/");
	raw.append((getProtocolVersion() + " "
	  + Utils::intToString(getStatusCode()) + " "
	  + getStatusCodeMessage() +  "\r\n"));
	std::map<std::string, std::string>::iterator it;
	for(it = getHeaders().begin(); it != getHeaders().end(); it++)
	{
		raw.append(it->first + it->second + "\r\n");
	}
	raw.append("\r\n" + getContent());
	setRaw(raw);
}

Response::Response(int errCode, Request *request) : _errorResponse(true)
{
	initFileExt();
	setStatusCode(errCode);
	errorResponseContentRoutine(*request->getServer());
	setProtocol(request->getProtocol());
	setProtocolVersion(request->getProtocolVersion());
	ResponseHeaderRoutine(*this, request);
	ResponseRawRoutine();
}

Response::Response(int errCode, Server &server) : _errorResponse(true)
{
	initFileExt();
	setStatusCode(errCode);
	setProtocol("HTTP");
	setProtocolVersion("1.1");
	errorResponseContentRoutine(server);
	ResponseHeaderRoutine(*this, NULL);
	ResponseRawRoutine();
}

Response::Response()
{
	_statusCode = 0;
	_errorResponse = false;
	_raw = "";
	_content = "";
	_file = "";
	_protocol = "";
	_protocolVersion = "";
	initFileExt();
}

Response::Response(const Response &response)
{
	*this = response;
}

Response &Response::operator=(const Response &response)
{
	if (this != &response)
	{
		this->_statusCode = response._statusCode;
		this->_headers = response._headers;
		this->_raw = response._raw;
		this->_content = response._content;
		this->_protocol = response._protocol;
		this->_protocolVersion = response._protocolVersion;
	}
	return (*this);
}

Response::~Response() {}

int Response::getStatusCode()
{
	return (this->_statusCode);
}

void Response::setStatusCode(int statusCode)
{
	this->_statusCode = statusCode;
}

std::string & Response::getStatusCodeMessage()
{
	return (_statusCodeMessage);
}

void Response::setStatusCodeMessage(std::string & message)
{
	_statusCodeMessage = message;
}

std::map<std::string, std::string> &Response::getHeaders()
{
	return (this->_headers);
}

void Response::setHeaders(std::map<std::string, std::string> &headers)
{
	this->_headers = headers;
}

std::string &Response::getRaw()
{
	return (this->_raw);
}

void Response::setRaw(std::string &raw)
{
	this->_raw = raw;
}

std::string &Response::getContent()
{
	return (this->_content);
}

void Response::setContent(std::string &content)
{
	this->_content = content;
}

std::string &Response::getProtocol()
{
	return (this->_protocol);
}

void Response::setProtocol(const std::string &protocol)
{
	this->_protocol = protocol;
}

std::string &Response::getProtocolVersion()
{
	return (this->_protocolVersion);
}

void Response::setProtocolVersion(const std::string &protocolVersion)
{
	this->_protocolVersion = protocolVersion;
}

std::string &Response::getFile()
{
	return (this->_file);
}

void Response::setFile(std::string & file)
{
	_file = file;
}

std::string & Response::getFileExt(std::string & ext)
{
	if (_exts.count(ext))
		return (_exts[ext]);
	return (_exts["default"]);
}

void Response::setErrorResponse(bool isError)
{
	_errorResponse = isError;
}

bool & Response::getErrorResponse()
{
	return (_errorResponse);
}

void Response::initFileExt()
{
	_exts[".html"] = "text/html";
    _exts[".htm"] = "text/html";
    _exts[".css"] = "text/css";
    _exts[".ico"] = "image/x-icon";
    _exts[".avi"] = "video/x-msvideo";
    _exts[".bmp"] = "image/bmp";
    _exts[".doc"] = "application/msword";
    _exts[".gif"] = "image/gif";
    _exts[".gz"] = "application/x-gzip";
    _exts[".ico"] = "image/x-icon";
    _exts[".jpg"] = "image/jpeg";
    _exts[".jpeg"] = "image/jpeg";
    _exts[".png"] = "image/png";
    _exts[".txt"] = "text/plain";
    _exts[".mp3"] = "audio/mp3";
    _exts[".pdf"] = "application/pdf";
    _exts["default"] = "text/html";
}




/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvicina <alvicina@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 12:27:41 by alvicina          #+#    #+#             */
/*   Updated: 2024/07/31 11:56:26 by alvicina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Request.hpp"
#include "../includes/Response.hpp"

Server::Server(): _masterSocket(NULL), _isDefault(false)
{
	_port = 0;
	_host = 0;
	_serverName = "";
	_root = "";
	_clientMaxBodySize = MAX_CONTENT_LENGTH;
	_index = "";
	_autoIndex = false;
	initErrorPages();
}

Server::Server(Server const & copy)
{
	*this = copy;	
}

Server::~Server()
{
	if (this->_masterSocket)
		delete this->_masterSocket;
}

Server& Server::operator=(Server const & other)
{
	if (this != &other)
	{
		_port = other._port;
		_host = other._host;
		_serverName = other._serverName;
		_root = other._root;
		_clientMaxBodySize = other._clientMaxBodySize;
		_index = other._index;
		_autoIndex = other._autoIndex;
		_errorPages = other._errorPages;
		_locations = other._locations;
		_masterSocket = other._masterSocket;
		_isDefault = other._isDefault;
	}
	return (*this);
}

std::string & Server::getServerName(void)
{
	return (_serverName);
}

long int & Server::getClientMaxBodySize(void)
{
	return (_clientMaxBodySize);
}

std::string & Server::getRoot(void)
{
	return (_root);
}

in_addr_t & Server::getHost(void)
{
	return (_host);
}

uint16_t & Server::getPort(void)
{
	return (_port);
}

std::string & Server::getIndex(void)
{
	return (_index);
}

bool & Server::getAutoindex(void)
{
	return (_autoIndex);
}

std::vector<Location> & Server::getLocation(void)
{
	return (_locations);
}

std::map<int, std::string> & Server::getErrorPages(void)
{
	return (_errorPages);
}

static in_addr_t isHostValid(std::string const & param)
{
	struct addrinfo hints;
	struct addrinfo *res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	int result = getaddrinfo(param.c_str(), NULL, &hints, &res);
	if (result)
		throw ServerErrorException("Error: Invalid syntax for host");
	struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
	in_addr_t addr = ipv4->sin_addr.s_addr;
	freeaddrinfo(res);
	return(addr);
}

void Server::setHost(std::string param)
{
	checkParamToken(param);
	if (param == "localhost")
		param = "127.0.0.1";
	if (param == "any")
		_host = INADDR_ANY;
	else
		_host = isHostValid(param);
}

static bool isRootDirectory(std::string const & param)
{
	struct stat buffer;
	int			status;

	if (param[0] != '/')
		throw ServerErrorException("Error: invalid root for server");
	status = stat(param.c_str(), &buffer);
	if (status == 0)
	{
		if (buffer.st_mode & S_IFDIR)
			return (true);
		return (false);
	}
	throw ServerErrorException("Error: could not check root stats");
}

void Server::setRoot(std::string param)
{
	checkParamToken(param);
	if (isRootDirectory(param) == true)
	{
		if (access(param.c_str(), R_OK | X_OK) == 0)
			_root = param;
		else
			throw ServerErrorException("Error: invalid root for server");
	}
	else
			throw ServerErrorException("Error: invalid root for server");
}

void Server::setPort(std::string & param)
{
	unsigned int	serverPort = 0;
	size_t			i = 0; 		
	
	checkParamToken(param);
	while (i < param.size())
	{
		if (!isdigit(param[i]))
			throw ServerErrorException("Error: Invalid format for port");
		i++;
	}
	serverPort = Utils::stringToInt(param);
	if (serverPort < 1 || serverPort > 65636)
		throw ServerErrorException("Error: Invalid format for port");
	_port = (uint16_t) serverPort;
}

void Server::setServerName(std::string const & params)
{
	_serverName = params;
}

void Server::setIndex(std::string const & params)
{
	_index = params;
}

void Server::setAutoIndex(std::string const & params)
{
	if (params != "on" && params != "off")
		throw ParserErrorException("Error: invalid syntax for autoindex");
	if (params == "on")
		_autoIndex = true;
}

void Server::setClientMaxSize(std::string const & params)
{	
	long int number = 0;
	number = Utils::stringToInt(params);
	if (number > MAX_CONTENT_LENGTH)
		throw ParserErrorException("Error: client body size out of bounds");
	_clientMaxBodySize = number;
}

void Server::locationRootRoutine(std::string & locationVars, Location & location)
{
	if (!location.getLocationRoot().empty())
			throw ServerErrorException("Error: location root duplicated");
	this->checkParamToken(locationVars);
	if (isRootDirectory(locationVars) == true)
		location.setRootLocation(locationVars);
	else
		location.setRootLocation(_root + locationVars);
}

void Server::locationMethodsRoutine(std::vector<std::string> & locationVars, size_t & pos, bool & methodsFlag, Location & location)
{
	std::vector<std::string> methods;
	
	if (methodsFlag)
		throw ParserErrorException("Error: methods of location is duplicated");
	while (++pos < locationVars.size())
	{
		if (locationVars[pos].find(";") != std::string::npos)
		{
			checkParamToken(locationVars[pos]);
			methods.push_back(locationVars[pos]);
			break ;
		}
		else
		{
			if ((pos + 1) >= locationVars.size())
				throw ParserErrorException("Error: location method is invalid");
			methods.push_back(locationVars[pos]);
		}
	}
	location.setLocationMethods(methods);
	methodsFlag = true;
}

void Server::locationAutoIndexRoutine(std::string & autoIndex, bool & autoIndexFlag, Location & location)
{
	if (autoIndexFlag)
		throw ServerErrorException("Error: Autoindex duplicated in location");
	checkParamToken(autoIndex);
	location.setLocationAutoIndex(autoIndex);
	autoIndexFlag = true;
}

void Server::locationIndexRoutine(std::string & index, Location & location)
{
	if (!location.getIndexLocation().empty())
		throw ServerErrorException("Error: location index duplicated");
	checkParamToken(index);
	location.setIndexLocation(index);
}

void Server::locationReturnRoutine(std::string & Return, Location & location)
{
	if (!location.getReturnLocation().empty())
		throw ServerErrorException("Error: return duplicated in location");
	checkParamToken(Return);
	location.setReturnLocation(Return);
}

void Server::locationAliasRoutine(std::string & alias, Location & location)
{
	if (!location.getAliasLocation().empty())
		throw ServerErrorException("Error: return duplicated in location");
	checkParamToken(alias);
	location.setAliasLocation(alias);
}

void Server::locationCgiExtRoutine(std::vector<std::string> & locationVars, size_t & pos, Location & location)
{
	std::vector<std::string> cgiExt;

	while (++pos < locationVars.size())
	{
		if (locationVars[pos].find(".") == std::string::npos)
			throw ServerErrorException("Error: CgiExt in location is invalid");
		if (locationVars[pos].find(";") != std::string::npos)
		{
			checkParamToken(locationVars[pos]);
			cgiExt.push_back(locationVars[pos]);
			break ;
		}
		else
		{
			if (pos + 1 >= locationVars.size())
				throw ServerErrorException("Error: CgiExt in location is invalid");
			cgiExt.push_back(locationVars[pos]);
		}
	}
	location.setCgiExtensionLocation(cgiExt);
}

void Server::locationCgiPathRoutine(std::vector<std::string> & locationVars, size_t & pos, Location & location)
{
	std::vector<std::string> cgiPath;

	while (++pos < locationVars.size())
	{
		if (locationVars[pos].find(";") != std::string::npos)
		{
			checkParamToken(locationVars[pos]);
			if (locationVars[pos][0] != '/')
				throw ServerErrorException("Error: CgiPath in location is invalid"); 
			if (access(locationVars[pos].c_str(), F_OK) == -1) 
				throw ServerErrorException("Error: CgiPath in location is invalid"); 
			if (access(locationVars[pos].c_str(), X_OK) == -1) 
				throw ServerErrorException("Error: CgiPath in location is invalid");
			cgiPath.push_back(locationVars[pos]);
			break ;
		}
		else
		{
			if ((pos + 1) >= locationVars.size())
				throw ServerErrorException("Error: CgiPath in location is invalid");
			if (locationVars[pos][0] != '/')
				throw ServerErrorException("Error: CgiPath in location is invalid"); 
			if (access(locationVars[pos].c_str(), F_OK) == -1) 
				throw ServerErrorException("Error: CgiPath in location is invalid"); 
			if (access(locationVars[pos].c_str(), X_OK) == -1) 
				throw ServerErrorException("Error: CgiPath in location is invalid");
			cgiPath.push_back(locationVars[pos]);
		}
	}
	location.setCgiPathLocation(cgiPath);
}

void Server::locationMaxSizeRoutine(std::string & maxSize, bool & maxSizeFlag, Location & location)
{
	if (maxSizeFlag)
		throw ServerErrorException("Error: Max body of location is duplicated");
	checkParamToken(maxSize);
	location.setMaxBodySizeLocation(maxSize);
	maxSizeFlag = true;
}

void Server::locationUploadRoutine(std::string & upLoadPath, Location & location)
{
	if (!location.getUploadStore().empty())
		throw ServerErrorException("Error: upload_store duplicated in location");
	checkParamToken(upLoadPath);
	location.setUploadStore(upLoadPath);
}

void Server::locationExtractionRoutine(std::vector<std::string> & locationVars, size_t & pos, Location & location,
bool & methodsFlag, bool & autoIndexFlag, bool & maxSizeFlag)
{
	if (locationVars[pos] == "root" && (pos + 1) < locationVars.size())
		locationRootRoutine(locationVars[++pos], location);
	else if ((locationVars[pos] == "allow_methods" || locationVars[pos] == "methods" ) && (pos + 1) < locationVars.size())
		locationMethodsRoutine(locationVars, pos, methodsFlag, location);
	else if (locationVars[pos] == "autoindex" && (pos + 1) < locationVars.size())
		locationAutoIndexRoutine(locationVars[++pos], autoIndexFlag, location);
	else if (locationVars[pos] == "index" && (pos + 1) < locationVars.size())
		locationIndexRoutine(locationVars[++pos], location);
	else if (locationVars[pos] == "return" && (pos + 1) < locationVars.size())
		locationReturnRoutine(locationVars[++pos], location);
	else if (locationVars[pos] == "alias" && (pos + 1) < locationVars.size())
		locationAliasRoutine(locationVars[++pos], location);
	else if (locationVars[pos] == "cgi_ext" && (pos + 1) < locationVars.size())
		locationCgiExtRoutine(locationVars, pos, location);
	else if (locationVars[pos] == "cgi_path" && (pos + 1) < locationVars.size())
		locationCgiPathRoutine(locationVars, pos, location);
	else if (locationVars[pos] == "client_max_body_size" && (pos + 1) < locationVars.size())
		locationMaxSizeRoutine(locationVars[++pos], maxSizeFlag, location);
	else if (locationVars[pos] == "upload_store" && (pos + 1) < locationVars.size())
		locationUploadRoutine(locationVars[++pos], location);
	else if (pos < locationVars.size())
		throw ServerErrorException("Error: directive in location is invalid");
}

void Server::checkLocationCgiPath(Location & location)
{
	std::vector<std::string>::const_iterator it;
	for (it = location.getCgiPathLocation().begin(); it != location.getCgiPathLocation().end(); it++)
	{
		if ((access(it->c_str(), F_OK) == -1) )
			throw ServerErrorException("Error: Error: CGI not valid2");
		if ((access(it->c_str(), X_OK) == -1) )
			throw ServerErrorException("Error: Error: CGI not valid1");
	}
}

void Server::checkLocationCgiExtension(Location & location)
{
	std::vector<std::string>::const_iterator it;
	std::vector<std::string>::const_iterator itPath;
	if (!location.getCgiPathLocation().empty())
		itPath = location.getCgiPathLocation().begin();
	for (it = location.getCgiExtensionLocation().begin(); it != location.getCgiExtensionLocation().end(); it++)
	{
		if (itPath != location.getCgiPathLocation().end())
		{
			location.getExtPathMap().insert(std::make_pair(*it, *itPath));
			itPath++;
		}
		else
			location.getExtPathMap().insert(std::make_pair(*it, ""));
	}
}

void Server::checkLocationForCGI(Location & location)
{
	checkLocationCgiPath(location);
	checkLocationCgiExtension(location);
}

void Server::isLocationValid(Location & location)
{
	if (!location.getCgiExtensionLocation().empty())
		checkLocationForCGI(location);
	if (location.getLocationPath()[0] != '/')
		throw ServerErrorException("Error: invalid path in location");
	if (location.getLocationRoot().empty())
		location.setRootLocation(this->_root);
	if (!location.getReturnLocation().empty())
	{
		if (Utils::fileExistsAndReadable(location.getLocationRoot(), location.getReturnLocation()))
			throw ServerErrorException("Error: Return for location invalid");	
	}
	if (!location.getAliasLocation().empty())
	{
		std::string path = "";
		if (Utils::fileExistsAndReadable(path, location.getAliasLocation()))
			throw ServerErrorException("Error: Alias for location invalid");
	}
	if (!location.getUploadStore().empty())
	{
		if (access(location.getUploadStore().c_str(), W_OK | X_OK) == -1 && 
		Utils::typeOfFile(location.getUploadStore()) != 2)
			throw ServerErrorException("Error: upload_store for location invalid");
	}
}

void Server::setLocation(std::string & locationPath, std::vector<std::string> & locationVars)
{
	Location location;
	bool 	methodsFlag = false;
	bool	autoIndexFlag = false;
	bool	maxSizeFlag = false;
	
	
	location.setPath(locationPath);
	size_t pos = 0;
	while (pos < locationVars.size())
	{
		locationExtractionRoutine(locationVars, pos, location, methodsFlag, autoIndexFlag, maxSizeFlag);
		pos++;
	}
	if (location.getIndexLocation().empty())
		location.setIndexLocation(this->_index);
	if (!maxSizeFlag)
		location.setMaxBodySizeLocation(this->_clientMaxBodySize);
	_locations.push_back(location);
}

void Server::checkParamToken(std::string & param)
{
	size_t pos = param.rfind(';');
	if (pos != param.size() - 1)
		throw ServerErrorException("Error: Invalid param token");
	param.erase(pos);
}

void Server::initMasterSocket()
{
	this->_masterSocket = new Socket();
	this->_masterSocket->initAsMasterSocket(this->_host, this->_port);
}

Socket &Server::getSocket()
{
	return (*this->_masterSocket);
}

void Server::initErrorPages(void)
{
	_errorPages[301] = "";
	_errorPages[302] = "";
	_errorPages[400] = "";
	_errorPages[401] = "";
	_errorPages[402] = "";
	_errorPages[403] = "";
	_errorPages[404] = "";
	_errorPages[405] = "";
	_errorPages[406] = "";
	_errorPages[500] = "";
	_errorPages[501] = "";
	_errorPages[502] = "";
	_errorPages[503] = "";
	_errorPages[504] = "";
	_errorPages[505] = "";
}

bool Server::checkForDuplicateLocation(void)
{
	if (_locations.size() < 2)
		return (false);
		
	std::vector<Location>::iterator itA;
	std::vector<Location>::iterator itB;
	for (itA = _locations.begin(); itA != _locations.end(); itA++)
	{
		for (itB = itA + 1; itB != _locations.end(); itB++)
		{
			if (itA->getLocationPath() == itB->getLocationPath())
				return (true);
		}
	}
	return (false);
}

static void checkErrorPagesForModule(std::vector<std::string> & errorCodes)
{
	if (errorCodes.size() % 2 != 0)
		throw ServerErrorException("Error: invalid Error page directive (module)");
}

static void checkErrorIsAllDigits(std::string const & error)
{
	size_t i = 0;
	while (i < error.size())
	{
		if (!isdigit(error[i]))
			throw ServerErrorException("Error: Error code invalid");
		i++;
	}
}

static void checkErrorNumberDigits(std::string const & error)
{
	if (error.size() != 3)
		throw ServerErrorException("Error: Error code is invalid");
}

static void checkErrorCodeStatus(int & errorToNumber)
{
	if (Utils::codeStatus(errorToNumber) == "Undefined" || errorToNumber < 400 || errorToNumber > 599)
		throw ServerErrorException("Error: invalid code for error");
}

void Server::checkErrorPageForFile(std::string & errorPath)
{
	if (Utils::typeOfFile(_root + "/" + errorPath) != 1)
		throw ServerErrorException("Error: incorrect path for error page file");
	if (Utils::checkFile(_root + "/" + errorPath, 0) == -1 || Utils::checkFile(_root + "/" + errorPath, 4) == -1)
		throw ServerErrorException("Error: Error page file not accesible");
}

void Server::setErrorPages(std::vector<std::string> & errorCodes)
{
	if (errorCodes.empty())
		return ;
	checkErrorPagesForModule(errorCodes);
	size_t	i = 0;
	while (i < errorCodes.size() - 1)
	{
		checkErrorIsAllDigits(errorCodes[i]);
		checkErrorNumberDigits(errorCodes[i]);
		int errorToNumber = Utils::stringToInt(errorCodes[i]);
		checkErrorCodeStatus(errorToNumber);
		std::string errorPath = errorCodes[++i];
		checkParamToken(errorPath);
		if (Utils::typeOfFile(errorPath) == 2)
			return ;
		else
			checkErrorPageForFile(errorPath);
		_errorPages[errorToNumber] = errorPath;
		i++;
	}
}

void Server::serverPrinter(void)
{	
	std::cout << "-------------------server conf---------------" << std::endl;
	std::cout << "Port: " << getPort() << std::endl;
	std::cout << "Host: " << getHost() << std::endl;
	std::cout << "Server Name: " << getServerName() << std::endl;
	std::cout << "Root: " << getRoot() << std::endl;
	std::cout << "Body size: " << getClientMaxBodySize() << std::endl;
	std::cout << "Index: " << getIndex() << std::endl;
	std::cout << "AutoIndex: " << getAutoindex() << std::endl;
	for (std::map<int, std::string>::iterator it = getErrorPages().begin(); it != getErrorPages().end(); it++)
		std::cout << "Error code: " << it->first << " " << "Error path: " << it->second << std::endl;
	for (std::vector<Location>::iterator it = getLocation().begin(); it != getLocation().end(); it++)
		(*it).locationPrinter();

	std::cout << "---------------------------------------------" << std::endl;
}

Response *Server::handleRequest(Request &request)
{
	RequestHandler *handler = NULL;

	try
	{
		handler = RequestFactory::makeRequestHandler(request);
		Response *response = handler->handleRequest();
		delete handler;
		return (response);
	}
	catch (FactoryErrorException & e)
	{
		if (handler)
			delete handler;
		return (e.createResponse());
	}
	catch (HandlerErrorException & e)
	{
		if (handler)
			delete handler;
		return (e.createResponse());
	}
	catch (CgiHandler::CGIChildProcessErrorException &e)
	{
		if (handler)
			delete handler;
		throw CgiHandler::CGIChildProcessErrorException();
	}
	catch (std::exception &e)
	{
		if (handler)
			delete handler;
		return (new Response(500, &request));
	}
}

bool Server::getIsDefault()
{
	return (this->_isDefault);
}

void Server::setIsDefault(bool isDefault)
{
	this->_isDefault = isDefault;
}

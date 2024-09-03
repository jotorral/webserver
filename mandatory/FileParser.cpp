/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvicina <alvicina@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 12:12:35 by alvicina          #+#    #+#             */
/*   Updated: 2024/07/31 11:29:56 by alvicina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/FileParser.hpp"

FileParser::FileParser(std::string content) : _content(content), _nbServers(0)
{
	
}

FileParser::~FileParser()
{
	
}

FileParser::FileParser(FileParser const & copy) : _content(copy._content),
_configs(copy._configs), _nbServers(copy._nbServers)
{
	
}

FileParser& FileParser::operator=(FileParser const & other)
{
	if (this != &other)
	{
		_content = other._content;
		_configs = other._configs;
		_nbServers = other._nbServers;
		_servers = other._servers;
	}
	return (*this);
}

std::string FileParser::getContent(void)
{
	return (_content);
}

std::vector<std::string> const & FileParser::getConfig(void)
{
	return (_configs);
}

size_t FileParser::getNbServers(void)
{
	return (_nbServers);
}

void FileParser::removeComments(void)
{
	size_t commentStart = _content.find("#", 0);
	while (commentStart != std::string::npos)
	{ 
		size_t commentEnd = _content.find("\n", commentStart);
		_content.erase(commentStart, commentEnd - commentStart);
		commentStart = _content.find("#");
	}
}

void FileParser::removeWhitespace(void)
{
	size_t i = 0;

	while (_content[i] && isspace(_content[i]))
		i++;
	_content = _content.substr(i);
	i = _content.length() - 1;
	while (i > 0 && isspace(_content[i]))
		i--;
	_content = _content.substr(0, i + 1);
}

static int getBeginServer(ssize_t startServer, std::string const & content)
{
	size_t i = startServer;
	
	while (content[i])
	{
		if (content[i] == 's')
			break ;
		else if (!isspace(content[i]))
			throw ParserErrorException("Error: character out of scope");
		i++;
	}
	if (content.compare(i, 6, "server") != 0)
		throw ParserErrorException("Error: character out of scope");
	i = i + 6;
	while (content[i] && isspace(content[i]))
		i++;
	if (content[i] == '{')
		return (i);
	else
		throw ParserErrorException("Error: character out of scope");
}

static int getEndServer(ssize_t startServer, std::string const & content)
{
	int 	scope = 0;
	size_t 	i = startServer + 1;
	
	while (content[i])
	{
		if (content[i] == '{')
			scope++;
		else if (content[i] == '}')
		{
			if (!scope)
				return (i);
			scope--;
		}
		i++;
	}
	return (startServer);
}

void FileParser::splitServer(void)
{
	ssize_t startServer = 0;
	ssize_t endServer = 1;

	if (_content.find("server") == std::string::npos)
		throw ParserErrorException("Error: No server conf found");
	while (startServer != endServer && startServer < (ssize_t)_content.length())
	{
		startServer = getBeginServer(startServer, _content);
		endServer = getEndServer(startServer, _content);
		if (startServer == endServer)
			throw ParserErrorException("Error: scope problem in conf file");
		_configs.push_back(_content.substr(startServer, endServer - startServer + 1));
		_nbServers++;
		startServer = endServer + 1;
	}
}

static std::vector<std::string> getParams(std::string separators, std::string conf)
{
	std::vector<std::string>	params;
	std::string::size_type		start = 0;
	std::string::size_type		end = 0;
	
	while (1)
	{
		end = conf.find_first_of(separators, start);
		if (end == std::string::npos)
			break ;
		std::string temp = conf.substr(start, end - start);
		params.push_back(temp);
		start = conf.find_first_not_of(separators, end);
	}	
	return (params);
}

static void portRoutine(std::string & params, Server & serv)
{
	if (serv.getPort())
		throw ServerErrorException("Error: Port duplicated in server conf");
	serv.setPort(params);
}

static void hostRoutine(std::string & params, Server & serv)
{
	if (serv.getHost())
		throw ServerErrorException("Error: Host duplicated");
	serv.setHost(params);
}

static void rootRoutine(std::string & params, Server & serv)
{
	if (!serv.getRoot().empty())
		throw ServerErrorException("Error: Root is duplicated");
	serv.setRoot(params);
}

static void errorPageRoutine(std::vector<std::string> const & params, size_t & pos,
std::vector<std::string> & errCodes)
{
	while (++pos < params.size())
	{
		errCodes.push_back(params[pos]);
		if (params[pos].find(';') != std::string::npos)
			break;
		if (pos + 1 >= params.size())
			throw ServerErrorException("Error: error page out of scope");
	}
}

static void clientMaxSizeRoutine(std::string & params, bool *clientMaxSize, Server & serv)
{
	serv.checkParamToken(params);
	serv.setClientMaxSize(params);
	*clientMaxSize = true;
}

static void serverNameRoutine(std::string & params, Server & serv)
{
	serv.checkParamToken(params);
	if (!serv.getServerName().empty())
		throw ParserErrorException("Error: server name duplicated");
	serv.setServerName(params);
}

static void indexRoutine(std::string & params, Server & serv)
{
	serv.checkParamToken(params);
	if (!serv.getIndex().empty())
		throw ParserErrorException("Error: Index duplicated");
	serv.setIndex(params);
}

static void autoIndexRoutine(std::string & params, Server & serv, bool *autoIndex)
{
	if (*autoIndex == true)
		throw ParserErrorException("Error: autoindex duplicated");
	serv.checkParamToken(params);
	serv.setAutoIndex(params);
	*autoIndex = true;
}

static void locationRoutine(std::vector<std::string> const & params, Server & serv, 
size_t & pos, int *locationFlag)
{
	std::string locationPath;
	std::vector<std::string> locationVars;
	pos++;
	if (params[pos] == "{" || params[pos] == "}")
		throw ParserErrorException("Error: wrong character in Location scope");
	locationPath = params[pos];
	if (params[++pos] != "{")
		throw ParserErrorException("Error: wrong character in Location scope");
	pos++;
	while (pos < params.size() && params[pos] != "}")
		locationVars.push_back(params[pos++]);
	serv.setLocation(locationPath, locationVars);
	*locationFlag = 0;
}

static void extractionRoutine(std::vector<std::string> params, Server & serv, size_t & pos, int *locationFlag,
bool *clientMaxSize,  bool *autoIndex, std::vector<std::string> & errCodes)
{
	if (params[pos] == "listen" && (pos + 1) < params.size() && *locationFlag)
		return (portRoutine(params[++pos], serv));
	else if (params[pos] == "location" && (pos + 1) < params.size())
		locationRoutine(params, serv, pos, locationFlag);
	else if (params[pos] == "host" && (pos + 1) < params.size() && *locationFlag)
		hostRoutine(params[++pos], serv);
	else if (params[pos] == "root" && (pos + 1) < params.size() && *locationFlag)
		rootRoutine(params[++pos], serv);
	else if (params[pos] == "error_page" && (pos + 1) < params.size() && *locationFlag)
		errorPageRoutine(params, pos, errCodes);
	else if (params[pos] == "client_max_body_size" && (pos + 1) < params.size() && *locationFlag)
		clientMaxSizeRoutine(params[++pos], clientMaxSize, serv);
	else if (params[pos] == "server_name" && (pos + 1) < params.size() && *locationFlag)
		serverNameRoutine(params[++pos], serv);
	else if (params[pos] == "index" && (pos + 1) < params.size() && *locationFlag)
		indexRoutine(params[++pos], serv);
	else if (params[pos] == "autoindex" && (pos + 1) < params.size() && *locationFlag)
		autoIndexRoutine(params[++pos], serv, autoIndex);
	else if (params[pos] != "}" && params[pos] != "{")
	{
		if (*locationFlag == 0)
			throw ParserErrorException("Error: Params after location");
		else
			throw ParserErrorException("Error: invalid param");
	}
}

static void	setUpServer(Server & serv, std::string & config)
{
	std::vector<std::string> params;
	std::vector<std::string> errCodes;
	int		locationFlag = 1;
	bool	clientMaxSize = false;
	bool	autoIndex = false;
	
	params = getParams(std::string(" \n\t"), config += ' ');
	if (params.size() < 3)
		throw ParserErrorException("Error: not enough params in server conf");
	size_t i = 0;
	while (i < params.size())
	{
		extractionRoutine(params, serv, i, &locationFlag, &clientMaxSize, &autoIndex, errCodes);
		i++;
	}
	if (serv.getRoot().empty())
		serv.setRoot("/;");
	if (serv.getHost() == 0)
		serv.setHost("any;");
	if (serv.getIndex().empty())
		serv.setIndex("index.html");
	if (Utils::fileExistsAndReadable(serv.getRoot() + "/", serv.getIndex()))
		throw ParserErrorException("Error: Invalid index for server");
	if (serv.checkForDuplicateLocation() == true)
		throw ParserErrorException("Error: Location duplicated");
	if (!serv.getPort())
		throw ParserErrorException("Error: Port not found for server");
	serv.setErrorPages(errCodes);
	
	std::vector<Location>::iterator it;
	for (it = serv.getLocation().begin(); it != serv.getLocation().end(); it++)
		serv.isLocationValid(*it);
}

void FileParser::buildServers(void)
{
	size_t i = 0;

	while (i < _nbServers)
	{
		Server serv;
		setUpServer(serv, _configs[i]);
		if (!this->isServerDuplicated(serv))
			_servers.push_back(serv);
		i++;
	}
	//printServers();
}

bool FileParser::isServerDuplicated(Server &server)
{
	bool isDefault = true;

	for (size_t i = 0; i < _servers.size(); i++)
	{
		if (_servers[i].getPort() == server.getPort() &&
			(_servers[i].getHost() == server.getHost() ||
			_servers[i].getHost() == INADDR_ANY))
		{
			isDefault = false;
			if (_servers[i].getServerName() == server.getServerName())
				return (true);
		}
	}
	if (server.getHost() != INADDR_ANY)
		server.setIsDefault(isDefault);
	return (false);
}

void FileParser::printServers()
{
	for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end();  it++)
		(*it).serverPrinter();
}

void FileParser::parse()
{
	removeComments();
	removeWhitespace();
	splitServer();
	buildServers();
}

std::vector<Server> &FileParser::getServers()
{
	return (this->_servers);
}

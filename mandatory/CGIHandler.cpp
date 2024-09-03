/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvicina <alvicina@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 10:11:48 by alvicina          #+#    #+#             */
/*   Updated: 2024/07/31 18:06:15 by alvicina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CGIHandler.hpp"

CgiHandler::CgiHandler(Request & request)
{
	_request = &request;
	_file = "";
	_env = NULL;
	_args = NULL;
}

CgiHandler::~CgiHandler()
{
	Utils::ftFree(_env);
	Utils::ftFree(_args);
}

CgiHandler::CgiHandler(CgiHandler & copy)
{
	*this = copy;
}

CgiHandler& CgiHandler::operator=(CgiHandler & other)
{
	if (this != &other)
	{
		_request = other._request;
		_env = other._env;
		_args = other._args;
		_mapEnv = other._mapEnv;
		_file = other._file;
	}
	return (*this);
}

void CgiHandler::setRequest(Request & request)
{
	_request = &request;
}

Request* CgiHandler::getRequest(void)
{
	return (_request);
}

std::string& CgiHandler::getFile(void)
{
	return (_file);
}

void CgiHandler::setFile(std::string & file)
{
	_file = file;
}

std::string CgiHandler::createPathToResource(void)
{
	std::string pathToResource;
	pathToResource = this->_request->getServer()->getRoot() 
	+ this->_request->getUri();
	if (_request->getLocation())
		pathToResource = _request->getLocation()->getLocationRoot()
		 + _request->getUri();
	return (pathToResource);
}

void CgiHandler::exceptionRoutine(int statusCode, Response *response)
{
	delete response;
	throw HandlerErrorException(statusCode, *_request);
}

void CgiHandler::contentForFile(Response *response, std::string & pathToResource)
{
	if (access(pathToResource.c_str(), F_OK) == -1) 
		exceptionRoutine(404, response); 
	if (access(pathToResource.c_str(), R_OK) == -1)
		exceptionRoutine(403, response);
	if (access(pathToResource.c_str(), X_OK) == -1) 
			exceptionRoutine(403, response); 
	else
		cgiExecute(response,  pathToResource);
}

void CgiHandler::contentForDIR(Response *response, std::string & pathToResource)
{
	if (access(pathToResource.c_str(), R_OK) == -1)
		exceptionRoutine(403, response);
	else
	{
		if (_request->getLocation())
		{
			pathToResource = pathToResource + "/"
			 + _request->getLocation()->getIndexLocation();
			if (access(pathToResource.c_str(), F_OK) == -1) 
				exceptionRoutine(404, response); 
			if (access(pathToResource.c_str(), R_OK) == -1)
				exceptionRoutine(403, response);
			if (access(pathToResource.c_str(), X_OK) == -1) 
				exceptionRoutine(403, response); 
			else
				cgiExecute(response, pathToResource);
		}
		else
			exceptionRoutine(403, response);
	}
}

std::string CgiHandler::methodToString(int number)
{
	switch (number)
	{
		case 1:
			return "GET";
		case 2:
			return "POST";
		case 3:
			return "DELETE";
		case 4:
			return "PUT";
		case 5:
			return "HEAD";
		default:
			return "UNKNOWN";
	}
}

std::string CgiHandler::getScriptName(void)
{
	std::string Uri = _request->getUri();
	size_t pos = Uri.find(".");
	if (pos == std::string::npos)
		return (_request->getLocation()->getIndexLocation());
	pos = Uri.find_last_of('/');
	std::string scriptName = Uri.substr(pos);
	return (scriptName);
}

void CgiHandler::initEnvironmentForCgi(std::string & pathToResource)
{
	_mapEnv["AUTH_TYPE"] = "Basic";
	_mapEnv["CONTENT_LENGTH"] = _request->getHeaders()["content-length"];
	_mapEnv["CONTENT_TYPE"] = _request->getHeaders()["content-type"];
	_mapEnv["GATEWAY_INTERFACE"] = "CGI/1.1";
	_mapEnv["HTTP_ACCEPT"] = _request->getHeaders()["accept"];
	_mapEnv["HTTP_ACCEPT_CHARSET"] = _request->getHeaders()["accept-charset"];
	_mapEnv["HTTP_ACCEPT_ENCODING"] = _request->getHeaders()["accept-encoding"];
	_mapEnv["HTTP_ACCEPT_LANGUAGE"] = _request->getHeaders()["accept-language"];
	_mapEnv["HTTP_FORWARDED"] = _request->getHeaders()["forwarded"];
	_mapEnv["HTTP_HOST"] = _request->getHeaders()["host"];
	_mapEnv["HTTP_PROXY_AUTHORIZATION"] = _request->getHeaders()["proxy-authorization"];
	_mapEnv["HTTP_USER_AGENT"] = _request->getHeaders()["user-agent"];
	_mapEnv["PATH_INFO"] = _request->getPathInfo();
	_mapEnv["QUERY_STRING"] = _request->getQueryString();
	_mapEnv["REQUEST_METHOD"] = methodToString((int)_request->getMethod());
	_mapEnv["SCRIPT_NAME"] = _request->getUri();
	_mapEnv["SCRIPT_FILENAME"] = pathToResource;
	_mapEnv["SERVER_NAME"] = _request->getServer()->getServerName();
	_mapEnv["SERVER_PORT"] = Utils::intToString(_request->getServer()->getPort());
	_mapEnv["SERVER_PROTOCOL"] = _request->getProtocol() + "/" +_request->getProtocolVersion();
	_mapEnv["SERVER_SOFTWARE"] = "WebServer42";
	_mapEnv["HTTP_COOKIE"] = _request->getHeaders()["cookie"];
	_mapEnv["DOCUMENT_ROOT"] = _request->getLocation()->getLocationRoot();
	_mapEnv["REQUEST_URI"] = _request->getUri();
	_mapEnv["REDIRECT_STATUS"] = "200";
}

void CgiHandler::transformEnvToChar(Response *response)
{
	std::map<std::string, std::string>::iterator it;
	size_t i = 0;
	for (it = _mapEnv.begin(); it != _mapEnv.end(); it++)
	{
		std::string temp = it->first + "=" + it->second;
		_env[i] = strdup(temp.c_str());
		if (_env[i] == NULL)
			exceptionRoutine(500, response);
		i++;
	}
	_env[_mapEnv.size()] = NULL;
}

void CgiHandler::allocSpaceForEnv(Response *response)
{
	_env = (char **)malloc(sizeof(char *) * (_mapEnv.size() + 1));
	if (_env == NULL)
		exceptionRoutine(500, response);
}

void CgiHandler::parseEnvironmentForCgi(Response *response)
{
	allocSpaceForEnv(response);
	transformEnvToChar(response);
}

std::string CgiHandler::validateResourseExtension(std::string & pathToResource)
{
	size_t pos = pathToResource.find_last_of('.');
	if (pos == std::string::npos)
		return ("");
	std::string extension = pathToResource.substr(pos);
	std::map<std::string, std::string> map = _request->getLocation()->getExtPathMap();
	std::map<std::string, std::string>::iterator it;
	for (it = map.begin(); it != map.end(); it++)
	{
		if (it->first == extension)
		{
			return (it->second);
		}
	}
	return ("");
}

void CgiHandler::allocSpaceForCgiArgs(Response *response,
 size_t numberOfArguments)
{
	_args = (char **)malloc(sizeof(char *) * numberOfArguments);
	if (_args == NULL)
		exceptionRoutine(500, response);
}

void CgiHandler::setResourcePathAndInterpreter(std::string & pathToResource,
 std::string & pathToInterpreter, Response *response)
{
	size_t pos = pathToResource.find_last_of('/');
	std::string file = pathToResource.substr(pos + 1);
	setFile(file);
	std::string dirToChange = pathToResource.erase(pos);
	if (chdir(dirToChange.c_str()) != 0)
		exceptionRoutine(500, response);
	_args[0] = strdup(getFile().c_str());
	if (_args[0] == NULL)
		exceptionRoutine(500, response);
	if (pathToInterpreter != "")
	{
		_args[1] = strdup(pathToInterpreter.c_str());
		if (_args[1] == NULL)
			exceptionRoutine(500, response);
	}
}

void CgiHandler::setOtherArgs(size_t & numberOfArguments, Response *response,
std::string pathToInterpreter)
{
	std::map<std::string, std::string>::iterator it;
	size_t i = 2;
	if (pathToInterpreter == "")
		i = 1;
	for (it = _request->getArgs().begin(); it != _request->getArgs().end(); it++)
	{
		std::string temp = it->first + "=" + it->second;
		_args[i] = strdup(temp.c_str());
		if (_args[i] == NULL)
			exceptionRoutine(500, response);
		i++;
	}
	_args[numberOfArguments - 1] = NULL;
}

void CgiHandler::isInterpreterOK(std::string & pathToInterpreter, 
Response *response)
{
	if (access(pathToInterpreter.c_str(), F_OK) == -1) 
			exceptionRoutine(500, response); 
	if (access(pathToInterpreter.c_str(), X_OK) == -1) 
			exceptionRoutine(500, response); 
}

void CgiHandler::initArgsForCgi(std::string & pathToResource, 
Response *response)
{
	std::string pathToInterpreter = validateResourseExtension(pathToResource);
	size_t numberOfArguments = 2 + _request->getArgs().size() + 1;
	if (pathToInterpreter == "")
		numberOfArguments = 1 + _request->getArgs().size() + 1;
	else
		isInterpreterOK(pathToInterpreter, response);
	allocSpaceForCgiArgs(response, numberOfArguments);
	setResourcePathAndInterpreter(pathToResource, pathToInterpreter, response);
	setOtherArgs(numberOfArguments, response, pathToInterpreter);
}

void CgiHandler::childRoutine(int *pipeFD, Response *response, int *pipeFD2)
{
	close(pipeFD2[1]); //cerramos extremo de escritura porque el hijo lee en este pipe y no escribe
	if (dup2(pipeFD2[0], STDIN_FILENO) == -1)
		exceptionRoutine(500, response);
	close(pipeFD2[0]); //cerramos extremo de lectura porque hemos redireccinado el stdin al extremo de lectura de este pipe
	close(pipeFD[0]); //cerramos extremo de lectura porque el hijo escribe en este pipe y no lee
	if (dup2(pipeFD[1], STDOUT_FILENO) == -1)
		exceptionRoutine(500, response);
	close(pipeFD[1]); //cerramos extremo de escritura porque hemos redireccinado el stdout al extremo de escritura de este pipe
	if (execve(getFile().c_str(), _args, _env) == -1)
	{
		delete response;
		throw CGIChildProcessErrorException();
	}
}


void CgiHandler::timeOutRoutine(pid_t *pid, int *pipeFD, Response *response)
{
	int status;
	std::time_t now = std::time(NULL);
	std::time_t future = now + 5;

	bool childActive = true;
	while (childActive)
	{
		pid_t pid2 = waitpid(*pid, &status, WNOHANG);
		if (pid2 == 0)
		{	
			now = std::time(NULL);
			if (now <= future)
				continue ;
			else
			{
				if (childActive)
				{
					kill(*pid, SIGKILL);
					childActive = false;
				}
				close(pipeFD[0]);
				if (waitpid(*pid, &status, 0) == -1)
					exceptionRoutine(500, response);
				exceptionRoutine(504, response);
			}
		}
		else if (pid2 == -1 || status != 0)
		{
			close(pipeFD[0]);
			exceptionRoutine(500, response);
		}
		else
			childActive = false;
	}
}

void CgiHandler::parentRoutine(int *pipeFD, Response *response, pid_t *pid, int *pipeFD2)
{
	close(pipeFD2[0]);
	write(pipeFD2[1], (const void*)_request->getContent().c_str(), _request->getContent().size());
	close(pipeFD2[1]);
	close(pipeFD[1]);
	timeOutRoutine(pid, pipeFD, response);

	char buffer[1024];
	ssize_t bytesRead = 1;
	std::string content = "";
	while (bytesRead)
	{
		bytesRead = read(pipeFD[0], buffer, sizeof(buffer));
		buffer[bytesRead] = 0;
		if (bytesRead == 0)
			break ;
		if (bytesRead == -1)
		{
			close(pipeFD[0]);
			exceptionRoutine(500, response);
		}
		std::string bufferString(buffer);
		content = content + bufferString;
	}
	close(pipeFD[0]);
	response->setContent(content);
}

void CgiHandler::forkAndExecve(Response *response)
{
	int pipeFD[2];
	int pipeFD2[2];
	
	if (pipe(pipeFD) == -1)
		exceptionRoutine(500, response);
	if (pipe(pipeFD2) == -1)
		exceptionRoutine(500, response);
	pid_t pid = fork();
	if (pid == -1)
		exceptionRoutine(500, response);
	else if (pid == 0)
		childRoutine(pipeFD, response, pipeFD2);
	else
		parentRoutine(pipeFD, response, &pid, pipeFD2);
}

void CgiHandler::cgiExecute(Response *response, std::string & pathToResource)
{
	initEnvironmentForCgi(pathToResource);
	parseEnvironmentForCgi(response);
	initArgsForCgi(pathToResource, response);
	forkAndExecve(response);
}

void CgiHandler::handleCgiRequest(Response *response)
{
	std::string pathToResource = createPathToResource();
	int typeOfResource = Utils::typeOfFile(pathToResource);
	if (typeOfResource == 1)
		contentForFile(response, pathToResource);
	else if (typeOfResource == 2)
		contentForDIR(response, pathToResource);
	else if (typeOfResource == -1)
		exceptionRoutine(404, response);
}
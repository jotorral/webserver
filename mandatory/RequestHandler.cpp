/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvicina <alvicina@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 12:03:13 by alvicina          #+#    #+#             */
/*   Updated: 2024/07/31 10:52:12 by alvicina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/RequestHandler.hpp"
#include "../includes/Server.hpp"

RequestHandler::RequestHandler(Request & request) : _method(request.getMethod()),
_request(&request), _errorCode(0)
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

RequestHandler::~RequestHandler()
{
	
}

RequestHandler::RequestHandler(RequestHandler & copy) : _method(copy._method),
_request(copy._request)
{
	
}

RequestHandler& RequestHandler::operator=(RequestHandler & other)
{
	if (this != &other)
	{
		_method = other._method;
		_request = other._request;
	}
	return (*this);
}

Methods& RequestHandler::getMethods(void)
{
	return (_method);
}

Request* RequestHandler::getRequest(void)
{
	return (_request);
}

std::string & RequestHandler::getExts(std::string & ext)
{
	if (_exts.count(ext))
		return (_exts[ext]);
	return (_exts["default"]);
}

Response* RequestHandler::handleRequest()
{
    try
    {
        Response* response = doHandleRequest();
        return (response);
    }
    catch(HandlerErrorException &e)
    {
        Response* errorResponse = e.createResponse();
        return (errorResponse);
    }
}

bool RequestHandler::isRequestMethodAllow()
{
	if (_request->getLocation())
	{
		std::vector<int>::iterator it;
		std::vector<int> methods = _request->getLocation()->getLocationMethods();
        if (methods.empty())
            return (true);
		for (it = methods.begin(); it != methods.end(); it++)
		{
			if (_request->getMethod() == *it)
				return (true);
		}
		return (false);
	}
	return (true);
}

bool RequestHandler::checkAndSetReturn()
{
	std::string pathToResource = createPathToResource();
	int typeOfFile = Utils::typeOfFile(pathToResource);
	size_t pos = pathToResource.size();
	if (typeOfFile == 2 && pathToResource[pos - 1] != '/')
	{
		return (true);
	}
	else if (_request->getLocation())
	{
		if (!_request->getLocation()->getReturnLocation().empty())
			return (true);
	}
	return (false);
}

std::string RequestHandler::createPathToResource()
{
	std::string pathToResource;
	pathToResource = this->_request->getServer()->getRoot() + this->_request->getUri();
	if (_request->getLocation())
		pathToResource = _request->getLocation()->getLocationRoot() + _request->getUri();
	return (pathToResource);
}

std::string RequestHandler::createNewUriForAlias(std::string & alias)
{
	std::string root = _request->getServer()->getRoot();
	size_t rootSize = root.size();
	std::string insertToOldUri = alias.substr(rootSize);
	if (insertToOldUri[0] != '/')
		insertToOldUri = "/" + insertToOldUri;
	std::string locationPath = _request->getLocation()->getLocationPath();
	size_t locationPathSize = locationPath.size();
	std::string oldUri = _request->getUri();
	std::string newUri = insertToOldUri + oldUri.erase(0, locationPathSize);
	return (newUri);
}

void RequestHandler::setNewLocation(Request & request)
{
	std::vector<Location> &locations = request.getServer()->getLocation();

	for (size_t i = 0; i < locations.size(); i++)
	{
		std::string testDir;
		size_t uriSize;
		size_t locationPathSize;

		uriSize = request.getUri().size();
		locationPathSize = locations[i].getLocationPath().size();
		testDir = request.getUri();
		testDir = testDir.substr(0, locationPathSize);
		if (testDir.compare(locations[i].getLocationPath()) != 0)
			continue;
		if (uriSize > locationPathSize && request.getUri()[locationPathSize] != '/')
			continue;
		Location *requestLocation = request.getLocation();
		if (!requestLocation || requestLocation->getLocationPath().size() < locationPathSize)
			request.setLocation(locations[i]);
	}
}

void RequestHandler::checkAndSetAlias()
{
	if (_request->getLocation())
	{
		if(!_request->getLocation()->getAliasLocation().empty())
		{
			std::string newUri = createNewUriForAlias(_request->getLocation()->getAliasLocation());
			_request->setUri(newUri);
			setNewLocation(*_request);
		}
	}
}

void RequestHandler::doCgi(Response *response)
{
    CgiHandler cgiHandler(*_request);
    cgiHandler.handleCgiRequest(response);
}

int RequestHandler::fileHasDot(bool *isCgi, std::string & pathToResource)
{
    size_t pos = pathToResource.find_last_of('.');
    if (pos == std::string::npos)
    {
        *isCgi = false;
        return (1);
    }
    return (0);
}

int RequestHandler::thereIsExtensionMatch(bool *isCgi, std::string & pathToResource)
{
    size_t pos = pathToResource.find_last_of('.');
    std::string extension = pathToResource.substr(pos);
    std::vector<std::string> exts = _request->getLocation()->getCgiExtensionLocation();
    std::vector<std::string>::iterator it;
    for (it = exts.begin(); it != exts.end(); it++)
    {
        if (*it == extension)
        {
            *isCgi = true;
            return (0);
        }
    }
    return (1);
}

void RequestHandler::fileRoutine(bool *isCgi, std::string & pathToResource)
{
    if (fileHasDot(isCgi, pathToResource) == 1)
        return ;
    if (thereIsExtensionMatch(isCgi, pathToResource) == 0)
        return ;
    *isCgi = false;       
}

void RequestHandler::contentRoutine(bool *isCgi, std::string & pathToResource)
{
    std::string newPathToResource = pathToResource + "/"
     + _request->getLocation()->getIndexLocation();
    fileRoutine(isCgi, newPathToResource);
}

bool RequestHandler::checkResourseExtension(Response *response)
{
    bool isCgi = false;
    std::string pathToResource = createPathToResource();
    int typeOfResource = Utils::typeOfFile(pathToResource.c_str());
    if (typeOfResource == 1)
        fileRoutine(&isCgi, pathToResource);
    else if (typeOfResource == 2)
        contentRoutine(&isCgi, pathToResource);
    else if (typeOfResource == -1)
        exceptionRoutine(404, response);
    return (isCgi);
}

bool RequestHandler::isCgiRequest(Response *response)
{
	if (_request->getLocation())
    {
        if (!_request->getLocation()->getCgiExtensionLocation().empty())
        {
            bool isCgiRequest = checkResourseExtension(response);
            return (isCgiRequest);
        }
    }
    return (false);
}

void RequestHandler::exceptionRoutine(int statusCode, Response *response)
{
		delete response;
		throw HandlerErrorException(statusCode, *_request);
}

void RequestHandler::checkUploadPath(Response *response)
{
	std::string whereToUpload = _request->getLocation()->getUploadStore();
	if (access(whereToUpload.c_str(), F_OK) == -1)
		exceptionRoutine(404, response);
	if (access(whereToUpload.c_str(), W_OK | X_OK) == -1)
		exceptionRoutine(403, response);
}

void RequestHandler::createFile(Response *response)
{
	std::ofstream outfile(_request->getUploadFileName().c_str());
	if (!outfile)
		exceptionRoutine(500, response);
	outfile << _request->getUploadFileContent();
	outfile.close();
}

void RequestHandler::uploadFile(Response *response)
{
	checkUploadPath(response);
	std::string whereToUpload = _request->getLocation()->getUploadStore();
	char *cwd = getcwd(NULL, 0);
	if (cwd == NULL)
		exceptionRoutine(500, response);
	if (chdir(whereToUpload.c_str()) != 0)
		exceptionRoutine(500, response);
	createFile(response);
	if (chdir(cwd) != 0)
		exceptionRoutine(500, response);
	free(cwd);
}

HandlerErrorException::HandlerErrorException(int errCode, Request & request) throw()
{
    _errCode = errCode;
    _request = &request;
}

Response* HandlerErrorException::createResponse() const throw()
{
    return (new Response(_errCode, _request));
}

HandlerErrorException::~HandlerErrorException(void) throw()
{
    
}

int & HandlerErrorException::getErrCode(void)
{
    return (_errCode);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlerGET.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvicina <alvicina@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 12:39:56 by alvicina          #+#    #+#             */
/*   Updated: 2024/07/31 17:19:30 by alvicina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/RequestHandlerGET.hpp"
#include "../includes/Server.hpp"

RequestHandlerGet::RequestHandlerGet(Request & request) : 
RequestHandler(request)
{
	
}

RequestHandlerGet::~RequestHandlerGet()
{
	
}

RequestHandlerGet::RequestHandlerGet(RequestHandlerGet & copy) : 
RequestHandler(*copy._request)
{
	
}

RequestHandlerGet& RequestHandlerGet::operator=(RequestHandlerGet & other)
{
	if (this != &other)
	{
		_method = other._method;
		_request = other._request;
	}
	return (*this);
}

void RequestHandlerGet::openReadCopyFile(Response *response, std::string & pathToResource)
{
	std::ifstream resourse(pathToResource.c_str());
	std::stringstream resourceContent;
	resourceContent << resourse.rdbuf();
	std::string content = resourceContent.str();
	response->setContent(content);
	response->setFile(pathToResource);
}

void RequestHandlerGet::htmlIndexBuilder(Response * response)
{
	int status = response->buildHtmlIndex(*_request);
	if (status)
		exceptionRoutine(status, response);
	std::string file = "default";
	response->setFile(file);
}

void RequestHandlerGet::contentForDIR(Response * response, std::string & pathToResource)
{
	if (access(pathToResource.c_str(), R_OK) == -1)
		exceptionRoutine(403, response);
	else
	{
		if (_request->getLocation())
		{
			pathToResource = pathToResource + "/" + _request->getLocation()->getIndexLocation();
			if (_request->getLocation()->getAutoIndexLocation())
				htmlIndexBuilder(response);
			else if (Utils::typeOfFile(pathToResource) != 1)
				exceptionRoutine(403, response);
			else
			{
				if (access(pathToResource.c_str(), R_OK) != -1)
					openReadCopyFile(response, pathToResource);
				else
					exceptionRoutine(403, response);
			}
		}
		else
			exceptionRoutine(403, response);
	}
}

void RequestHandlerGet::contentForFile(Response *response, std::string & pathToResource)
{
	if (access(pathToResource.c_str(), R_OK) == -1)
		exceptionRoutine(403, response);
	else
		openReadCopyFile(response,  pathToResource);
}

void RequestHandlerGet::ResponseContentRoutine(Response *response)
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

Response * RequestHandlerGet::doHandleRequest(void)
{
	Response	*response = new Response();
	bool		reddir = false;
	bool 		isCgi = false;

	bool isValid = isRequestMethodAllow();
	if (isValid == false)
		exceptionRoutine(405, response);
	reddir = checkAndSetReturn();
		if (reddir == false)
		checkAndSetAlias();
	isCgi = isCgiRequest(response);
	if (isCgi == true)
		doCgi(response);
	if (reddir == false && isCgi == false)
		ResponseContentRoutine(response);
	response->setProtocol(_request->getProtocol());
	response->setProtocolVersion(_request->getProtocolVersion());
	response->ResponseHeaderRoutine(*response, _request);
	response->setStatusCode(200);
	if (reddir)
		response->setStatusCode(301);
	std::string statusCodeMessage = Utils::codeStatus(response->getStatusCode());
	response->setStatusCodeMessage(statusCodeMessage);
	response->ResponseRawRoutine();
	return (response);
}

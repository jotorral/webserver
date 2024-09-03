/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlerDELETE.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvicina <alvicina@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 12:58:56 by alvicina          #+#    #+#             */
/*   Updated: 2024/07/08 17:13:23 by alvicina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/RequestHandlerDELETE.hpp"
#include "../includes/Server.hpp"

RequestHandlerDelete::RequestHandlerDelete(Request & request) : 
RequestHandler(request)
{
	
}

RequestHandlerDelete::~RequestHandlerDelete()
{
	
}

RequestHandlerDelete::RequestHandlerDelete(RequestHandlerDelete & copy) : 
RequestHandler(*copy._request)
{
	
}

RequestHandlerDelete& RequestHandlerDelete::operator=(RequestHandlerDelete & other)
{
	if (this != &other)
	{
		_method = other._method;
		_request = other._request;
	}
	return (*this);
}

Response* RequestHandlerDelete::doHandleRequest(void)
{
	Response	*response = new Response();
	bool		reddir = false;

	if (isRequestMethodAllow() == false)
		exceptionRoutine(405, response);
	reddir = checkAndSetReturn();
	if (reddir == false)
		checkAndSetAlias();

	std::string	pathAndFile = createPathToResource();

	if (reddir == false)
	{
		// ********** Deletes file if possible **********
		size_t pos = pathAndFile.size();
		if (pathAndFile[pos - 1] == '/')
			pathAndFile.erase(pos - 1);
		int err = std::remove(pathAndFile.c_str());
		if (err != 0)
		{
			int errnum = this->fileError(pathAndFile);
			exceptionRoutine(errnum, response);
		}
		else
		{
			std::string content = pathAndFile + " has been successfully deleted!!";
			response->setContent(content);
			response->setProtocol(_request->getProtocol());
			response->setProtocolVersion(_request->getProtocolVersion());
			response->ResponseHeaderRoutine(*response, _request);
			response->setStatusCode(200);
			std::string statusCodeMessage = Utils::codeStatus(response->getStatusCode());
			response->setStatusCodeMessage(statusCodeMessage);
			response->ResponseRawRoutine();
		}
	}
	else
	{
		response->setProtocol(_request->getProtocol());
		response->setProtocolVersion(_request->getProtocolVersion());
		response->ResponseHeaderRoutine(*response, _request);
		response->setStatusCode(301);
		std::string statusCodeMessage = Utils::codeStatus(response->getStatusCode());
		response->setStatusCodeMessage(statusCodeMessage);
		response->ResponseRawRoutine();
	}
	return (response);
}

// Considering the fact that an error has happend, returns the type of the error
int RequestHandlerDelete::fileError(std::string pathAndFile)
{
	std::ifstream file(pathAndFile.c_str());
	if (!file.good())
		return (404); // If file/folder doesn't exist, 404 doesn't exist error
	return (403); // If it exist, 403 forbidden error
}


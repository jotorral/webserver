/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlerPOST.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvicina <alvicina@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 12:52:15 by alvicina          #+#    #+#             */
/*   Updated: 2024/07/31 11:54:28 by alvicina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/RequestHandlerPOST.hpp"

RequestHandlerPost::RequestHandlerPost(Request & request) : 
RequestHandler(request)
{
	
}

RequestHandlerPost::~RequestHandlerPost()
{
	
}

RequestHandlerPost::RequestHandlerPost(RequestHandlerPost & copy) : 
RequestHandler(*copy._request)
{
	
}

RequestHandlerPost& RequestHandlerPost::operator=(RequestHandlerPost & other)
{
	if (this != &other)
	{
		_method = other._method;
		_request = other._request;
	}
	return (*this);
}

Response * RequestHandlerPost::doHandleRequest(void)
{
	Response	*response = new Response();
	bool		reddir = false;
	bool 		isCgi = false;

	if (!_request->getUploadFileName().empty())
		uploadFile(response);
	bool isValid = isRequestMethodAllow();
	if (isValid == false)
		exceptionRoutine(405, response);
	reddir = checkAndSetReturn();
	if (reddir == false)
		checkAndSetAlias();
	if (Utils::typeOfFile(createPathToResource()) == -1)
		exceptionRoutine(404, response);
	isCgi = isCgiRequest(response);
	if (isCgi == true)
		doCgi(response);
	else
		exceptionRoutine(405, response);
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
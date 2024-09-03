/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestFactory.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvicina <alvicina@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 11:04:21 by alvicina          #+#    #+#             */
/*   Updated: 2024/07/19 10:22:07 by alvicina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/RequestFactory.hpp"

RequestFactory::RequestFactory()
{
	
}

RequestFactory::~RequestFactory()
{

}

RequestFactory::RequestFactory(RequestFactory & copy)
{
	*this = copy;
}

RequestFactory& RequestFactory::operator=(RequestFactory & other)
{
	(void) other;
	return (*this);	
}

RequestHandler* RequestFactory::makeRequestHandler(Request & request)
{
	switch(request.getMethod())
	{
		case GET:
			return (new RequestHandlerGet(request));
		case POST:
			return (new RequestHandlerPost(request));
		case DELETE:
			return (new RequestHandlerDelete(request));
		default:
			throw FactoryErrorException(405, request);
	}
}

FactoryErrorException::FactoryErrorException(int errCode, Request & request) throw()
{
    _errCode = errCode;
    _request = &request;
}

Response* FactoryErrorException::createResponse() const throw()
{
    return (new Response(_errCode, _request));
}

FactoryErrorException::~FactoryErrorException(void) throw()
{
    
}

int & FactoryErrorException::getErrCode(void)
{
    return (_errCode);
}
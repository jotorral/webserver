/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestFactory.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvicina <alvicina@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 18:51:51 by alvicina          #+#    #+#             */
/*   Updated: 2024/07/19 10:19:42 by alvicina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTFACTORY_HPP
#define REQUESTFACTORY_HPP

#include "webserv.hpp"
#include "RequestHandler.hpp"
#include "RequestHandlerDELETE.hpp"
#include "RequestHandlerGET.hpp"
#include "RequestHandlerPOST.hpp"
#include "Request.hpp"

class RequestFactory
{
	private:
		
	public:
		RequestFactory();
		~RequestFactory();
		RequestFactory(RequestFactory & copy);
		RequestFactory& operator=(RequestFactory & other);
		static RequestHandler* makeRequestHandler(Request & request);	
};

class FactoryErrorException : public std::exception
{
	private:
		int	_errCode;
		Request *_request;
	public:
		FactoryErrorException(int errCode, Request & request) throw();
		Response* createResponse() const throw();
		virtual ~FactoryErrorException() throw();
		int &getErrCode();
};

#endif
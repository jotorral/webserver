/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlerPOST.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvicina <alvicina@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 11:42:40 by alvicina          #+#    #+#             */
/*   Updated: 2024/07/08 17:13:14 by alvicina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTHANDLERPOST_HPP
#define REQUESTHANDLERPOST_HPP

#include "RequestHandler.hpp"
#include "RequestFactory.hpp"

class RequestHandlerPost : public RequestHandler
{
	private:

	public:
		RequestHandlerPost(Request & request);
		RequestHandlerPost(RequestHandlerPost & copy);
		RequestHandlerPost& operator=(RequestHandlerPost & other);
		virtual ~RequestHandlerPost();
		virtual Response* doHandleRequest(void);
};

#endif
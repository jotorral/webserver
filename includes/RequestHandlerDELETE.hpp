/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlerDELETE.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvicina <alvicina@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 11:39:28 by alvicina          #+#    #+#             */
/*   Updated: 2024/07/08 17:13:05 by alvicina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTHANDLERDELETE_HPP
#define REQUESTHANDLERDELETE_HPP

#define MAX_FILENAME_LENGTH 30

#include "RequestHandler.hpp"

class RequestHandlerDelete : public RequestHandler
{
	private:
		void ResponseContentRoutine(Response *response);
	public:
		RequestHandlerDelete(Request & request);
		RequestHandlerDelete(RequestHandlerDelete & copy);
		RequestHandlerDelete& operator=(RequestHandlerDelete & other);
		virtual ~RequestHandlerDelete();
		virtual Response* doHandleRequest(void);
		std::string strPathAndFile();
		int fileError(std::string pathAndFile);
};

#endif
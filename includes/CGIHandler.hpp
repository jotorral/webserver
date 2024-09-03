/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvicina <alvicina@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 10:11:57 by alvicina          #+#    #+#             */
/*   Updated: 2024/07/31 18:02:26 by alvicina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"

class CgiHandler
{
	private:
		Request 	*_request;
		char		**_env;
		char		**_args;
		std::map<std::string, std::string>  _mapEnv;
		std::string	_file;
		fd_set		_readfds;
		
		std::string createPathToResource(void);
		void exceptionRoutine(int statusCode, Response *response);
		void contentForFile(Response *response, std::string & pathToResource);
		void contentForDIR(Response *response, std::string & pathToResource);
		void cgiExecute(Response *response, std::string & pathToResource);
		void allocSpaceForEnv(Response *response);
		void initEnvironmentForCgi(std::string & pathToResource);
		void parseEnvironmentForCgi(Response *response);
		void transformEnvToChar(Response *response);
		void setOtherArgs(size_t & numberOfArgurments, Response *response, std::string pathToInterpreter);
		void setResourcePathAndInterpreter(std::string & pathToResource, std::string & pathToInterpreter, Response *response);
		void isInterpreterOK(std::string & pathToInterpreter, Response *response);
		void allocSpaceForCgiArgs(Response *response, size_t numberOfArguments);
		void initArgsForCgi(std::string & pathToResource, Response *response);
		std::string validateResourseExtension(std::string & pathToResource);
		void childRoutine( int *pipeFD, Response *response, int *pipeFD2);
		void parentRoutine(int *pipeFD, Response *response, pid_t *pid, int *pipeFD2);
		void forkAndExecve(Response *response);
		void timeOutRoutine(pid_t *pid, int *pipeFD, Response *response);
		std::string methodToString(int number);
		std::string getScriptName(void);
		/*fd_set& getFdSet(void);
		void setFdSet(fd_set & readfds);*/
	
		
	public:
		CgiHandler(Request & request);
		~CgiHandler();
		CgiHandler(CgiHandler & copy);
		CgiHandler& operator=(CgiHandler & other);
		void setRequest(Request & request);
		Request* getRequest(void);
		void handleCgiRequest(Response * response);
		std::string &getFile(void);
		void setFile(std::string & file);

		class CGIChildProcessErrorException : public std::exception
		{
			public:
				const char *what() const throw()
				{
					return ("The child process failed execute the cgi script.");
				}
		};
};
#endif
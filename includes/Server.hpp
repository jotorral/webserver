/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvicina <alvicina@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 11:56:49 by alvicina          #+#    #+#             */
/*   Updated: 2024/07/29 18:33:37 by alvicina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "webserv.hpp"
#include "Socket.hpp"
#include "Utils.hpp"
#include "Location.hpp"
#include "RequestHandler.hpp"
#include "RequestFactory.hpp"

class Request;
class Response;

class Server
{
	private:
		uint16_t					_port;
		in_addr_t					_host;
		std::string					_serverName;
		std::string					_root;
		long int					_clientMaxBodySize;
		std::string					_index;
		bool						_autoIndex;
		std::map<int, std::string> 	_errorPages;
		std::vector<Location>		_locations;
		Socket						*_masterSocket;
		bool						_isDefault;

	public:
		Server();
		Server(Server const & copy);
		~Server();
		Server& operator=(Server const & other);
		uint16_t & getPort();
		in_addr_t & getHost();
		std::string & getRoot();
		std::string & getServerName();
		long int & getClientMaxBodySize(void);
		std::string & getIndex(void);
		bool & getAutoindex(void);
		std::vector<Location> & getLocation(void);
		std::map<int, std::string> & getErrorPages(void);
		void 	setLocation(std::string & locationPath, std::vector<std::string> & locationVars);
		void 	setAutoIndex(std::string const & param);
		void 	setIndex(std::string const & param);
		void 	setRoot(std::string param);
		void	setPort(std::string & param);
		void 	setHost(std::string param);
		void 	setServerName(std::string const & param);
		void 	setClientMaxSize(std::string const & param);
		void	checkParamToken(std::string & param);
		void	initErrorPages(void);
		void 	locationRootRoutine(std::string & locationVars, Location & location);
		void 	locationExtractionRoutine(std::vector<std::string> & locationVars, size_t & pos, Location & location, bool & methodsFlag, bool & autoIndexFlag, bool & maxSizeFlag);
		void	locationMethodsRoutine(std::vector<std::string> & locationVars, size_t & pos, bool & methodsFlag, Location & location);
		void	locationAutoIndexRoutine(std::string & autoIndex, bool & autoIndexFlag, Location & location);
		void 	locationIndexRoutine(std::string & index, Location & location);
		void 	locationReturnRoutine(std::string & Return, Location & location);
		void	locationAliasRoutine(std::string & alias, Location & location);
		void	locationCgiExtRoutine(std::vector<std::string> & locationVars, size_t & pos, Location & location);
		void	locationCgiPathRoutine(std::vector<std::string> & locationVars, size_t & pos, Location & location);
		void	locationMaxSizeRoutine(std::string & maxSize, bool & maxSizeFlag, Location & location);
		void 	locationUploadRoutine(std::string & upLoadPath, Location & location);
		void	isLocationValid(Location & location);
		void	checkLocationForCGI(Location & location);
		void 	checkLocationCgiPath(Location & location);
		void	checkLocationCgiExtension(Location & location);
		bool	checkForDuplicateLocation(void);
		void	setErrorPages(std::vector<std::string> & errorCodes);
		void 	checkErrorPageForFile(std::string & errorPath);
		void	serverPrinter(void);
		void	initMasterSocket();
		Socket 	&getSocket();
		Response *handleRequest(Request &request);
		bool getIsDefault();
		void setIsDefault(bool isDefault);
};		

#endif
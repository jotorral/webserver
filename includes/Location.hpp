/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvicina <alvicina@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 12:19:46 by alvicina          #+#    #+#             */
/*   Updated: 2024/07/29 18:26:14 by alvicina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "webserv.hpp"
#include "Utils.hpp"

class Location
{
	private:
		std::string					_path;
		std::string					_root;
		bool						_autoIndex;
		std::string					_index;
		std::vector<int> 			_methods;
		std::string					_return;
		std::string					_alias;
		std::vector<std::string>	_cgiPath;
		std::vector<std::string>	_cgiExt;
		long int					_clientMaxBodySize;
		std::map<std::string, std::string> _extPath;
		std::string					_uploadStore;
		
	public:
		
		
		Location();
		Location(Location const & copy);
		~Location();
		Location& operator=(Location const & other);
		std::string & getLocationPath();
		std::string & getLocationRoot();
		std::vector<int> & getLocationMethods();
		bool &			 getAutoIndexLocation();
		std::string & 	 getIndexLocation();
		std::string & 	 getReturnLocation();
		std::string &		 getAliasLocation();
		std::vector<std::string> & getCgiExtensionLocation();
		std::vector<std::string> & getCgiPathLocation();
		long int & getMaxBodySizeLocation();
		std::map<std::string, std::string> & getExtPathMap();
		void	setPath(std::string const & path);
		void	setRootLocation(std::string const & root);
		void	setLocationMethods(std::vector<std::string> & methods);
		void 	setLocationAutoIndex(std::string const & autoIndex);
		void 	setIndexLocation(std::string const & index);
		void 	setReturnLocation(std::string const & Return);
		void 	setAliasLocation(std::string const & alias);
		void 	setCgiExtensionLocation(std::vector<std::string> const & cgiExt);
		void	setCgiPathLocation(std::vector<std::string> const & cgiPath);
		void	setMaxBodySizeLocation(std::string const & maxSize);
		void 	setMaxBodySizeLocation(long int & maxBodySize);
		void	locationPrinter(void);
		std::string &getUploadStore();
		void setUploadStore(const std::string &uploadStore);
			
};

#endif
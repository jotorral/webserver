/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvicina <alvicina@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 12:37:22 by alvicina          #+#    #+#             */
/*   Updated: 2024/07/31 11:51:41 by alvicina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Location.hpp"

Location::Location()
{
	_path = "";
	_root = "";
	_autoIndex = false;
	_index = "";
	_return = "";
	_alias = "";
	_clientMaxBodySize = MAX_CONTENT_LENGTH;
	_uploadStore = "";
	_methods.reserve(5);
	_methods.push_back(1);
	_methods.push_back(0);
	_methods.push_back(0);
	_methods.push_back(0);
	_methods.push_back(0);
}

Location::~Location()
{
	
}

Location::Location(Location const & copy)
{
	*this = copy;
}

Location& Location::operator=(Location const & other)
{
	if (this != &other)
	{
		_path = other._path;
		_root = other._root;
		_autoIndex = other._autoIndex;
		_index = other._index;
		_methods = other._methods;
		_return = other._return;
		_alias = other._alias;
		_cgiPath = other._cgiPath;
		_cgiExt = other._cgiExt;
		_clientMaxBodySize = other._clientMaxBodySize;
		_extPath = other._extPath;
		_uploadStore = other._uploadStore;
	}
	return (*this);
}

std::string & Location::getLocationPath()
{
	return (_path);
}

		
std::string & Location::getLocationRoot()
{
	return (_root);
}

std::vector<int> & Location::getLocationMethods()
{
	return (_methods);
}

bool & Location::getAutoIndexLocation()
{
	return (_autoIndex);
}

std::string & Location::getIndexLocation()
{
	return (_index);
}

std::string & Location::getReturnLocation()
{
	return (_return);
}

std::string & Location::getAliasLocation()
{
	return (_alias);
}

std::vector<std::string> & Location::getCgiExtensionLocation()
{
	return (_cgiExt);
}

std::vector<std::string> & Location::getCgiPathLocation()
{
	return (_cgiPath);
}

long int & Location::getMaxBodySizeLocation()
{
	return (_clientMaxBodySize);
}

std::map<std::string, std::string> & Location::getExtPathMap()
{
	return (_extPath);
}

void Location::setPath(std::string const & path)
{
	_path = path;
}

static bool isDirectoryAndAbsPath(std::string const & param)
{
	struct stat buffer;
	int			status;

	if (param[0] != '/')
		throw ServerErrorException("Error: invalid root for Location");
	status = stat(param.c_str(), &buffer);
	if (status == 0)
	{
		if (buffer.st_mode & S_IFDIR)
			return (true);
		return (false);
	}
	throw ServerErrorException("Error: could not check root stats");
}

void Location::setRootLocation(std::string const & root)
{
	if (isDirectoryAndAbsPath(root) == true)
	{
		if (access(root.c_str(), R_OK | X_OK) == 0)
			_root = root;
		else
			throw ServerErrorException("Error: invalid root for Location");
	}
	else
		throw ServerErrorException("Error: invalid root for Location");
}

void Location::setLocationMethods(std::vector<std::string> & methods)
{
	for (std::vector<int>::iterator it = _methods.begin(); it != _methods.end(); it++)
		*it = 0;
	for (std::vector<std::string>::const_iterator it = methods.begin(); it != methods.end(); it++)
	{
		if (*it == "GET")
			_methods[0] = GET;
		else if (*it == "POST")
			_methods[1] = POST;
		else if (*it == "DELETE")
			_methods[2] = DELETE;
		else if (*it == "PUT")
			_methods[3] = PUT;
		else if (*it == "HEAD")
			_methods[4] = HEAD;
		else
			throw ServerErrorException("Error: method not supported");
	}
}

void 	Location::setLocationAutoIndex(std::string const & autoIndex)
{
	if (autoIndex != "on" && autoIndex != "off")
		throw ParserErrorException("Error: invalid syntax for autoindex");
	if (autoIndex == "on")
		_autoIndex = true;
}

void Location::setIndexLocation(std::string const & index)
{
	_index = index;
}

void Location::setReturnLocation(std::string const & Return)
{
	_return = Return;
}

void Location::setAliasLocation(std::string const & alias)
{
	if (isDirectoryAndAbsPath(alias) == true)
	{
		if (access(alias.c_str(), R_OK | X_OK) == 0)
			_alias = alias;
		else
			throw ServerErrorException("Error: invalid alias for Location");
	}
	else
		throw ServerErrorException("Error: invalid alias for Location");
	
}

void Location::setCgiExtensionLocation(std::vector<std::string> const & cgiExt)
{
	_cgiExt = cgiExt;
}

void Location::setCgiPathLocation(std::vector<std::string> const & cgiPath)
{
	_cgiPath = cgiPath;
}

void Location::setMaxBodySizeLocation(std::string const & maxSize)
{
	long int number = 0;

	number = Utils::stringToInt(maxSize);
	if (number > MAX_CONTENT_LENGTH)
		throw ServerErrorException("Error: ");
	_clientMaxBodySize = number;
}

void Location::setMaxBodySizeLocation(long int & maxBodySize)
{
	_clientMaxBodySize = maxBodySize;
}

void Location::locationPrinter(void)
{
	std::cout << "Location-----" << "path: " << getLocationPath() << std::endl;
	std::cout << "Location-----" << "root: " << getLocationRoot() << std::endl;
	std::cout << "Location-----" << "autoIndex: " << getAutoIndexLocation() << std::endl;
	std::cout << "Location-----" << "index: " << getIndexLocation() << std::endl;
	std::cout << "Location-----" << "return: " << getReturnLocation() << std::endl;
	std::cout << "Location-----" << "alias: " << getAliasLocation() << std::endl;
	std::cout << "Location-----" << "body size: " << getMaxBodySizeLocation() << std::endl;
	std::cout << "Location-----" << "upload_store: " << getUploadStore() << std::endl;
	for (std::vector<int>::iterator it = getLocationMethods().begin(); it != getLocationMethods().end(); it++)
		std::cout << "Location------" << "method: " << *it << std::endl;
	for (std::vector<std::string>::iterator it = getCgiPathLocation().begin(); it != getCgiPathLocation().end(); it++)
		std::cout << "Location------" << "cgi Path: " << *it << std::endl;
	for (std::vector<std::string>::iterator it = getCgiExtensionLocation().begin(); it != getCgiExtensionLocation().end(); it++)
		std::cout << "Location------" << "cgi Extension: " << *it << std::endl;
	for (std::map<std::string, std::string>::iterator it = getExtPathMap().begin(); it != getExtPathMap().end(); it++)
		std::cout << "Location------" << "cgi Map ext: " << it->first << " cgi Map path: " << it->second << std::endl;
}

std::string &Location::getUploadStore()
{
	return (this->_uploadStore);
}

void Location::setUploadStore(const std::string &uploadStore)
{
	this->_uploadStore = uploadStore;
}


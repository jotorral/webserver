/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileChecker.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvicina <alvicina@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/19 12:26:46 by alvicina          #+#    #+#             */
/*   Updated: 2024/07/03 16:49:07 by alvicina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/FileChecker.hpp"

FileChecker::FileChecker(std::string const & path) : _pathToFile(path) //_size(0)
{
	
}

FileChecker::~FileChecker()
{
	
}

FileChecker::FileChecker(FileChecker const & copy) : _pathToFile(copy._pathToFile)
{
	
}

FileChecker& FileChecker::operator=(FileChecker const & other)
{
	if (this != &other)
	{
		this->_pathToFile = other._pathToFile;
	}
	return (*this);
}

static void checkStats(struct stat *buffer)
{
	if (!(buffer->st_mode & S_IFREG))
		throw ParserErrorException("Error: invalid type of file");
}

static void getStats(char const *path, struct stat *buffer)
{
	if(stat(path, buffer) == -1)
		throw ParserErrorException("Error: could not get stats of file");
	checkStats(buffer);
}

void FileChecker::getTypeOfFile(std::string const & path) const
{
	struct stat buffer;

	getStats(path.c_str(), &buffer);
}

std::string const & FileChecker::getPath(void)
{
	return (this->_pathToFile);
}

void FileChecker::getAccess(void)
{
	int accessOK = access(this->_pathToFile.c_str(), R_OK);
	if (accessOK != 0)
		throw ParserErrorException("Error: file has no permission to read");
}

std::string FileChecker::readFile(std::string const & path) const
{
	if (path.empty() || path.length() == 0)
		throw ParserErrorException("Error: path to config File is empty");
	std::ifstream configFileOpen(path.c_str());
	if (!configFileOpen)
		throw ParserErrorException("Error: Could not open config file");
	std::stringstream configFileContent;
	configFileContent << configFileOpen.rdbuf();
	return (configFileContent.str());
}

std::string FileChecker::checkAndRead()
{
	std::string content;
	
	getTypeOfFile(getPath());
	getAccess();
	content = readFile(getPath());
	if (content.empty())
		throw ParserErrorException("Config file is empty");
	return (content);
}
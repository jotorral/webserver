/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvicina <alvicina@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/19 11:41:57 by alvicina          #+#    #+#             */
/*   Updated: 2024/07/17 11:58:31 by alvicina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Utils.hpp"


ParserErrorException::ParserErrorException(std::string msg) throw()
{
	_msg = "FileParser " + msg;
}

const char* ParserErrorException::what(void) const throw()
{
	return (_msg.c_str());
}

ParserErrorException::~ParserErrorException(void) throw()
{
	
}

ServerErrorException::ServerErrorException(std::string msg) throw()
{
	_msg = "Server conf " + msg;
}

const char* ServerErrorException::what(void) const throw()
{
	return (_msg.c_str());
}

ServerErrorException::~ServerErrorException(void) throw()
{
	
}

long int	Utils::stringToInt(std::string const & string)
{
	long int	number = 0;
	size_t		i = 0;

	while (i < string.length())
	{
		if (!isdigit(string[i]))
			throw std::exception();
		i++;
	}
	std::stringstream ss(string);
	ss >> number;
	return (number);
}

int Utils::checkFile(std::string const & path, int mode)
{
	return (access(path.c_str(), mode));
}

int Utils::typeOfFile(std::string const & path)
{
	struct stat buff;
	int			type;

	type = stat(path.c_str(), &buff);
	if (type == 0)
	{
		if (buff.st_mode & S_IFREG)
			return (1);
		else if (buff.st_mode & S_IFDIR)
			return (2);
		else
			return (3);
	}
	return (-1);
}

std::string Utils::intToString(int number)
{
    std::stringstream string;
    string << number;
    return (string.str());
}

int Utils::fileExistsAndReadable(std::string const & path, std::string const & index)
{
	if ((typeOfFile(path + index) == 1 || typeOfFile(path + index) == 2) && checkFile(index, 4) == 0)
		return (0);
	if ((typeOfFile(path + index) == 1 || typeOfFile(path + index) == 2) && checkFile(path + index, 4) == 0)
		return (0);
	return (-1);
}

std::string Utils::codeStatus(int code)
{
    switch (code)
    {
        case 100:
            return "Continue";
        case 101:
            return "Switching Protocol";
        case 200:
            return "OK";
        case 201:
            return "Created";
        case 202:
            return "Accepted";
        case 203:
            return "Non-Authoritative Information";
        case 204:
            return "No Content";
        case 205:
            return "Reset Content";
        case 206:
            return "Partial Content";
        case 300:
            return "Multiple Choice";
        case 301:
            return "Moved Permanently";
        case 302:
            return "Moved Temporarily";
        case 303:
            return "See Other";
        case 304:
            return "Not Modified";
        case 307:
            return "Temporary Redirect";
        case 308:
            return "Permanent Redirect";
        case 400:
            return "Bad Request";
        case 401:
            return "Unauthorized";
        case 403:
            return "Forbidden";
        case 404:
            return "Not Found";
        case 405:
            return "Method Not Allowed";
        case 406:
            return "Not Acceptable";
        case 407:
            return "Proxy Authentication Required";
        case 408:
            return "Request Timeout";
        case 409:
            return "Conflict";
        case 410:
            return "Gone";
        case 411:
            return "Length Required";
        case 412:
            return "Precondition Failed";
        case 413:
            return "Payload Too Large";
        case 414:
            return "URI Too Long";
        case 415:
            return "Unsupported Media Type";
        case 416:
            return "Requested Range Not Satisfiable";
        case 417:
            return "Expectation Failed";
        case 418:
            return "I'm a teapot";
        case 421:
            return "Misdirected Request";
        case 425:
            return "Too Early";
        case 426:
            return "Upgrade Required";
        case 428:
            return "Precondition Required";
        case 429:
            return "Too Many Requests";
        case 431:
            return "Request Header Fields Too Large";
        case 451:
            return "Unavailable for Legal Reasons";
        case 500:
            return "Internal Server Error";
        case 501:
            return "Not Implemented";
        case 502:
            return "Bad Gateway";
        case 503:
            return "Service Unavailable";
        case 504:
            return "Gateway Timeout";
        case 505:
            return "HTTP Version Not Supported";
        case 506:
            return "Variant Also Negotiates";
        case 507:
            return "Insufficient Storage";
        case 510:
            return "Not Extended";
        case 511:
            return "Network Authentication Required";
        default:
            return "Undefined";
        }
}

void    Utils::ftFree(char **str)
{
    if (str)
    {
        size_t i = 0;
        while (str[i])
        {
            free(str[i]);
            i++;
        }
        free(str);
        str = NULL;
    }
}

std::string Utils::strToLower(std::string str)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		if (isalpha(str[i]))
			str[i] = tolower(str[i]);
	}
	return (str);
}

size_t Utils::hexToDecimal(const std::string& hexStr)
{
    size_t decimalValue = 0;
    
    for (size_t i = 0; i < hexStr.size(); ++i)
	{
        char c = hexStr[i];
        int value;

        if (c >= '0' && c <= '9')
            value = c - '0';
        else if (c >= 'A' && c <= 'F')
            value = c - 'A' + 10;
        else if (c >= 'a' && c <= 'f')
            value = c - 'a' + 10;
        else
			throw std::invalid_argument("Invalid argument for hexadecimal to decimal conversion");

        decimalValue = decimalValue * 16 + value; // Update the decimal value
    }

    return decimalValue;
}

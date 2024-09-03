/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvicina <alvicina@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/19 11:40:14 by alvicina          #+#    #+#             */
/*   Updated: 2024/07/10 10:23:39 by alvicina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include "webserv.hpp"

class Utils
{
	private:
	
	public:
		static long int	stringToInt(std::string const & string);
		static int		checkFile(std::string const & path, int mode);
		static int		typeOfFile(std::string const & path);
		static int		fileExistsAndReadable(std::string const & path, std::string const & index);
		static std::string codeStatus(int code);
		static std::string intToString(int number);
		static void		ftFree(char **str);
		static std::string strToLower(std::string str);
		static size_t hexToDecimal(const std::string& hexStr);
};

class ParserErrorException : public std::exception
{
	private:
		std::string _msg;
	public:
		ParserErrorException(std::string msg) throw();
		virtual const char* what() const throw();
		virtual ~ParserErrorException() throw();
};

class ServerErrorException : public std::exception
{
	private:
		std::string _msg;
	public:
		ServerErrorException(std::string msg) throw();
		virtual const char* what() const throw();
		virtual ~ServerErrorException() throw();
};

#endif

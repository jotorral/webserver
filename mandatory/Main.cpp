/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvicina <alvicina@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/19 11:31:25 by alvicina          #+#    #+#             */
/*   Updated: 2024/07/03 17:19:35 by alvicina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Utils.hpp"
#include "../includes/FileChecker.hpp"
#include "../includes/FileParser.hpp"
#include "../includes/Location.hpp"
#include "../includes/Server.hpp"
#include "../includes/ServerManager.hpp"
#include "../includes/Logger.hpp"
#include "../includes/webserv.hpp"

int startRoutine(std::string const & file)
{
	try
	{	
		FileChecker configFile(file);
		std::string content;
		content = configFile.checkAndRead();
		FileParser serverConf(content);
		serverConf.parse();
		ServerManager &serverManager = ServerManager::getInstance();
		serverManager.setServers(serverConf.getServers());
		if (serverManager.serve() == 0)
			return (EXIT_SUCCESS);
		return (EXIT_FAILURE);
	}
	catch (std::exception &e)
	{
		Logger::logError(e.what());
		return (EXIT_FAILURE);
	}
}

void handleSIGINT(int signum)
{
	if (signum == SIGINT || signum == SIGTERM)
		ServerManager::getInstance().stop();
}

int main(int argc,  char **argv)
{
	std::string file;
	
	if (argc == 1)
		file = "config/default.conf";
	else if (argc == 2)
		file = argv[1];
	else
	{
		Logger::logError("Wrong number or arguments");
		return (EXIT_FAILURE);
	}
	signal(SIGINT, handleSIGINT);
	signal(SIGTERM, handleSIGINT);
	return (startRoutine(file));
}

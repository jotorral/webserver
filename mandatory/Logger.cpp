#include "../includes/Logger.hpp"

const std::string Logger::IO_RESET = "\033[0m";
const std::string Logger::IO_RED = "\033[1;31m";
const std::string Logger::IO_GREEN = "\033[1;32m";
const std::string Logger::IO_YELLOW = "\033[1;33m";
const std::string Logger::IO_BLUE = "\033[1;34m";

Logger::Logger() {}

Logger::Logger(const Logger &logger)
{
	*this = logger;
}

Logger &Logger::operator=(const Logger &logger)
{
	(void) logger;
	return (*this);
}

Logger::~Logger() {}

void Logger::logError(const std::string &str, int level)
{
	std::cerr 
		<< Logger::IO_RED
		<< "[ERROR][lvl." << level << "] " << str
		<< Logger::IO_RESET << std::endl;
}

void Logger::logError(const std::ostringstream &oss, int level)
{
	Logger::logError(oss.str(), level);
}

void Logger::logInfo(const std::string &str, int level)
{
	if (level <= 0)
	{
		std::cout << "[INFO][lvl." << level << "] " << str << std::endl;
	}
	else
	{
		std::cout
			<< Logger::IO_YELLOW << "[INFO][lvl." << level << "] "
			<< str << Logger::IO_RESET << std::endl;
	}
}

void Logger::logInfo(const std::ostringstream &oss, int level)
{
	Logger::logInfo(oss.str(), level);
}

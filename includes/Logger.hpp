#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "webserv.hpp"

class Logger
{
	private:
		Logger();
		Logger(const Logger &logger);
		Logger &operator=(const Logger &logger);
		~Logger();

	public:
		static const std::string IO_RESET;
		static const std::string IO_RED;
		static const std::string IO_GREEN;
		static const std::string IO_YELLOW;
		static const std::string IO_BLUE;

		static void logError(const std::string &str, int level = 0);
		static void logError(const std::ostringstream &oss, int level = 0);
		static void logInfo(const std::string &str, int level = 0);
		static void logInfo(const std::ostringstream &oss, int level = 0);
};

#endif

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>

enum struct logLevel{
    Info,
    Warning,
    Error
};

void initLogger(std::string const filepath);

void Logger(logLevel level, std::string const &message);

void shutdownLogger();

#endif
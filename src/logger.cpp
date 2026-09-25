#include <logger.hpp>

#include <fstream>
#include <iostream>
#include <string>


std::ofstream logFile;

void initLogger(std::string const filepath)
{
    logFile.open(filepath, std::ios::out | std::ios::trunc);

    if (!logFile.is_open())
    {
        std::cerr << "Failed to initialize Logger";
    }
}

std::string logLevelString(logLevel level)
{
    switch (level)
    {
    case logLevel::Info:
    return "[Info] ";

    case logLevel::Warning:
    return "[Warning] ";

    case logLevel::Error:
    return "[Error] ";

    default:
    return "[Unknown]";
    }
}

void Logger(logLevel level, std::string const &message)
{
    if (!logFile.is_open())
    {
        return;
    }
    std::string levelstr = logLevelString(level);
    logFile << levelstr
    << message << "\n";
    logFile.flush();
}

void shutdownLogger()
{
    if (logFile.is_open())
    {
        logFile.close();
    }
}

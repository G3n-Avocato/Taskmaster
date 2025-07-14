#include "logger.hpp"
#include <fstream>
#include <ctime>
#include <iostream>
#include <string>

Logger::Logger() 
{
    _timestamp = std::time(nullptr);
    std::ofstream file(_filePath, std::ios::app);
    if (file.is_open()) {
        file << "=== Log File Created: " << makeTimeStamp() << " ===\n";
        file.close();
    }
}

Logger::~Logger()
{

}

void Logger::logFileCreation() 
{
    writeToFile("Log file initialized at " + makeTimeStamp());
}

void Logger::writeSupervisorStart() 
{
    writeToFile("Supervisor started at " + makeTimeStamp());
}

void Logger::writeProcessStartedInfo(std::string processName) 
{
    writeToFile("Process " + processName + " started at " + makeTimeStamp());
}

void Logger::writeProcessStoppedInfo(std::string processName) 
{
    writeToFile("Process " + processName + " stopped at " + makeTimeStamp());
}

void Logger::writeProcessRestartedInfo(std::string processName) 
{
    writeToFile("Process " + processName + " restarted at " + makeTimeStamp());
}

void Logger::writeConfReloadedInfo() 
{
    writeToFile("Configuration reloaded at " + makeTimeStamp());
}

void Logger::writeProcessDieUnexpectedly(std::string processName) 
{
    writeToFile("Process " + processName + " died unexpectedly at " + makeTimeStamp());
}

std::string Logger::makeTimeStamp() const 
{
    std::time_t now = std::time(nullptr);
    std::string timeStr = std::ctime(&now);
    if (!timeStr.empty() && timeStr.back() == '\n') {
        timeStr.pop_back(); 
    }
    return timeStr;
}

void Logger::writeToFile(const std::string& message) 
{
    std::ofstream file(_filePath, std::ios::app);
    if (file.is_open()) {
        file << message << std::endl;
        file.close();
    }
}

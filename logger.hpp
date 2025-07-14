#ifndef LOGGER_HPP
# define LOGGER_HPP

#include <ctime>
#include <string>
class Logger {

public:
    Logger();
    ~Logger();

    void logFileCreation();
    void writeSupervisorStart();
    void writeProcessStartedInfo(std::string processName);
    void writeProcessStoppedInfo(std::string processName);
    void writeProcessRestartedInfo(std::string processName);
    void writeConfReloadedInfo();
    void writeProcessDieUnexpectedly(std::string processName);

private:
    std::time_t _timestamp;
    std::string _filePath = "./log.txt";
    std::string makeTimeStamp() const;
    void writeToFile(const std::string& message);

};

# endif
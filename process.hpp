#ifndef PROCESS_HPP
# define PROCESS_HPP

#include <iostream>
#include <map>
#include <vector>

struct Config {
    std::string                         cmd;
    int                                 numProcs;
    std::string                         umask;
    std::string                         workingDir;
    std::string                         autoStart;
    std::string                         autoRestart;
    std::vector<int>                    exitCodes;
    int                                 startRetries;
    int                                 startTime;
    std::string                         stopSignal;
    int                                 stopTime;
    std::string                         stdout;
    std::string                         stderr;
    std::map<std::string, std::string>  env;
};

class Process {
    
    public:
    
        Process(void);
        Process(Process const &src);
        Process& operator=(Process const &rhs);
        ~Process(void);


    private:
        std::string _name;
        Config      _config;
        pid_t       _processus;

};

#endif
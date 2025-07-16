#ifndef PROCESS_HPP
# define PROCESS_HPP

#include <iostream>
#include <map>
#include <vector>
#include <signal.h>
#include <mutex>

enum class ProcessStatus
{
    START,
    DOWN,
    UNEXPECT_DOWN,
    SHUTING_DOWN,
    STARTING,
};

typedef struct s_config {
    std::string                         cmd;
    int                                 numProcs;
    std::string                         umask;
    std::string                         workingDir;
    std::string                         autoStart;
    std::string                         autoRestart;
    std::vector<int>                    exitCodes;
    int                                 startRetries;
    int                                 startTime;
    int                                 stopSignal;
    int                                 stopTime;
    std::string                         stdout;
    std::string                         stderr;
    std::map<std::string, std::string>  env;
} t_config;

class Process {
    
    public:
    
        Process(const t_config& config, std::mutex& mutex, ProcessStatus* status);
        Process(Process const &src);
        Process& operator=(Process const &rhs);
        ~Process(void);

        int stopProcess();
        void startProcess(int retry);
        bool isProcessUp();
        void killProcess();
        pid_t getPid() const ;

    private:
        std::vector<char*> buildArgv(const std::string& cmd);
        std::vector<char*> buildEnvp(const std::map<std::string, std::string>& envMap);
        void freeCStringVector(std::vector<char*>& vec);

        std::string     _name;
        t_config      _config;
        pid_t         _processus;
        std::mutex& _statusMutex;
        ProcessStatus *_processStatus;

};

#endif
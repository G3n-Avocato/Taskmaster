#ifndef PROCESS_HPP
# define PROCESS_HPP

#include <iostream>
#include <map>
#include <vector>
#include <unistd.h>
#include <cstring>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <thread>
#include <mutex>

enum ProcessStatus {
    START,
    DOWN,
    UNEXPECT_DOWN,
    SHUTING_DOWN,
    STARTING,
};

typedef struct s_config {
    std::string                         cmd;
    int                                 numProcs;
    mode_t                              umask;
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
    
        Process(const t_config& config, ProcessStatus stat);
        Process(Process const &src);
        Process& operator=(Process const &rhs);
        ~Process(void);

        bool isProcessUp();
        int stopProcess();
        bool startProcess();
        
        void killProcess();
        pid_t getPid() const ;

        const char* getStatus() const;

    private:

        std::vector<char*>  buildArgv(const std::string& cmd);
        std::vector<char*>  buildEnvp(const std::map<std::string, std::string>& envMap);
        void                freeCStringVector(std::vector<char*>& vec);

        bool                open_file_std();

        void                thread_monitoring_status();
        
        t_config                _config;

        int                     _fd_out;
        int                     _fd_err;

        pid_t                   _processus;
        
        std::thread             _t1;
        mutable std::mutex      _status_mutex;
        ProcessStatus           _status;

};

const char* enumtoString(ProcessStatus stat);

#endif
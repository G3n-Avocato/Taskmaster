#ifndef PROCESS_HPP
# define PROCESS_HPP

#include <filesystem>
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
    STOPPED,
    STARTING,
    RUNNING,
    BACKOFF,
    STOPPING,
    EXITED,
    FATAL,
    UNKNOWN,
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


typedef struct s_exec_args {
    std::vector<char*>                  argv;
    std::vector<char*>                  envp;
    int                                 fd_out;
    int                                 fd_err;
} t_execs;

class Process {
    
    public:
    
        Process(int i, std::string name, const t_config& config, ProcessStatus stat);
        Process(Process const &src);
        Process& operator=(Process const &rhs);
        ~Process(void);

        bool isProcessUp();
        int stopProcess();
        
        bool                startProcess();
        
        void killProcess();
        pid_t getPid() const ;

        const char*         getStatus() const;
        //static void         freeCStringVector(std::vector<char*>& vec);

    private:

        std::vector<char*>  buildArgv(const std::string& cmd);
        std::vector<char*>  buildEnvp(const std::map<std::string, std::string>& envMap);
        void                freeCStringVector(std::vector<char*>& vec);

        bool                open_file_std();
        void                thread_monitoring_status();
        void                child_exec_process();
        //void                init_workDir();
        
        int                     _id;
        std::string             _name;
        t_config                _config;
        t_execs                 _exec;

        pid_t                   _processus;
        std::thread             _t1;
        mutable std::mutex      _status_mutex;
        ProcessStatus           _status;

};

const char* enumtoString(ProcessStatus stat);
void        child_exec_process(t_execs& _exec);

#endif
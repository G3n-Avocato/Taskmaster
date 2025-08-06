#ifndef PROCESS_HPP
# define PROCESS_HPP

#include "task_define.hpp"

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

        bool    isProcessUp();
        int     stopProcess();
        
        bool                startProcess();
    
        void    killProcess();

        pid_t               getPid() const ;
        const char*         getPrintStatus() const ;
        ProcessStatus       getStatus() const ;
        bool                getautoStart() const ;
        int                 getautoRestart() const ;
        int                 getStartTime() const ;
        std::time_t         getStartRun() const ;
        int                 getStartRetries() const ; 
        int                 getCountRetries() const ;

        void                setCountRetries() ;
        void                setProcessStatus(ProcessStatus tmp) ;
        
    private:

        std::vector<char*>  buildArgv(const std::string& cmd);
        std::vector<char*>  buildEnvp(const std::map<std::string, std::string>& envMap);
        void                freeCStringVector(std::vector<char*>& vec);
        void                open_file_std();
        bool                buildStd_process(std::string& str, int& fd);

        void                thread_monitoring_status();
        void                child_exec_process();
        
        int                     _id;
        std::string             _name;
        t_config                _config;
        t_execs                 _exec;

        pid_t                   _processus;
        std::thread             _t1;
        mutable std::mutex      _status_mutex;
        ProcessStatus           _status;

        mutable std::mutex      _exitc_mutex;
        bool                    _exit_code;

        std::time_t             _start_run;
        //std::time_t             _end;
        //double                  _difftime;

        int                     _count_retries;
};

const char* enumtoString(ProcessStatus stat);
void        child_exec_process(t_execs& _exec);

#endif
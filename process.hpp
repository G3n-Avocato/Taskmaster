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
        pid_t   getPid() const ;

        const char*         getStatus() const;
        const bool          getautoStart() const;
        const int           getautoRestart() const;

    private:

        std::vector<char*>  buildArgv(const std::string& cmd);
        std::vector<char*>  buildEnvp(const std::map<std::string, std::string>& envMap);
        void                freeCStringVector(std::vector<char*>& vec);

        bool                open_file_std();
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

};

const char* enumtoString(ProcessStatus stat);
void        child_exec_process(t_execs& _exec);

#endif
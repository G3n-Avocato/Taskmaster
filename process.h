#ifndef PROCESS_H
# define PROCESS_H

#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>

typedef struct ctrl_cmds t_ctrl_cmds;

typedef enum ProcessStatus {
    STOPPED,
    STARTING,
    RUNNING,
    BACKOFF,
    STOPPING,
    EXITED,
    FATAL,
    UNKNOWN,
} ProcessStatus;

typedef struct s_exec_args {
    char**  argv;
    char**  envp;
    char*   stdout;
    char*   stderr;
    int     fd_out;
    int     fd_err;
} t_execs;

typedef struct s_processus {
    pid_t           processus;
    int             id;
    t_config*       config;
    t_execs*        exec;
    ProcessStatus   state;
    time_t          start_run;
    bool            exit_code;
    bool            run_reached;
    bool            boot_auto;
    int             count_retries;
    time_t          start_restart;
    int             count_restart;
    bool            ctrl_cmd_start;
    bool            ctrl_cmd_stop;
} t_procs;

typedef struct s_supervisorMap {
    char*           name;
    int             id;
    t_procs         proc;
} t_superMap;

// process.c
bool    startProcess(t_procs* proc, t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl);
void    parent_exec_proc(t_procs* proc);
bool    waitpid_monitoring_status(t_superMap** superMap);
void    child_exec_proc(t_procs* proc, t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl);
void    child_exit_error_execve(t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl);
void    child_close_fd(t_procs* proc);

bool    isProcessUp(pid_t processus);
bool    stopProcess(t_procs* proc);
bool    killProcess(t_procs* proc);

// process_init.c
bool    init_process_struct(t_procs* proc, t_config* conf, unsigned int j);
bool    buildStd_process(char* std, char** ptr, t_config* conf, int j);
bool    build_default_Std_process(char** ptr, t_config *conf, char *end, int j);
bool    splitWordsArgv(char ***ptr, char *input);
bool    buildEnvp(char ***ptr, t_config* conf);
bool    open_file(char* std, int* fd);
bool    open_file_std(t_execs* exec);
bool    init_config_process(t_procs* proc, t_config* conf);
bool    copy_exitcode_struct(t_procs* proc, t_config* conf);
bool    copy_env_struct(t_procs* proc, t_config* conf);

// process_init_utils.c
char*   ft_itoa(int n);

#endif
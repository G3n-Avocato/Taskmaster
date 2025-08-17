#ifndef PROCESS_H
# define PROCESS_H

# include "parser.h"

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
    int             count_retries;
} t_procs;


// process_init.c
bool    init_process_struct(t_procs* proc, t_config* conf, unsigned int j);
bool    buildStd_process(char* std, char** ptr, t_config* conf, int j);
bool    build_default_Std_process(char** ptr, t_config *conf, char *end, int j);

// process_init_utils.c
char	*ft_itoa(int n);

#endif
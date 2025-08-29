#ifndef LOGGER_H
# define LOGGER_H

extern FILE* g_fdlog;

typedef enum Log_Level {
    CRIT,   //critical error
    ERRO,   //ignorable error
    WARN,   //abnormal condition not an error
    INFO,   //normal info
    DEBG,   //debug
    TRAC,   //debug supervisor plugin
    BLAT    //debug supervisor
} LogLevel;

// logger.c //
bool    open_logger_file(const char* str);
bool    timestamp_logger();
bool    loglevel_logger(LogLevel nvl);
bool    message_logger(char *msg);
bool    logger(LogLevel nvl, char* msg);

bool    start_supervisor_logger();
bool    end_supervisor_logger();
bool    ctrl_supervisor_logger();

bool    start_process_logger(char *name, int id, pid_t pid);
bool    running_process_logger(char *name, int id, int t);
const char*    signal_int_tostring(int sign);
bool    stopped_process_signal_logger(char *name, int id, int sign);
bool    wait_stop_process_logger(char *name, int id);
bool    exit_expected_process_logger(char *name, int id, int exit);
bool    exit_not_expected_process_logger(char *name, int id, int exit);

bool    reload_config_logger();
bool    config_change_logger(char *name);
bool    updating_process_logger(char *name, int id);
bool    removing_process_logger(char *name, int id);
bool    added_process_logger(char *name, int id);

bool    fatal_state_logger(char *name, int id);
bool    fatal_logger(char *name, int id);
bool    error_sigkill_logger(char *name, int id, int pid, char* msg);
bool    error_kill_logger(char *name, int id, int pid, int sign, char* msg);
bool    error_fork_logger(char *cmd, char *msg);
bool    error_chdir_child(char *path, char* msg, char *name, int id);

#endif
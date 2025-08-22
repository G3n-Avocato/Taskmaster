#ifndef LOGGER_H
# define LOGGER_H

#include <time.h>
#include <sys/time.h>

extern FILE* g_fdlog;

typedef enum Log_Level {
    CRIT, //erreur critique attention immediate 
    ERRO, //potentiel erreur mais ignorable 
    WARN, //condition anormal mais qui n'est pas une erreur
    INFO, // info normal 
    DEBG, //debug process config and process output state change event notif
    TRAC, //debug supervisor plugin, info about http, rpc request et response
    BLAT //debug supervisor lui meme
} LogLevel;


bool    open_logger_file(const char* str);
bool    timestamp_logger();
bool    loglevel_logger(LogLevel nvl);
bool    message_logger(char *msg, FILE* fd);
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
bool    config_change_logger(char *name);
bool    updating_process_logger(char *name);
bool    fatal_state_logger(char *name, int id);
bool    error_sigkill_logger(char *name, int id, int pid, char* msg);
bool    error_kill_logger(char *name, int id, int pid, int sign, char* msg);
bool    error_fork_logger(char *cmd, char *msg);

#endif
#ifndef SUPERVISOR_H
# define SUPERVISOR_H

#include "parser.h"
#include "process.h"
#include "logger.h"
#include "supervisor_ctrl.h"
#include "signal_handler.h"

#include <sys/stat.h>
#include <sys/sendfile.h>

#define MAX_SIZE_STD_FILE (1024 * 1024)

extern int                      g_processCount;
extern volatile sig_atomic_t    sigchld_received;
extern volatile sig_atomic_t    sighup_reload;

// supervisor.c
bool    init_supervisor_processMap(t_process_para* para, t_superMap** superMap);
bool    autostart_boot(t_superMap** superMap, t_process_para* para, int i, t_ctrl_cmds* ctrl);
bool    main_loop(t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl);
void    command_ctrl(t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl);
bool    state_Running(t_superMap** superMap, int i);
bool    startRetries_loop(t_superMap** superMap, t_process_para* para, int i, t_ctrl_cmds* ctrl);
bool    autoRestart_loop(t_superMap** superMap, t_process_para* para, int i, t_ctrl_cmds* ctrl);
bool    timeCalcul_Restart(t_superMap** superMap, int i);
bool    logRotate_loop(t_superMap** superMap);

// logrotate.c
void    monitor_log_file(const char *path);

// enlever
const char* enumtoString(ProcessStatus stat);
void        printf_processus(t_superMap** super);

// free_process.c
void    free_supervisor(t_superMap** superMap);
void    free_exit_supervisor(t_superMap** superMap, t_process_para* para, pthread_t tid);
void    free_exit_para(t_process_para* para);

#endif
#ifndef SUPERVISOR_H
# define SUPERVISOR_H

#include "parser.h"
#include "process.h"
#include "logger.h"

extern int                      g_processCount;
extern volatile sig_atomic_t    sigchld_received;

bool    init_supervisor_processMap(t_process_para* para, t_superMap** superMap);
bool    autostart_boot(t_superMap** superMap, t_process_para* para, int i);
bool    main_loop(t_superMap** superMap, t_process_para* para);
//bool    waitpid_loop(t_superMap** superMap);
bool    state_Running(t_superMap** superMap, int i);
bool    startRetries_loop(t_superMap** superMap, t_process_para *para, int i);
bool    autoRestart_loop(t_superMap** superMap, t_process_para *para, int i);
bool    timeCalcul_Restart(t_superMap** superMap, int i);


// enlever
const char* enumtoString(ProcessStatus stat);
void        printf_processus(t_superMap** super);
bool        test_stopProcess(t_superMap** superMap, int i);


// free_process.c
void    free_supervisor(t_superMap** superMap);

#endif
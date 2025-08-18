#ifndef SUPERVISOR_H
# define SUPERVISOR_H

#include "process.h"


extern int g_processCount;

bool    init_supervisor_processMap(t_process_para* para, t_superMap** superMap);
bool    autostart_boot(t_superMap** superMap, t_process_para* para);
bool    waitpid_loop(t_superMap** superMap);


const char* enumtoString(ProcessStatus stat);
void        printf_processus(t_superMap** super);

void    free_supervisor(t_superMap** superMap);

#endif
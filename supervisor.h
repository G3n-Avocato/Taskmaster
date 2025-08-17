#ifndef SUPERVISOR_H
# define SUPERVISOR_H

#include "process.h"


extern int g_processCount;

typedef struct s_supervisorMap {
    char*           name;
    int             id;
    t_procs         proc;
} t_superMap;

bool    init_supervisor_processMap(t_process_para* para, t_superMap** superMap);

void    printf_processus(t_superMap** super);

void    free_supervisor(t_superMap** superMap);

#endif
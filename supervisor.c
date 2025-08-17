# include "supervisor.h"

bool    init_supervisor_processMap(t_process_para* para, t_superMap** superMap) {

    t_procs     current = {0};
    t_superMap* tmp = NULL;

    for (unsigned int i = 0; i < para->count; i++) {
        t_config*   conf = &para->config[i];
        for (unsigned int j = 0; j < conf->numProcs; j++) {
            g_processCount++;
            
            tmp = realloc(*superMap, sizeof(t_superMap) * g_processCount);
            if (!tmp)
                return false ;
            *superMap = tmp;

            current = (t_procs){0};
            (*superMap)[g_processCount - 1].proc = current;
            (*superMap)[g_processCount - 1].id = j + 1;
            (*superMap)[g_processCount - 1].name = conf->name;
            if (!init_process_struct(&(*superMap)[g_processCount - 1].proc, conf, j))
                return false ;
        } 
    }

    return true ;
}

void    printf_processus(t_superMap** super) {

    for (unsigned int i = 0; i < g_processCount; i++) {
        printf("init name procs -> %s + %d\n", (*super)[i].name, (*super)[i].id);
        printf("proc->exec->stdout : %s\n", (*super)[i].proc.exec->stdout);
        printf("proc->exec->stderr : %s\n", (*super)[i].proc.exec->stderr);
    }
}
# include "supervisor.h"

void    free_supervisor(t_superMap** superMap) {
    if ((*superMap)) {
        for (unsigned int i = 0; i < g_processCount; i++) {
            if ((*superMap)[i].proc.exec) {
                free((*superMap)[i].proc.exec->stdout);
                free((*superMap)[i].proc.exec->stderr);
                free((*superMap)[i].proc.exec);
            }
        }
        free((*superMap));
    }
}
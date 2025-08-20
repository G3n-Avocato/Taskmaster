# include "supervisor.h"

void    free_supervisor(t_superMap** superMap) {
    
    if ((*superMap)) {

        for (int i = 0; i < g_processCount; i++) {

            if ((*superMap)[i].proc.exec) {
                free((*superMap)[i].proc.exec->stdout);
                free((*superMap)[i].proc.exec->stderr);
                
                if ((*superMap)[i].proc.exec->argv) {
                
                    for (unsigned int j = 0; (*superMap)[i].proc.exec->argv[j] != NULL; j++)
                        free((*superMap)[i].proc.exec->argv[j]);
                    free((*superMap)[i].proc.exec->argv);
                }

                if ((*superMap)[i].proc.exec->envp) {
                
                    for (unsigned int j = 0; (*superMap)[i].proc.exec->envp[j] != NULL; j++)
                        free((*superMap)[i].proc.exec->envp[j]);
                    free((*superMap)[i].proc.exec->envp);
                }

                free((*superMap)[i].proc.exec);
            }
        }
        free((*superMap));
    }
}
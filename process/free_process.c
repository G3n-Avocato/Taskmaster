# include "../include/supervisor.h"

void    free_supervisor(t_superMap** superMap) {
    
    if ((*superMap)) {

        for (int i = 0; i < g_processCount; i++) {

            free_config((*superMap)[i].proc.config);
            free((*superMap)[i].proc.config);
            
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

void    free_ctrl(pthread_t tid) {
    pthread_cancel(tid);
    pthread_join(tid, NULL);

    for (int i = 0; i < histo_size; i++) {
        free(history[i]);
    }
    pthread_mutex_destroy(&lock_read);
}

void    free_exit_supervisor(t_superMap** superMap, t_process_para* para, pthread_t tid) {
    end_supervisor_logger();

    free_supervisor(superMap);
    free_process_para(para);
    fclose(g_fdlog);
    free_ctrl(tid);
}

void    free_exit_para(t_process_para* para) {
    end_supervisor_logger();

    free_process_para(para);
    fclose(g_fdlog);
}
# include "supervisor.h"

bool    init_proc_superMap_reload(t_superMap** superMap, char *name, t_config* conf) {
    int len = 0;
    int nump = 0;
    t_procs     current = {0};

    for (int i = 0; i < g_processCount; i++) {
        len = strlen(name);
        if (!strncmp((*superMap)[i].name, name, len)) {
            if (nump < conf->numProcs) {
                free_supervisor_case(superMap, i);
                current = (t_procs){0};
                (*superMap)[i].proc = current;
                (*superMap)[i].id = nump + 1;
                if (!init_process_struct(&(*superMap)[i].proc, conf, nump)) {
                    fprintf(stderr, "Error ctrl command : init process\n");
                    return false ;
                }
                nump++;
            }
            else if (nump == conf->numProcs && (*superMap)[i].id >= nump) {
                free_supervisor_case(superMap, i);
                int y;
                for (y = i; y < g_processCount; y++) {
                    (*superMap)[y] = (*superMap)[y + 1];
                }
                current = (t_procs){0};
                (*superMap)[y].proc = current;
                (*superMap)[y].name = NULL;
                g_processCount--;
                i--;
            }

        }
    }

    t_superMap* tmp = NULL;
    while (nump < conf->numProcs) {
        g_processCount++;

        tmp = realloc(*superMap, sizeof(t_superMap) * g_processCount);
        if (!tmp) {
            fprintf(stderr, "Error ctrl command : allocation error (realloc)\n");
            return false ;
        }
        *superMap = tmp;
        current = (t_procs){0};
        (*superMap)[g_processCount - 1].proc = current;
        (*superMap)[g_processCount - 1].id = nump + 1;
        (*superMap)[g_processCount - 1].name = conf->name;
        if (!init_process_struct(&(*superMap)[g_processCount - 1].proc, conf, nump)) {
            fprintf(stderr, "Error ctrl command : init process\n");
            return false ;
        }
        nump++;
    }

    return true ;
}

void    free_supervisor_case(t_superMap** superMap, int i) {

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
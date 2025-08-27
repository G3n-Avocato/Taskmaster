# include "supervisor.h"

bool    init_proc_superMap_reload(t_superMap** superMap, char *name, t_config* conf) {
    int nump = 0;
    t_procs     current = {0};

    for (int i = 0; i < g_processCount; i++) {

        if (!strcmp((*superMap)[i].name, name)) {
            if (nump < conf->numProcs) {
                free_superMap_case(superMap, i);
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
                free_superMap_case(superMap, i);
                int y;
                for (y = i; y < g_processCount - 1; y++) {
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

void    free_superMap_case(t_superMap** superMap, int i) {
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

bool    init_new_para_struct_reload(t_process_para* old, t_process_para* new, int j) {

    t_config current = {0};

    old->count++;
    old->config = realloc(old->config, sizeof(t_config) * old->count);
    if (!old->config) {
        fprintf(stderr, "Error ctrl command : allocation error (realloc)\n");
        return false ;
    }

    current = (t_config){0};
    old->config[old->count - 1] = current;
    old->config[old->count - 1].name = strdup(new->config[j].name);
    if (!old->config[old->count - 1].name) {
        fprintf(stderr, "Error ctrl command : allocation error (strdup)\n");
        return false ;
    }
    
    old->config[old->count - 1].cmd = strdup(new->config[j].cmd);
    if (!old->config[old->count - 1].cmd) {
        fprintf(stderr, "Error ctrl command : allocation error (strdup)\n");
        return false ;
    }
    old->config[old->count - 1].has_cmd = new->config[j].has_cmd;
    old->config[old->count - 1].numProcs = new->config[j].numProcs;
    old->config[old->count - 1].umask = new->config[j].umask;
    
    old->config[old->count - 1].workingDir = strdup(new->config[j].workingDir);
    if (!old->config[old->count - 1].workingDir) {
        fprintf(stderr, "Error ctrl command : allocation error (strdup)\n");
        return false ;
    }
    
    old->config[old->count - 1].autoStart = new->config[j].autoStart;
    old->config[old->count - 1].autoRestart = new->config[j].autoRestart;
    old->config[old->count - 1].startRetries = new->config[j].startRetries;
    old->config[old->count - 1].startTime = new->config[j].startTime;
    old->config[old->count - 1].stopSignal = new->config[j].stopSignal;
    old->config[old->count - 1].stopTime = new->config[j].stopTime;
    
    if (!swap_exitCodes_struct_in_para(old, new, old->count - 1, j))
        return false ;

    if (new->config[j].stdout) { 
        old->config[old->count - 1].stdout = strdup(new->config[j].stdout);
        if (!old->config[old->count - 1].stdout) {
            fprintf(stderr, "Error ctrl command : allocation error (strdup)\n");
            return false ;
        }
    }
    else
        old->config[old->count - 1].stdout = NULL;

    if (new->config[j].stderr) { 
        old->config[old->count - 1].stderr = strdup(new->config[j].stderr);
        if (!old->config[old->count - 1].stderr) {
            fprintf(stderr, "Error ctrl command : allocation error (strdup)\n");
            return false ;
        }
    }
    else
        old->config[old->count - 1].stderr = NULL;

    old->config[old->count - 1].env = NULL;
    old->config[old->count - 1].count_env = 0;
    if (!swap_env_struct_in_para(old, new, old->count - 1, j))
        return false ;


    return true ;
}

bool    delete_old_para_struct_reload(t_process_para* old, int i) {
    if (!old || !old->config || (unsigned int)i >= old->count)
        return false ;

    free_config(&old->config[i]);

    for (unsigned y = i; y < old->count - 1; y++) {
        old->config[y] = old->config[y + 1];
    }

    old->count--;

    return true ;
}

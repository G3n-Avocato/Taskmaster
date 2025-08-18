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
            if (!init_process_struct(&(*superMap)[g_processCount - 1].proc, conf, j)) {
                fprintf(stderr, "Error init process\n");
                return false ;
            }
        } 
    }

    return true ;
}

bool    autostart_boot(t_superMap** superMap, t_process_para* para) {

    for (unsigned int i = 0; i < g_processCount; i++) {
        
        if ((*superMap)[i].proc.config->autoStart) {
            if (!startProcess(&(*superMap)[i].proc, superMap, para))
                return false ;
        }
    }
    return true ;
}

bool    waitpid_loop(t_superMap** superMap) {
    
    for (unsigned int i = 0; i < g_processCount; i++) {

        if (!waitpid_monitoring_status(&(*superMap)[i].proc))
            return false ;
    }
    return true ;
}

void    printf_processus(t_superMap** super) {

    for (unsigned int i = 0; i < g_processCount; i++) {
        printf("init name procs -> %s + %d\n", (*super)[i].name, (*super)[i].id);
        printf("proc->exec->stdout : %s\n", (*super)[i].proc.exec->stdout);
        printf("proc->exec->stderr : %s\n", (*super)[i].proc.exec->stderr);
        printf("\nArgv :\n");
        for (unsigned int j = 0; (*super)[i].proc.exec->argv[j] != NULL; j++)
            printf("%s ", (*super)[i].proc.exec->argv[j]);
        printf("\n\nEnvp :\n");
        if ((*super)[i].proc.exec->envp) { 
            for (unsigned int j = 0; (*super)[i].proc.exec->envp[j] != NULL; j++)
                printf("%s\n", (*super)[i].proc.exec->envp[j]);
        }
        printf("\n");
        printf("autostart in config in procs ----------> %d\n", (*super)[i].proc.config->autoStart);
    }
}
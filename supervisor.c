# include "supervisor.h"

bool    init_supervisor_processMap(t_process_para* para, t_superMap** superMap) {

    t_procs     current = {0};
    t_superMap* tmp = NULL;

    for (unsigned int i = 0; i < para->count; i++) {
        t_config*   conf = &para->config[i];
        for (int j = 0; j < conf->numProcs; j++) {
            g_processCount++;
            
            tmp = realloc(*superMap, sizeof(t_superMap) * g_processCount);
            if (!tmp) {
                logger(CRIT, "Error init : allocation error (realloc)");
                return false ;
            }
            *superMap = tmp;

            current = (t_procs){0};
            (*superMap)[g_processCount - 1].proc = current;
            (*superMap)[g_processCount - 1].id = j + 1;
            (*superMap)[g_processCount - 1].name = conf->name;
            if (!init_process_struct(&(*superMap)[g_processCount - 1].proc, conf, j)) {
                logger(CRIT, "Error init : process map");
                return false ;
            }
        } 
    }

    return true ;
}

bool    autostart_boot(t_superMap** superMap, t_process_para* para) {

    for (int i = 0; i < g_processCount; i++) {
        
        if ((*superMap)[i].proc.config->autoStart) {
            if (!startProcess(&(*superMap)[i].proc, superMap, para))
                return false ;
        }
    }
    return true ;
}

bool    main_loop(t_superMap** superMap, t_process_para* para) {
    
    struct timespec wait_for = {0};
    wait_for.tv_sec = 0;
    wait_for.tv_nsec = 100 * 1000000L;

    while (1) {

        if (!waitpid_loop(superMap)) {
            return false ;        
        }
        if (!state_Running(superMap)) {
            return false ;
        }
        if (!startRetries_loop(superMap, para)) {
            return false ;
        }
        if (!autoRestart_loop(superMap, para)) {
            return false ;
        }

        if (!test_stopProcess(superMap))
            return false ;

        nanosleep(&wait_for, NULL);
    }
    return true ;
}

bool    waitpid_loop(t_superMap** superMap) {
    
    for (int i = 0; i < g_processCount; i++) {
        if (!waitpid_monitoring_status(&(*superMap)[i].proc))
            return false ; // a voir pour ca
        printf("%d - %s - %d -----------> %s\n\n", (*superMap)[i].proc.processus, (*superMap)[i].name, (*superMap)[i].id, enumtoString((*superMap)[i].proc.state));
    }
    
    return true ;
}

bool    state_Running(t_superMap** superMap) {
    
    for (int i = 0; i < g_processCount; i++) {
        ProcessStatus   p_state = (*superMap)[i].proc.state;

        if (p_state == STARTING && isProcessUp((*superMap)[i].proc.processus)) {
            time_t  end = time(NULL);
            double  diff_time = difftime(end, (*superMap)[i].proc.start_run);
            printf("Running state %s-%d difftime %f\n", (*superMap)[i].name, (*superMap)[i].id, diff_time);

            if (diff_time >= (*superMap)[i].proc.config->startTime) {
                printf("%f\n", diff_time);
                (*superMap)[i].proc.state = RUNNING;
                (*superMap)[i].proc.run_reached = true ;
            }
        }
    }
    return true ;
}

bool    test_stopProcess(t_superMap** superMap) {
    for (int i = 0; i < g_processCount; i++) {
        ProcessStatus   p_state = (*superMap)[i].proc.state;
        if (p_state == RUNNING) {
            time_t  end = time(NULL);
            double  diff_time = difftime(end, (*superMap)[i].proc.start_run);
            printf("stopprocess difftime %f\n", diff_time);
            if (diff_time > 5) {
                if (!stopProcess(&(*superMap)[i].proc))
                    return false ;
            }
        }
    }
    return true ;
}

bool    startRetries_loop(t_superMap** superMap, t_process_para *para) {

    for (int i = 0; i < g_processCount; i++) {
        if ((*superMap)[i].proc.config->startRetries > 0) {
            ProcessStatus   p_state = (*superMap)[i].proc.state;
            
            if (p_state != RUNNING && !(*superMap)[i].proc.run_reached) {
                
                if ((*superMap)[i].proc.config->startRetries > (*superMap)[i].proc.count_retries) {
                    
                    if (p_state == EXITED || p_state == BACKOFF) {
                        
                        (*superMap)[i].proc.state = BACKOFF;
                        (*superMap)[i].proc.count_retries++;
                        if (!startProcess(&(*superMap)[i].proc, superMap, para))
                            return false ;
                    }
                }
                else
                    (*superMap)[i].proc.state = FATAL;
            }
        }
    }

    return true ;
}

bool    autoRestart_loop(t_superMap** superMap, t_process_para *para) {

    for (int i = 0; i < g_processCount; i++) {
        ProcessStatus   p_state = (*superMap)[i].proc.state;
        int             st_restart = (*superMap)[i].proc.config->autoRestart;

        if ((*superMap)[i].proc.run_reached && p_state == EXITED) {
            if (st_restart == TRUE || (st_restart == UNEXPECTED && !(*superMap)[i].proc.exit_code)) {

                (*superMap)[i].proc.count_restart++;
                if (!startProcess(&(*superMap)[i].proc, superMap, para))
                            return false ;
            }
        }
    }

    return true ;
}

void    printf_processus(t_superMap** super) {

    for (int i = 0; i < g_processCount; i++) {
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
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
                fprintf(stderr, "Error init : allocation error (realloc)\n");
                return false ;
            }
            *superMap = tmp;

            current = (t_procs){0};
            (*superMap)[g_processCount - 1].proc = current;
            (*superMap)[g_processCount - 1].id = j + 1;
            (*superMap)[g_processCount - 1].name = conf->name;
            if (!init_process_struct(&(*superMap)[g_processCount - 1].proc, conf, j)) {
                fprintf(stderr,"Error init : process in process Map\n");
                return false ;
            }
        } 
    }

    return true ;
}

bool    autostart_boot(t_superMap** superMap, t_process_para* para, int i) {

    if (!(*superMap)[i].proc.boot_auto && (*superMap)[i].proc.config->autoStart) {
        if (!startProcess(&(*superMap)[i].proc, superMap, para))
            return false ;
        (*superMap)[i].proc.boot_auto = true;
    }
    return true ;
}

bool    main_loop(t_superMap** superMap, t_process_para* para) {
    
    struct timespec wait_for = {0};
    wait_for.tv_sec = 0;
    wait_for.tv_nsec = 100 * 1000000L;

    while (1) {

        if (sigchld_received) {
            waitpid_monitoring_status(superMap);
            sigchld_received = 0;
        }
        
        for (int i = 0; i < g_processCount; i++) {
            
            if (!autostart_boot(superMap, para, i))
                return false ;
            
            if (!state_Running(superMap, i)) {
                return false ;
            }

            if (!startRetries_loop(superMap, para, i)) {
                return false ;
            }
            if (!autoRestart_loop(superMap, para, i)) {
                return false ;
            }
            printf("%d - %s - %d -----------> %s\n\n", (*superMap)[i].proc.processus, (*superMap)[i].name, (*superMap)[i].id, enumtoString((*superMap)[i].proc.state));
    
            //if (!test_stopProcess(superMap, i))
            //    return false ;

        }
        nanosleep(&wait_for, NULL);
    }
    return true ;
}

bool    state_Running(t_superMap** superMap, int i) {

    ProcessStatus   p_state = (*superMap)[i].proc.state;

    if (p_state == STARTING && isProcessUp((*superMap)[i].proc.processus)) {
        time_t  end = time(NULL);
        double  diff_time = difftime(end, (*superMap)[i].proc.start_run);
        printf("Running state %s-%d difftime %f\n", (*superMap)[i].name, (*superMap)[i].id, diff_time); ///
        
        if (diff_time >= (*superMap)[i].proc.config->startTime) {
            printf("%f\n", diff_time); ///
            
            (*superMap)[i].proc.state = RUNNING;
            (*superMap)[i].proc.run_reached = true ;
            running_process_logger((*superMap)[i].name, (*superMap)[i].id - 1, (*superMap)[i].proc.config->startTime);
        }
    }

    return true ;
}

/// testeur a enlever
bool    test_stopProcess(t_superMap** superMap, int i) {

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

    return true ;
}

bool    startRetries_loop(t_superMap** superMap, t_process_para *para, int i) {

    ProcessStatus   p_state = (*superMap)[i].proc.state;
        
    if ((p_state == EXITED || p_state == BACKOFF) && !(*superMap)[i].proc.run_reached) {
    
        if ((*superMap)[i].proc.config->startRetries > (*superMap)[i].proc.count_retries) {
        
            (*superMap)[i].proc.state = BACKOFF;
            (*superMap)[i].proc.count_retries++;
            if (!startProcess(&(*superMap)[i].proc, superMap, para))
                return false ;
        }
        else if ((*superMap)[i].proc.config->startRetries == (*superMap)[i].proc.count_retries && p_state != FATAL) {
            fatal_logger((*superMap)[i].name, (*superMap)[i].id - 1);
            (*superMap)[i].proc.state = FATAL;
        }
        
    }

    return true ;
}

bool    autoRestart_loop(t_superMap** superMap, t_process_para *para, int i) {

    ProcessStatus   p_state = (*superMap)[i].proc.state;
    int             st_restart = (*superMap)[i].proc.config->autoRestart;

    if ((*superMap)[i].proc.run_reached && p_state == EXITED) {
        
        if (st_restart == TRUE || (st_restart == UNEXPECTED && !(*superMap)[i].proc.exit_code)) {

            if (!timeCalcul_Restart(superMap, i))
                return true ;

            (*superMap)[i].proc.count_restart++;
            if (!startProcess(&(*superMap)[i].proc, superMap, para))
                        return false ;
        }
    }

    return true ;
}

bool    timeCalcul_Restart(t_superMap** superMap, int i) {

    if ((*superMap)[i].proc.count_restart == 0)
        (*superMap)[i].proc.start_restart = time(NULL);
    if ((*superMap)[i].proc.count_restart == 5) {
        time_t  end_auto = time(NULL);
        double  diff_time = difftime(end_auto, (*superMap)[i].proc.start_restart);

        if (diff_time < 5) {
            (*superMap)[i].proc.state = FATAL;
            fatal_state_logger((*superMap)[i].name, (*superMap)[i].id - 1);
            return false ;
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
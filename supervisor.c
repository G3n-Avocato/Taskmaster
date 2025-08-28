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
                pthread_mutex_lock(&lock_read);
                fprintf(stderr, "Error init : allocation error (realloc)\n");
                pthread_mutex_unlock(&lock_read);
                return false ;
            }
            *superMap = tmp;

            current = (t_procs){0};
            (*superMap)[g_processCount - 1].proc = current;
            (*superMap)[g_processCount - 1].id = j + 1;
            (*superMap)[g_processCount - 1].name = conf->name;
            if (!init_process_struct(&(*superMap)[g_processCount - 1].proc, conf, j)) {
                pthread_mutex_lock(&lock_read);
                fprintf(stderr,"Error init : process in process Map\n");
                pthread_mutex_unlock(&lock_read);
                return false ;
            }
        } 
    }

    return true ;
}

bool    main_loop(t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl) {
    
    struct timespec wait_for = {0};
    wait_for.tv_sec = 0;
    wait_for.tv_nsec = 100 * 1000000L;

    struct timespec last_check = {0};
    struct timespec now;

    ctrl->split_cmd = NULL;
    ctrl->group = NULL;
    ctrl->name = NULL;
    ctrl->tab_len = 0;

    while (running) {

        if (sigchld_received) {
            waitpid_monitoring_status(superMap);
            sigchld_received = 0;
        }
        if (sighup_reload) {
            reload_cmd(superMap, para, ctrl);
            sighup_reload = 0;
        }

        pthread_mutex_lock(&lock_read);
        if (cmd_ready) {
            pthread_mutex_unlock(&lock_read);
            command_ctrl(superMap, para, ctrl);
        }
        else 
            pthread_mutex_unlock(&lock_read);

        for (int i = 0; i < g_processCount; i++) {
            
            if (!autostart_boot(superMap, para, i, ctrl))
                return false ;
            
            if (!state_Running(superMap, i)) {
                return false ;
            }
            if ((*superMap)[i].proc.ctrl_cmd_stop == false) {
                if (!startRetries_loop(superMap, para, i, ctrl))
                    return false ;
            }
            
            if ((*superMap)[i].proc.ctrl_cmd_stop == false) {
                if (!autoRestart_loop(superMap, para, i, ctrl))
                    return false ;
            }
        }
        
        clock_gettime(CLOCK_MONOTONIC, &now);
        if (now.tv_sec - last_check.tv_sec >= 3) {
            logRotate_loop(superMap);
            last_check = now;
        }

        nanosleep(&wait_for, NULL);
    }
    return true ;
}

void    command_ctrl(t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl) {
    
    char tmp[MAX_CMD];
    pthread_mutex_lock(&lock_read);
    strncpy(tmp, cmd_buffer, MAX_CMD);
    pthread_mutex_unlock(&lock_read);
    tmp[MAX_CMD - 1] = '\0';
    
    cmd_ready = 0;
    if (running) {
        pthread_mutex_lock(&lock_read);
        printf("\n");
        fflush(stdout);
        pthread_mutex_unlock(&lock_read);
    }
    process_command(tmp, superMap, para, ctrl);
}

bool    autostart_boot(t_superMap** superMap, t_process_para* para, int i, t_ctrl_cmds* ctrl) {

    if (!(*superMap)[i].proc.boot_auto && (*superMap)[i].proc.config->autoStart) {
        if (!startProcess(&(*superMap)[i].proc, superMap, para, ctrl))
            return false ;
        (*superMap)[i].proc.boot_auto = true;
        //fprintf(stderr, "trest1\n");
        //fflush(stderr);
    }
    return true ;
}

bool    state_Running(t_superMap** superMap, int i) {

    ProcessStatus   p_state = (*superMap)[i].proc.state;

    if (p_state == STARTING && isProcessUp((*superMap)[i].proc.processus)) {
        time_t  end = time(NULL);
        double  diff_time = difftime(end, (*superMap)[i].proc.start_run);
        
        if (diff_time >= (*superMap)[i].proc.config->startTime) {
            
            (*superMap)[i].proc.state = RUNNING;
            (*superMap)[i].proc.run_reached = true;
            running_process_logger((*superMap)[i].name, (*superMap)[i].id - 1, (*superMap)[i].proc.config->startTime);
            if ((*superMap)[i].proc.ctrl_cmd_start) {
                pthread_mutex_lock(&lock_read);
                fprintf(stdout, "%s_%d: started\n", (*superMap)[i].name, ((*superMap)[i].id - 1));
                fflush(stdout);
                pthread_mutex_unlock(&lock_read);
                (*superMap)[i].proc.ctrl_cmd_start = false;
            }
        }
    }

    return true ;
}

bool    startRetries_loop(t_superMap** superMap, t_process_para* para, int i, t_ctrl_cmds* ctrl) {

    ProcessStatus   p_state = (*superMap)[i].proc.state;
        
    if ((p_state == EXITED || p_state == BACKOFF) && !(*superMap)[i].proc.run_reached) {
    
        if ((*superMap)[i].proc.config->startRetries > (*superMap)[i].proc.count_retries) {
        
            (*superMap)[i].proc.state = BACKOFF;
            (*superMap)[i].proc.count_retries++;
            if (!startProcess(&(*superMap)[i].proc, superMap, para, ctrl))
                return false ;
            //fprintf(stderr, "trest2\n");
            //fflush(stderr);
        }
        else if ((*superMap)[i].proc.config->startRetries == (*superMap)[i].proc.count_retries && p_state != FATAL) {
            fatal_logger((*superMap)[i].name, (*superMap)[i].id - 1);
            (*superMap)[i].proc.state = FATAL;
            if ((*superMap)[i].proc.ctrl_cmd_start) {
                pthread_mutex_lock(&lock_read);
                fprintf(stderr, "%s_%d: ERROR (abnormal termination)\n", (*superMap)[i].name, (*superMap)[i].id - 1);
                fflush(stderr);
                pthread_mutex_unlock(&lock_read);
            }
        }
        
    }

    return true ;
}

bool    autoRestart_loop(t_superMap** superMap, t_process_para* para, int i, t_ctrl_cmds* ctrl) {

    ProcessStatus   p_state = (*superMap)[i].proc.state;
    int             st_restart = (*superMap)[i].proc.config->autoRestart;

    if ((*superMap)[i].proc.run_reached && p_state == EXITED) {
        
        if (st_restart == TRUE || (st_restart == UNEXPECTED && !(*superMap)[i].proc.exit_code)) {

            if (!timeCalcul_Restart(superMap, i))
                return true ;

            (*superMap)[i].proc.count_restart++;
            if (!startProcess(&(*superMap)[i].proc, superMap, para, ctrl))
                return false ;
            //fprintf(stderr, "trest3\n");
            //fflush(stderr);
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

bool    logRotate_loop(t_superMap** superMap) {

    for (int i = 0; i < g_processCount; i++) {
        monitor_log_file((*superMap)[i].proc.exec->stdout);
        monitor_log_file((*superMap)[i].proc.exec->stderr);
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

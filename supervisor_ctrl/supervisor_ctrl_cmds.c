# include "../include/supervisor.h"

/* START */
bool    find_name_proc_start(t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl) {
    int id_p;
    if (!int_parser(ctrl->name, &id_p)) {
        pthread_mutex_lock(&lock_read);
        fprintf(stderr, "%s_%s: ERROR (no such proces)\n", ctrl->group, ctrl->name);
        fflush(stderr);
        pthread_mutex_unlock(&lock_read);
        return false ;
    }

    for (int i = 0; i < g_processCount; i++) {
        if (id_p == ((*superMap)[i].id - 1) && !strcmp(ctrl->group, (*superMap)[i].name)) {
            if (!start_cmd(&(*superMap)[i].proc, superMap, para, ctrl))
                return false ;
            return true ;
        }
    }
    pthread_mutex_lock(&lock_read);
    fprintf(stderr, "%s_%s: ERROR (no such proces)\n", ctrl->group, ctrl->name);
    fflush(stderr);
    pthread_mutex_unlock(&lock_read);
    return false ;
}

bool    find_group_proc_start(t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl) {
    
    bool    start = false;

    for (int i = 0; i < g_processCount; i++) {
        if (!strcmp((*superMap)[i].name, ctrl->group)) {
            if (!start_cmd(&(*superMap)[i].proc, superMap, para, ctrl))
                return false ;
            start = true;
        }
    }
    if (!start) {
        pthread_mutex_lock(&lock_read);
        fprintf(stderr, "ERROR: no such group: %s\n", ctrl->group);
        fflush(stderr);
        pthread_mutex_unlock(&lock_read);
        return false ;
    }

    return true ;
}

bool    find_all_proc_start(t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl) {
    
    for (int i = 0; i < g_processCount; i++) {
        if (!start_cmd(&(*superMap)[i].proc, superMap, para, ctrl))
            return false ;
    }
    
    return true ;
}

bool    start_cmd(t_procs* proc, t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl) {

    if (proc->state == RUNNING) {
        pthread_mutex_lock(&lock_read);
        fprintf(stderr, "%s_%d: ERROR (already started)\n", proc->config->name, proc->id);
        fflush(stderr);
        pthread_mutex_unlock(&lock_read);
        return true ;
    }
    if (proc->state == STARTING) {
        pthread_mutex_lock(&lock_read);
        fprintf(stderr, "%s_%d: ERROR (already starting)\n", proc->config->name, proc->id);
        fflush(stderr);
        pthread_mutex_unlock(&lock_read);
        return true ;
    }

    proc->count_retries = 0;
    proc->count_restart = 0;
    proc->run_reached = false;

    if (!startProcess(proc, superMap, para, ctrl)) {
        pthread_mutex_lock(&lock_read);
        fprintf(stderr, "%s_%d: ERROR (abnormal termination)\n", proc->config->name, proc->id);
        fflush(stderr);
        pthread_mutex_unlock(&lock_read);
        return true ;
    }

    r_init_clear_var(proc);
    proc->ctrl_cmd_stop = false;
    proc->ctrl_cmd_start = true; 
    proc->boot_auto = true;

    return true ;
}

/* RESTART */
bool    find_name_proc_restart(t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl) {
    int id_p;
    if (!int_parser(ctrl->name, &id_p)) {
        pthread_mutex_lock(&lock_read);
        fprintf(stderr, "%s_%s: ERROR (no such proces)\n", ctrl->group, ctrl->name);
        fflush(stderr);
        pthread_mutex_unlock(&lock_read);
        return false ;
    }

    for (int i = 0; i < g_processCount; i++) {
        if (id_p == ((*superMap)[i].id - 1) && !strcmp(ctrl->group, (*superMap)[i].name)) {
            if (!restart_cmd(&(*superMap)[i].proc, superMap, para, ctrl))
                return false ;
            return true ;
        }
    }
    pthread_mutex_lock(&lock_read);
    fprintf(stderr, "%s_%s: ERROR (no such proces)\n", ctrl->group, ctrl->name);
    fflush(stderr);
    pthread_mutex_unlock(&lock_read);
    return false ;
}

bool    find_group_proc_restart(t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl) {
    
    bool    start = false;

    for (int i = 0; i < g_processCount; i++) {
        if (!strcmp((*superMap)[i].name, ctrl->group)) {
            if (!restart_cmd(&(*superMap)[i].proc, superMap, para, ctrl))
                return false ;
            start = true;
        }
    }
    if (!start) {
        pthread_mutex_lock(&lock_read);
        fprintf(stderr, "ERROR: no such group: %s\n", ctrl->group);
        fflush(stderr);
        pthread_mutex_unlock(&lock_read);
        return false ;
    }

    return true ;
}

bool    find_all_proc_restart(t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl) {
    
    for (int i = 0; i < g_processCount; i++) {
        if (!restart_cmd(&(*superMap)[i].proc, superMap, para, ctrl))
            return false ;
    }
    
    return true ;
}

bool    restart_cmd(t_procs* proc, t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl) {

    if (proc->state == RUNNING || proc->state == STARTING) {
        if (!stopProcess(proc)) {
            pthread_mutex_lock(&lock_read);
            fprintf(stderr, "%s_%d: ERROR (not stopped)\n", proc->config->name, proc->id);
            fflush(stderr);
            pthread_mutex_unlock(&lock_read);
            return true ;
        }
        pthread_mutex_lock(&lock_read);
        fprintf(stdout, "%s_%d: stopped\n", proc->config->name, proc->id);
        fflush(stdout);
        pthread_mutex_unlock(&lock_read);
    }

    proc->count_retries = 0;
    proc->count_restart = 0;
    proc->run_reached = false;

    if (!startProcess(proc, superMap, para, ctrl)) {
        pthread_mutex_lock(&lock_read);
        fprintf(stderr, "%s_%d: ERROR (abnormal termination)\n", proc->config->name, proc->id);
        fflush(stderr);
        pthread_mutex_unlock(&lock_read);
        return true ;
    }
    
    r_init_clear_var(proc);
    proc->ctrl_cmd_stop = false;
    proc->ctrl_cmd_start = true; 
    proc->boot_auto = true;

    return true ;
}

/* STOP */
bool    find_name_proc_stop(t_superMap** superMap, t_ctrl_cmds* ctrl) {
    int id_p;
    if (!int_parser(ctrl->name, &id_p)) {
        pthread_mutex_lock(&lock_read);
        fprintf(stderr, "%s_%s: ERROR (no such proces)\n", ctrl->group, ctrl->name);
        fflush(stderr);
        pthread_mutex_unlock(&lock_read);
        return false ;
    }

    for (int i = 0; i < g_processCount; i++) {
        if (id_p == ((*superMap)[i].id - 1) && !strcmp(ctrl->group, (*superMap)[i].name)) {
            if (!stop_cmd(&(*superMap)[i].proc))
                return false ;
            return true ;
        }
    }
    pthread_mutex_lock(&lock_read);
    fprintf(stderr, "%s_%s: ERROR (no such proces)\n", ctrl->group, ctrl->name);
    fflush(stderr);
    pthread_mutex_unlock(&lock_read);
    return false ;
}

bool    find_group_proc_stop(t_superMap** superMap, t_ctrl_cmds* ctrl) {
    
    bool    start = false;

    for (int i = 0; i < g_processCount; i++) {
        if (!strcmp((*superMap)[i].name, ctrl->group)) {
            if (!stop_cmd(&(*superMap)[i].proc))
                return false ;
            start = true;
        }
    }
    if (!start) {
        pthread_mutex_lock(&lock_read);
        fprintf(stderr, "ERROR: no such group: %s\n", ctrl->group);
        fflush(stderr);
        pthread_mutex_unlock(&lock_read);
        return false ;
    }

    return true ;
}

bool    find_all_proc_stop(t_superMap** superMap) {
    
    for (int i = 0; i < g_processCount; i++) {
        if (!stop_cmd(&(*superMap)[i].proc))
            return false ;
    }
    
    return true ;
}

bool    stop_cmd(t_procs* proc) {

    if (proc->state == RUNNING || proc->state == STARTING || proc->state == BACKOFF) {
        if (!stopProcess(proc)) {
            pthread_mutex_lock(&lock_read);
            fprintf(stderr, "%s_%d: ERROR (not stopped)\n", proc->config->name, proc->id);
            fflush(stderr);
            pthread_mutex_unlock(&lock_read);
            return true ;
        }
        pthread_mutex_lock(&lock_read);
        fprintf(stdout, "%s_%d: stopped\n", proc->config->name, proc->id);
        fflush(stdout);
        pthread_mutex_unlock(&lock_read);
    }
    else if (proc->state == STOPPED || proc->state == EXITED || proc->state == FATAL) {
        pthread_mutex_lock(&lock_read);
        fprintf(stderr, "%s_%d: ERROR (not running)\n", proc->config->name, proc->id);
        fflush(stderr);
        pthread_mutex_unlock(&lock_read);
        return true ;
    }
    
    proc->ctrl_cmd_stop = true;
    proc->ctrl_cmd_start = false; 

    return true ;
}

/* STATUS */
bool    find_name_proc_status(t_superMap** superMap, t_ctrl_cmds* ctrl) {
    int id_p;
    if (!int_parser(ctrl->name, &id_p)) {
        pthread_mutex_lock(&lock_read);
        fprintf(stderr, "%s_%s: ERROR (no such proces)\n", ctrl->group, ctrl->name);
        fflush(stderr);
        pthread_mutex_unlock(&lock_read);
        return false ;
    }

    for (int i = 0; i < g_processCount; i++) {
        if (id_p == ((*superMap)[i].id - 1) && !strcmp(ctrl->group, (*superMap)[i].name)) {
            if (!status_cmd(&(*superMap)[i].proc))
                return false ;
            return true ;
        }
    }
    pthread_mutex_lock(&lock_read);
    fprintf(stderr, "%s_%s: ERROR (no such proces)\n", ctrl->group, ctrl->name);
    fflush(stderr);
    pthread_mutex_unlock(&lock_read);
    return false ;
}

bool    find_group_proc_status(t_superMap** superMap, t_ctrl_cmds* ctrl) {
    
    bool    start = false;

    for (int i = 0; i < g_processCount; i++) {
        if (!strcmp((*superMap)[i].name, ctrl->group)) {
            if (!status_cmd(&(*superMap)[i].proc))
                return false ;
            start = true;
        }
    }
    if (!start) {
        pthread_mutex_lock(&lock_read);
        fprintf(stderr, "ERROR: no such group: %s\n", ctrl->group);
        fflush(stderr);
        pthread_mutex_unlock(&lock_read);
        return false ;
    }

    return true ;
}

bool    find_all_proc_status(t_superMap** superMap) {
    
    for (int i = 0; i < g_processCount; i++) {
        if (!status_cmd(&(*superMap)[i].proc))
            return false ;
    }
    
    return true ;
}

bool    status_cmd(t_procs* proc) {

    int     len = snprintf(NULL, 0, "%s:%d", proc->config->name, proc->id) + 1; 
    char*   name_id = malloc(len);
    pthread_mutex_lock(&lock_read);
    if (!name_id) {
        fprintf(stderr, "Error ctrl : allocation error (malloc)\n");
        return false ;
    }
    snprintf(name_id, len, "%s:%d", proc->config->name, proc->id);
    fprintf(stdout, "%-20.20s %-10s ", name_id, ProcessStatus_toString(proc->state));
    free(name_id);

    if (proc->state == RUNNING) {
        time_t  end = time(NULL);
        double  diff = difftime(end, proc->start_run);
        int     h = (int)diff / 3600;
        int     m = ((int)diff % 3600) / 60;
        int     s = (int)diff % 60; 
        fprintf(stdout, "pid %d, uptime: %d:%02d:%02d\n", proc->processus, h, m, s);
    }
    else if (proc->state == STARTING) {
        fprintf(stdout, "starting\n");
    }
    else if (proc->state == STOPPED) {
        fprintf(stdout, "Not started\n");
    }
    else if (proc->state == FATAL || proc->state == BACKOFF || (proc->state == EXITED && !proc->exit_code)) {
        fprintf(stdout, "Exited too quickly (process log may have details)\n");
    }
    else if (proc->state == EXITED && proc->exit_code) {
        fprintf(stdout, "Exited normally\n");
    }
    else
        fprintf(stdout, "unknown state\n");

    pthread_mutex_unlock(&lock_read);
    
    return true ;
}

bool    r_init_clear_var(t_procs* proc) {

    proc->run_reached = false;
    proc->count_retries = 0;
    proc->count_restart = 0;
    proc->start_restart = 0;

    return true ;

}
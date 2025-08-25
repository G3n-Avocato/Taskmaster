# include "supervisor.h"

/* START */
bool     find_name_proc_start(t_superMap** superMap, t_process_para* para, char* group, char* name) {
    int id_p;
    if (!int_parser(name, &id_p)) {
        fprintf(stderr, "%s_%s: ERROR (no such proces)\n", group, name);
        fflush(stderr);
        return false ;
    }
    int len = strlen(group);

    for (int i = 0; i < g_processCount; i++) {
        if (id_p == ((*superMap)[i].id - 1) && !strncmp(group, (*superMap)[i].name, len)) {
            if (!start_cmd(&(*superMap)[i].proc, superMap, para))
                return false ;
            return true ;
        }
    }
    
    fprintf(stderr, "%s_%s: ERROR (no such proces)\n", group, name);
    fflush(stderr);
    return false ;
}

bool    find_group_proc_start(t_superMap** superMap, t_process_para* para, char* group) {
    
    bool    start = false;
    int     len = strlen(group);

    for (int i = 0; i < g_processCount; i++) {
        if (!strncmp((*superMap)[i].name, group, len)) {
            if (!start_cmd(&(*superMap)[i].proc, superMap, para))
                return false ;
            start = true;
        }
    }
    if (!start) {
        fprintf(stderr, "ERROR: no such group: %s\n", group);
        fflush(stderr);
        return false ;
    }

    return true ;
}

bool    find_all_proc_start(t_superMap** superMap, t_process_para* para) {
    
    for (int i = 0; i < g_processCount; i++) {
        if (!start_cmd(&(*superMap)[i].proc, superMap, para))
            return false ;
    }
    
    return true ;
}


bool    start_cmd(t_procs* proc, t_superMap** superMap, t_process_para* para) {

    if (proc->state == RUNNING) {
        fprintf(stderr, "%s_%d: ERROR (already started)\n", proc->config->name, proc->id);
        fflush(stderr);
        return false ;
    }
    if (proc->state == STARTING) {
        fprintf(stderr, "%s_%d: ERROR (already starting)\n", proc->config->name, proc->id);
        fflush(stderr);
        return false ;
    }

    if (!startProcess(proc, superMap, para)) {
        fprintf(stderr, "%s_%d: ERROR (abnormal termination)\n", proc->config->name, proc->id);
        return false ;
    }
    
    proc->ctrl_cmd = true; 

    return true ;
}

/* RESTART */
bool     find_name_proc_restart(t_superMap** superMap, t_process_para* para, char* group, char* name) {
    int id_p;
    if (!int_parser(name, &id_p)) {
        fprintf(stderr, "%s_%s: ERROR (no such proces)\n", group, name);
        fflush(stderr);
        return false ;
    }
    int len = strlen(group);

    for (int i = 0; i < g_processCount; i++) {
        if (id_p == ((*superMap)[i].id - 1) && !strncmp(group, (*superMap)[i].name, len)) {
            if (!restart_cmd(&(*superMap)[i].proc, superMap, para))
                return false ;
            return true ;
        }
    }
    
    fprintf(stderr, "%s_%s: ERROR (no such proces)\n", group, name);
    fflush(stderr);
    return false ;
}

bool    find_group_proc_restart(t_superMap** superMap, t_process_para* para, char* group) {
    
    bool    start = false;
    int     len = strlen(group);

    for (int i = 0; i < g_processCount; i++) {
        if (!strncmp((*superMap)[i].name, group, len)) {
            if (!restart_cmd(&(*superMap)[i].proc, superMap, para))
                return false ;
            start = true;
        }
    }
    if (!start) {
        fprintf(stderr, "ERROR: no such group: %s\n", group);
        fflush(stderr);
        return false ;
    }

    return true ;
}

bool    find_all_proc_restart(t_superMap** superMap, t_process_para* para) {
    
    for (int i = 0; i < g_processCount; i++) {
        if (!restart_cmd(&(*superMap)[i].proc, superMap, para))
            return false ;
    }
    
    return true ;
}

bool    restart_cmd(t_procs* proc, t_superMap** superMap, t_process_para* para) {

    if (proc->state == RUNNING || proc->state == STARTING) {
        if (!stopProcess(proc)) {
            fprintf(stderr, "%s_%d: ERROR (not stopped)\n", proc->config->name, proc->id);
            return false ;
        }
        fprintf(stdout, "%s_%d: stopped\n", proc->config->name, proc->id);
    }

    if (!startProcess(proc, superMap, para)) {
        fprintf(stderr, "%s_%d: ERROR (abnormal termination)\n", proc->config->name, proc->id);
        return false ;
    }
    
    proc->ctrl_cmd = true; 

    return true ;
}

/* STOP */
bool     find_name_proc_stop(t_superMap** superMap, char* group, char* name) {
    int id_p;
    if (!int_parser(name, &id_p)) {
        fprintf(stderr, "%s_%s: ERROR (no such proces)\n", group, name);
        fflush(stderr);
        return false ;
    }
    int len = strlen(group);

    for (int i = 0; i < g_processCount; i++) {
        if (id_p == ((*superMap)[i].id - 1) && !strncmp(group, (*superMap)[i].name, len)) {
            if (!stop_cmd(&(*superMap)[i].proc))
                return false ;
            return true ;
        }
    }
    
    fprintf(stderr, "%s_%s: ERROR (no such proces)\n", group, name);
    fflush(stderr);
    return false ;
}

bool    find_group_proc_stop(t_superMap** superMap, char* group) {
    
    bool    start = false;
    int     len = strlen(group);

    for (int i = 0; i < g_processCount; i++) {
        if (!strncmp((*superMap)[i].name, group, len)) {
            if (!stop_cmd(&(*superMap)[i].proc))
                return false ;
            start = true;
        }
    }
    if (!start) {
        fprintf(stderr, "ERROR: no such group: %s\n", group);
        fflush(stderr);
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
            fprintf(stderr, "%s_%d: ERROR (not stopped)\n", proc->config->name, proc->id);
            return false ;
        }
        fprintf(stdout, "%s_%d: stopped\n", proc->config->name, proc->id);
    }
    else if (proc->state == STOPPED || proc->state == EXITED || proc->state == FATAL) {
        fprintf(stderr, "%s_%d: ERROR (not running)\n", proc->config->name, proc->id);
        return false ;
    }
    
    proc->ctrl_cmd = true; 

    return true ;
}

/* STATUS */
bool     find_name_proc_status(t_superMap** superMap, char* group, char* name) {
    int id_p;
    if (!int_parser(name, &id_p)) {
        fprintf(stderr, "%s_%s: ERROR (no such proces)\n", group, name);
        fflush(stderr);
        return false ;
    }
    int len = strlen(group);

    for (int i = 0; i < g_processCount; i++) {
        if (id_p == ((*superMap)[i].id - 1) && !strncmp(group, (*superMap)[i].name, len)) {
            if (!status_cmd(&(*superMap)[i].proc))
                return false ;
            return true ;
        }
    }
    
    fprintf(stderr, "%s_%s: ERROR (no such proces)\n", group, name);
    fflush(stderr);
    return false ;
}

bool    find_group_proc_status(t_superMap** superMap, char* group) {
    
    bool    start = false;
    int     len = strlen(group);

    for (int i = 0; i < g_processCount; i++) {
        if (!strncmp((*superMap)[i].name, group, len)) {
            if (!status_cmd(&(*superMap)[i].proc))
                return false ;
            start = true;
        }
    }
    if (!start) {
        fprintf(stderr, "ERROR: no such group: %s\n", group);
        fflush(stderr);
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

    fprintf(stdout, "%s:%d\t\t\t%s\t", proc->config->name, proc->id, ProcessStatus_toString(proc->state));

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


    return true ;
}
# include "supervisor.h"

bool    init_process_struct(t_procs* proc, t_config* conf, unsigned int j) {

    proc->config = conf;
    proc->exec = malloc(sizeof(t_execs) * 1);
    if (!proc->exec)
        return false ;

    // fct argv init
    // fct envp init
    
    if (!conf->stdout) {
        if (!build_default_Std_process(&proc->exec->stdout, conf, ".stdout", j))
            return false ;
    }
    else if (!buildStd_process(conf->stdout, &proc->exec->stdout, conf, j))
        return false ;
    if (!conf->stderr) {
        if (!build_default_Std_process(&proc->exec->stderr, conf, ".stderr", j))
            return false ;
    }
    else if (!buildStd_process(conf->stderr, &proc->exec->stderr, conf, j))
        return false ;

    proc->id = j;
    proc->state = STOPPED;
    proc->exit_code = false;
    proc->run_reached = false;
    proc->count_retries = 0;
    
    return true ;
}

bool    buildStd_process(char* std, char** ptr, t_config* conf, int j) {

    if (conf->numProcs == 1) {
        (*ptr) = strdup(std);
        if (!(*ptr))
            return false ;
    }
    else {
        const char  *c = strchr(std, '.');
        size_t      pos;
        if (!c)
            pos = strlen(std);
        else
            pos = c - std; 

        char*    idstr = ft_itoa(j);
        if (!idstr)
            return false ;
        size_t  idlen = strlen(idstr);

        size_t newlen = strlen(std) + 1 + idlen + 1;
        (*ptr) = malloc(newlen);
        if (!(*ptr)) {
            free(idstr);
            return false ;
        }

        strncpy((*ptr), std, pos);
        (*ptr)[pos] = '_';
        strcpy((*ptr) + pos + 1, idstr);
        strcpy((*ptr) + pos + 1 + idlen, std + pos);

        free(idstr);
    }
    return true ;
}

bool    build_default_Std_process(char** ptr, t_config *conf, char *end, int j) {

    char*   workdir = "/tmp/";
    char*   idstr = NULL;
    size_t  len = 0;
    
    if (conf->numProcs > 1) {
        idstr = ft_itoa(j);
        if (!idstr)
            return false ;
        len = strlen(workdir) + strlen(conf->name) + 1 + strlen(idstr) + strlen(end) + 1;
    }
    else
        len = strlen(workdir) + strlen(conf->name) + strlen(end) + 1;
    (*ptr) = malloc(len);
    if (!(*ptr)) {
        if (conf->numProcs > 1)
            free(idstr);
        return false ;
    }

    strcpy((*ptr), workdir);
    strcpy((*ptr) + strlen(workdir), conf->name);
    if (conf->numProcs > 1) {
        strcpy((*ptr) + strlen(workdir) + strlen(conf->name), "_");
        strcpy((*ptr) + strlen(workdir) + strlen(conf->name) + 1, idstr);
        strcpy((*ptr) + strlen(workdir) + strlen(conf->name) + 1 + strlen(idstr), end);
        free(idstr);
    }
    else
        strcpy((*ptr) + strlen(workdir) + strlen(conf->name), end);

    return true ;
}
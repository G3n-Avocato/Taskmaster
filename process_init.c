# include "supervisor.h"

bool    init_process_struct(t_procs* proc, t_config* conf, unsigned int j) {

    proc->config = conf;
    proc->exec = malloc(sizeof(t_execs) * 1);
    if (!proc->exec)
        return false ;

    if (!splitWordsArgv(&proc->exec->argv, conf->cmd))
        return false ;
    if (!buildEnvp(&proc->exec->envp, conf))
        return false ;
    
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

bool    splitWordsArgv(char ***ptr, char *input) {

    char**  tmp = NULL;
    int     len = strlen(input);
    int     i = 0;
    int     start = 0;
    char*   word = NULL;
    int     count = 0;

    (*ptr) = NULL;

    while (i < len) {
        while (i < len && isspace(input[i]))
            i++;
        if (i >= len)
            break ;
        start = i;
        while (i < len && !isspace(input[i]))
            i++;

        word = malloc((i - start) + 1);
        if (!word)
            return false ;
        strncpy(word, &input[start], (i - start));
        word[(i - start)] = '\0';

        tmp = realloc(*ptr, (count + 2) * sizeof(char*));
        if (!tmp) {
            free(word);
            return false ;
        }

        (*ptr) = tmp;
        (*ptr)[count] = word;
        count++;
    }

    if ((*ptr))
        (*ptr)[count] = NULL;

    return true ;

}

bool    buildEnvp(char ***ptr, t_config* conf) {
    if (!conf->env) {
        (*ptr) = NULL;
        return true ; 
    }
    t_env_p*    loop = conf->env;
    (*ptr) = malloc((conf->count_env + 1) * sizeof(char*));
    if (!(*ptr))
        return false ;

    size_t  key_len;
    size_t  val_len;
    char*   tmp = NULL;

    for (size_t i = 0; i < conf->count_env; i++) {
        key_len = strlen(loop[i].key);
        val_len = strlen(loop[i].value);

        tmp = malloc(key_len + 1 + val_len + 1);
        if (!tmp)
            return false;

            memcpy(tmp, loop[i].key, key_len);
            tmp[key_len] = '=';
            memcpy(tmp + key_len + 1, loop[i].value, val_len);
            tmp[key_len + 1 + val_len] = '\0';

            (*ptr)[i] = tmp;
    }
    (*ptr)[conf->count_env] = NULL;
    
    return true ;
}

bool    open_file(char* std, int* fd) {
    (*fd) = open(std, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if ((*fd) < 0) {
        perror("open std");
        return false ;
    }
    return true ;
}

bool    open_file_std(t_execs* exec) {
    if (!open_file(exec->stdout, &exec->fd_out))
        exec->fd_out = 1;
    if (!open_file(exec->stderr, &exec->fd_err))
        exec->fd_err = 2;
    return true ;
}
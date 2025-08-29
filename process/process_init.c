# include "../include/supervisor.h"

bool    init_process_struct(t_procs* proc, t_config* conf, unsigned int j) {

    if (!init_config_process(proc, conf))
        return false ;
    
    proc->exec = malloc(sizeof(t_execs) * 1);
    if (!proc->exec) {
        logger(CRIT, "Error init exec: allocation error (malloc)");
        return false ;
    }

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
    proc->start_run = 0;
    proc->count_restart = 0;
    proc->start_restart = 0;
    proc->boot_auto = false;

    proc->ctrl_cmd_start = false;
    proc->ctrl_cmd_stop = false;

    return true ;
}

bool    buildStd_process(char* std, char** ptr, t_config* conf, int j) {

    if (conf->numProcs == 1) {
        (*ptr) = strdup(std);
        if (!(*ptr)) {
            logger(CRIT, "Error init std: allocation error (strdup)");
            return false ;
        }
    }
    else {
        const char  *c = strchr(std, '.');
        size_t      pos;
        if (!c)
            pos = strlen(std);
        else
            pos = c - std; 

        char*    idstr = ft_itoa(j);
        if (!idstr) {
            logger(CRIT, "Error init std: allocation error (calloc)");
            return false ;
        }
        size_t  idlen = strlen(idstr);

        size_t newlen = strlen(std) + 1 + idlen + 1;
        (*ptr) = malloc(newlen);
        if (!(*ptr)) {
            logger(CRIT, "Error init std: allocation error (malloc)");
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
        if (!idstr) {
            logger(CRIT, "Error init std: allocation error (calloc)");
            return false ;
        }
        len = strlen(workdir) + strlen(conf->name) + 1 + strlen(idstr) + strlen(end) + 1;
    }
    else
        len = strlen(workdir) + strlen(conf->name) + strlen(end) + 1;
    (*ptr) = malloc(len);
    if (!(*ptr)) {
        if (conf->numProcs > 1)
            free(idstr);
        logger(CRIT, "Error init std: allocation error (malloc)");
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
        if (!word) {
            logger(CRIT, "Error init argv: allocation error (malloc)");
            return false ;
        }
        strncpy(word, &input[start], (i - start));
        word[(i - start)] = '\0';

        tmp = realloc(*ptr, (count + 2) * sizeof(char*));
        if (!tmp) {
            logger(CRIT, "Error init argv: allocation error (realloc)");
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
    if (!(*ptr)) {
        logger(CRIT, "Error init envp: allocation error (strdup)");
        return false ;
    }

    size_t  key_len;
    size_t  val_len;
    char*   tmp = NULL;

    for (size_t i = 0; i < conf->count_env; i++) {
        key_len = strlen(loop[i].key);
        val_len = strlen(loop[i].value);

        tmp = malloc(key_len + 1 + val_len + 1);
        if (!tmp) {
            logger(CRIT, "Error init envp: allocation error (strdup)");
            return false;
        }

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
        char    buffer[100];
        snprintf(buffer, sizeof(buffer), "Error open stdout/err : %s", strerror(errno));
        logger(CRIT, buffer);
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

bool    init_config_process(t_procs* proc, t_config* conf) {

    proc->config = malloc(sizeof(t_config) * 1);
    if (!proc->config) {
        logger(CRIT, "Error init : allocation error (realloc)");
        return false ;
    }

    proc->config->name = strdup(conf->name);
    if (!proc->config->name) {
        logger(CRIT, "Error init : allocation error (strdup)");
        return false ;
    }
    proc->config->cmd = strdup(conf->cmd);
    if (!proc->config->cmd) {
        logger(CRIT, "Error init : allocation error (strdup)");
        return false ;
    }
    proc->config->numProcs = conf->numProcs;
    proc->config->umask = conf->umask;
    proc->config->workingDir = strdup(conf->workingDir);
    if (!proc->config->workingDir) {
        logger(CRIT, "Error init : allocation error (strdup)");
        return false ;
    }
    proc->config->autoStart = conf->autoStart;
    proc->config->autoRestart = conf->autoRestart;
    if (!copy_exitcode_struct(proc, conf))
        return false ;

    proc->config->startRetries = conf->startRetries;
    proc->config->startTime = conf->startTime;
    proc->config->stopSignal = conf->stopSignal;
    proc->config->stopTime = conf->stopTime;
    if (conf->stdout) {
        proc->config->stdout = strdup(conf->stdout);
        if (!proc->config->stdout) {
            logger(CRIT, "Error init : allocation error (strdup)");
            return false ;
        }
    }
    else
        proc->config->stdout = NULL;
    if (conf->stderr) {
        proc->config->stderr = strdup(conf->stderr);
        if (!proc->config->stderr) {
            logger(CRIT, "Error init : allocation error (strdup)");
            return false ;
        }
    }
    else
        proc->config->stderr = NULL;
    proc->config->env = NULL;
    proc->config->count_env = 0;
    if (!copy_env_struct(proc, conf))
        return false ;


    return true ;
}

bool    copy_exitcode_struct(t_procs* proc, t_config* conf) {
    proc->config->exitCodes.count = conf->exitCodes.count;

    if (proc->config->exitCodes.count > 0) {
        proc->config->exitCodes.codes = malloc(sizeof(int) * proc->config->exitCodes.count);
        if (!proc->config->exitCodes.codes) {
            logger(CRIT, "Error init : allocation error (malloc)");
            return false ;
        }

        memcpy(proc->config->exitCodes.codes, conf->exitCodes.codes, sizeof(int) * proc->config->exitCodes.count);
    }
    else
        proc->config->exitCodes.codes = NULL;

    return true ;
}

bool    copy_env_struct(t_procs* proc, t_config* conf) {
    proc->config->env = malloc(sizeof(t_env_p) * conf->count_env);
    if (!proc->config->env) {
        logger(CRIT, "Error init : allocation error (malloc)");
        return false ;
    }
    proc->config->count_env = conf->count_env;

    for (unsigned int i = 0; i < conf->count_env; i++) {
        proc->config->env[i].key = strdup(conf->env[i].key);
        proc->config->env[i].value = strdup(conf->env[i].value);

        if (!proc->config->env[i].key || !proc->config->env[i].value) {
            logger(CRIT, "Error init : allocation error (malloc)");
            for (unsigned int j = 0; j <= i; j++) {
                free(proc->config->env[i].key);
                free(proc->config->env[i].value);
            }
            free(proc->config->env);
            proc->config->env = NULL;
            proc->config->count_env = 0;
            return false ;
        }
    }
    return true ;
}
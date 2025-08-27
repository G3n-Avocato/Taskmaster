# include "supervisor.h"

bool    reload_parse_configFile(t_process_para* para) {

    if (!parser_name_file(para[0].file_name, 2))
        return false ;

    if (para[1].config != NULL)
        free_paraNew_reload_error(&para[1]);

    if (!parser_file_yaml(para[0].file_name[1], &para[1])) {
        free_paraNew_reload_error(&para[1]);
        return false ;
    }
    
    return true ;
}

bool    reload_cmd(t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl) {

    bool    reload = false;

    if (!reload_parse_configFile(para)) {
        return false ;
    }
    if (!comp_configFile(superMap, para, &reload, ctrl)) {
        return false ;
    }
    if (!comp_configFile_for_delete_config(superMap, para)) {
        return false ;
    }

    return true ;
}

bool    comp_configFile_for_delete_config(t_superMap** superMap, t_process_para* para) {
    (void)superMap;
    unsigned int j = 0;
    unsigned int i = 0;
    int tp = 0;
    int len = 0;
    
    while (i < para[0].count) {
        j = 0;
        while (j < para[1].count) {
            len = strlen(para[1].config[j].name);
            if (!strncmp(para[0].config[i].name, para[1].config[j].name, len)) {
                tp = 1;
                break ;
            }
            else
                j++;
        }
        if (tp == 1) {
            i++;
            tp = 0;
        }
        else {
            ;
            // stop process associer 
            // delete process 
            // delete config 
        }
    }
    return true ;
}

bool    comp_configFile(t_superMap** superMap, t_process_para* para, bool *reload, t_ctrl_cmds* ctrl) {
    unsigned int j = 0;
    unsigned int i = 0;
    int tp = 0;
    int len = 0;

    while (j < para[1].count) {
        i = 0;
        while (i < para[0].count) {
            len = strlen(para[1].config[j].name);
            if (!strncmp(para[0].config[i].name, para[1].config[j].name, len)) {
                
                if (!comp_configStruct(&para[0], &para[1], i, j, reload)) {
                    if (*reload == true) {
                        if (!stop_proc_superMap(superMap, para[1].config[j].name))
                            return false ;
                        if (!r_init_configStruct(&para[0], &para[1], i, j))
                            return false ;
                        if (!init_proc_superMap_reload(superMap, para[1].config[j].name, &para[0].config[i]))
                            return false ;
                        if (!start_proc_superMap(superMap, para[1].config[j].name, para, ctrl))
                            return false ;

                    }
                    else 
                        r_init_proc_superMap(superMap, para[1].config[j].name, &para[0].config[i]);
                }
                tp = 1;
                break ;
            }
            else
                i++;
        }
        if (tp == 1) {
            j++;
            tp = 0;
        }
        else {
            ;
            // new 
            // ajouter +1 a config de para
            // init new process in superMap
            // start
        }
    }

    return true ;
}

bool    r_init_configStruct(t_process_para* old, t_process_para* new, int i, int j) {

    int len = strlen(new->config[j].cmd);
    if (strncmp(old->config[i].cmd, new->config[j].cmd, len)) {
        if (old->config[i].cmd)
            free(old->config[i].cmd);
        old->config[i].cmd = strdup(new->config[j].cmd);
        if (!old->config[i].cmd)
            return false ;

    }
    if (old->config[i].numProcs != new->config[j].numProcs) {
        old->config[i].numProcs = new->config[j].numProcs;
    }
    if (old->config[i].umask != new->config[j].umask) {
        old->config[i].umask = new->config[j].umask;
    }
    len = strlen(new->config[j].workingDir);
    if (strncmp(old->config[i].workingDir, new->config[j].workingDir, len)) {
        if (old->config[i].workingDir)
            free(old->config[i].workingDir);
        old->config[i].workingDir = strdup(new->config[j].workingDir);
        if (!old->config[i].workingDir)
            return false ;
    }
    if (new->config[j].stdout) {
        len = strlen(new->config[j].stdout);
        if (strncmp(old->config[i].stdout, new->config[j].stdout, len)) {
            if (old->config[i].stdout)
                free(old->config[i].stdout);
            old->config[i].stdout = strdup(new->config[j].stdout);
            if (!old->config[i].stdout)
                return false ;
        }
    }
    if (new->config[j].stderr) {
        len = strlen(new->config[j].stderr);
        if (strncmp(old->config[i].stderr, new->config[j].stderr, len)) {
            if (old->config[i].stderr)
                free(old->config[i].stderr);
            old->config[i].stderr = strdup(new->config[j].stderr);
            if (!old->config[i].stderr)
                return false ;
        }
    }
    t_config* cfg_old = &old->config[i];
    t_config* cfg_new = &new->config[j];
    if (!comp_configStruct_env(cfg_old, cfg_new)) {
        if (!swap_env_struct_in_para(old, new, i, j))
            return false ;
    }

    return true ;
}

bool    start_proc_superMap(t_superMap** superMap, char *name, t_process_para* para, t_ctrl_cmds* ctrl) {
    int len = 0;

    for (int i = 0; i < g_processCount; i++) {
        len = strlen(name);
        if (!strncmp((*superMap)[i].name, name, len)) {
            if (!start_cmd(&(*superMap)[i].proc, superMap, para, ctrl))
                return false ;
            (*superMap)[i].proc.boot_auto = true;
        }
    }
    return true ;

}

bool    stop_proc_superMap(t_superMap** superMap, char* name) {
    int len = 0;

    for (int i = 0; i < g_processCount; i++) {
        len = strlen(name);
        if (!strncmp((*superMap)[i].name, name, len)) {
            if (!stop_cmd(&(*superMap)[i].proc))
                return false ;
        }
    }
    return true ;
}

bool    r_init_proc_superMap(t_superMap** superMap, char* name, t_config* conf) {
    int len = 0;

    for (int i = 0; i < g_processCount; i++) {
        len = strlen(name);
        if (!strncmp((*superMap)[i].name, name, len)) {
            (*superMap)[i].proc.config = conf;
        }
    }

    return true ;
}

bool    comp_configStruct(t_process_para* old, t_process_para* new, int i, int j, bool *reload) {

    bool comp = true ;

    int len = strlen(new->config[j].cmd);
    if (strncmp(old->config[i].cmd, new->config[j].cmd, len)) {
        *reload = true ;
        comp = false;
    }
    if (old->config[i].numProcs != new->config[j].numProcs) {
        *reload = true ;
        comp = false;
    }
    if (old->config[i].umask != new->config[j].umask) {
        *reload = true ;
        comp = false;
    }
    len = strlen(new->config[j].workingDir);
    if (strncmp(old->config[i].workingDir, new->config[j].workingDir, len)) {
        *reload = true ;
        comp = false;
    }
    if (old->config[i].autoStart != new->config[j].autoStart) {
        old->config[i].autoStart = new->config[j].autoStart;
        comp = false;
    }
    if (old->config[i].autoRestart != new->config[j].autoRestart) {
        old->config[i].autoRestart = new->config[j].autoRestart;
        comp = false;
    }
    t_config* cfg_old = &old->config[i];
    t_config* cfg_new = &new->config[j];
    if (!comp_configStruct_exitcode(cfg_old, cfg_new)) {
        swap_exitCodes_struct_in_para(old, new, i, j);
        comp = false;
    }
    if (old->config[i].startRetries != new->config[j].startRetries) {
        old->config[i].startRetries = new->config[j].startRetries;
        comp = false;
    }
    if (old->config[i].startTime != new->config[j].startTime) {
        old->config[i].startTime = new->config[j].startTime;
        comp = false;
    }
    if (old->config[i].stopSignal != new->config[j].stopSignal) {
        old->config[i].stopSignal = new->config[j].stopSignal;
        comp = false;
    }
    if (old->config[i].stopTime != new->config[j].stopTime) {
        old->config[i].stopTime = new->config[j].stopTime;
        comp = false;
    }
    if (new->config[j].stdout) { 
        len = strlen(new->config[j].stdout);
        if (strncmp(old->config[i].stdout, new->config[j].stdout, len)) {//
            *reload = true;
            comp = false;
        }
    }
    if (new->config[j].stderr) {
        len = strlen(new->config[j].stderr);
        if (strncmp(old->config[i].stderr, new->config[j].stderr, len)) {//
            *reload = true;
            comp = false;
        }
    }
    if (!comp_configStruct_env(cfg_old, cfg_new)) {
        *reload = true;
        comp = false;
    }
    
    return comp ;
}

bool    comp_configStruct_exitcode(t_config* old, t_config* new) {
    unsigned int i = 0;
    unsigned int j = 0;
    int tp = 0;

    if (new->exitCodes.count != old->exitCodes.count)
        return false ;

    while (j < new->exitCodes.count) {
        i = 0;
        while (i < old->exitCodes.count) {
            if (new->exitCodes.codes[j] == old->exitCodes.codes[i]) {
                tp = 1;
                break ;
            }
            else
                i++;
        }
        if (tp == 1) {
            j++;
            tp = 0;
        }
        else 
            return false ;
    }

    return true ;
}

bool    swap_exitCodes_struct_in_para(t_process_para* old, t_process_para* new, int i, int j) {

    int*    tmp = NULL;
    if (new->config[j].exitCodes.count != 0) {
        tmp = realloc(new->config[j].exitCodes.codes, sizeof(int) * (new->config[j].exitCodes.count));
        if (!tmp) {
            fprintf(stderr, "Error command ctrl : allocation error (realloc)\n");
            return false ;
        }
    }

    if (old->config[i].exitCodes.codes) {
        free(old->config[i].exitCodes.codes);
        old->config[i].exitCodes.codes = NULL;
        old->config[i].exitCodes.count = new->config[j].exitCodes.count;
        if (new->config[j].exitCodes.count == 0)
            return true ;
    }

    old->config[i].exitCodes.codes = tmp ;
    old->config[i].exitCodes.count = new->config[j].exitCodes.count;

    return true ;
}

bool    comp_configStruct_env(t_config* old, t_config* new) {
    unsigned int i = 0;
    unsigned int j = 0;
    int tp = 0;
    int len = 0;

    if (new->count_env != old->count_env)
        return false ;

    while (j < new->count_env) {
        i = 0;
        while (i < old->count_env) {
            len = strlen(new->env[j].key);
            if (!strncmp(old->env[i].key, new->env[j].key, len)) {
                len = strlen(new->env[j].value);
                if (!strncmp(old->env[i].value, new->env[j].value, len)) {
                    tp = 1;
                    break ;
                }
                else
                    return false ;
            }
            else
                i++;
        }
        if (tp == 1) {
            j++;
            tp = 0;
        }
        else
            return false ;

    }

    return true ;
}

bool    swap_env_struct_in_para(t_process_para* old, t_process_para* new, int i, int j) {

    for (size_t h = 0; h < old->config[i].count_env; h++) {
        if (old->config[i].env[h].key)
            free(old->config[i].env[h].key);
        if (old->config[i].env[i].value)
            free(old->config[i].env[h].value);
    }
    if (old->config[i].env) {
        free(old->config[i].env);
        old->config[i].env = NULL;
        old->config[i].count_env = new->config[j].count_env;
        if (new->config[j].count_env == 0)
            return true ;
    }

    t_env_p*    envs = NULL;
    for (unsigned int h = 0; h < new->config[j].count_env; h++) {
        char* key = strdup(new->config[j].env[h].key);
        if (!key) {
            return false ;
        }
        char* value = strdup(new->config[j].env[h].value);
        if (!value) {
            free(key);
            return false ;
        }
        t_env_p*    tmp = realloc(envs, sizeof(t_env_p) * (h + 1));
        if (!tmp) {
            free(key);
            free(value);
            return false;
        }
        envs = tmp;
        envs[h].key = key;
        envs[h].value = value;
    }

    old->config[i].env = envs;
    old->config[i].count_env = new->config[j].count_env;
    return true ;
}

void    free_paraNew_reload_error(t_process_para *para) {
    
    if (para && para->config != NULL) {
        
        for (size_t j = 0; j < para->count; j++) {
            free_config(&para->config[j]);
        }
        free(para->config);
        para->config = NULL;
    }
}
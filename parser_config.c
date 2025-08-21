# include "supervisor.h"

bool    parsing_name(t_process_para* para, char* val) {

    t_config current = {0};

    para->count++;
    para->config = realloc(para->config, sizeof(t_config) * para->count);
    if (!para->config) {
        logger(CRIT, "Error parser config : allocation error (realloc)");
        return false ;
    }

    current = (t_config){0};
    para->config[para->count - 1] = current;
    para->config[para->count - 1].name = strdup(val);
    if (!para->config[para->count - 1].name) {
        logger(CRIT, "Error parser config : allocation error (strdup)");
        return false ;
    }
    para->config[para->count - 1].has_cmd = false;
    para->config[para->count - 1].numProcs = 1;
    para->config[para->count - 1].umask = 022;
    para->config[para->count - 1].workingDir = strdup("/");
    if (!para->config[para->count - 1].workingDir)  {
        logger(CRIT, "Error parser config : allocation error (strdup)");
        return false ;
    }
    para->config[para->count - 1].autoStart = true ;
    para->config[para->count - 1].autoRestart = UNEXPECTED;
    parser_exitcodes_no_sequence("0", &para->config[para->count - 1]);
    para->config[para->count - 1].startRetries = 3;
    para->config[para->count - 1].startTime = 1;
    parser_stopsignal("TERM", &para->config[para->count - 1]);
    para->config[para->count - 1].stopTime = 10;
    para->config[para->count - 1].stdout = NULL;
    para->config[para->count - 1].stderr = NULL;
    para->config[para->count - 1].env = NULL;
    return true ;
}

bool    parser_list_options_config(char *last_key, char *val, t_config* cfg) {

    if (!strcmp(last_key, "cmd")) {
        cfg->cmd = strdup(val);
        if (!cfg->cmd) {
            logger(CRIT, "Error parser config : allocation error (strdup)");
            return false ;
        }
        cfg->has_cmd = true;
    }
    else if (!strcmp(last_key, "numprocs"))
        return (parser_numprocs(val, cfg));
    else if (!strcmp(last_key, "umask"))
        return (parser_umask(val, cfg));
    else if (!strcmp(last_key, "workingdir")) {
        free(cfg->workingDir);
        cfg->workingDir = strdup(val);
        if (!cfg->workingDir) {
            logger(CRIT, "Error parser config : allocation error (strdup)");
            return false ;
        }
    }
    else if (!strcmp(last_key, "autostart")) {
        if (!bool_parser(val, &cfg->autoStart)) {
            logger(DEBG, "Error config file : Bad Paramater 'autostart'");
            return false ;
        }
    }
    else if (!strcmp(last_key, "autorestart"))
        return (parser_autorestart(val, cfg));
    else if (!strcmp(last_key, "startretries"))
        return (parser_startretries(val, cfg));
    else if (!strcmp(last_key, "exitcodes"))
        return (parser_exitcodes_no_sequence(val, cfg));
    else if (!strcmp(last_key, "starttime"))
        return (parser_starttime(val, cfg));
    else if (!strcmp(last_key, "stopsignal"))
        return (parser_stopsignal(val, cfg));
    else if (!strcmp(last_key, "stoptime"))
        return (parser_stoptime(val, cfg));
    else if (!strcmp(last_key, "stdout")) {
        cfg->stdout = strdup(val);
        if (!cfg->stdout)  {
            logger(CRIT, "Error parser config : allocation error (strdup)");
            return false ;
        }
    }
    else if (!strcmp(last_key, "stderr")) {
        cfg->stderr = strdup(val);
        if (!cfg->stderr)  {
            logger(CRIT, "Error parser config : allocation error (strdup)");
            return false ;
        }
    }

    return true ;
}
//------------------------------------------------------> ici logger
bool    parser_numprocs(char *val, t_config* cfg) {
    
    if (!int_parser(val, &cfg->numProcs)) {
        printf("Error config file : Bad Paramater 'numprocs'\n");
        return false ;
    }
    if (cfg->numProcs < 0) {
        printf("Error config file : Bad Paramater 'numprocs'\n");
        return false ;
    }
    return true ;
}

bool    parser_umask(char *val, t_config* cfg) {
    
    if (!int_parser(val, &cfg->umask)) {
        printf("Error config file : Bad Paramater 'umask'\n");
        return false ;
    }
    if (cfg->umask < 0 || cfg->umask > 511) {
        printf("Error config file : Bad Paramater 'umask'\n");
        return false ;
    }
    return true ;
}

bool    parser_autorestart(char *val, t_config* cfg) {
    char*   _tr = "true";
    char*   _fa = "false";
    char*   _un = "unexpected";
    
    if (!strncmp(val, _tr, 5)) {
        cfg->autoRestart = TRUE;
        return true ;
    }
    else if (!strncmp(val, _fa, 6)) {
        cfg->autoRestart = FALSE;
        return true ;
    }
    else if (!strncmp(val, _un, 11)) {
        cfg->autoRestart = UNEXPECTED;
            return true ;
    }   
    else
        printf("Error config file : Bad Paramater 'autorestart'\n");
    return false ;
}

bool    parser_startretries(char *val, t_config* cfg) {
    
    if (!int_parser(val, &cfg->startRetries)) {
        printf("Error config file : Bad Paramater 'startretries'\n");
        return false ;
    }
    if (cfg->startRetries < 0) {
        printf("Error config file : Bad Paramater 'startretries'\n");
        return false ;
    }
    return true ;
}

bool    parser_exitcodes_no_sequence(char* val, t_config* cfg) {
    free(cfg->exitCodes.codes);

    int code;
    if (!int_parser(val, &code)) {
        printf("Error config file : Bad Paramater 'exitcodes'\n");
        return false ;
    }
    
    int* codes = malloc(sizeof(int));
    if (!codes) {
        printf("Error config file : Bad Paramater 'exitcodes'\n");
        return false ;
    }

    codes[0] = code;
    cfg->exitCodes.codes = codes;
    cfg->exitCodes.count = 1;

    return true ;
}

bool    parser_starttime(char *val, t_config *cfg) {
    
    if (!int_parser(val, &cfg->startTime)) {
        printf("Error config file : Bad Paramater 'starttime'\n");
        return false ;
    }
    
    if (cfg->startTime < 0) {
        printf("Error config file : Bad Paramater 'starttime'\n");
        return false ;
    }

    return true ;
}

bool    parser_stopsignal(char *val, t_config* cfg) {
    for (size_t i = 0; i < signal_map_size; i++) {
        if (!strcmp(signalMap[i].name, val)) {
            cfg->stopSignal = signalMap[i].value;
            return true ;
        }
    }
    printf("Error config file : Bad Paramater 'sstopsignal'\n");
    return false ;
}

bool    parser_stoptime(char *val, t_config *cfg) {
    
    if (!int_parser(val, &cfg->stopTime)) {
        printf("Error config file : Bad Paramater 'stoptime'\n");
        return false ;
    }
    
    if (cfg->stopTime < 0) {
        printf("Error config file : Bad Paramater 'stoptime'\n");
        return false ;
    }

    return true ;
}

bool    parser_env(yaml_parser_t* parser, yaml_event_t* event, t_config* cfg) {
    if (event->type != YAML_MAPPING_START_EVENT) {
        fprintf(stderr, "Error config file : Expected a mapping for 'env'\n");
        return false;
    }

    t_env_p*        envs = NULL;
    unsigned int    count = 0;

    while (1) {
        yaml_parser_parse(parser, event);

        if (event->type == YAML_MAPPING_END_EVENT)
            break;

        if (event->type != YAML_SCALAR_EVENT) {
            fprintf(stderr, "Error config file : Expected scalar key in 'env' mapping\n");
            return false ;
        }

        char* key = strdup((char*)event->data.scalar.value);
        if (!key)
            return false ;

        yaml_event_delete(event);
        yaml_parser_parse(parser, event);

        if (event->type != YAML_SCALAR_EVENT) {
            fprintf(stderr, "Error config file : Expected scalar value in env mapping\n");
            free(key);
            return false ;
        }
        
        char* value = strdup((char*)event->data.scalar.value);
        if (!value) {
            free(key);
            return false ;
        }

        t_env_p* tmp = realloc(envs, sizeof(t_env_p) * (count + 1));
        if (!tmp) {
            free(key);
            free(value);
            return false ;
        }
        envs = tmp;
        envs[count].key = key;
        envs[count].value = value;
        count++;

        yaml_event_delete(event);
    }

    cfg->env = envs;
    cfg->count_env = count;

    return true ; 
}

bool    parser_exitcodes(yaml_parser_t* parser, yaml_event_t* event, t_config* cfg) {
    if (event->type != YAML_SEQUENCE_START_EVENT) {
        fprintf(stderr, "Error config file : Expected a sequence for 'exitcodes'\n");
        return false;
    }

    int*    codes = NULL;
    size_t  code_count = 0;
    free(cfg->exitCodes.codes);

    while (1) {
        yaml_parser_parse(parser, event);

        if (event->type == YAML_SEQUENCE_END_EVENT) {
            break;
        }

        if (event->type != YAML_SCALAR_EVENT) {
            fprintf(stderr, "Error config file : Expected scalar in 'exitcodes' sequence\n");
            return false;
        }

        int code;
        if (!int_parser((char*)event->data.scalar.value, &code))
            return false ; 
        
        int *tmp = realloc(codes, sizeof(int) * (code_count + 1));
        if (!tmp) {
            free(codes);
            fprintf(stderr, "Memory error\n");
            return false;
        }
        codes = tmp;
        codes[code_count++] = code;

        yaml_event_delete(event);
    }
    cfg->exitCodes.codes = codes;
    cfg->exitCodes.count = code_count;
    
    return true ;
}

bool    parser_option_config_requis(t_process_para* para) {

    for (unsigned int i = 0; i < para->count; i++) {
        t_config*   conf = &para->config[i];
        if (!conf->cmd) {
            printf("Error config file : Required Paramater 'cmd'\n");
            return false ;
        }
    }
    return true ;
}
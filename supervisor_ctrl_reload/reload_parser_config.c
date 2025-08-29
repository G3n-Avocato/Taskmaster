# include "../include/supervisor.h"

bool    logger_parsing_name(t_process_para* para, char* val) {

    if (!logger_double_name_parsing(para, val))
        return false ;
        
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
    para->config[para->count - 1].cmd = NULL;
    para->config[para->count - 1].numProcs = 1;
    para->config[para->count - 1].umask = 022;
    para->config[para->count - 1].workingDir = strdup("/");
    if (!para->config[para->count - 1].workingDir)  {
        logger(CRIT, "Error parser config : allocation error (strdup)");
        return false ;
    }
    para->config[para->count - 1].autoStart = true ;
    para->config[para->count - 1].autoRestart = UNEXPECTED;
    logger_parser_exitcodes_no_sequence("0", &para->config[para->count - 1]);
    para->config[para->count - 1].startRetries = 3;
    para->config[para->count - 1].startTime = 1;
    logger_parser_stopsignal("TERM", &para->config[para->count - 1]);
    para->config[para->count - 1].stopTime = 10;
    para->config[para->count - 1].stdout = NULL;
    para->config[para->count - 1].stderr = NULL;
    para->config[para->count - 1].env = NULL;
    return true ;
}

bool    logger_double_name_parsing(t_process_para* para, char* val) {

    for (unsigned int i = 0; i < para->count; i++) {
        if (!strcmp(para->config[i].name, val)) {
            int size_buffer = strlen(val);
            char buffer[100 + size_buffer];
            snprintf(buffer, sizeof(buffer), "Error parser config : Duplicate group name \'%s\' in configuration file", val);
            logger(ERRO, buffer);
            return false;
        }
    }
    return true ;
}

bool    logger_parser_list_options_config(char *last_key, char *val, t_config* cfg) {

    if (!strcmp(last_key, "cmd")) {
        cfg->cmd = strdup(val);
        if (!cfg->cmd) {
            logger(CRIT, "Error parser config : allocation error (strdup)");
            return false ;
        }
    }
    else if (!strcmp(last_key, "numprocs"))
        return (logger_parser_numprocs(val, cfg));
    else if (!strcmp(last_key, "umask"))
        return (logger_parser_umask(val, cfg));
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
            logger(ERRO, "Error config file : Bad Paramater 'autostart'");
            return false ;
        }
    }
    else if (!strcmp(last_key, "autorestart"))
        return (logger_parser_autorestart(val, cfg));
    else if (!strcmp(last_key, "startretries"))
        return (logger_parser_startretries(val, cfg));
    else if (!strcmp(last_key, "exitcodes"))
        return (logger_parser_exitcodes_no_sequence(val, cfg));
    else if (!strcmp(last_key, "starttime"))
        return (logger_parser_starttime(val, cfg));
    else if (!strcmp(last_key, "stopsignal"))
        return (logger_parser_stopsignal(val, cfg));
    else if (!strcmp(last_key, "stoptime"))
        return (logger_parser_stoptime(val, cfg));
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
    else {
        if (strcmp(last_key, "env")) {
            int size_buffer = strlen(last_key);
            char buffer[50 + size_buffer];
            snprintf(buffer, sizeof(buffer), "Error config file : Parameter unknown '%s'", last_key);
            logger(ERRO, buffer);
            return false ;
        }
    }
    return true ;
}

bool    logger_parser_numprocs(char *val, t_config* cfg) {
    
    if (!int_parser(val, &cfg->numProcs)) {
        logger(ERRO, "Error config file : Bad Paramater 'numprocs'");
        return false ;
    }
    if (cfg->numProcs < 0) {
        logger(ERRO, "Error config file : Bad Paramater 'numprocs'");
        return false ;
    }
    return true ;
}

bool    logger_parser_umask(char *val, t_config* cfg) {
    
    if (!int_parser(val, &cfg->umask)) {
        logger(ERRO, "Error config file : Bad Paramater 'umask'");
        return false ;
    }
    if (cfg->umask < 0 || cfg->umask > 511) {
        logger(ERRO, "Error config file : Bad Paramater 'umask'");
        return false ;
    }
    return true ;
}

bool    logger_parser_autorestart(char *val, t_config* cfg) {
    char*   _tr = "true";
    char*   _fa = "false";
    char*   _un = "unexpected";
    
    if (!strcmp(val, _tr)) {
        cfg->autoRestart = TRUE;
        return true ;
    }
    else if (!strcmp(val, _fa)) {
        cfg->autoRestart = FALSE;
        return true ;
    }
    else if (!strcmp(val, _un)) {
        cfg->autoRestart = UNEXPECTED;
            return true ;
    }   
    else
        logger(ERRO, "Error config file : Bad Paramater 'autorestart'");
    return false ;
}

bool    logger_parser_startretries(char *val, t_config* cfg) {
    
    if (!int_parser(val, &cfg->startRetries)) {
        logger(ERRO, "Error config file : Bad Paramater 'startretries'");
        return false ;
    }
    if (cfg->startRetries < 0) {
        logger(ERRO, "Error config file : Bad Paramater 'startretries'");
        return false ;
    }
    return true ;
}

bool    logger_parser_exitcodes_no_sequence(char* val, t_config* cfg) {
    free(cfg->exitCodes.codes);

    int code;
    if (!int_parser(val, &code)) {
        logger(ERRO, "Error config file : Bad Paramater 'exitcodes'");
        return false ;
    }
    
    int* codes = malloc(sizeof(int));
    if (!codes) {
        logger(ERRO, "Error config file : Bad Paramater 'exitcodes'");
        return false ;
    }

    codes[0] = code;
    cfg->exitCodes.codes = codes;
    cfg->exitCodes.count = 1;

    return true ;
}

bool    logger_parser_starttime(char *val, t_config *cfg) {
    
    if (!int_parser(val, &cfg->startTime)) {
        logger(ERRO, "Error config file : Bad Paramater 'starttime'");
        return false ;
    }
    
    if (cfg->startTime < 0) {
        logger(ERRO, "Error config file : Bad Paramater 'starttime'");
        return false ;
    }

    return true ;
}

bool    logger_parser_stopsignal(char *val, t_config* cfg) {
    for (size_t i = 0; i < signal_map_size; i++) {
        if (!strcmp(signalMap[i].name, val)) {
            cfg->stopSignal = signalMap[i].value;
            return true ;
        }
    }
    logger(ERRO, "Error config file : Bad Paramater 'stopsignal'");
    return false ;
}

bool    logger_parser_stoptime(char *val, t_config *cfg) {
    
    if (!int_parser(val, &cfg->stopTime)) {
        logger(ERRO, "Error config file : Bad Paramater 'stoptime'");
        return false ;
    }
    
    if (cfg->stopTime < 0) {
        logger(ERRO, "Error config file : Bad Paramater 'stoptime'");
        return false ;
    }

    return true ;
}

bool    logger_parser_env(yaml_parser_t* parser, yaml_event_t* event, t_config* cfg) {
    if (event->type != YAML_MAPPING_START_EVENT) {
        logger(ERRO, "Error config file : Expected a mapping for 'env'");
        return false;
    }

    t_env_p*        envs = NULL;
    unsigned int    count = 0;

    while (1) {
        yaml_parser_parse(parser, event);

        if (event->type == YAML_MAPPING_END_EVENT)
            break;

        if (event->type != YAML_SCALAR_EVENT) {
            logger(ERRO, "Error config file : Expected scalar key in 'env' mapping");
            return false ;
        }

        char* key = strdup((char*)event->data.scalar.value);
        if (!key)
            return false ;

        yaml_event_delete(event);
        yaml_parser_parse(parser, event);

        if (event->type != YAML_SCALAR_EVENT) {
            logger(ERRO, "Error config file : Expected scalar value in env mapping");
            free(key);
            return false ;
        }
        
        char* value = strdup((char*)event->data.scalar.value);
        if (!value) {
            logger(CRIT, "Error parser config : allocation error (strdup)");
            free(key);
            return false ;
        }

        t_env_p* tmp = realloc(envs, sizeof(t_env_p) * (count + 1));
        if (!tmp) {
            logger(CRIT, "Error parser config : allocation error (realloc)");
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

bool    logger_parser_exitcodes(yaml_parser_t* parser, yaml_event_t* event, t_config* cfg) {
    if (event->type != YAML_SEQUENCE_START_EVENT) {
        logger(ERRO, "Error config file : Expected a sequence for 'exitcodes'");
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
            logger(ERRO, "Error config file : Expected scalar in 'exitcodes' sequence");
            return false;
        }

        int code;
        if (!int_parser((char*)event->data.scalar.value, &code)) {
            logger(ERRO, "Error config file : Bad Paramater 'exitcodes'");
            return false ; 
        }
        
        int *tmp = realloc(codes, sizeof(int) * (code_count + 1));
        if (!tmp) {
            logger(CRIT, "Error parser config : allocation error (realloc)");
            free(codes);
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

bool    logger_parser_option_config_requis(t_process_para* para) {

    for (unsigned int i = 0; i < para->count; i++) {
        t_config*   conf = &para->config[i];
        if (!conf->cmd) {
            logger(ERRO, "Error config file : Required Paramater 'cmd'");
            return false ;
        }
    }
    return true ;
}

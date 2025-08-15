# include "parser.h"

bool    parser_name_file(char **argv, int argc) {
    const char* tmp = strrchr(argv[1], '.');

    if (argc != 2 || !tmp)
        return false ;

    if (strcmp(tmp, ".yaml") == 0 || strcmp(tmp, ".yml") == 0)
        return true ;
    return false ;
}

bool    parser_file_yaml(char *file, t_process_para* procs) {

    FILE*   fd = fopen(file, "r");
    if (!fd) {
        perror("fopen");
        return false ;
    }

    yaml_parser_t   parser;
    yaml_event_t    event;

    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, fd);

    enum parsing_state state = ST_INIT;
    char*   last_key = NULL;
    t_config current = {0};

    procs->config = NULL;
    procs->count = 0;

    bool done = false;

    while (!done && yaml_parser_parse(&parser, &event)) {
        switch (event.type) {
            case YAML_SCALAR_EVENT:

                char* val = strdup((char *)event.data.scalar.value);

                if (state == ST_INIT && !strcmp(val, "programs")) {
                    state = ST_IN_PROGRAMS;
                    free(val);
                }
                else if (state == ST_IN_PROGRAMS) {
                    procs->count++;
                    procs->config = realloc(procs->config, sizeof(t_config) * procs->count);
                    current = (t_config){0};
                    procs->config[procs->count - 1] = current;
                    procs->config[procs->count - 1].name = strdup(val);
                    state = ST_IN_PROGRAM;
                    free(val);
                }
                else if (state == ST_IN_PROGRAM) {
                    last_key = strdup(val);
                    state = ST_IN_CONFIG;
                    free(val);
                }
                else if (state == ST_IN_CONFIG) {
                    
                    if (!last_key) {
                        last_key = strdup(val);
                        free(val);
                    }
                    else {
                        t_config*   cfg = &procs->config[procs->count - 1];
                        
                        if (!strcmp(last_key, "cmd"))
                            cfg->cmd = strdup(val);
                        else if (!strcmp(last_key, "numprocs")) {
                            if (!int_parser(val, &cfg->numProcs)) {
                                free(last_key);
                                return false ; // free ce qu'il y a free
                            }
                        }
                        else if (!strcmp(last_key, "umask")) {
                            if (!int_parser(val, &cfg->umask)) {
                                free(last_key);
                                return false ; // free ce qu'il y a free
                            }
                        } 
                        else if (!strcmp(last_key, "workingdir")) {
                            cfg->workingDir = strdup(val);
                        }
                        else if (!strcmp(last_key, "autostart")) {
                            if (!bool_parser(val, &cfg->autoStart)) {
                                free(last_key);
                                return false ;
                            }
                        } 
                        else if (!strcmp(last_key, "autorestart")) {
                            cfg->autoRestart = strdup(val);
                        }
                        else if (!strcmp(last_key, "startretries")) {
                            if (!int_parser(val, &cfg->startRetries)) {
                                free(last_key);
                                return false ; // free ce qu'il y a free
                            }
                        }
                        else if (!strcmp(last_key, "starttime")) {
                            if (!int_parser(val, &cfg->startTime)) {
                                free(last_key);
                                return false ; // free ce qu'il y a free
                            }
                        }
                        else if (!strcmp(last_key, "stopsignal")) {
                            cfg->stopSignal = strdup(val);
                        }
                        else if (!strcmp(last_key, "stoptime")) {
                            if (!int_parser(val, &cfg->stopTime)) {
                                free(last_key);
                                return false ; // free ce qu'il y a free
                            }
                        }
                        else if (!strcmp(last_key, "stdout")) {
                            cfg->stdout = strdup(val);
                        }
                        else if (!strcmp(last_key, "stderr")) {
                            cfg->stderr = strdup(val);
                        }
                        else if (!strcmp(last_key, "env")) {
                            printf("env a faire\n");
                        }
                        free(val);
                        free(last_key);
                        last_key = NULL;
                    }
                }
                break ;
            case YAML_SEQUENCE_START_EVENT:
                if (last_key && !strcmp(last_key, "exitcodes")) {
                t_config* cfg = &procs->config[procs->count - 1];
                if (!parsing_exitcodes(&parser, &event, cfg)) {
                    free(val);
                    free(last_key);
                    return false;
                }
                free(last_key);
                last_key = NULL;
                }
                break ;
            case YAML_MAPPING_END_EVENT:
                if (state == ST_IN_CONFIG)
                    state = ST_IN_PROGRAMS;
                break ; 
            case YAML_STREAM_END_EVENT:
                done = true;
                free(last_key);
                break ;
            default:
                break ;
        }
        yaml_event_delete(&event);
    }
    
    yaml_parser_delete(&parser);
    fclose(fd);

    return true ;
}


bool     int_parser(char* str, int *out) {
    char *end;
    errno = 0;

    long int val = strtol(str, &end, 10);

    if (errno == ERANGE || val > INT_MAX || val < INT_MIN) {
        free(str);
        return false ;
    }

    if (end == str || *end != '\0') {
        free(str);
        return false ;
    }

    *out = (int)val;
    return true ;
}

bool    bool_parser(char *str, bool* out) {
    char* _tr = "true";
    char* _fa = "false";

    if (!strncmp(str, _tr, 5)) {
        *out = true ;
        return true ;
    }
    else if (!strncmp(str, _fa, 6)) {
        *out = false ;
        return true ;
    }
    free(str);
    return false;
}

bool    parsing_exitcodes(yaml_parser_t* parser, yaml_event_t* event, t_config* cfg) {
    if (event->type != YAML_SEQUENCE_START_EVENT) {
        fprintf(stderr, "Expected a sequence for exitcodes\n");
        return false;
    }

    int*    codes = NULL;
    size_t  code_count = 0;

    while (1) {
        yaml_parser_parse(parser, event);

        if (event->type == YAML_SEQUENCE_END_EVENT) {
            break;
        }

        if (event->type != YAML_SCALAR_EVENT) {
            fprintf(stderr, "Expected scalar in exitcodes sequence\n");
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
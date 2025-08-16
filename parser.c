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

    procs->config = NULL;
    procs->count = 0;

    bool done = false;

    while (!done && yaml_parser_parse(&parser, &event)) {
        switch (event.type) {
            case YAML_SCALAR_EVENT:

                char* val = strdup((char *)event.data.scalar.value);
                if (!val) {
                    free_lib_yaml(&parser, &event, fd);
                    return false ;
                }

                if (state == ST_INIT) {
                    if (!strcmp(val, "programs"))
                        state = ST_IN_PROGRAMS;
                    else {
                        fprintf(stderr, "Expected 'programs' key\n");
                        free(val);
                        free_lib_yaml(&parser, &event, fd);
                        return false ;
                    }
                    free(val);
                }
                else if (state == ST_IN_PROGRAMS) {
                    if (!parsing_name(procs, val)) {
                        free_var_yaml(&val, &last_key);
                        free_lib_yaml(&parser, &event, fd);
                        return false ;
                    }
                    state = ST_IN_CONFIG;
                    free_var_yaml(&val, &last_key);
                }
                else if (state == ST_IN_CONFIG) {
                    if (!last_key) {
                        last_key = strdup(val);
                        if (!last_key) {
                            free_lib_yaml(&parser, &event, fd);
                            free(val);
                            return false ;
                        }
                        free(val);
                    }
                    else {
                        t_config*   cfg = &procs->config[procs->count - 1];
                        
                        if (!parser_list_options_config(last_key, val, cfg)) {
                            free_var_yaml(&val, &last_key);
                            free_lib_yaml(&parser, &event, fd);
                            return false ; 
                        }
                        free_var_yaml(&val, &last_key);
                    }
                }
                break ;
            case YAML_SEQUENCE_START_EVENT:
                if (last_key && !strcmp(last_key, "exitcodes")) {
                    t_config* cfg = &procs->config[procs->count - 1];
                    
                    if (!parser_exitcodes(&parser, &event, cfg)) {
                        free_var_yaml(&val, &last_key);
                        free_lib_yaml(&parser, &event, fd);
                        return false;
                    }
                    free(last_key);
                    last_key = NULL;
                }
                break ;
            case YAML_MAPPING_START_EVENT:
                if (last_key && !strcmp(last_key, "env")) {
                    t_config* cfg = &procs->config[procs->count - 1];

                    if (!parser_env(&parser, &event, cfg)) {
                        free_var_yaml(&val, &last_key);
                        free_lib_yaml(&parser, &event, fd);
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

    // fonction qui verifie pour chaque struct que cmd est present 

    return true ;
}


bool     int_parser(char* val, int *out) {
    char *end;
    errno = 0;

    long int res = strtol(val, &end, 10);

    if (errno == ERANGE || res > INT_MAX || res < INT_MIN)
        return false ;

    if (end == val || *end != '\0')
        return false ;

    *out = (int)res;
    return true ;
}

bool    bool_parser(char *val, bool* out) {
    char* _tr = "true";
    char* _fa = "false";

    if (!strncmp(val, _tr, 5)) {
        *out = true ;
        return true ;
    }
    else if (!strncmp(val, _fa, 6)) {
        *out = false ;
        return true ;
    }
    return false;
}

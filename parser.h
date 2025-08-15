#ifndef PARSER_H
# define PARSER_H

#include <stdio.h>
#include <string.h>
#include <yaml.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

enum StateRestart {
    True,
    False,
    Unexpected,
};

typedef struct env_p {
    char*           key;
    char*           value;
} t_env_p;

typedef struct exitcodes {
    int*            codes;
    unsigned int    count;
} t_exitcodes; 

typedef struct s_config {
    char*                               name;
    char*                               cmd;
    int                                 numProcs;
    int                                 umask;
    char*                               workingDir;
    bool                                autoStart;
    char*                               autoRestart;
    struct exitcodes                    exitCodes;
    int                                 startRetries;
    int                                 startTime;
    char*                               stopSignal;
    int                                 stopTime;
    char*                               stdout;
    char*                               stderr;
    t_env_p*                            env;
    unsigned int                        count_env;
} t_config;

typedef struct s_process_para {
    t_config*       config;
    unsigned int    count;
} t_process_para;

// YAML LIB
enum parsing_state {
    ST_INIT,
    ST_IN_PROGRAMS,
    ST_IN_PROGRAM,
    ST_IN_CONFIG,
    ST_KEY
};


// struct cyaml lib

//static const cyaml_config_t cyaml_cfg = {
//    .log_fn    = cyaml_log,
//    .mem_fn    = cyaml_mem,
//    .log_level = CYAML_LOG_WARNING,
//};
//
//static const cyaml_schema_field_t exitcodes_entry[] = {
//    CYAML_FIELD_INT("code", CYAML_FLAG_DEFAULT, struct exitcodes, codes),
//    CYAML_FIELD_END
//};
//
//static const cyaml_schema_value_t exitcode_schema = {
//    CYAML_VALUE_MAPPING(CYAML_FLAG_POINTER, struct exitcodes, exitcodes_entry),
//};
//
//static const cyaml_schema_field_t env_fields[] = {
//    CYAML_FIELD_STRING_PTR("key", CYAML_FLAG_DEFAULT, struct env_p, key),
//    CYAML_FIELD_STRING_PTR("value", CYAML_FLAG_DEFAULT, struct env_p, value),
//    CYAML_FIELD_END
//};
//
//static const cyaml_schema_value_t env_schema = {
//    CYAML_VALUE_MAPPING(CYAML_FLAG_POINTER, struct env_p, env_fields),
//};
//
//static const cyaml_schema_field_t config_fields[] = {
//    CYAML_FIELD_STRING_PTR("cmd", CYAML_FLAG_POINTER, struct s_config, cmd, 0, CYAML_UNLIMITED),
//    CYAML_FIELD_INT("numprocs", CYAML_FLAG_DEFAULT, struct s_config, numProcs),
//    CYAML_FIELD_INT("umask", CYAML_FLAG_DEFAULT, struct s_config, umask),
//    CYAML_FIELD_STRING_PTR("workingdir", CYAML_FLAG_POINTER, struct s_config, workingDir, 0, CYAML_UNLIMITED),
//    CYAML_FIELD_BOOL("autostart", CYAML_FLAG_DEFAULT, struct s_config, autoStart),
//    CYAML_FIELD_STRING_PTR("autorestart", CYAML_FLAG_POINTER, struct s_config, autoRestart, 0, CYAML_UNLIMITED),
//    CYAML_FIELD_SEQUENCE("exitcodes", CYAML_FLAG_POINTER, struct s_config, exitCodes, &exitcodes_schema, 0, CYAML_UNLIMITED),
//    CYAML_FIELD_INT("startretries", CYAML_FLAG_DEFAULT, struct s_config, startRetries),
//    CYAML_FIELD_INT("starttime", CYAML_FLAG_DEFAULT, struct s_config, startTime),
//    CYAML_FIELD_STRING_PTR("stopsignal", CYAML_FLAG_POINTER, struct s_config, stopSignal, 0, CYAML_UNLIMITED),
//    CYAML_FIELD_INT("stoptime", CYAML_FLAG_DEFAULT, struct s_config, stopTime),
//    CYAML_FIELD_STRING_PTR("stdout", CYAML_FLAG_POINTER, struct s_config, stdout, 0, CYAML_UNLIMITED),
//    CYAML_FIELD_STRING_PTR("stderr", CYAML_FLAG_POINTER, struct s_config, stderr, 0, CYAML_UNLIMITED),
//    CYAML_FIELD_SEQUENCE("env", CYAML_FLAG_POINTER, struct s_config, env, &env_schema, 0, CYAML_UNLIMITED),
//    CYAML_FIELD_END
//};
//
//static const cyaml_schema_value_t config_schema = {
//    CYAML_VALUE_MAPPING(CYAML_FLAG_POINTER, struct s_config, config_fields),
//};
//
//static const cyaml_schema_field_t process_para_fields[] = {
//    CYAML_FIELD_SEQUENCE("programs", CYAML_FLAG_POINTER, struct s_process_para, config, &config_schema, 0, CYAML_UNLIMITED),
//    CYAML_FIELD_END
//};
//
//static const cyaml_schema_value_t process_para_schema = {
//    CYAML_VALUE_MAPPING(CYAML_FLAG_POINTER, struct s_process_para, process_para_fields),
//};


///fonction declaration

bool    parser_name_file(char **argv, int argc);
bool    parser_file_yaml(char *file, t_process_para* procs);
bool    int_parser(char* str, int *out);
bool    bool_parser(char *str, bool* out);
bool    parsing_exitcodes(yaml_parser_t* parser, yaml_event_t* event, t_config* cfg);

void    free_process_para(t_process_para* procs);
void    free_config(t_config *cfg);

#endif
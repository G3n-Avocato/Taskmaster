#ifndef PARSER_H
# define PARSER_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <yaml.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>

// STRUCT MAIN CONFIG FILE OPTIONS 
typedef enum Restart_State {
    TRUE,
    FALSE,
    UNEXPECTED
} t_StateRestart;

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
    bool                                has_cmd;
    int                                 numProcs;
    int                                 umask;
    char*                               workingDir;
    bool                                autoStart;
    t_StateRestart                      autoRestart;
    struct exitcodes                    exitCodes;
    int                                 startRetries;
    int                                 startTime;
    int                                 stopSignal;
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

// STATE for pos in YAML file config
typedef enum parsing_state {
    ST_INIT,
    ST_IN_PROGRAMS,
    ST_IN_CONFIG,
    ST_KEY
} t_ParsingState;

// STRUCT const SIGNAL 
typedef struct s_signal_entry {
    const char *name;
    int         value;
} t_signal_entry;

static const t_signal_entry signalMap[] = {
    {"HUP", SIGHUP}, {"INT", SIGINT}, {"QUIT", SIGQUIT},
    {"ILL", SIGILL}, {"TRAP", SIGTRAP}, {"ABRT", SIGABRT},
    {"IOT", SIGIOT}, {"BUS", SIGBUS}, {"FPE", SIGFPE},
    {"KILL", SIGKILL}, {"USR1", SIGUSR1}, {"SEGV", SIGSEGV},
    {"USR2", SIGUSR2}, {"PIPE", SIGPIPE}, {"ALRM", SIGALRM},
    {"TERM", SIGTERM}, {"STKFLT", SIGSTKFLT}, {"CHLD", SIGCHLD},
    {"CONT", SIGCONT}, {"STOP", SIGSTOP}, {"TSTP", SIGTSTP},
    {"TTIN", SIGTTIN}, {"TTOU", SIGTTOU}, {"URG", SIGURG},
    {"XCPU", SIGXCPU}, {"XFSZ", SIGXFSZ}, {"VTALRM", SIGVTALRM},
    {"PROF", SIGPROF}, {"WINCH", SIGWINCH}, {"POLL", SIGPOLL},
    {"IO", SIGIO}, {"PWR", SIGPWR}, {"SYS", SIGSYS}

};

static const size_t signal_map_size = sizeof(signalMap) / sizeof(signalMap[0]);

///fonction declaration

// parser.c //
bool    parser_name_file(char **argv, int argc);
bool    parser_file_yaml(char *file, t_process_para* para);
bool    int_parser(char* str, int *out);
bool    bool_parser(char *str, bool* out);
bool    syntax_error_file_config(yaml_parser_t* parser, yaml_event_t* event, FILE* fd);

// parser_config.c //
bool    parsing_name(t_process_para* para, char* val);
bool    parser_list_options_config(char *last_key, char *val, t_config* cfg);
bool    parser_numprocs(char *val, t_config* cfg);
bool    parser_umask(char *val, t_config* cfg);
bool    parser_autorestart(char *val, t_config* cfg);
bool    parser_startretries(char *val, t_config* cfg);
bool    parser_exitcodes_no_sequence(char* val, t_config* cfg);
bool    parser_starttime(char *val, t_config *cfg);
bool    parser_stopsignal(char *val, t_config* cfg);
bool    parser_stoptime(char *val, t_config *cfg);
bool    parser_env(yaml_parser_t* parser, yaml_event_t* event, t_config* cfg);
bool    parser_exitcodes(yaml_parser_t* parser, yaml_event_t* event, t_config* cfg);
bool    parser_option_config_requis(t_process_para* para);


// free_parser.c //
void    free_process_para(t_process_para* para);
void    free_config(t_config *cfg);
void    free_var_yaml(char **val, char **last_key);
void    free_lib_yaml(yaml_parser_t* parser, yaml_event_t* event, FILE* fd);

#endif
#ifndef CTRL_RELOAD_CMD_H
# define CTRL_RELOAD_CMD_H

// reload_parser.c //
bool    logger_parser_name_file(char **argv, int argc);
bool    logger_parser_file_yaml(char *file, t_process_para* para);
bool    logger_syntax_error_file_config(yaml_parser_t* parser, yaml_event_t* event, FILE* fd);

// reload_parser_config.c //
bool    logger_parsing_name(t_process_para* para, char* val);
bool    logger_double_name_parsing(t_process_para* para, char* val);
bool    logger_parser_list_options_config(char *last_key, char *val, t_config* cfg);
bool    logger_parser_numprocs(char *val, t_config* cfg);
bool    logger_parser_umask(char *val, t_config* cfg);
bool    logger_parser_autorestart(char *val, t_config* cfg);
bool    logger_parser_startretries(char *val, t_config* cfg);
bool    logger_parser_exitcodes_no_sequence(char* val, t_config* cfg);
bool    logger_parser_starttime(char *val, t_config *cfg);
bool    logger_parser_stopsignal(char *val, t_config* cfg);
bool    logger_parser_stoptime(char *val, t_config *cfg);
bool    logger_parser_env(yaml_parser_t* parser, yaml_event_t* event, t_config* cfg);
bool    logger_parser_exitcodes(yaml_parser_t* parser, yaml_event_t* event, t_config* cfg);
bool    logger_parser_option_config_requis(t_process_para* para);

#endif
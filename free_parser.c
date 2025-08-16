#include "parser.h"

void    free_config(t_config *cfg) {
    if (cfg->name)
        free(cfg->name);
    if (cfg->cmd)
        free(cfg->cmd);
    if (cfg->workingDir)
        free(cfg->workingDir);
    //if (cfg->autoRestart)
    //    free(cfg->autoRestart);
    //if (cfg->stopSignal)
    //    free(cfg->stopSignal);
    if (cfg->stdout)
        free(cfg->stdout);
    if (cfg->stderr)
        free(cfg->stderr);

    if (cfg->exitCodes.codes)
        free(cfg->exitCodes.codes);

    for (size_t i = 0; i < cfg->count_env; i++) {
        if (cfg->env[i].key)
            free(cfg->env[i].key);
        if (cfg->env[i].value)
            free(cfg->env[i].value);
    }
    if (cfg->env)
        free(cfg->env);
}

void    free_process_para(t_process_para* procs) {
    for (size_t i = 0; i < procs->count; i++)
        free_config(&procs->config[i]);
    
    if (procs->config)
        free(procs->config);
    if (procs)
        free(procs);
}

void    free_var_yaml(char **val, char **last_key) {
    if (*val)
        free(*val);
    if (*last_key)
        free(*last_key);
    *last_key = NULL;
}

void    free_lib_yaml(yaml_parser_t* parser, yaml_event_t* event, FILE* fd) {

    yaml_event_delete(event);
    yaml_parser_delete(parser);
    fclose(fd);
}
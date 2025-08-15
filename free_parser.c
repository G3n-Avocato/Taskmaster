#include "parser.h"

void    free_config(t_config *cfg) {
    free(cfg->name);
    free(cfg->cmd);
    free(cfg->workingDir);
    free(cfg->autoRestart);
    free(cfg->stopSignal);
    free(cfg->stdout);
    free(cfg->stderr);

    free(cfg->exitCodes.codes);

    for (size_t i = 0; i < cfg->count_env; i++) {
        free(cfg->env[i].key);
        free(cfg->env[i].value);
    }
    free(cfg->env);
}

void    free_process_para(t_process_para* procs) {
    for (size_t i = 0; i < procs->count; i++)
        free_config(&procs->config[i]);
    free(procs->config);
    free(procs);
}
#include "parser.h"

// fct de test pour printf config 
const char*     printf_var(t_StateRestart res) {
    
    if (res == TRUE) {
        return "true";
    }
    else if (res == FALSE) {
        return "false";
    }
    else if (res == UNEXPECTED) {
        return "unexpected";
    }
    
    return "default";
}

const char*     get_signal_name(int val) {
    
    for (size_t i = 0; i < signal_map_size; i++) {
        
        if (signalMap[i].value == val) {
            return signalMap[i].name;
        }
    }
    
    return "UNKNOWN";
}

int main(int argc, char **argv) {

    if (!parser_name_file(argv, argc))
        return 1;

    t_process_para* procs = NULL;
    procs = malloc(sizeof(t_process_para) * 1);
    if (!procs) {
        printf("Error malloc\n");
        return 1;
    }
    if (!parser_file_yaml(argv[1], procs)) {
        free_process_para(procs);
        return 1;
    }

    // PRINT TEST CONFIG //
    for (unsigned int i = 0; i < procs->count; i++) {
        t_config*   conf = &procs->config[i];
        printf("\nProgram Name : %s\n cmd : %s\n", conf->name, conf->cmd);
        printf("numprocs : %d\numask : %d\n", conf->numProcs, conf->umask);
        printf("workingdir : %s\nautostart : %d\n", conf->workingDir, conf->autoStart);
        printf("autorestart : %s\n", printf_var(conf->autoRestart));
        
        t_exitcodes* cod = &conf->exitCodes;
        for (unsigned int j = 0; j < cod->count; j++)
            printf("exitcodes : %d\n", cod->codes[j]);

        printf("startRetries : %d\nstarttime : %d\n", conf->startRetries, conf->startTime);
        printf("stopsignal : %s\nstoptime : %d\n", get_signal_name(conf->stopSignal), conf->stopTime);
        printf("stdout : %s\nstderr : %s\n", conf->stdout, conf->stderr);

        t_env_p*    tmp = conf->env;
        for (unsigned int h = 0; h < conf->count_env; h++)
            printf("\tkey : %s\n\tvalue : %s\n", tmp[h].key, tmp[h].value);

    }


    free_process_para(procs);

    return 0;
}
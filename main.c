#include "supervisor.h"

int g_processCount = 0;
FILE* g_fdlog;

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

const char* enumtoString(ProcessStatus stat) {
    switch (stat) {
        case STOPPED: return "STOPPED";
        case STARTING: return "STARTING";
        case RUNNING: return "RUNNING";
        case BACKOFF: return "BACKOFF";
        case STOPPING: return "STOPPING";
        case EXITED: return "EXITED";
        case FATAL: return "FATAL";
        case UNKNOWN: return "UNKNOWN";
        default: return "default";
    }
}

int main(int argc, char **argv) {

    // BOOT START SUPERVISOR PARSING CONFIG 1ER + INIT STRUCT PARA 
    if (!parser_name_file(argv, argc))
        return 1;
    t_process_para* para = NULL;
    para = malloc(sizeof(t_process_para) * 1);
    if (!para) {
        fprintf(stderr, "Error parser : (malloc) %s\n", strerror(errno));
        return 1;
    }
    if (!parser_file_yaml(argv[1], para)) {
        free_process_para(para);
        return 1;
    }

    // OPEN LOGGER ONLY IF CONFIG FILE VALID 
    if (!open_logger_file("/tmp/Supervisor.log")) {
        free_process_para(para);
        return 1 ;
    }

    // A enlever PRINT TEST CONFIG /////////////////////////////////
    for (unsigned int i = 0; i < para->count; i++) {
        t_config*   conf = &para->config[i];
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
    ///////////////////////////////////////////////////////


    // START SUPERVISOR INIT
    t_superMap*     superMap = NULL;
    superMap = malloc(sizeof(t_superMap) * 1);
    if (!superMap) {
        fprintf(stderr, "Error init map : allocation error (malloc)\n");
        free_process_para(para);
        fclose(g_fdlog);
        return 1;
    }
    if (!init_supervisor_processMap(para, &superMap)) {
        free_process_para(para);
        free_supervisor(&superMap);
        fclose(g_fdlog);
        return 1 ;
    }

    //printf_processus(&superMap);

    // START SUPERVISOR BOOT
    if (!autostart_boot(&superMap, para)) {
        free_process_para(para);
        free_supervisor(&superMap);
        fclose(g_fdlog);
        return 1 ;
    }
    // MAIN LOOP -> ajouter boucle startretrie  
    if (!main_loop(&superMap, para)) {
        free_process_para(para);
        free_supervisor(&superMap);
        fclose(g_fdlog);
        return 1 ;
    }

    free_supervisor(&superMap);
    free_process_para(para);
    fclose(g_fdlog);

    end_supervisor_logger();

    return 0;
}
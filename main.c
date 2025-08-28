#include "supervisor.h"

// global main loop process
int                     g_processCount = 0;
FILE*                   g_fdlog;
volatile sig_atomic_t   sigchld_received = 0;
volatile sig_atomic_t   sighup_reload = 0;

// global ctrl
volatile sig_atomic_t   running = 1;
char                    cmd_buffer[MAX_CMD];
volatile sig_atomic_t   cmd_ready = 0;
char*                   history[MAX_HISTORY] = {0};
int                     histo_size = 0;
int                     histo_index = 0;
pthread_mutex_t         lock_read;

// Terminal settings pour lire caractère par caractère
struct termios orig_termios;


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

bool    start_supervisor_parsing_init_para(int argc, char **argv, t_process_para** para) {
    
    if (!parser_name_file(argv, argc))
        return false;

    *para = malloc(sizeof(t_process_para) * 2);
    if (!*para) {
        fprintf(stderr, "Error parser : (malloc) %s\n", strerror(errno));
        return false;
    }
    (*para)[0].file_name = argv;
    if (!init_para_null(&(*para)[1])) {
        free(*para);
        return false;
    }
    if (!parser_file_yaml(argv[1], &(*para)[0])) {
        free_process_para(*para);
        return false;
    }

    // OPEN LOGGER ONLY IF CONFIG FILE VALID 
    if (!open_logger_file("/tmp/Supervisor.log")) {
        free_process_para(*para);
        return false ;
    }

    return true ;
}

bool    sig_handler_init() {
    
    // SIGINT HANDLER
    if (!setup_sigint_handler()) {
        return false ;
    }
    // SIGHUP HANDLER
    if (!setup_sighup_handler()) {
        return false ;
    }
    // sigchild HANDLER
    if(!setup_sigchld_handler()) {
        return false ;
    }
    return true ;
}

int main(int argc, char **argv) {

    // BOOT START SUPERVISOR PARSING CONFIG 1ER + INIT STRUCT PARA 
    t_process_para* para = NULL;
    if (!start_supervisor_parsing_init_para(argc, argv, &para))
        return 1;
    start_supervisor_logger();
    
    // sig handler INIT
    if (!sig_handler_init()) {
        free_exit_para(para);
        return 1;
    }

    if (pthread_mutex_init(&lock_read, NULL) != 0) {
        printf("Échec de l'initialisation du mutex\n");
        free_exit_para(para);
        return 1;
    }

    // SUPERVISOR CTRL
    pthread_t   tid;
    t_ctrl_cmds ctrl;
    if (pthread_create(&tid, NULL, reader_thread, NULL) != 0) {
        perror("pthread_create");
        free_exit_para(para);
        pthread_mutex_destroy(&lock_read);
        return 1;
    }
    ctrl_supervisor_logger();

    // START SUPERVISOR INIT
    t_superMap*     superMap = NULL;
    superMap = malloc(sizeof(t_superMap) * 1);
    if (!superMap) {
        fprintf(stderr, "Error init map : allocation error (malloc)\n");
        free_exit_para(para);
        free_ctrl(tid);
        return 1;
    }
    if (!init_supervisor_processMap(&para[0], &superMap)) {
        free_exit_supervisor(&superMap, para, tid);
        return 1 ;
    }
    
    // START LOOP MAIN SUPERVISOR
    if (!main_loop(&superMap, para, &ctrl)) {
        free_exit_supervisor(&superMap, para, tid);
        return 1 ;
    }

    free_exit_supervisor(&superMap, para, tid);
    return 0;
}
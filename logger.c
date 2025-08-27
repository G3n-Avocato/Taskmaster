# include "supervisor.h"

bool    open_logger_file(const char* str) {

    g_fdlog = fopen(str, "a");
    if (!g_fdlog) {
        perror("fopen");
        return false ;
    }

    return true ;
}

bool    timestamp_logger() {
    struct timeval  tv;
    struct tm*      tm_info;
    char            buffer[64];

    gettimeofday(&tv, NULL);
    tm_info = localtime(&tv.tv_sec);

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(g_fdlog, "%s,%03ld", buffer, tv.tv_usec / 1000);
    fflush(g_fdlog);

    return true ;
}

bool    loglevel_logger(LogLevel nvl) {
    const char *buf;

    switch (nvl) {
        case CRIT: 
            buf = "CRIT";
            break ;
        case ERRO: 
            buf = "ERRO";
            break ;
        case WARN: 
            buf = "WARN";
            break ;
        case INFO: 
            buf = "INFO";
            break ;
        case DEBG: 
            buf = "DEBG";
            break ;
        case TRAC: 
            buf = "TRAC";
            break ;
        case BLAT: 
            buf = "BLAT";
            break ;
        default: buf = "INFO";
    }

    fprintf(g_fdlog, " %s ", buf);
    fflush(g_fdlog);

    return true ;
}

bool    message_logger(char* msg) {

    fprintf(g_fdlog, "%s\n", msg);
    fflush(g_fdlog);

    return true ;
}

bool    logger(LogLevel nvl, char* msg) {

    timestamp_logger();
    loglevel_logger(nvl);
    message_logger(msg);

    return true ;
}

bool    start_supervisor_logger() {

    char    buffer[100];
    pid_t   pid = getpid();
    snprintf(buffer, sizeof(buffer), "supervisor started with pid %d", pid);

    logger(INFO, buffer);

    return true ;
}

bool    end_supervisor_logger() {

    logger(INFO, "supervisor stopped");
    return true ;
}

bool    ctrl_supervisor_logger() {
    
    logger(INFO, "supervisorctrl initialized");
    return true ;
}

bool    start_process_logger(char *name, int id, pid_t pid) {

    int len = strlen(name);
    char buffer[50 + len];
    snprintf(buffer, sizeof(buffer), "spawned: '%s_%d' with pid %d", name, id, pid);
    logger(INFO, buffer);

    return true ;
}

bool    running_process_logger(char *name, int id, int t) {
    
    int len = strlen(name);
    char buffer[100 + len];
    snprintf(buffer, sizeof(buffer), "success: %s_%d entered RUNNING state, process has stayed up for > than %d seconds (startsecs)", name, id, t);
    logger(INFO, buffer);

    return true ;
}

const char*    signal_int_tostring(int sign) {

    for (size_t i = 0; i < signal_map_size; i++) {
        if (signalMap[i].value == sign)
            return signalMap[i].name;
    }
    return "none";
}


bool    stopped_process_signal_logger(char *name, int id, int sign) {
    
    int len = strlen(name);
    const char *signal = signal_int_tostring(sign);
    char buffer[50 + len];
    snprintf(buffer, sizeof(buffer), "stopped: %s_%d (terminated by SIG%s)", name, id, signal);
    logger(INFO, buffer);

    return true ;
}

bool    wait_stop_process_logger(char *name, int id) {
    
    int len = strlen(name);
    char buffer[50 + len];
    snprintf(buffer, sizeof(buffer), "waiting for %s_%d to die", name, id);
    logger(INFO, buffer);

    return true ;
}

bool    exit_expected_process_logger(char *name, int id, int exit) {

    int len = strlen(name);
    char buffer[50 + len];
    snprintf(buffer, sizeof(buffer), "exited: %s_%d (exit status %d; expected)", name, id, exit);
    logger(INFO, buffer);

    return true ;
}

bool    exit_not_expected_process_logger(char *name, int id, int exit) {

    int len = strlen(name);
    char buffer[50 + len];
    snprintf(buffer, sizeof(buffer), "exited: %s_%d (exit status %d; not expected)", name, id, exit);
    logger(INFO, buffer);

    return true ;
}

bool    config_change_logger(char *name) {
    
    int len = strlen(name);
    char buffer[50 + len];
    snprintf(buffer, sizeof(buffer), "found config changes to %s", name);
    logger(INFO, buffer);

    return true ;
}

bool    updating_process_logger(char *name) {
    
    int len = strlen(name);
    char buffer[50 + len];
    snprintf(buffer, sizeof(buffer), "updating process %s", name);
    logger(INFO, buffer);

    return true ;
}

bool    fatal_state_logger(char *name, int id) {

    int len = strlen(name);
    char buffer[100 + len];
    snprintf(buffer, sizeof(buffer), "gave up: %s_%d entered FATAL state, too many start retries too quickly", name, id);
    logger(INFO, buffer);

    return true ;
}

bool    fatal_logger(char *name, int id) {

    int len = strlen(name);
    char buffer[100 + len];
    snprintf(buffer, sizeof(buffer), "gave up: %s_%d entered FATAL state", name, id);
    logger(INFO, buffer);

    return true ;
}

bool    error_sigkill_logger(char *name, int id, int pid, char* msg) {

    int len = strlen(name);
    char buffer[100 + len];
    snprintf(buffer, sizeof(buffer), "killing '%s_%d' (pid %d) failed: %s", name, id, pid, msg);
    logger(WARN, buffer);

    return true ;
}

bool    error_kill_logger(char *name, int id, int pid, int sign, char* msg) {

    const char *signal = signal_int_tostring(sign);
    int len = strlen(name);
    char buffer[100 + len];
    snprintf(buffer, sizeof(buffer), "killing '%s_%d' (pid %d) with SIG%s failed: %s", name, id, pid, signal, msg);
    logger(WARN, buffer);

    return true ;
}

bool    error_fork_logger(char *cmd, char *msg) {

    char buffer[50];
    snprintf(buffer, sizeof(buffer), "spawnerr: can't exec %s: %s", cmd, msg);
    logger(INFO, buffer);

    return true ;
}

bool    error_chdir_child(char *path, char* msg, char *name, int id) {
    int len = strlen(path) + strlen(name);
    char buffer[100 + len];
    snprintf(buffer, sizeof(buffer), "spawn error: %s_%d  %s: %s", name, id, msg, path);
    logger(ERRO, buffer);

    return true ;
}
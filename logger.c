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

    return true ;
}

bool    message_logger(char* msg) {

    fprintf(g_fdlog, "%s\n", msg);

    return true ;
}

bool    logger(LogLevel nvl, char* msg) {

    timestamp_logger();
    loglevel_logger(nvl);
    message_logger(msg);

    return true ;
}

//bool    msg_build_logger(t_logger* log, char* msg, char* var) {
//
//    char*   buf;
//
//
//}
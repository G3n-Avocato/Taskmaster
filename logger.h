#ifndef LOGGER_H
# define LOGGER_H

#include <time.h>
#include <sys/time.h>

extern FILE* g_fdlog;

typedef enum Log_Level {
    CRIT, //erreur critique attention immediate 
    ERRO, //potentiel erreur mais ignorable 
    WARN, //condition anormal mais qui n'est pas une erreur
    INFO, // info normal 
    DEBG, //debug process config and process output state change event notif
    TRAC, //debug supervisor plugin, info about http, rpc request et response
    BLAT //debug supervisor lui meme
} LogLevel;


bool    open_logger_file(const char* str);
bool    timestamp_logger();
bool    loglevel_logger(LogLevel nvl);
bool    message_logger(char *msg);
bool    logger(LogLevel nvl, char* msg);

#endif
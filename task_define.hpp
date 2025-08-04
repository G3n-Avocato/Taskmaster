#ifndef TASK_DEFINE_HPP
# define TASK_DEFINE_HPP

#include <filesystem>
#include <iostream>
#include <map>
#include <vector>
#include <unistd.h>
#include <cstring>
#include <string>
#include <signal.h>
#include <exception>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <thread>
#include <mutex>


enum ProcessStatus {
    STOPPED,
    STARTING,
    RUNNING,
    BACKOFF,
    STOPPING,
    EXITED,
    FATAL,
    UNKNOWN,
};

enum StateRestart {
    True,
    False,
    Unexpected,
};

typedef struct s_config {
    std::string                         cmd;
    int                                 numProcs;
    mode_t                              umask;
    std::string                         workingDir;
    bool                                autoStart;
    StateRestart                        autoRestart;
    std::vector<int>                    exitCodes;
    int                                 startRetries;
    int                                 startTime;
    int                                 stopSignal;
    int                                 stopTime;
    std::string                         stdout;
    std::string                         stderr;
    std::map<std::string, std::string>  env;
} t_config;

# endif
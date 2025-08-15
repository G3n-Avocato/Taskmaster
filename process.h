#ifndef PROCESS_H
# define PROCESS_H

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


#endif
# include "process.hpp"

#include <sys/types.h>
#include <unistd.h>


Process::Process(void) {
    
    pid_t   pid = fork();

    if (pid == -1) {
        perror("fork");
        exit (1);
    }
    else if (pid > 0) {
        // parent process

        std::cout << "Parent process" << std::endl;
    }
    else if (pid == 0) {
        // child process
        if (execlp(cmd, cmd, nullptr)) {
            perror("execlp");
            exit(1);
        }
        std::cout << "Child Process PID: " << pid << std::endl;
    }

}

Process::~Process(void) {

}
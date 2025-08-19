# include "supervisor.h"

bool    startProcess(t_procs* proc, t_superMap** superMap, t_process_para* para) {

    open_file_std(proc->exec);
    proc->start_run = 0;
    proc->exit_code = false;

    proc->processus = fork();
    if (proc->processus == -1) {
        perror("fork");
        return false ;
    }
    else if (proc->processus == 0) {
        child_exec_proc(proc, superMap, para);
    }
    else if (proc->processus > 0) {
        parent_exec_proc(proc);
        return true ;
    }
    return false ;
}

void    parent_exec_proc(t_procs* proc) {
    if (proc->exec->fd_out != 1)
        close(proc->exec->fd_out);
    if (proc->exec->fd_err != 2)
        close(proc->exec->fd_err);

    proc->state = STARTING;
}

bool    waitpid_monitoring_status(t_procs* proc) {
    int status;

    pid_t   child_pid = waitpid(proc->processus, &status, WNOHANG);
    if (child_pid == -1) {
        proc->state = EXITED;
        perror("waitpid");
    }
    if (child_pid == 0) {
        if (proc->start_run == 0)
            proc->start_run = time(NULL);
    }
    else if (child_pid == proc->processus) {
        if (WIFEXITED(status)) {
            for (size_t i = 0; i < proc->config->exitCodes.count; i++) {
                if (WEXITSTATUS(status) == proc->config->exitCodes.codes[i])
                    proc->exit_code = true;
            }
        }
        proc->state = EXITED;
        //printf("1state waitpid %d = %s\n", proc->id, enumtoString(proc->state));
    }

   // printf("2state waitpid %d = %s\n", proc->id, enumtoString(proc->state));
    return true ;
}

void    child_exec_proc(t_procs* proc, t_superMap** superMap, t_process_para* para) {
    
    umask(proc->config->umask);
    if (chdir(proc->config->workingDir) < 0) {
        perror("chdir");
        exit(EXIT_FAILURE);
    }
    if (dup2(proc->exec->fd_out, STDOUT_FILENO) < 0) {
        perror("dup2 stdout");
        exit(EXIT_FAILURE);
    }
    if (dup2(proc->exec->fd_err, STDERR_FILENO) < 0) {
        perror("dup2 stderr");
        exit(EXIT_FAILURE);
    }

    if (proc->exec->fd_out != 1)
        close(proc->exec->fd_out);
    if (proc->exec->fd_err != 2)
        close(proc->exec->fd_err);

    if (execve(proc->exec->argv[0], proc->exec->argv, proc->exec->envp) == -1) {
        fprintf(stderr, "Execve Error\n");
        free_supervisor(superMap);
        free_process_para(para);
        _exit(EXIT_FAILURE);
    }
}

bool    isProcessUp(pid_t processus) {
    return processus > 0 && kill(processus, 0) == 0;
}

bool    stopProcess(t_procs* proc) {

    if (kill(proc->processus, proc->config->stopSignal) == -1) {
        perror("Error kill stop");
        return false ;
    }
    proc->state = STOPPING;
    sleep(proc->config->stopTime);

    if (isProcessUp(proc->processus)) {
        if (!killProcess(proc))
            return false ;
    }

    proc->state = STOPPED;
    return true ;
}

bool    killProcess(t_procs* proc) {

    if (kill(proc->processus, SIGKILL) == -1) {
            perror("Error SIGKILL");
            return false ;
    }

    return true ;
}
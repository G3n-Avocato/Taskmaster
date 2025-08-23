# include "supervisor.h"

bool    startProcess(t_procs* proc, t_superMap** superMap, t_process_para* para) {

    open_file_std(proc->exec);
    proc->start_run = 0;
    proc->exit_code = false;

    proc->processus = fork();
    if (proc->processus == -1) {
        error_fork_logger(proc->config->cmd, strerror(errno));
        return false ;
    }
    else if (proc->processus == 0) {
        child_exec_proc(proc, superMap, para);
    }
    else if (proc->processus > 0) {
        parent_exec_proc(proc);
        return true ;
    }
    return true ;
}

void    parent_exec_proc(t_procs* proc) {
    if (proc->exec->fd_out != 1)
        close(proc->exec->fd_out);
    if (proc->exec->fd_err != 2)
        close(proc->exec->fd_err);

    if (isProcessUp(proc->processus))
        proc->start_run = time(NULL);
    proc->state = STARTING;
    start_process_logger(proc->config->name, proc->id, proc->processus);
}

bool    waitpid_monitoring_status(t_superMap** superMap) {
    int status;
    pid_t   child_pid;
    
    while ((child_pid = waitpid(-1, &status, WNOHANG)) > 0) {

        for (int i = 0; i < g_processCount; i++) {

            if ((*superMap)[i].proc.processus == child_pid) {

                if (WIFEXITED(status)) {
                    
                    for (size_t j = 0; j < (*superMap)[i].proc.config->exitCodes.count; j++) {
                        if (WEXITSTATUS(status) == (*superMap)[i].proc.config->exitCodes.codes[j]) {
                            (*superMap)[i].proc.exit_code = true;
                            exit_expected_process_logger((*superMap)[i].name, (*superMap)[i].id - 1, (*superMap)[i].proc.config->exitCodes.codes[j]);
                            break ;
                        }
                    }
                    if (!(*superMap)[i].proc.exit_code)
                        exit_not_expected_process_logger((*superMap)[i].name, (*superMap)[i].id - 1, WEXITSTATUS(status));
                }
                //if (WIFSIGNALED(status)) {
                //    ;
                //}
                (*superMap)[i].proc.state = EXITED;
                break ;
            }
        }
    }

    return true ;
}

void    child_exec_proc(t_procs* proc, t_superMap** superMap, t_process_para* para) {
    fclose(g_fdlog);
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
        //fclose(g_fdlog);
        _exit(EXIT_FAILURE);
    }
}

bool    isProcessUp(pid_t processus) {
    if (processus > 0 && kill(processus, 0) == 0)
            return true ;
    return false ;
}

bool    stopProcess(t_procs* proc) {

    if (kill(proc->processus, proc->config->stopSignal) == -1) {
        error_kill_logger(proc->config->name, proc->id, proc->processus, proc->config->stopSignal, strerror(errno));
        return false ;
    }
    proc->state = STOPPING;
    wait_stop_process_logger(proc->config->name, proc->id);
    
    sleep(proc->config->stopTime);

    if (isProcessUp(proc->processus)) {
        if (!killProcess(proc))
            return false ;
    }
    else {
        printf("--------------> stopsignal = %s\n", signal_int_tostring(proc->config->stopSignal));
        stopped_process_signal_logger(proc->config->name, proc->id, proc->config->stopSignal);
    }

    proc->state = STOPPED;

    return true ;
}

bool    killProcess(t_procs* proc) {

    if (kill(proc->processus, SIGKILL) == -1) {
            error_sigkill_logger(proc->config->name, proc->id, proc->processus, strerror(errno));
            return false ;
    }
    stopped_process_signal_logger(proc->config->name, proc->id, SIGKILL);

    return true ;
}
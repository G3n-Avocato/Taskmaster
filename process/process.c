# include "../include/supervisor.h"

static const char* logfile_path_out = NULL;
static const char* logfile_path_err = NULL;

bool    startProcess(t_procs* proc, t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl) {

    open_file_std(proc->exec);
    proc->start_run = 0;
    proc->exit_code = false;

    proc->processus = fork();
    if (proc->processus == -1) {
        error_fork_logger(proc->config->cmd, strerror(errno));
        return false ;
    }
    else if (proc->processus == 0) {
        child_exec_proc(proc, superMap, para, ctrl);
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

void    reopen_logout_child(int sig) {
    (void)sig;
    if (!logfile_path_out)
        return ;
    int fd = open(logfile_path_out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
}

void    reopen_logerr_child(int sig) {
    (void)sig;
    if (!logfile_path_err)
        return ;
    int fd = open(logfile_path_err, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        dup2(fd, STDERR_FILENO);
        close(fd);
    }
}

void    child_exec_proc(t_procs* proc, t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl) {
    logfile_path_out = proc->exec->stdout;
    signal(SIG_ROTATE_OUT, reopen_logout_child);
    logfile_path_err = proc->exec->stderr;
    signal(SIG_ROTATE_ERR, reopen_logerr_child);

    umask(proc->config->umask);
    if (chdir(proc->config->workingDir) < 0) {
        error_chdir_child(proc->config->workingDir, strerror(errno), proc->config->name, proc->id);
        child_close_fd(proc);
        child_exit_error_execve(superMap, para, ctrl);
        exit(EXIT_FAILURE);
    }
    if (dup2(proc->exec->fd_out, STDOUT_FILENO) < 0) {
        error_chdir_child(proc->exec->stdout, strerror(errno), proc->config->name, proc->id);
        child_close_fd(proc);
        child_exit_error_execve(superMap, para, ctrl);
        exit(EXIT_FAILURE);
    }
    if (dup2(proc->exec->fd_err, STDERR_FILENO) < 0) {
        error_chdir_child(proc->exec->stderr, strerror(errno), proc->config->name, proc->id);
        child_close_fd(proc);
        child_exit_error_execve(superMap, para, ctrl);
        exit(EXIT_FAILURE);
    }
    child_close_fd(proc);

    if (execve(proc->exec->argv[0], proc->exec->argv, proc->exec->envp) == -1) {
        fprintf(stderr, "Error execve: %s\n", strerror(errno));
        child_exit_error_execve(superMap, para, ctrl);
        _exit(EXIT_FAILURE);
    }
}

void    child_exit_error_execve(t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl) {
    free_supervisor(superMap);
    free_process_para(para);
    
    for (int i = 0; i < histo_size; i++)
        free(history[i]);
    
    if (ctrl->split_cmd) {
        for (int i = 0; i < ctrl->tab_len; i++) {
            if (ctrl->split_cmd[i]) {
                free(ctrl->split_cmd[i]);
                ctrl->split_cmd[i] = NULL;
            }
        }
        free(ctrl->split_cmd);
        ctrl->split_cmd = NULL;
    }
    if (ctrl->name) {
        free(ctrl->name);
        ctrl->name = NULL;
    }
    if (ctrl->group) {
        free(ctrl->group);
        ctrl->group = NULL;
    }
}

void    child_close_fd(t_procs* proc) {
    fclose(g_fdlog);

    if (proc->exec->fd_out != 1)
        close(proc->exec->fd_out);
    if (proc->exec->fd_err != 2)
        close(proc->exec->fd_err);
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
    else
        stopped_process_signal_logger(proc->config->name, proc->id, proc->config->stopSignal);

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
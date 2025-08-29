# include "../include/supervisor.h"

char* build_old_stdFile(const char* path) {

    char*   old = ".old";
    size_t len = strlen(path) + strlen(old);

    char *tmp = malloc(sizeof(char) * (len + 1));
    if (!tmp) {
        logger(CRIT, "Error rotate log: allocation error (malloc)");
        return NULL;
    }
    strcpy(tmp, path);
    strcpy(tmp + strlen(path), old);
    
    return tmp;
}

bool    send_sig_rotate_out(pid_t pid) {
    if (kill(pid, SIG_ROTATE_OUT) < 0) {
        logger(CRIT, "Error rotate log: send signal rotate for stdout");
        return false ;
    }
    return true ;
}

bool    send_sig_rotate_err(pid_t pid) {
    if (kill(pid, SIG_ROTATE_ERR) < 0) {
        logger(CRIT, "Error rotate log: send signal rotate for stderr");
        return false ;
    }
    return true ;
}

bool    rotate_log(const char* path, pid_t pid, bool std) {
    char* old = build_old_stdFile(path);
    if (!old)
        return false ;
    if (rename(path, old) < 0) {
        logger(CRIT, "Error rotate log: rename file");
        free(old);
        return false ;
    }
    free(old);

    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        logger(CRIT, "Error rotate log: open file");
        return false ;
    }
    close(fd);

    if (std == true) {
        if (!send_sig_rotate_out(pid))
            return false ;
    }
    else {
        if (!send_sig_rotate_err(pid))
            return false ;
    }

    return true ;
}

off_t   get_file_size(const char *file) {
    struct stat st;

    if (stat(file, &st) == 0) {
        return st.st_size;
    }
    return -1;
}

void    monitor_log_file(const char *path, pid_t pid, bool std) {
    off_t size = get_file_size(path);
    if (size > MAX_SIZE_STD_FILE) {
        rotate_log(path, pid, std);
    }
}
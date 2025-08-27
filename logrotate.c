# include "supervisor.h"

off_t   get_file_size(const char *file) {
    struct stat st;

    if (stat(file, &st) == 0) {
        return st.st_size;
    }
    return -1;
}

bool     copy_file(const char *path, const char *new) {
    int src_fd = open(path, O_RDONLY);
    if (src_fd < 0)
        return false ;
    int dst_fd = open(new, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd < 0) {
        close(src_fd);
        return false ;
    }
    struct stat st;

    if (fstat(src_fd, &st) < 0) {
        close(src_fd);
        close(dst_fd);
        return false ;
    }

    sendfile(dst_fd, src_fd, NULL, st.st_size);

    close(src_fd);
    close(dst_fd);
    return true ;
}

int     truncate_file(const char *path) {
    if (truncate(path, 0) < 0) {
        return false ;
    }
    return true ;
}

char* build_old_stdFile(const char* path) {

    char*   old = ".old";
    size_t len = strlen(path) + strlen(old);

    char *tmp = malloc(sizeof(char) * len);
    if (!tmp)
        return NULL;
    strcpy(tmp, path);
    strcpy(tmp + strlen(path), old);
    
    return tmp;
}

void    monitor_log_file(const char *path) {
    off_t size = get_file_size(path);
    if (size > MAX_SIZE_STD_FILE) {
        char *new = build_old_stdFile(path);
        if (new) {
            copy_file(path, new);
            free(new);
        }
        truncate_file(path);
    }
}
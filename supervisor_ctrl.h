#ifndef SUPERVISOR_CTRL_H
# define SUPERVISOR_CTRL_H

# include <stddef.h>
# include <strings.h>
# include <pthread.h>
# include <termios.h>

//#define _POSIX_C_SOURCE 200112L
#define MAX_CMD 256
#define MAX_HISTORY 100

extern volatile sig_atomic_t    running;
extern char                     cmd_buffer[MAX_CMD];
extern volatile sig_atomic_t    cmd_ready;
extern char*                    history[MAX_HISTORY];
extern int                      histo_size;
extern int                      histo_index;

extern struct termios orig_termios;

// supervisor_ctrl.c
void    disable_raw_mode();
void    enable_raw_mode();
void    add_history(const char *cmd);
void    call_start_cmd(char **cmd);
void    call_stop_cmd(char **cmd);
void    call_restart_cmd(char **cmd);
void    call_reload_cmd(char **cmd);
void    call_status_cmd(char **cmd);
void    process_command(const char *cmd);
void*   reader_thread(void *arg);

// supervisor_ctrl_utils.c
void    handle_sigint(int sig);
bool	setup_sigint_handler();
char*   ft_substr(char const *s, unsigned int start, size_t len);
int     ft_pointer_tab_len(char **ptr);
void    print_stringss(char **list);
char**  split(const char* str, char sep);

// free_process.c
void    free_ctrl(pthread_t tid);

#endif
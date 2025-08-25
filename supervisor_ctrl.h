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
void    call_start_cmd(char **cmd, t_superMap** superMap, t_process_para* para);
void    call_stop_cmd(char **cmd, t_superMap** superMap);
void    call_restart_cmd(char **cmd, t_superMap** superMap, t_process_para* para);
void    call_reload_cmd(char **cmd);
void    call_status_cmd(char **cmd, t_superMap** superMap);
void    process_command(const char *cmd, t_superMap** superMap, t_process_para* para);
void*   reader_thread(void *arg);

// supervisor_ctrl_utils.c
void        handle_sigint(int sig);
bool	    setup_sigint_handler();
const char* ProcessStatus_toString(ProcessStatus stat);
char*       ft_substr(char const *s, unsigned int start, size_t len);
int         ft_pointer_tab_len(char **ptr);
void        print_stringss(char **list);
char**      split(const char* str, char sep);

// supervisor_ctrl_cmds.c
bool    find_name_proc_start(t_superMap** superMap, t_process_para* para, char* group, char* name);
bool    find_group_proc_start(t_superMap** superMap, t_process_para* para, char* group);
bool    find_all_proc_start(t_superMap** superMap, t_process_para* para);
bool    start_cmd(t_procs* proc, t_superMap** superMap, t_process_para* para);

bool    find_name_proc_restart(t_superMap** superMap, t_process_para* para, char* group, char* name);
bool    find_group_proc_restart(t_superMap** superMap, t_process_para* para, char* group);
bool    find_all_proc_restart(t_superMap** superMap, t_process_para* para);
bool    restart_cmd(t_procs* proc, t_superMap** superMap, t_process_para* para);

bool    find_name_proc_stop(t_superMap** superMap, char* group, char* name);
bool    find_group_proc_stop(t_superMap** superMap, char* group);
bool    find_all_proc_stop(t_superMap** superMap);
bool    stop_cmd(t_procs* proc);

bool    find_name_proc_status(t_superMap** superMap, char* group, char* name);
bool    find_group_proc_status(t_superMap** superMap, char* group);
bool    find_all_proc_status(t_superMap** superMap);
bool    status_cmd(t_procs* proc);

// free_process.c
void    free_ctrl(pthread_t tid);

#endif
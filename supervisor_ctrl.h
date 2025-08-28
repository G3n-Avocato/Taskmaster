#ifndef SUPERVISOR_CTRL_H
# define SUPERVISOR_CTRL_H

# include <stddef.h>
# include <strings.h>
# include <pthread.h>
# include <termios.h>
# include <stdatomic.h>

//#define _POSIX_C_SOURCE 200112L
#define MAX_CMD 256
#define MAX_HISTORY 100

extern volatile sig_atomic_t    running;
extern char                     cmd_buffer[MAX_CMD];
extern volatile sig_atomic_t    cmd_ready;
extern char*                    history[MAX_HISTORY];
extern int                      histo_size;
extern int                      histo_index;
extern int                      histo_index;
extern pthread_mutex_t          lock_read;

extern struct termios orig_termios;

typedef struct ctrl_cmds {
    char**  split_cmd;
    int     tab_len;
    char*   name;
    char*   group;
} t_ctrl_cmds;

// supervisor_ctrl.c
void    disable_raw_mode();
void    enable_raw_mode();
void    add_history(const char *cmd);
void    call_start_cmd(t_ctrl_cmds* ctrl, t_superMap** superMap, t_process_para* para);
void    call_stop_cmd(t_ctrl_cmds* ctrl, t_superMap** superMap);
void    call_restart_cmd(t_ctrl_cmds* ctrl, t_superMap** superMap, t_process_para* para);
void    call_status_cmd(t_ctrl_cmds* ctrl, t_superMap** superMap);
void    process_command(const char *cmd, t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl);
void*   reader_thread(void *arg);

// supervisor_ctrl_utils.c
const char* ProcessStatus_toString(ProcessStatus stat);
char*       ft_substr(char const *s, unsigned int start, size_t len);
int         ft_pointer_tab_len(char **ptr);
void        print_stringss(char **list);
char**      split(const char* str, char sep);

// supervisor_ctrl_cmds.c
bool    find_name_proc_start(t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl);
bool    find_group_proc_start(t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl);
bool    find_all_proc_start(t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl);
bool    start_cmd(t_procs* proc, t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl);

bool    find_name_proc_restart(t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl);
bool    find_group_proc_restart(t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl);
bool    find_all_proc_restart(t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl);
bool    restart_cmd(t_procs* proc, t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl);

bool    find_name_proc_stop(t_superMap** superMap, t_ctrl_cmds* ctrl);
bool    find_group_proc_stop(t_superMap** superMap, t_ctrl_cmds* ctrl);
bool    find_all_proc_stop(t_superMap** superMap);
bool    stop_cmd(t_procs* proc);

bool    find_name_proc_status(t_superMap** superMap, t_ctrl_cmds* ctrl);
bool    find_group_proc_status(t_superMap** superMap, t_ctrl_cmds* ctrl);
bool    find_all_proc_status(t_superMap** superMap);
bool    status_cmd(t_procs* proc);

bool    r_init_clear_var(t_procs* proc);

// supervisor_ctrl_reload.c

bool    reload_parse_configFile(t_process_para* para);
bool    reload_cmd(t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl);
bool    comp_configFile_for_delete_config(t_superMap** superMap, t_process_para* para, bool* change);
bool    comp_configFile(t_superMap** superMap, t_process_para* para, bool *reload, bool* change, t_ctrl_cmds* ctrl);

bool    r_init_configStruct(t_process_para* old, t_process_para* new, int i, int j);
bool    start_proc_superMap(t_superMap** superMap, char *name, t_process_para* para, t_ctrl_cmds* ctrl);
bool    stop_proc_superMap(t_superMap** superMap, char* name);
bool    r_init_proc_superMap(t_superMap** superMap, char* name, t_config* conf);
bool    comp_configStruct(t_process_para* old, t_process_para* new, int i, int j, bool *reload);

bool    comp_configStruct_exitcode(t_config* old, t_config* new);
bool    swap_exitCodes_struct_in_para(t_process_para* old, t_process_para* new, int i, int j);
bool    comp_configStruct_env(t_config* old, t_config* new);
bool    swap_env_struct_in_para(t_process_para* old, t_process_para* new, int i, int j);

void    free_paraNew_reload_error(t_process_para *para);

// supervisor_ctrl_struct_reload.c
bool    init_proc_superMap_reload(t_superMap** superMap, char *name, t_config* conf);
void    free_superMap_case(t_superMap** superMap, int i);
bool    init_new_para_struct_reload(t_process_para* old, t_process_para* new, int j);
bool    delete_old_para_struct_reload(t_process_para* old, int i);

// free_process.c
void    free_ctrl(pthread_t tid);

#endif
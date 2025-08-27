# include "supervisor.h"

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON); // désactiver echo et mode canonique
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// Ajout d'une commande dans l'historique
void add_history(const char *cmd) {
    if (histo_size < MAX_HISTORY) {
        history[histo_size++] = strdup(cmd);
    } else {
        free(history[0]);
        memmove(history, history + 1, sizeof(char*) * (MAX_HISTORY - 1));
        history[MAX_HISTORY - 1] = strdup(cmd);
    }
}

void call_start_cmd(t_ctrl_cmds* ctrl, t_superMap** superMap, t_process_para* para)
{
    if (!ctrl->split_cmd || ft_pointer_tab_len(ctrl->split_cmd) < 2)
        return;
    if (ft_pointer_tab_len(ctrl->split_cmd) == 2 && strcmp(ctrl->split_cmd[1],"all") == 0)
    {
        find_all_proc_start(superMap, para, ctrl);
    }
    else
    {
        int i = 1;
        while (ctrl->split_cmd[i])
        {
            int j = 0;
            int cursor = 0;
            ctrl->group = NULL;
            ctrl->name = NULL;
            char *arg = ctrl->split_cmd[i];
            if (arg[0] == ':')
            {
                printf("→ Start Error name arg : %s\n",ctrl->split_cmd[i]);
            }
            else
            {
                while (arg[j])
                {
                    if (arg[j] == ':')
                    {
                        if (ctrl->group)
                        {
                            free(ctrl->group);
                            ctrl->group = NULL;
                            printf("→ Start Error name arg : %s\n",ctrl->split_cmd[i]);
                            break;
                        }
                        ctrl->group = ft_substr(ctrl->split_cmd[i], cursor, j-cursor);
                        cursor = j+1;
                    }
                    j++;
                }
                if (ctrl->group && cursor != j)
                {
                    ctrl->name = ft_substr(ctrl->split_cmd[i], cursor, j-cursor);
                }
                if (ctrl->name)
                {
                    find_name_proc_start(superMap, para, ctrl);
                    free(ctrl->group);
                    ctrl->group = NULL;
                    free(ctrl->name);
                    ctrl->name = NULL;
                }
                else if (ctrl->group)
                {
                    find_group_proc_start(superMap, para, ctrl);
                    free(ctrl->group);
                    ctrl->group = NULL;
                }
            }
            i++;
        }
    }
}

void call_stop_cmd(t_ctrl_cmds* ctrl, t_superMap** superMap)
{
    if (!ctrl->split_cmd || ft_pointer_tab_len(ctrl->split_cmd) < 2)
        return;
    if (ft_pointer_tab_len(ctrl->split_cmd) == 2 && strcmp(ctrl->split_cmd[1],"all") == 0)
    {
        find_all_proc_stop(superMap);
    }
    else
    {
        int i = 1;
        while (ctrl->split_cmd[i])
        {
            int j = 0;
            int cursor = 0;
            ctrl->group = NULL;
            ctrl->name = NULL;
            char *arg = ctrl->split_cmd[i];
            if (arg[0] == ':')
            {
                printf("→ Stop Error name arg : %s\n",ctrl->split_cmd[i]);
            }
            else
            {
                while (arg[j])
                {
                    if (arg[j] == ':')
                    {
                        if (ctrl->group)
                        {
                            free(ctrl->group);
                            ctrl->group = NULL;
                            printf("→ Stop Error name arg : %s\n",ctrl->split_cmd[i]);
                            break;
                        }
                        ctrl->group = ft_substr(ctrl->split_cmd[i], cursor, j-cursor);
                        cursor = j+1;
                    }
                    j++;
                }
                if (ctrl->group && cursor != j)
                {
                    ctrl->name = ft_substr(ctrl->split_cmd[i], cursor, j-cursor);
                }
                if (ctrl->name)
                {
                    find_name_proc_stop(superMap, ctrl);
                    free(ctrl->group);
                    ctrl->group = NULL;
                    free(ctrl->name);
                    ctrl->name = NULL;
                }
                else if (ctrl->group)
                {
                    find_group_proc_stop(superMap, ctrl);
                    free(ctrl->group);
                    ctrl->group = NULL;
                }
            }
            i++;
        }
    }
}

void call_restart_cmd(t_ctrl_cmds* ctrl, t_superMap** superMap, t_process_para* para)
{
    if (!ctrl->split_cmd || ft_pointer_tab_len(ctrl->split_cmd) < 2)
        return;
    if (ft_pointer_tab_len(ctrl->split_cmd) == 2 && strcmp(ctrl->split_cmd[1],"all") == 0)
    {
        find_all_proc_restart(superMap, para, ctrl);
    }
    else
    {
        int i = 1;
        while (ctrl->split_cmd[i])
        {
            int j = 0;
            int cursor = 0;
            ctrl->group = NULL;
            ctrl->name = NULL;
            char *arg = ctrl->split_cmd[i];
            if (arg[0] == ':')
            {
                printf("→ Restart Error name arg : %s\n",ctrl->split_cmd[i]);
            }
            else
            {
                while (arg[j])
                {
                    if (arg[j] == ':')
                    {
                        if (ctrl->group)
                        {
                            free(ctrl->group);
                            ctrl->group = NULL;
                            printf("→ Restart Error name arg : %s\n",ctrl->split_cmd[i]);
                            break;
                        }
                        ctrl->group = ft_substr(ctrl->split_cmd[i], cursor, j-cursor);
                        cursor = j+1;
                    }
                    j++;
                }
                if (ctrl->group && cursor != j)
                {
                    ctrl->name = ft_substr(ctrl->split_cmd[i], cursor, j-cursor);
                }
                if (ctrl->name)
                {
                    find_name_proc_restart(superMap, para, ctrl);
                    free(ctrl->group);
                    ctrl->group = NULL;
                    free(ctrl->name);
                    ctrl->name = NULL;
                }
                else if (ctrl->group)
                {
                    find_group_proc_restart(superMap, para, ctrl);
                    free(ctrl->group);
                    ctrl->group = NULL;
                }
            }
            i++;
        }
    }
}

void call_reload_cmd(t_ctrl_cmds* ctrl, t_superMap** superMap, t_process_para* para)
{
    if (!ctrl->split_cmd || ft_pointer_tab_len(ctrl->split_cmd) < 2)
        return;
    if (ft_pointer_tab_len(ctrl->split_cmd) == 2 && strcmp(ctrl->split_cmd[1],"all") == 0)
    {
        reload_cmd(superMap, para, ctrl);
    }
    else
    {
        int i = 1;
        while (ctrl->split_cmd[i])
        {
            int j = 0;
            int cursor = 0;
            ctrl->group = NULL;
            ctrl->name = NULL;
            char *arg = ctrl->split_cmd[i];
            if (arg[0] == ':')
            {
                printf("→ Reload Error name arg : %s\n",ctrl->split_cmd[i]);
            }
            else
            {
                while (arg[j])
                {
                    if (arg[j] == ':')
                    {
                        if (ctrl->group)
                        {
                            free(ctrl->group);
                            ctrl->group = NULL;
                            printf("→ Reload Error name arg : %s\n",ctrl->split_cmd[i]);
                            break;
                        }
                        ctrl->group = ft_substr(ctrl->split_cmd[i], cursor, j-cursor);
                        cursor = j+1;
                    }
                    j++;
                }
                if (ctrl->group && cursor != j)
                {
                    ctrl->name = ft_substr(ctrl->split_cmd[i], cursor, j-cursor);
                }
                if (ctrl->name)
                {
                    //printf("→ Reload Name %s:%s !\n",group,name);
                    free(ctrl->group);
                    ctrl->group = NULL;
                    free(ctrl->name);
                    ctrl->name = NULL;
                }
                else if (ctrl->group)
                {
                    //printf("→ Reload Group %s: !\n",group);
                    free(ctrl->group);
                    ctrl->group = NULL;
                }
            }
            i++;
        }
    }
}

void call_status_cmd(t_ctrl_cmds* ctrl, t_superMap** superMap)
{
    if (!ctrl->split_cmd || ft_pointer_tab_len(ctrl->split_cmd) < 2)
        return;
    if (ft_pointer_tab_len(ctrl->split_cmd) == 2 && strcmp(ctrl->split_cmd[1],"all") == 0)
    {
        find_all_proc_status(superMap);
    }
    else
    {
        int i = 1;
        while (ctrl->split_cmd[i])
        {
            int j = 0;
            int cursor = 0;
            ctrl->group = NULL;
            ctrl->name = NULL;
            char *arg = ctrl->split_cmd[i];
            if (arg[0] == ':')
            {
                printf("→ Status Error name arg : %s\n",ctrl->split_cmd[i]);
            }
            else
            {
                while (arg[j])
                {
                    if (arg[j] == ':')
                    {
                        if (ctrl->group)
                        {
                            free(ctrl->group);
                            ctrl->group = NULL;
                            printf("→ Status Error name arg : %s\n",ctrl->split_cmd[i]);
                            break;
                        }
                        ctrl->group = ft_substr(ctrl->split_cmd[i], cursor, j-cursor);
                        cursor = j+1;
                    }
                    j++;
                }
                if (ctrl->group && cursor != j)
                {
                    ctrl->name = ft_substr(ctrl->split_cmd[i], cursor, j-cursor);
                }
                if (ctrl->name)
                {
                    find_name_proc_status(superMap, ctrl);
                    free(ctrl->group); 
                    ctrl->group = NULL;
                    free(ctrl->name);
                    ctrl->name = NULL;
                }
                else if (ctrl->group)
                {
                    find_group_proc_status(superMap, ctrl);
                    free(ctrl->group);
                    ctrl->group = NULL;
                }
            }
            i++;
        }
    }
}

// Traitement de la commande
void process_command(const char *cmd, t_superMap** superMap, t_process_para* para, t_ctrl_cmds* ctrl) {
    printf("\nTraitement de la commande : %s\n", cmd);
    ctrl->split_cmd = split(cmd,' ');
    if (!ctrl->split_cmd)
        return;
    ctrl->tab_len = ft_pointer_tab_len(ctrl->split_cmd);
    if (strcmp(ctrl->split_cmd[0], "start") == 0) {
        call_start_cmd(ctrl, superMap, para);
    } else if (strcmp(ctrl->split_cmd[0], "quit") == 0) {
        printf("→ Demande d'arrêt.\n");
        running = 0;
    }  else if (strcmp(ctrl->split_cmd[0], "stop") == 0) {
        call_stop_cmd(ctrl, superMap);
    }  else if (strcmp(ctrl->split_cmd[0], "reload") == 0) {
        call_reload_cmd(ctrl, superMap, para);
    }  else if (strcmp(ctrl->split_cmd[0], "status") == 0) {
        call_status_cmd(ctrl, superMap);
    }  else if (strcmp(ctrl->split_cmd[0], "restart") == 0) {
        call_restart_cmd(ctrl, superMap, para);
    } else {
        printf("→ Commande inconnue.\n");
    }
    for (int i = 0; ctrl->split_cmd[i];i++) {
        free (ctrl->split_cmd[i]);
        ctrl->split_cmd[i] = NULL;
    }
    free (ctrl->split_cmd);
    ctrl->split_cmd = NULL; 
}

void *reader_thread(void *arg) {
    (void)arg;
    enable_raw_mode();

    int pos = 0;
    int hist_cursor = histo_size;

    printf("> ");
    fflush(stdout);

    while (running) {
        char c;
        int n = read(STDIN_FILENO, &c, 1);
        if (n <= 0) continue;

        if (c == 3) { // Ctrl+C
            running = 0;
            break;
        } else if (c == '\n') { // Enter
            cmd_buffer[pos] = '\0';
            if (pos > 0) {
                add_history(cmd_buffer);
                hist_cursor = histo_size;
                cmd_ready = 1;
            }
            pos = 0;
            fflush(stdout);
        } else if (c == 127 || c == 8) { // Backspace
            if (pos > 0) {
                pos--;
                cmd_buffer[pos] = '\0';
                printf("\b \b");
                fflush(stdout);
            }
        } else if (c == 27) { // séquence d'échappement pour flèches
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) == 0) continue;
            if (read(STDIN_FILENO, &seq[1], 1) == 0) continue;

            if (seq[0] == '[') {
                if (seq[1] == 'A') { // flèche haut
                    if (histo_size == 0) continue;
                    if (hist_cursor > 0) hist_cursor--;
                    pos = snprintf(cmd_buffer, MAX_CMD, "%s", history[hist_cursor]);
                    printf("\r\33[2K> %s", cmd_buffer);
                    fflush(stdout);
                } else if (seq[1] == 'B') { // flèche bas
                    if (histo_size == 0) continue;
                    if (hist_cursor < histo_size - 1) hist_cursor++;
                    else hist_cursor = histo_size;

                    if (hist_cursor < histo_size)
                        pos = snprintf(cmd_buffer, MAX_CMD, "%s", history[hist_cursor]);
                    else {
                        pos = 0;
                        cmd_buffer[0] = '\0';
                    }
                    printf("\r\33[2K> %s", cmd_buffer);
                    fflush(stdout);
                }
            }
        } else { // caractère normal
            if (pos < MAX_CMD - 1) {
                cmd_buffer[pos++] = c;
                write(STDOUT_FILENO, &c, 1);
            }
        }
    }

    return NULL;
}
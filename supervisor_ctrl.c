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

void call_start_cmd(char **cmd)
{
    if (!cmd || ft_pointer_tab_len(cmd) < 2)
        return;
    if (ft_pointer_tab_len(cmd) == 2 && strcmp(cmd[1],"all") == 0)
    {
        printf("→ Start all Process !\n");
    }
    else
    {
        int i = 1;
        while (cmd[i])
        {
            int j = 0;
            int cursor = 0;
            char *group = NULL;
            char *name = NULL;
            char *arg = cmd[i];
            if (arg[0] == ':')
            {
                printf("→ Start Error name arg : %s\n",cmd[i]);
            }
            else
            {
                while (arg[j])
                {
                    if (arg[j] == ':')
                    {
                        if (group)
                        {
                            free(group);
                            group = NULL;
                            printf("→ Start Error name arg : %s\n",cmd[i]);
                            break;
                        }
                        group = ft_substr(cmd[i], cursor, j-cursor);
                        cursor = j+1;
                    }
                    j++;
                }
                if (group && cursor != j)
                {
                    name = ft_substr(cmd[i], cursor, j-cursor);
                }
                if (name)
                {
                    printf("→ Start Name %s:%s !\n",group,name);
                    free(group);
                    free(name);
                }
                else if (group)
                {
                    printf("→ Start Group %s: !\n",group);
                    free(group);
                }
            }
            i++;
        }
    }
}

void call_stop_cmd(char **cmd)
{
    if (!cmd || ft_pointer_tab_len(cmd) < 2)
        return;
    if (ft_pointer_tab_len(cmd) == 2 && strcmp(cmd[1],"all") == 0)
    {
        printf("→ Stop all Process !\n");
    }
    else
    {
        int i = 1;
        while (cmd[i])
        {
            int j = 0;
            int cursor = 0;
            char *group = NULL;
            char *name = NULL;
            char *arg = cmd[i];
            if (arg[0] == ':')
            {
                printf("→ Stop Error name arg : %s\n",cmd[i]);
            }
            else
            {
                while (arg[j])
                {
                    if (arg[j] == ':')
                    {
                        if (group)
                        {
                            free(group);
                            group = NULL;
                            printf("→ Stop Error name arg : %s\n",cmd[i]);
                            break;
                        }
                        group = ft_substr(cmd[i], cursor, j-cursor);
                        cursor = j+1;
                    }
                    j++;
                }
                if (group && cursor != j)
                {
                    name = ft_substr(cmd[i], cursor, j-cursor);
                }
                if (name)
                {
                    printf("→ Stop Name %s:%s !\n",group,name);
                    free(group);
                    free(name);
                }
                else if (group)
                {
                    printf("→ Stop Group %s: !\n",group);
                    free(group);
                }
            }
            i++;
        }
    }
}

void call_restart_cmd(char **cmd)
{
    if (!cmd || ft_pointer_tab_len(cmd) < 2)
        return;
    if (ft_pointer_tab_len(cmd) == 2 && strcmp(cmd[1],"all") == 0)
    {
        printf("→ Restart all Process !\n");
    }
    else
    {
        int i = 1;
        while (cmd[i])
        {
            int j = 0;
            int cursor = 0;
            char *group = NULL;
            char *name = NULL;
            char *arg = cmd[i];
            if (arg[0] == ':')
            {
                printf("→ Restart Error name arg : %s\n",cmd[i]);
            }
            else
            {
                while (arg[j])
                {
                    if (arg[j] == ':')
                    {
                        if (group)
                        {
                            free(group);
                            group = NULL;
                            printf("→ Restart Error name arg : %s\n",cmd[i]);
                            break;
                        }
                        group = ft_substr(cmd[i], cursor, j-cursor);
                        cursor = j+1;
                    }
                    j++;
                }
                if (group && cursor != j)
                {
                    name = ft_substr(cmd[i], cursor, j-cursor);
                }
                if (name)
                {
                    printf("→ Restart Name %s:%s !\n",group,name);
                    free(group);
                    free(name);
                }
                else if (group)
                {
                    printf("→ Restart Group %s: !\n",group);
                    free(group);
                }
            }
            i++;
        }
    }
}

void call_reload_cmd(char **cmd)
{
    if (!cmd || ft_pointer_tab_len(cmd) < 2)
        return;
    if (ft_pointer_tab_len(cmd) == 2 && strcmp(cmd[1],"all") == 0)
    {
        printf("→ Reload all Process !\n");
    }
    else
    {
        int i = 1;
        while (cmd[i])
        {
            int j = 0;
            int cursor = 0;
            char *group = NULL;
            char *name = NULL;
            char *arg = cmd[i];
            if (arg[0] == ':')
            {
                printf("→ Reload Error name arg : %s\n",cmd[i]);
            }
            else
            {
                while (arg[j])
                {
                    if (arg[j] == ':')
                    {
                        if (group)
                        {
                            free(group);
                            group = NULL;
                            printf("→ Reload Error name arg : %s\n",cmd[i]);
                            break;
                        }
                        group = ft_substr(cmd[i], cursor, j-cursor);
                        cursor = j+1;
                    }
                    j++;
                }
                if (group && cursor != j)
                {
                    name = ft_substr(cmd[i], cursor, j-cursor);
                }
                if (name)
                {
                    printf("→ Reload Name %s:%s !\n",group,name);
                    free(group);
                    free(name);
                }
                else if (group)
                {
                    printf("→ Reload Group %s: !\n",group);
                    free(group);
                }
            }
            i++;
        }
    }
}

void call_status_cmd(char **cmd)
{
    if (!cmd || ft_pointer_tab_len(cmd) < 2)
        return;
    if (ft_pointer_tab_len(cmd) == 2 && strcmp(cmd[1],"all") == 0)
    {
        printf("→ Status all Process !\n");
    }
    else
    {
        int i = 1;
        while (cmd[i])
        {
            int j = 0;
            int cursor = 0;
            char *group = NULL;
            char *name = NULL;
            char *arg = cmd[i];
            if (arg[0] == ':')
            {
                printf("→ Status Error name arg : %s\n",cmd[i]);
            }
            else
            {
                while (arg[j])
                {
                    if (arg[j] == ':')
                    {
                        if (group)
                        {
                            free(group);
                            group = NULL;
                            printf("→ Status Error name arg : %s\n",cmd[i]);
                            break;
                        }
                        group = ft_substr(cmd[i], cursor, j-cursor);
                        cursor = j+1;
                    }
                    j++;
                }
                if (group && cursor != j)
                {
                    name = ft_substr(cmd[i], cursor, j-cursor);
                }
                if (name)
                {
                    printf("→ Status Name %s:%s !\n",group,name);
                    free(group);
                    free(name);
                }
                else if (group)
                {
                    printf("→ Status Group %s: !\n",group);
                    free(group);
                }
            }
            i++;
        }
    }
}

// Traitement de la commande
void process_command(const char *cmd) {
    printf("\nTraitement de la commande : %s\n", cmd);
    char ** split_cmd = split(cmd,' ');
    if (!split_cmd)
        return;
    if (strcmp(split_cmd[0], "start") == 0) {
        call_start_cmd(split_cmd);
    } else if (strcmp(split_cmd[0], "quit") == 0) {
        printf("→ Demande d'arrêt.\n");
        running = 0;
    }  else if (strcmp(split_cmd[0], "stop") == 0) {
        call_stop_cmd(split_cmd);
    }  else if (strcmp(split_cmd[0], "reload") == 0) {
        call_reload_cmd(split_cmd);
    }  else if (strcmp(split_cmd[0], "status") == 0) {
        call_status_cmd(split_cmd);
    }  else if (strcmp(split_cmd[0], "restart") == 0) {
        call_restart_cmd(split_cmd);
    } else {
        printf("→ Commande inconnue.\n");
    }
    for (int i = 0; split_cmd[i];i++)
        free (split_cmd[i]);
    free (split_cmd);
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
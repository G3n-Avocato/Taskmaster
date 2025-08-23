# include "supervisor.h"

// Handler Ctrl+C
void handle_sigint(int sig) {

    running = 0;
    if (sig == SIGINT)
        write(STDOUT_FILENO, "\n→ Ctrl+C received : stop...\n", 31);
    else if (sig == SIGQUIT)
        write(STDOUT_FILENO, "\n→ Ctrl+\\ received : quit...\n", 31);
}

bool	setup_sigint_handler() {
	struct sigaction saa;
	saa.sa_handler = handle_sigint;
	sigemptyset(&saa.sa_mask);
	saa.sa_flags = 0;
    //saa.sa_flags = SA_RESTART;

	if (sigaction(SIGINT, &saa, NULL) == -1) {
		perror("sigaction SIGINT");
		return false ;
	}
    if (sigaction(SIGQUIT, &saa, NULL) == -1) {
        perror("sigaction SIGQUIT");
        return false ;
    }
	return true ;
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char			*str;
	unsigned int	i;

	if (!s)
		return (NULL);
	i = -1;
	if (len > strlen(s))
		len = strlen(s);
	if (start > strlen(s))
		len = 0;
	if (len == strlen(s) && start < len)
		len -= start;
	str = calloc(sizeof (char), len + 1);
	if (!str)
		return (NULL);
	while (start + ++i < start + len)
		str[i] = s[start + i];
	return (str);
}

int ft_pointer_tab_len(char **ptr)
{
    if (!ptr)
		return (0);
    int i = 0;
    while (ptr[i])
        i++;
    return (i);
}

void print_stringss(char **list) {
    int i = 0;
    while (list[i] != NULL) {
        printf("|%s|\n", list[i]);
        i++;
    }
}

char ** split(const char* str, char sep)
{
    int cursor = 0;
    int i = 0;
    int j = 0;
    char **ret = calloc(sizeof( char*), 1);
    while (str[i])
    {
        if (str[i] == sep)
        {
            char ** new_ret = calloc(sizeof( char*), ft_pointer_tab_len(ret)+2);
            j = 0;
            while (ret[j])
            {
                new_ret[j] = ret[j];
                j++;
            }
            new_ret[j] = ft_substr(str, cursor, i-cursor);
            free (ret);
            ret = new_ret;
            while (str[i] == sep && str[i])
                i++;
            cursor = i;
        }
        else
            i++;
    }
    if (i != cursor)
    {
        char ** new_ret = calloc(sizeof( char*), ft_pointer_tab_len(ret)+2);
        j = 0;
        while (ret[j])
        {
            new_ret[j] = ret[j];
            j++;
        }
        new_ret[j] = ft_substr(str, cursor, i-cursor);
        free (ret);
        ret = new_ret;
    }
    return (ret);
}
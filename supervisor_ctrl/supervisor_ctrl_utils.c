# include "../include/supervisor.h"

const char* ProcessStatus_toString(ProcessStatus stat) {
    switch (stat) {
        case STOPPED: return "STOPPED";
        case STARTING: return "STARTING";
        case RUNNING: return "RUNNING";
        case BACKOFF: return "BACKOFF";
        case STOPPING: return "STOPPING";
        case EXITED: return "EXITED";
        case FATAL: return "FATAL";
        case UNKNOWN: return "UNKNOWN";
        default: return "default";
    }
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
            
            if (i - cursor > 0)
            {
                char** new_ret = calloc(sizeof( char*), ft_pointer_tab_len(ret)+2);

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
            while (str[i] == sep && str[i])
                i++;
            cursor = i;
        }
        else
            i++;
    }
    if (i != cursor)
    {
        char** new_ret = calloc(sizeof( char*), ft_pointer_tab_len(ret)+2);
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
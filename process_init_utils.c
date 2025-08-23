# include "supervisor.h"

void	sigchld_handler(int sign) {
	(void)sign;
	sigchld_received = 1;
}

bool	setup_sigchld_handler() {
	struct sigaction sa;
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;

	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		return false ;
	}
	return true ;
}

static int	ft_nlen(int n)
{
	int	i;

	i = 0;
	if (n < 0)
	{
		i++;
		n = -n;
	}
	if (n == 0)
		return (1);
	while (n > 0)
	{
		n /= 10;
		i++;
	}
	return (i);
}

char	*ft_itoa(int n)
{
	int		i;
	char	*str;
	int		nb;

	i = ft_nlen(n);
	nb = n;
	if (n == -2147483648)
		str = strdup("-2147483648");
	else
		str = calloc(1, i + 1);
	if (!str)
		return (NULL);
	if (n == -2147483648)
		return (str);
	if (n < 0)
		n = -n;
	while ((i != 0 && n >= 0) || (i != 1 && n < 0))
	{
		str[--i] = n % 10 + '0';
		n /= 10;
	}
	if (nb < 0)
		str[0] = '-';
	return (str);
}
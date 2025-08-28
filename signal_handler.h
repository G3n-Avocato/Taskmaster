#ifndef SIGNAL_HANDLER_H
# define SIGNAL_HANDLER_H


void	sigchld_handler(int sign);
bool	setup_sigchld_handler();

void    handle_sigint(int sig);
bool	setup_sigint_handler();

void    handle_sighup(int sig);
bool    setup_sighup_handler();

#endif
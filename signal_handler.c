# include "supervisor.h"

// Handler Exit-Proc
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

// Handler Ctrl+C + Ctrl+quit
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

// handler sighup for reload cmd
void    handle_sighup(int sig) {
    (void)sig;
    sighup_reload = 1;
}

bool    setup_sighup_handler() {
    struct sigaction saa;
	saa.sa_handler = handle_sighup;
    sigemptyset(&saa.sa_mask);
	saa.sa_flags = 0;

    if (sigaction(SIGHUP, &saa, NULL) == -1) {
        perror("sigaction SIGHUP");
		return false ;
    }
    return true ;
}
#include "../include/libs.h"

/* SIGCHLD flag defined in shell.c */
extern volatile sig_atomic_t sigchld_flag;

static void sigchld_handler(int sig) {
    sigchld_flag = 1; /* set the SIGCHLD flag */
}

void install_sigchld_handler() {
    struct sigaction action;
    action.sa_handler = sigchld_handler;

    /* do not block signals of any other type */
    sigemptyset(&action.sa_mask);

    /* restart syscalls if possible, man 2 sigaction flags section */
    action.sa_flags = SA_RESTART;
    
    int ret_val = sigaction(SIGCHLD, &action, NULL);
    
    if (ret_val != 0) {
        /* error in installing SIGCHLD handler */
        fprintf(stderr, "vsh: SIGCHLD handler installer error: %s\n",
                strerror(errno));
        exit(EXIT_FAILURE);
    }
}

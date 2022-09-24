#include "../include/libs.h"

/* system info and environment variables */
char hostname[HOST_NAME_MAX];
char username[USER_NAME_MAX];
extern char **environ;
char cmdline[MAXLINE]; /* buffer for whole command line */
int cmd_len = 0;
int shell_terminal;
int shell_pgid;
struct termios shell_tmodes;

/* first job in job list */
job *first_job;

/* time taken by the previous foreground process */
int fg_process_time_taken = 0;

/* functions for displaying prompt and setting up the shell */
void setupshell(); /* does initial setup of the shell */
void set_environ(); /* set the environment variables */
void install_sighandlers(); /* install all the signal handlers */
void displayprompt(); /* displays the prompt */

/* flags for signal handlers */
volatile sig_atomic_t sigchld_flag = 0;

int main() {

    setupshell(); /* setup shell prompt and environment variables */
    /* TODO : add the functionality of a config file later */

    while(1) {
        
        if (sigchld_flag) {
            process_sigchld();
            sigchld_flag = 0;
        }

        displayprompt(); /* display the shell promt */

        /* read the command line input */
        int ret_val = take_input();
        if (ret_val == -1) {
            exit(EXIT_SUCCESS);
        }
        else if (ret_val <= 1) continue;
        
        //printf("command line = %s\n", cmdline);
        update_history(cmdline); /* update the command line history of shell */
        parseline(cmdline); /* parse the command line and evaluate
                               each command */
    }
}

void setupshell() {

    /* set the terminal fd to be same as stdin */
    shell_terminal = STDIN_FILENO;

    shell_pgid = getpid();

    /* Put shell in it's own process group */
    if (setpgid(shell_pgid, shell_pgid) != 0) {
       fprintf(stderr, "vsh: couldn't put the shell in it's own process group: %s\n",
               strerror(errno));
       exit(EXIT_FAILURE);
    }
    
    /* get control of the terminal */
    tcsetpgrp(shell_terminal, shell_pgid);

    /* save default terminal attributes for shell */
    tcgetattr(shell_terminal, &shell_tmodes);

    /* get the hostname */
    int ret_val = gethostname(hostname, HOST_NAME_MAX);
    if (ret_val == -1) {
        app_error("gethostname error");
    }

    /* get the username */
    ret_val = getusername(username, USER_NAME_MAX);
    if (ret_val == -1) {
        app_error("getusername error");
    }

    set_environ(); /* set the environment variables */

    install_sighandlers(); /* install all the signal handlers */
    
    retrieve_history(); /* retrieve the history file of vsh */
}

void set_environ() {
    /* Set $HOME, $PWD, $OLDPWD environment to pwd */
    char homedir[PATH_MAX + 1];
    Getcwd(homedir, PATH_MAX + 1);
    Setenv("HOME", homedir, 1);
    Setenv("PWD", homedir, 1);
    Setenv("OLDPWD", homedir, 1);
}

void install_sighandlers() {
    /* install the sigchld handler */
    install_sigchld_handler();

    /* ignore ctrl-z */
    signal(SIGTSTP, SIG_IGN);

    /* ignore ctrl-c */
    signal(SIGINT, SIG_IGN);

    /* ignore SIGTTOU */
    signal(SIGTTOU, SIG_IGN);
}

void displayprompt() {
    char absolute_path[PATH_MAX];
    char relative_path[PATH_MAX];
    Getcwd(absolute_path, PATH_MAX); /* get the current working path */
    char *ret_val = getrelativepath(relative_path, PATH_MAX, absolute_path);

    if (ret_val == NULL) {
        fprintf(stderr, "vsh: display error\n");
        /* exit due to relative path failure */
        exit(EXIT_FAILURE);
    }

    /* print the command prompt */

    /* printing username and hostname */
    printf(COLOR_BOLD);
    printf(ANSI_COLOR_GREEN "[%s@%s " ANSI_COLOR_RESET,
            username, hostname); 
    printf(COLOR_BOLD);

    /* printing the path */
    printf("%s", relative_path);

    /* printing the time taken by last foreground process */
    if (fg_process_time_taken) {
        printf(" (took %ds)", fg_process_time_taken);
        fg_process_time_taken = 0;
    }

    printf(ANSI_COLOR_GREEN "]\n--> " ANSI_COLOR_RESET);
    printf(COLOR_BOLD_OFF);
}

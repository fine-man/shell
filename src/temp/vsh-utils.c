#include "../include/libs.h"

/* variables defined in shell.c */
extern int fg_process_time_taken;
extern struct job_t jobs[MAXJOBS];

void eval(char *cmdline) {
    /* evaluate the cmdline */

    char *argv[MAXARGS]; /* argv for execve */
    char buf[MAXLINE]; /* copy of command line which we can change */
    int argc; /* number of arguments */
    int bg;

    strcpy(buf, cmdline);
    argc = getarguments(buf, argv); /* parsing all the arguments */

    if (argv[0] == NULL) return; /* it's an empty command */

    if (builtincommand(argc, argv)) return;

    if ((*argv[argc - 1]) == '&') {
        argv[--argc] = NULL;
        bg = 1; /* process to be executed in background */
    }
    else bg = 0;

    if (!bg) {
        /* execute process in foreground */
        execute_process_fg(argc, argv);
    }
    else {
        /* execute process in background */
        execute_process_bg(argc, argv);
    }
}

void execute_process_fg(int argc, char **argv) {
    /* start the process in foreground */

    /* initialize the mask sets */
    sigset_t mask_all, mask_sigchld, prev_one;

    sigfillset(&mask_all); /* fill mask_all */
    sigemptyset(&mask_sigchld);
    sigaddset(&mask_sigchld, SIGCHLD); /* add SIGCHLD to set */

    /* block the SIGCHLD signal */
    sigprocmask(SIG_BLOCK, &mask_sigchld, &prev_one);
    
    pid_t pid; /* pid of the child process */
    time_t start_time = time(NULL); /* start time of the process */
    pid = fork(); /* fork a child process */
    if (pid < 0) {
        /* fork error */
        fprintf(stderr, "vsh: fork error: %s\n", strerror(errno));
        return;
    }

    if (pid == 0) {
        /* child process */
        restore_sigdefault();

        /* restore the previous mask */
        sigprocmask(SIG_SETMASK, &prev_one, NULL);
        execvpe(argv[0], argv, environ);
        unix_error("execvpe error");
    }
    else {
        /* parent code */

        /* block all the signals */
        sigprocmask(SIG_BLOCK, &mask_all, NULL);

        int wstatus;
        /* add the job to the job list */
        int jid = addjob(jobs, pid, FG, argv[0]);
        if (jid <= 0) {
            fprintf(stderr, "vsh: addjob error\n");
        }

        waitpid(pid, &wstatus, WUNTRACED);

        if (WIFEXITED(wstatus) || WIFSIGNALED(wstatus)) {
            /* process exited */
            time_t end_time = time(NULL);
            double time_taken = difftime(end_time, start_time);
            fg_process_time_taken = time_taken;
            deletejob(jobs, pid);
        }
        else if (WIFSTOPPED(wstatus)) {
            /* process stopped */
            struct job_t *job = getjobjid(jobs, jid);
            if (job == NULL) {
                fprintf(stderr, "vsh: getjobjid error\n");
            }
            else {
                job->state = ST;
                job->signo = WSTOPSIG(wstatus);
                print_stoppedjob_status(job);
            }
        }

        /* restore the previous mask */
        sigprocmask(SIG_SETMASK, &prev_one, NULL);
    }
}

void execute_process_bg(int argc, char **argv) {
    /* start a process in background */
    
    /* initialize the mask sets */
    sigset_t mask_all, mask_sigchld, prev_one;

    sigfillset(&mask_all); /* fill mask_all */
    sigemptyset(&mask_sigchld);
    sigaddset(&mask_sigchld, SIGCHLD); /* add SIGCHLD to set */

    /* block the SIGCHLD signal */
    sigprocmask(SIG_BLOCK, &mask_sigchld, &prev_one);

    pid_t pid; /* pid of the child process */
    pid = fork(); /* fork a child process */
    if (pid < 0) {
        /* fork error */
        fprintf(stderr, "vsh: fork error: %s\n", strerror(errno));
        return;
    }

    if (pid == 0) {
        /* child process */
        /* restore default behaviour of all signals */
        setpgid(0, 0);
        restore_sigdefault();

        /* restore the previous mask */
        sigprocmask(SIG_SETMASK, &prev_one, NULL);
        execvpe(argv[0], argv, environ);
        unix_error("execvpe error");
    }
    else {
        /* parent code */

        /* Block all signals */
        sigprocmask(SIG_BLOCK, &mask_all, NULL);

        /* add job to job list */
        int jid = addjob(jobs, pid, BG, argv[0]); 
        if (jid < 1) {
            /* addjob error */
            fprintf(stderr, "vsh: not enough space in joblist\n");
        }
        else {
            /* print the JID and PID of the background process */
            printf("[%d] %d\n", jid, pid);
        }

        /* restore the previous mask */
        sigprocmask(SIG_SETMASK, &prev_one, NULL);
    }
}

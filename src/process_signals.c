#include "../include/libs.h"

/* job list defined in shell.c */
extern struct job_t jobs[MAXJOBS];
/* SIGCHLD flag defined in shell.c */
extern volatile sig_atomic_t sigchld_flag;

void process_sigchld() {
    sigset_t mask_sigchld, prev_mask;
    sigemptyset(&mask_sigchld);
    sigaddset(&mask_sigchld, SIGCHLD); /* add SIGCHLD to set */

    /* block the SIGCHLD signal */
    sigprocmask(SIG_BLOCK, &mask_sigchld, &prev_mask);

    int wstatus;
    pid_t pid;

    while ((pid = waitpid(-1, &wstatus, WNOHANG | WUNTRACED)) > 0) {
        /* TODO : add support for continued child as well (WCONTINUED) */
        /* loop while there is any child which terminated/stopped */
        /* get the job_t struct of job with PID = "pid" */
        struct job_t *job = getjobpid(jobs, pid);
        if (job == NULL) continue;
        job->wstatus = wstatus;

        if (WIFEXITED(wstatus) || WIFSIGNALED(wstatus)) {
            /* state of job = exited */
            job->state = EXITED;
            print_finishedjob_status(job);
            deletejob(jobs, pid); /* delete the job */
        }
        else if (WIFSTOPPED(wstatus)) {
            /* state of job = stopped */
            job->state = ST;
            job->signo = WSTOPSIG(wstatus);
            print_stoppedjob_status(job);
        }
    }

    /* restore the previous mask */
    sigprocmask(SIG_SETMASK, &prev_mask, NULL);
    sigchld_flag = 0;
}

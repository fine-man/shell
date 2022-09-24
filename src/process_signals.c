#include "../include/libs.h"

/* SIGCHLD flag defined in shell.c */
extern volatile sig_atomic_t sigchld_flag;

void process_sigchld() {
    sigset_t mask_sigchld, prev_mask;
    sigemptyset(&mask_sigchld);
    sigaddset(&mask_sigchld, SIGCHLD); /* add SIGCHLD to set */

    /* block the SIGCHLD signal */
    sigprocmask(SIG_BLOCK, &mask_sigchld, &prev_mask);

    /* update status of all the completed/stopped process */
    update_status();

    /* print info of all jobs which changed status */
    do_job_notification();
     
    /* restore the previous mask */
    sigprocmask(SIG_SETMASK, &prev_mask, NULL);
    sigchld_flag = 0;
}

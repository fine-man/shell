#include "../include/libs.h"

void block_sigchld(sigset_t *prev_mask) {
    /* blocks SIGCHLD signal and store previous mask
     * in "prev_mask" variable */
    if (prev_mask == NULL) return;
    sigset_t mask_sigchld;
    sigemptyset(&mask_sigchld);
    sigaddset(&mask_sigchld, SIGCHLD);
    sigprocmask(SIG_BLOCK, &mask_sigchld, prev_mask);
}

void restore_mask(sigset_t *prev_mask) {
    /* restores the mask by setting it to "prev_mask" */
    if (prev_mask == NULL) return;
    sigprocmask(SIG_SETMASK, prev_mask, NULL);
}

void restore_sigdefault() {
    /* restore default behaviour of all the signals */
    signal(SIGTSTP, SIG_DFL);
    signal(SIGINT, SIG_DFL);
    signal(SIGCHLD, SIG_DFL);
}

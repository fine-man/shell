#ifndef _VSH_SIGNAL_H
#define _VSH_SIGNAL_H

typedef void (*sighandler_t)(int);

/* function to block SIGCHLD */
void block_sigchld(sigset_t *prev_mask);
/* function to restore previous mask */
void restore_mask(sigset_t *prev_mask);

/* function to restore default action of each signal */
void restore_sigdefault();
#endif

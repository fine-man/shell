#ifndef _JOBS_H
#define _JOBS_H

#include "libs.h"

/* Job constants */
#define MAXJOBS 100 /* max jobs at any point in time */

/* Job States */
#define UNDEF 0 /* undefined */
#define FG 1 /* running in foreground */
#define BG 2 /* running in background */
#define ST 3 /* stopped */
#define EXITED 4 /* process has finish/exited */

struct job_t {          /* The job struct */
    char cmd[MAXLINE]; /* command line */
    pid_t pid;          /* job pid */
    int jid;            /* job ID [1, 2, ...] */
    int state;          /* current job state [UNDEF, FG, BG, ET...] */
    int wstatus;      /* exit status or signal value */
    int signo;
};

/* functions for the job_t struct */
void clearjob(struct job_t *job);
void initjobs(struct job_t *jobs);
int maxjid(struct job_t *jobs); 
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmd);
int deletejob(struct job_t *jobs, pid_t pid); 
pid_t fgpid(struct job_t *jobs);
struct job_t *getjobpid(struct job_t *jobs, pid_t pid);
struct job_t *getjobjid(struct job_t *jobs, int jid); 
int pid2jid(struct job_t *jobs, pid_t pid); 
void print_finishedjob_status(struct job_t *job);
void print_stoppedjob_status(struct job_t *job);
#endif

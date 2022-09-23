#include "../include/libs.h"

void clearjob(struct job_t *job) {
    /* clears the job info */

    /* set to store previous mask */
    sigset_t prev_mask;
    /* block SIGCHLD */
    block_sigchld(&prev_mask);

    job->cmd[0] = '\0';
    job->pid = 0;
    job->jid = 0;
    job->state = UNDEF;
    job->wstatus = 0;
    job->signo = 0;

    /* unblock SIGCHLD */
    restore_mask(&prev_mask);
}

void initjobs(struct job_t *jobs) {

    /* set to store previous mask */
    sigset_t prev_mask;
    /* block SIGCHLD */
    block_sigchld(&prev_mask);

    int i;

    for (i = 0; i < MAXJOBS; i++) {
        clearjob(&jobs[i]);
    }

    /* unblock SIGCHLD */
    restore_mask(&prev_mask);
}

/* maxjid - returns largest allocated job ID */
int maxjid(struct job_t *jobs) {

    /* set to store previous mask */
    sigset_t prev_mask;
    /* block SIGCHLD */
    block_sigchld(&prev_mask);

    int i;
    int max_jid = 0;
    for (i = 0; i < MAXJOBS; i++) {
        if (jobs[i].jid > max_jid) {
            max_jid = jobs[i].jid;
        }
    }
    
    /* unblock SIGCHLD */
    restore_mask(&prev_mask);
    return max_jid;
}

/* addjob - adds a job to the jobs list */
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmd) {

    if (pid < 1) {
        /* invalid pid */
        return -1;
    }
    
    /* set to store previous mask */
    sigset_t prev_mask;
    /* block SIGCHLD */
    block_sigchld(&prev_mask);

    /* flag to check if process got added or not */
    int allocated = 0;
    int jid; /* allocated jid of the job */

    int i;
    /* looking for an empty space in job list */
    for (i = 0; i < MAXJOBS; i++) {
        if (jobs[i].pid == 0) {
            jobs[i].pid = pid;
            int max_jid = maxjid(jobs);
            jobs[i].jid = ++max_jid;
            jobs[i].state = state;
            jobs[i].wstatus = 0;
            strcpy(jobs[i].cmd, cmd);
            allocated = 1;
            jid = max_jid;
            break;
        }
    }
    
    /* unblock SIGCHLD */
    restore_mask(&prev_mask);

    if (allocated) return jid;
    else return -1;
}

/* deletejob - deletes a job with PID = "pid" from job list */
int deletejob(struct job_t *jobs, pid_t pid) {
    
    if (pid < 1) {
        /* invalid PID */
        return -1;
    }

    /* set to store previous mask */
    sigset_t prev_mask;
    /* block SIGCHLD */
    block_sigchld(&prev_mask);

    /* flag to check if job got deleted or not */
    int deleted = 0;

    int i;
    for (i = 0; i < MAXJOBS; i++) {
        if (jobs[i].pid == pid) {
            /* clear the job with PID = "pid" */
            clearjob(&jobs[i]);
            deleted = 1;
            break;
        }
    }
    
    /* unblock SIGCHLD */
    restore_mask(&prev_mask);

    if (deleted) return 0;
    else return -1; /* job not found */
}

/* fgid - returns PID of the foreground process */
pid_t fgid(struct job_t *jobs) {

    /* set to store previous mask */
    sigset_t prev_mask;
    /* block SIGCHLD */
    block_sigchld(&prev_mask);

    pid_t pid;
    int found = 0;
    int i;
    for (i = 0; i < MAXJOBS; i++) {
        if (jobs[i].state == FG) {
            /* if current job state == Foregound,
             * return PID of current job */
            pid = jobs[i].pid;
            found = 1;
            break;
        }
    }

    /* unblock SIGCHLD */
    restore_mask(&prev_mask);

    if (found) return pid;
    return 0; /* There is no foreground process running */
}

/* getjobpid - get the job_t struct of a process from it's PID */
struct job_t *getjobpid(struct job_t *jobs, pid_t pid) {
    if (pid < 1) {
        /* invalid PID */
        return NULL;
    }

    /* set to store previous mask */
    sigset_t prev_mask;
    /* block SIGCHLD */
    block_sigchld(&prev_mask);

    int job_index = -1;
    int i;

    for (i = 0; i < MAXJOBS; i++) {
        if (jobs[i].pid == pid) {
            job_index = i;
            break;
        }
    }

    /* unblock SIGCHLD */
    restore_mask(&prev_mask);

    if (job_index != -1) return &jobs[job_index];
    /* process with PID = "pid" not found in the job list */
    else return NULL;
}

/* getjobjid - get the job_t struct of a process from it's JID */
struct job_t *getjobjid(struct job_t *jobs, int jid) {
    if (jid < 1) {
        /* invalid jid */
        return NULL;
    }

    /* set to store previous mask */
    sigset_t prev_mask;
    /* block SIGCHLD */
    block_sigchld(&prev_mask);

    int job_index = -1;
    int i;

    for (i = 0; i < MAXJOBS; i++) {
        if (jobs[i].jid == jid) {
            job_index = i;
            break;
        }
    }

    /* unblock SIGCHLD */
    restore_mask(&prev_mask);

    if (job_index != -1) return &jobs[job_index];
    /* process with JID = "jid" not found in the job list */
    return NULL;
}

/* pid2jid - return the JID of process with PID = "pid" */
int pid2jid(struct job_t *jobs, pid_t pid) {
    if (pid < 1) {
        /* invalid pid */
        return -1;
    }

    /* set to store previous mask */
    sigset_t prev_mask;
    /* block SIGCHLD */
    block_sigchld(&prev_mask);

    int jid = -1;
    int i;

    for (i = 0; i < MAXJOBS; i++) {
        if (jobs[i].pid == pid) {
            jid = jobs[i].jid;
            break;
        }
    }

    /* unblock SIGCHLD */
    restore_mask(&prev_mask);

    return jid;
}

void print_finishedjob_status(struct job_t *job) {
    if (job == NULL) return;
    if (job->state != EXITED) return;

    printf("%s with pid = %d exited ", job->cmd, job->pid);
    
    if (WIFEXITED(job->wstatus)) {
        printf("normally with Exit code = %d\n",
                WEXITSTATUS(job->wstatus));
    }
    else if (WIFSIGNALED(job->wstatus)) {
        int signo = WTERMSIG(job->wstatus);
        printf("abnormally with Signal: SIG%s: %s",
                sigabbrev_np(signo), sigdescr_np(signo));
        if (WCOREDUMP(job->wstatus)) {
            printf("(core dumped)\n");
        }
        else printf("\n");
    }
}

void print_stoppedjob_status(struct job_t *job) {
    if (job == NULL) return;
    if (job->state != ST) return;

    if (WIFEXITED(job->wstatus)) {
        int signo = job->signo;
        printf("[%d] %s with pid = %d stopped with Signal: SIG%s\n", 
                job->jid, job->cmd, job->pid, sigabbrev_np(signo));
    }
}

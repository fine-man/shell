#ifndef _JOBS2_H
#define _JOBS2_H

typedef struct process {
    struct process *next; /* next process in the pipeline */
    int argc;             /* number of arguments in the argument list */
    char **argv;          /* argument list for exec */
    pid_t pid;            /* pid of the process */
    int jid;              /* jid of the process */
    int status;            /* status of the process (completed, stopped, running) */
    int return_val;       /* return value of the process */
} process;

typedef struct job {
    struct job *next;        /* next active job */
    char *command;           /* command line, used for messages */
    process *first_process;  /* first process in the pipeline */
    int jid;                 /* job id */
    pid_t pgid;              /* process group id */
    int status;              /* status of the job */
    /* standard i/o channels */
    int in_file;
    int out_file;
    int error_file;
} job;

#endif

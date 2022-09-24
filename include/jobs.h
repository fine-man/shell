#ifndef _JOBS_H
#define _JOBS_H

#define RUNNING 1  /* process is running */
#define STOPPED 2  /* process is stopped */
#define SIGNALED 3 /* process was terminated by a signal */
#define EXITED 4   /* process exited normally */

typedef struct process {
    struct process *next; /* next process in the pipeline */
    int argc;             /* number of arguments in the argument list */
    char **argv;          /* argument list for exec */
    pid_t pid;            /* pid of the process */
    pid_t pgid;           /* group if of the process */
    int status;            /* status of the process (completed, stopped, running) */
    int return_code;       /* return value of the process */
} process;

typedef struct job {
    struct termios tmodes;   /* saved terminal modes */
    struct job *next;        /* next active job */
    char *command;           /* command line, used for messages */

    process *first_process;  /* first process in the pipeline */
    int proc_list_size;      /* size of the process list */

    /* status of the last waited process in pipeline */
    int status;
    /* return code of the last waited process in pipeline */
    int return_code;

    int jid;                 /* job id */
    pid_t pgid;              /* process group id */
    int notified;            /* whether the user has been notified */

    /* standard i/o channels */
    int infile;
    int outfile;
    int errfile;
} job;

/* process procedures */
process *init_process(int argc, char **argv);
void add_process(process *first_process, process *new_process);
void delete_process_list(process *first_process);
void delete_process_list(process *first_process);
process *get_process_by_pid(process *first_process, pid_t pid);
void print_process(process *proc);

/* job procedures */
job *init_job(char *command);
void free_job(job *jb);
void deletejob_by_jid(job *first_job, int jid);
void addjob(job *first_job, job *new_job);
int is_job_stopped(job *jb);
int is_job_completed(job *jb);
job *get_job_by_pgid(job *first_job, pid_t pid);
int get_maxjid(job *first_job);
void print_job(job *jb);
job *get_job_by_pid(job *first_job, pid_t pid);
void print_job_status(job *jb);

#endif

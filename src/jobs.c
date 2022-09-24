#include "../include/libs.h"

extern struct termios shell_tmodes;
extern job *first_job;

process *init_process(int argc, char **argv) {
    process *proc = (process *) malloc(sizeof(process));
    if (proc == NULL) return NULL;

    /* initializing the process values */
    proc->next = NULL;

    proc->argc = argc;
    
    /* copying the arguments list */
    proc->argv = (char **) malloc((argc + 1) * sizeof(char *));
    for (int i = 0; i < argc; i++) {
        size_t len = strlen(argv[i]);
        proc->argv[i] = (char *) malloc((len + 1) * sizeof(char));
        strcpy(proc->argv[i], argv[i]);
    }
    proc->argv[argc] = NULL;

    proc->pid = -1;
    proc->pgid = -1;
    proc->status = -1;
    proc->return_code = -1;

    return proc;
}

void add_process(process *first_process, process *new_process) {
    if (first_process == NULL) {
        return;
    }

    /* finding the last process in the proc_list */
    process *last_process = first_process;
    while (last_process->next != NULL) {
        last_process = last_process->next;
    }

    last_process->next = new_process;
}

void delete_process(process *proc) {
    if (proc == NULL) return;

    /* freeing the memory of the arguments list */
    for (int i = 0; i < proc->argc; i++) {
        free(proc->argv[i]);
    }

    free(proc);
}

void delete_process_list(process *first_process) {
    process *cur_process = first_process;
    process *next_process = NULL;

    /* deleting all the processes in the proc_list */
    while (cur_process != NULL) {
        next_process = cur_process->next;
        delete_process(cur_process);
        cur_process = next_process;
    }
}

void print_process(process *proc) {
    if (proc == NULL) return;

    printf("argc = %d\n", proc->argc);
    for (int i = 0; i < proc->argc; i++) {
        if (proc->argv[i] == NULL) {
            printf("error\n");
            exit(0);
        }
        printf("argv[%d] = %s\n", i, proc->argv[i]);
    }

    printf("\n");
}

process *get_process_by_pid(process *first_process, pid_t pid) {
    /* find a process with the given pid */
    if (first_process == NULL) return NULL;

    process *cur_process = first_process;
    while (cur_process != NULL) {
        if (cur_process->pid == pid) {
            return cur_process;
        }

        cur_process = cur_process->next;
    }

    /* process with the required pid not found */
    return NULL;
}

job *init_job(char *command) {
    job *jb = (job *) malloc(sizeof(job));
    jb->next = NULL;

    jb->tmodes = shell_tmodes;
    
    /* copying the command */
    size_t len = strlen(command);
    jb->command = (char *) malloc((len + 1) * sizeof(char)); 
    strcpy(jb->command, command);

    jb->first_process = NULL;
    jb->proc_list_size = 0;

    jb->status = -1;
    jb->return_code = -1;

    jb->jid = -1;
    jb->pgid = -1;
    jb->notified = 0;

    jb->infile = STDIN_FILENO;
    jb->outfile = STDOUT_FILENO;
    jb->errfile = STDERR_FILENO;

    return jb;
}

void free_job(job *jb) {
    if (jb == NULL) return;

    free(jb->command);
    delete_process_list(jb->first_process);
    free(jb);
}

void addjob(job *start_job, job *new_job) {
    if (start_job == NULL) {
        first_job = new_job;
        return;
    }
    
    job *cur_job = start_job;

    /* iterating to the last job in the job list */
    while (cur_job->next != NULL) {
        cur_job = cur_job->next;
    }

    cur_job->next = new_job;
}

void deletejob_by_jid(job *start_job, int jid) {
    if (first_job == NULL) return;

    job *cur_job = start_job;
    job *prev_job = NULL;

    /* iterate through all the jobs in the job list */
    while (cur_job != NULL) {
        if (cur_job->jid == jid) {
            /* found the job */
            if (prev_job != NULL) {
                prev_job->next = cur_job->next;
            }

            if (cur_job == start_job) {
                first_job = cur_job->next;
            }

            free_job(cur_job);
            return;
        }

        prev_job = cur_job;
        cur_job = cur_job->next;
    }
}

void print_job(job *jb) {
    process *cur_process = jb->first_process;

    while (cur_process != NULL) {
        print_process(cur_process);
        printf("\n");
        cur_process = cur_process->next;
    }
}

int is_job_stopped(job *jb) {
    process *cur_process = jb->first_process;

    while (cur_process != NULL) {
        if (cur_process->status != STOPPED) {
            return 0;
        }

        cur_process = cur_process->next;
    }

    return 1;
}

int is_job_completed(job *jb) {
    process *cur_process = jb->first_process;

    while (cur_process != NULL) {
        if (cur_process->status != EXITED &&
            cur_process->status != SIGNALED) {
            return 0;
        }

        cur_process = cur_process->next;
    }
    
    return 1;
}

job *get_job_by_pid(job *first_job, pid_t pid) {
    if (first_job == NULL) return NULL;

    job *cur_job = first_job;

    while (cur_job != NULL) {
        process *cur_process = cur_job->first_process;
        while (cur_process != NULL) {
            if (cur_process->pid == pid) {
                return cur_job;
            }

            cur_process = cur_process->next;
        }

        cur_job = cur_job->next;
    }

    return NULL;
}

job *get_job_by_pgid(job *first_job, pid_t pgid) {
    /* gets the job with the particular pgid */
    if (first_job == NULL) return NULL;

    job *cur_job = first_job;
    
    while (cur_job != NULL) {
        if (cur_job->pgid == pgid) {
            return cur_job;
        }

        cur_job = cur_job->next;
    }

    return NULL;
}

int get_maxjid(job *first_job) {
    if (first_job == NULL) return 0;
    
    int maxjid = 0;
    job *cur_job = first_job;
    while (cur_job != NULL) {
        if (cur_job->jid > maxjid) {
            maxjid = cur_job->jid;
        }

        cur_job = cur_job->next;
    }

    return maxjid;
}

void print_job_status(job *jb) {
    /* prints the info of a stopped/exited job */
    if (jb == NULL) return;

    printf("[%d] %s with pgid = %d ",
            jb->jid, jb->command, jb->pgid);

    if (jb->status == EXITED) {
        /* job exited normally */
        printf("exited normally with EXIT code = %d\n",
                jb->return_code);
    }
    else if (jb->status == SIGNALED) {
        /* job was terminated by a signal */
        printf("exited abnormally with Signal: SIG%s: %s\n",
                sigabbrev_np(jb->return_code),
                sigdescr_np(jb->return_code));
    }
    else if (jb->status == STOPPED) {
        /* job was stopped by a signal */
        printf("stopped with Signal: SIG%s\n",
                sigabbrev_np(jb->return_code));
    }
}

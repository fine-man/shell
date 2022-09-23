#include "../include/libs.h"

void delete_process(process *proc) {
    if (proc == NULL) return;

    /* freeing the memory of the arguments list */
    for (int i = 0; i < proc->argc; i++) {
        free(proc->argv[i]);
    }

    free(proc);
}

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
    proc->jid = -1;
    proc->status = -1;
    proc->return_val = -1;

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

process *find_process_by_pid(process *first_process, pid_t pid) {
    /* find a process with the given pid */
    if (first_process == NULL) return NULL;

    process *cur_process = first_process;
    while (cur_process != NULL) {
        if (cur_process->pid = pid) {
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
    
    /* copying the command */
    size_t len = strlen(command);
    jb->command = (char *) malloc((len + 1) * sizeof(char)); 
    strcpy(jb->command, command);

    jb->first_process = NULL;
    jb->jid = -1;
    jb->pgid = -1;
    jb->status = -1;
    jb->in_file = STDIN_FILENO;
    jb->out_file = STDOUT_FILENO;
    jb->error_file = STDERR_FILENO;

    return jb;
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
        if (cur_process->status != COMPLETED) {
            return 0;
        }

        cur_process = cur_process->next;
    }
    
    return 1;
}

int main(int argc, char **argv) {
    /*
    process *proc1 = init_process(argc, argv);
    process *proc2 = init_process(argc, argv);
    add_process(proc1, proc2);
    
    print_process(proc2);
    */
    char cmdline[MAXLINE];

    if (fgets(cmdline, MAXLINE, stdin) != NULL) {
        parseline(cmdline);
    }
}

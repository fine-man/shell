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

int main(int argc, char **argv) {
    process *proc1 = init_process(argc, argv);
    print_process(proc1);
}

#include "../include/libs.h"

int getarguments(char *buf, char **argv) {
    /* stores all the arguments from "buf" to "argv" */
    /* return value is the number of arguments "argc" */

    int argc = 0;  
    const char *delimeter = " \n\t\r";
    char *saveptr; /* for strtok_r, read man 3 strtok_r */
    char *token;

    token = strtok_r(buf, delimeter, &saveptr);
    while (token != NULL) {
        argv[argc++] = token;
        token = strtok_r(NULL, delimeter, &saveptr);
    }

    argv[argc] = token;
    return argc;
}

void parse_command(char *cmd) {
    /* parse a single job command */

    char command[MAXLINE]; /* copy of cmd which can be changed */
    strcpy(command, cmd);

    char *argv[MAXARGS];
    int argc;

    process *first_process = NULL; 

    const char *delimeter = "|";
    char *token;
    char *saveptr; /* for strtok_r , read man strtok_r*/
    token = strtok_r(command, delimeter, &saveptr);
    int process_count = 0; /* number of processes in the pipeline */

    argc = getarguments(token, argv);
    if (argc == 0) {
        fprintf(stderr, "vsh: syntax error\n");
        return;
    }

    /* initialize the first process */
    first_process = init_process(argc, argv);
    if (first_process == NULL) {
        fprintf(stderr, "vsh: init_process() error\n");
        return;
    }

    process_count++;

    /* tokenize the command with pipes and create the process list */
    token = strtok_r(NULL, delimeter, &saveptr);
    while (token != NULL) {
        if (token == NULL) break;

        argc = getarguments(token, argv);
        /* not a valid command */
        if (argc == 0) {
            fprintf(stderr, "vsh: syntax error\n");
            delete_process_list(first_process);
            return;
        }

        process *new_process = init_process(argc, argv);
        /* error in initializing the process */
        if (new_process == NULL) {
            fprintf(stderr, "vsh: init_process() error\n");
            return;
        }

        /* add the new process to the process list */
        add_process(first_process, new_process);
        process_count++;
        token = strtok_r(NULL, delimeter, &saveptr);
    }

    /* create the job for the pipelined processes */
    job *jb = init_job(command);
    jb->proc_list_size = process_count;
    jb->first_process = first_process;

    /* eval and execute the job */
    eval(jb);
}

void parseline(char *cmdline) {
    /* parses the cmdline and sends each individual command
     * to be parsed again */

    char cmd[MAXLINE]; /* buffer for individual commands on cmdline */
    size_t cmd_len = 0; /* length of an individual command */
    size_t len = strlen(cmdline); /* length of the whole command line */

    /* replacing the \n of commandline with \0 */
    if (cmdline[len - 1] == '\n') {
        cmdline[--len] = '\0'; 
    }

    int i = 0;
    for (i = 0; i < len; i++) {
        if (cmdline[i] == ';') {
            cmd[cmd_len] = '\0';
            if (cmd_len > 0) parse_command(cmd);
            cmd_len = 0;
        }
        else if (cmdline[i] == '&') {
            cmd[cmd_len++] = '&';
            cmd[cmd_len] = '\0';
            /* parse the individual job command */
            if (cmd_len > 1) parse_command(cmd);
            cmd_len = 0;
        }
        else {
            cmd[cmd_len++] = cmdline[i];
        }
    }

    cmd[cmd_len] = '\0';
    if (cmd_len > 0) parse_command(cmd);
}

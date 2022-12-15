#include "../include/libs.h"

int getarguments(char *buf, char **argv, int* infile, int* outfile) {
    /* stores all the arguments from "buf" to "argv" */
    /* NOTE: buf will be changed */
    /* return value is the number of arguments "argc" */

    int argc = 0;  
    const char *delimeter = " \n\t\r";
    char *saveptr; /* for strtok_r, read man 3 strtok_r */
    char *token;

    *infile = -1;
    *outfile = -1;
    int file_mode = 0644;
    char resolved_path[PATH_MAX];
    char temp[MAXLINE]; /* temp buffer for storing stuff */

    token = strtok_r(buf, delimeter, &saveptr);

    while (token != NULL) {
        if (strcmp(token, ">") == 0 ||
            strcmp(token, ">>") == 0 ||
            strcmp(token, "<") == 0) {

            /* storing token string */
            strcpy(temp, token);

            token = strtok_r(NULL, delimeter, &saveptr);
            if (token == NULL) return 0;

           /* get the resolved path */
           char *ret_val = resolve_tilda(resolved_path,
                   PATH_MAX, token);

           if (ret_val == NULL) {
               /* error */
               fprintf(stderr, "vsh: invalid path: %s\n", token);
               return 0;
           }

           int fd; /* file descriptor for redirection */

           if (strcmp(temp, ">") == 0) {
               /* write output to a different file */

               /* creating an empty(O_TRUNC) output file */
               fd = open(resolved_path, O_CREAT | O_RDONLY |
                       O_WRONLY | O_TRUNC, 0644);

               /* assign the file descriptor */
               *outfile = fd;
           }
           else if (strcmp(temp, ">>") == 0) {
               /* append output to a different file */

               /* create a file to append output to */
               fd = open(resolved_path, O_CREAT | O_RDONLY |
                       O_WRONLY | O_APPEND, 0644);

               /* assign the file descriptor */
               *outfile = fd;
           }
           else if (strcmp(temp, "<") == 0) {
               /* read input from a different file */

               /* create a file to append output to */
               fd = open(resolved_path, O_RDONLY, 0);
               //printf("path: %s, fd = %d\n", resolved_path, fd);

               /* assign the file descriptor */
               *infile = fd;
           }

           if (fd == -1) {
               /* file open error */
               fprintf(stderr, "vsh: %s: %s\n", resolved_path,
                       strerror(errno));
               return 0;
           }
            
        }
        else {
            argv[argc++] = token;
        }

        if (token != NULL) {
            token = strtok_r(NULL, delimeter, &saveptr);
        }
    }

    argv[argc] = token;
    return argc;
}

void parse_command(char *cmd, int foreground) {
    /* parse a single job command */

    char command[MAXLINE]; /* copy of cmd which can be changed */
    strcpy(command, cmd);

    char *argv[MAXARGS]; /* argument list */
    int argc;

    process *first_process = NULL; 
    int infile, outfile, job_infile, job_outfile;
    infile = -1, outfile = -1; job_infile = -1, job_outfile = -1;

    const char *delimeter = "|";
    char *token;
    char *saveptr; /* for strtok_r , read man strtok_r*/
    token = strtok_r(command, delimeter, &saveptr);
    int process_count = 0; /* number of processes in the pipeline */

    argc = getarguments(token, argv, &infile, &outfile);
    if (argc == 0) {
        fprintf(stderr, "vsh: syntax error\n");
        return;
    }

    if (infile != -1) job_infile = infile;
    if (outfile != -1) job_outfile = outfile;

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

        argc = getarguments(token, argv, &infile, &outfile);

        /* not a valid command */
        if (argc == 0) {
            fprintf(stderr, "vsh: syntax error\n");
            delete_process_list(first_process);
            return;
        }

        if (infile != -1) job_infile = infile;
        if (outfile != -1) job_outfile = outfile;

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

    if (process_count == 1) {
        if (is_shell_builtin(first_process->argv[0])) {
            /* process is a shell buitlin */
            execute_shell_builtin(first_process->argc,
                    first_process->argv);
            return;
        }
    }

    /* create the job for the pipelined processes */
    job *jb = init_job(cmd);
    jb->proc_list_size = process_count;
    jb->first_process = first_process;
    if (job_infile != -1) jb->infile = job_infile;
    if (job_outfile != -1) jb->outfile = job_outfile;
    
    //print_job(jb);
    launch_job(jb, foreground);
}

void parseline(char *cmdline) {
    /* parses the cmdline and sends each individual job command
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
            if (cmd_len > 0) parse_command(cmd, 1);
            cmd_len = 0;
        }
        else if (cmdline[i] == '&') {
            cmd[cmd_len] = '\0';
            /* parse the individual job command */
            if (cmd_len > 1) parse_command(cmd, 0);
            cmd_len = 0;
        }
        else {
            cmd[cmd_len++] = cmdline[i];
        }
    }

    cmd[cmd_len] = '\0';
    if (cmd_len > 0) parse_command(cmd, 1);
}

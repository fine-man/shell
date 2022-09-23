#include "../include/libs.h"

#define MAX_DUMMY_SIZE 1000 /* maximum size of the dummy buffer */
static const char proc_fs[] = "/proc"; /* path to proc pseudo fs */
static const char stat_file_name[] = "stat";
static const char exe_file_name[] = "exe";

static void print_usage() {
    printf("Usage: pinfo [<pid>]\n");
}

static int print_process_info(pid_t pid) {
    /* retrieve and prints the info about the process to stdout */

    char process_dir_path[PATH_MAX]; /* stores /proc/[pid] */
    sprintf(process_dir_path, "%s/%d\0", proc_fs, pid);

    char stat_file_path[PATH_MAX]; /* stores /proc/[pid]/stat */
    sprintf(stat_file_path, "%s/%s\0", process_dir_path, stat_file_name);

    char exe_file_path[PATH_MAX]; /* path of the system link to the executable */
    char executable_path[PATH_MAX]; /* path of the executable */
    sprintf(exe_file_path, "%s/%s\0", process_dir_path, exe_file_name);

    /* following the symbolic link pointed by exe_file_path */
    ssize_t len = readlink(exe_file_path, executable_path, PATH_MAX);
    if (len == -1) {
        fprintf(stderr, "pinfo: %s: %s\n", exe_file_path, strerror(errno));
        return -1;
    }
    executable_path[len] = '\0'; /* readlink doesn't add \0 */

    /* buffer for path of the executable relative to the home directory */
    char executable_relative_path[PATH_MAX];
    char *ret_val = getrelativepath(executable_relative_path, PATH_MAX,
            executable_path);
    if (ret_val == NULL) {
        fprintf(stderr, "pinfo: executable file path error\n");
    }

    /* opening the stat file of the process */
    FILE *stat_file_stream = fopen(stat_file_path, "r");
    if (stat_file_stream == NULL) {
        fprintf(stderr, "pinfo: %s: %s\n",
                stat_file_path, strerror(errno));
        return -1;
    }

    int fg = 0; /* tells whether the process is foreground or not */
    /* look at /proc/[pid]/stat/ section of man 5 proc */
    pid_t ppid; /* pid of parent process */
    char dummy_buffer[MAX_DUMMY_SIZE]; /* dummy buffer for useless things */
    char state; /* state of the process */
    gid_t pgrp, tpgid; /* process group and terminal group */
    int session, tty_nr; /* session id and controlling terminal */
    unsigned long vsize; /* virtual memory size in bytes */
    
    /* retrieving the basic information of process */
    fscanf(stat_file_stream, "%d %s %c %d %d %d %d %d", &pid, dummy_buffer, &state,
            &ppid, &pgrp, &session, &tty_nr, &tpgid);
    
    if (pgrp == tpgid) {
        /* current process is a foreground process */
        fg = 1;
    }

    for (int i = 9; i < 23; i++) {
        /* reading the values which are not useful for pinfo */
        fscanf(stat_file_stream, "%s", dummy_buffer);
    }

    /* reading the virtual memory size from the stat file */
    fscanf(stat_file_stream, "%lu", &vsize);

    /* printing information about the process */
    printf("pid : %d\n", pid); /* pid of process */
    printf("process status : %c", state); /* process status */
    if (fg) printf("+"); /* add '+' to process status if it is fg */
    printf("\n");
    printf("memory : %lu (virtual memory in bytes)\n", vsize); /* virtual memory size */
    printf("executable path : %s\n", executable_relative_path);

    fclose(stat_file_stream);

    return 0;
}

int pinfo(int argc, char **argv) {
    if (argc > 2) {
       fprintf(stderr, "pinfo: expected 0 or 1 arguments: given %d\n", argc);
       print_usage();
       return -1;
    }

    if (argc == 1) {
        pid_t pid = getpid(); /* pid of the shell */
        print_process_info(pid);
    }
    else {
        pid_t pid = strtol(argv[1], NULL, 10);
        if (errno == EINVAL || errno == ERANGE) {
            fprintf(stderr, "pinfo: \'%s\': %s\n", argv[1],
                    strerror(errno));
            return -1;
        }
        else if (pid == 0) {
            fprintf(stderr, "pinfo: expected a positive integer: given \'%s\'\n", argv[1]);
            return -1;
        }

        print_process_info(pid);
    }

    return 0;
}

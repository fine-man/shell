#include "../include/libs.h"

extern job *first_job;
static int flag_r, flag_s, f_help;

static void print_usage() {
    /* print the usage of the ls program */
    printf("Usage: jobs [rs]...\n");
}

static void print_help() {
    /* prints the help for ls */
    /* TODO : write a separate function for help */
    print_usage();
}

static void reset_jobs_flags() {
    flag_r = 0, flag_s = 0, f_help = 0;
}

static int list_jobs_get_options(int argc, char **argv){
    /* parses the command-line to get all the options
     * and set the appropriate flag */

    int opt; /* return value of getopt_long */

    /* struct option for long options */
    const struct option longopts[] = 
    {
        {"running", no_argument, NULL, 'r'},
        {"stopped", no_argument, NULL, 's'},
        {"help", no_argument, &f_help, 1}
    };

    optind = 0; /* set this to 0 before every new getopt call */
    optarg = NULL;
    int longindex = 0; /* index for longopts struct array */

    /* getting the command line options */
    while ((opt = getopt_long(argc, argv, ":rs", longopts, &longindex)) != -1) {
        switch(opt) {
            case 0:
                break;
            case 'r':
                flag_r = 1;
                break;
            case 's':
                flag_s = 1;
                break;
            case '?':
                fprintf(stderr, "jobs: invalid option -- \'%c\'\n", optopt);
                print_usage();
                //exit(EXIT_FAILURE);
                break;
            case ':':
                fprintf(stderr, "Missing option\n");
                print_usage();
                break;
            default:
                print_usage();
                //exit(EXIT_FAILURE);
        }
    }

    /* print help */
    if (f_help) {
        print_help();
        return -1;
        //exit(EXIT_SUCCESS);
    }
    
    if (optind < argc) {
        /* if there are extra arguments */
        fprintf(stderr, "jobs: invalid arguments\n"); 
        print_help();
        return -1;
    }

    return 0;
};

static void print_running_jobs() {
    job *cur_job = first_job;

    while (cur_job != NULL) {
        if (is_job_running(cur_job)) {
            printf("[%d] Running %s [%d]\n", cur_job->jid,
                    cur_job->command, cur_job->pgid);
        }

        cur_job = cur_job->next;
    }
}

static void print_stopped_jobs() {
    job *cur_job = first_job;

    while (cur_job != NULL) {
        if (is_job_stopped(cur_job)) {
            printf("[%d] Stopped %s [%d]\n", cur_job->jid,
                    cur_job->command, cur_job->pgid);
        }

        cur_job = cur_job->next;
    }
}

int list_jobs(int argc, char **argv) {

    reset_jobs_flags(); /* reset the flags for jobs */

    /* parse the command-line options */
    int ret_val = list_jobs_get_options(argc, argv);
    if (ret_val != 0) {
        return -1;
    }

    if (argc == 1) {
        flag_r = 1;
        flag_s = 1;
    }
    else list_jobs_get_options(argc, argv);

    if (flag_r) print_running_jobs();
    if (flag_s) print_stopped_jobs();
    printf("\n");
}

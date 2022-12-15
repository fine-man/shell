#include "../include/libs.h"

extern job *first_job;
static void fg_print_usage() {
    printf("Usage: fg <job-id>\n");
}

int fg(int argc, char **argv) {
    /* bring a job to foreground */
    /* Usage: fg <job-id> */

    if (argc != 2) {
        fprintf(stderr, "vsh: fg: required 2 arguments: provided %d\n",
                argc);
        fg_print_usage();
        return -1;
    }

    /* convert argument string to decimal number */
    int jid = strtol(argv[1], NULL, 10);
    if (errno == EINVAL || errno == ERANGE) {
        fprintf(stderr, "vsh: fg: \'%s\': %s\n", argv[1],
                strerror(errno));
        fg_print_usage();
        return -1;
    }
    else if (jid <= 0) {
        fprintf(stderr, "vsh: fg: expected a positive integer: given \'%s\'\n", argv[1]);
        fg_print_usage();
        return -1;
    }

    job *jb = get_job_by_jid(first_job, jid); 

    /* job not found */
    if (jb == NULL) {
        fprintf(stderr, "vsh: fg: job with job id = %d not found\n",
                jid);
        return -1;
    }

    if (is_job_stopped(jb)) {
        put_job_in_foreground(jb, 1);
    }
    else put_job_in_foreground(jb, 0);
}

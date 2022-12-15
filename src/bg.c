#include "../include/libs.h"

extern job *first_job;

static void bg_print_usage() {
    printf("Usage: bg <job-id>\n");
}

int bg(int argc, char **argv) {
    /* bring a job to foreground */
    /* Usage: bg <job-id> */

    if (argc != 2) {
        fprintf(stderr, "vsh: bg: required 2 arguments: provided %d\n",
                argc);
        bg_print_usage();
        return -1;
    }

    /* convert argument string to decimal number */
    int jid = strtol(argv[1], NULL, 10);
    if (errno == EINVAL || errno == ERANGE) {
        fprintf(stderr, "vsh: bg: \'%s\': %s\n", argv[1],
                strerror(errno));
        bg_print_usage();
        return -1;
    }
    else if (jid <= 0) {
        fprintf(stderr, "vsh: bg: expected a positive integer: given \'%s\'\n", argv[1]);
        bg_print_usage();
        return -1;
    }

    job *jb = get_job_by_jid(first_job, jid); 

    /* job not found */
    if (jb == NULL) {
        fprintf(stderr, "vsh: bg: job with job id = %d not found\n",
                jid);
        return -1;
    }

    if (is_job_stopped(jb)) {
        put_job_in_background(jb, 1);
    }
    else put_job_in_background(jb, 1);
}

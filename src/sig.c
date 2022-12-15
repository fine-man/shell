#include "../include/libs.h"

extern job *first_job;

static void sig_print_usage() {
    printf("Usage: sig <job-id> <signal-number>\n");
}

int sig(int argc, char **argv) {

    if (argc != 3) {
        fprintf(stderr, "vsh: sig: required 3 arguments: provided %d\n",
                argc);
        sig_print_usage();
        return -1;
    }
    
    /* jid of the job */
    int jid = strtol(argv[1], NULL, 10);
    if (errno == EINVAL || errno == ERANGE) {
        fprintf(stderr, "vsh: sig: \'%s\': %s\n", argv[1],
                strerror(errno));
        sig_print_usage();
        return -1;
    }

    /* signal to be sent to the process */
    int signo = strtol(argv[2], NULL, 10);
    if (errno == EINVAL || errno == ERANGE) {
        fprintf(stderr, "vsh: sig: \'%s\': %s\n", argv[1],
                strerror(errno));
        sig_print_usage();
        return -1;
    }

    /* check if signal number is valid */
    if (signo <= 0 || signo > 31) {
        fprintf(stderr, "vsh: sig: invalid signal number: %d\n",
                signo);
        return -1;
    }

    job *jb = get_job_by_jid(first_job, jid);
    if (jb == NULL) {
        fprintf(stderr, "vsh: sig: job with job id = %d not found\n",
                jid);
        return -1;
    }

    /* send the signal signo */
    if (kill(-jb->pgid, signo) != 0) {
        fprintf(stderr, "vsh: sig: couldn't send Signal: SIG%s to job with jid = %d\n",
                sigabbrev_np(signo), jid);
        return -1;
    }

    return 0;
}

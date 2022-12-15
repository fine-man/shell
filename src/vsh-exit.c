#include "../include/libs.h"

void vsh_exit(int argc, char **argv) {
    if (argc > 1) {
        fprintf(stderr, "vsh: exit: expected 0 arguments: given %d\n",
                argc);
        return;
    }

    /* kill all jobs under the current shell */
    kill_all_jobs();
    exit(EXIT_SUCCESS);
}

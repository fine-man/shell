#include "../include/libs.h"

/* defined in shell.c */
extern int shell_terminal;
extern int shell_pgid;
extern struct termios shell_tmodes;
extern job *first_job;

void launch_process(process *proc, pid_t pgid, int infile,
        int outfile, int errfile, int foreground) {
    /* launch the process "proc" */

    /* change the group id of the process
     * set the terminal group if process is foreground
     * change the stdin, stdout, stderr
     * restore default signal handlers
     * exec the process */

    pid_t pid = getpid();

    /* change the process group of the process */
    if (pgid == -1) {
        /* set process group = pid */
        pgid = pid;
    }
    /* set process group = pgid */
    if (setpgid(pid, pgid) == -1) {
        fprintf(stderr, "vsh: child process group change failure: %s\n",
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* set the terminal group if process is foreground */
    if (foreground) {
        tcsetpgrp(shell_terminal, pgid);
    }

    /* input/output and error file redirection */
    if (dup2(infile, STDIN_FILENO) == -1) {
        fprintf(stderr, "vsh: input redirection error: %s\n",
                strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (dup2(outfile, STDOUT_FILENO) == -1) {
        fprintf(stderr, "vsh: output redirection error: %s\n",
                strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (dup2(errfile, STDERR_FILENO) == -1) {
        fprintf(stderr, "vsh: error redirection error: %s\n",
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* restore all the signal handlers to default */
    restore_sigdefault();

    if (user_builtin_command(proc->argc, proc->argv) == 0) {
        /* launch the process if it is not user builtin */
        execvpe(proc->argv[0], proc->argv, environ);
        /* exec failed */
        fprintf(stderr, "vsh: execvpe error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}

void launch_job(job *jb, int foreground) {
    if (jb == NULL) return;

    int infile, outfile, errfile, process_pipe[2];

    /* input file for the first process */
    infile = jb->infile;
    errfile = jb->errfile;

    process *cur_process = jb->first_process;
    /* loop through all the processes in the pipeline */
    for (; cur_process != NULL; cur_process = cur_process->next) {

        /* check if there is a process ahead in the pipeline */
        int is_pipe = (cur_process->next != NULL) ? 1 : 0;

        if (is_pipe) {
           if (pipe(process_pipe) != 0) {
               /* pipe error */
               fprintf(stderr, "vsh: pipe error: %s\n",
                       strerror(errno));
               exit(EXIT_FAILURE);
           }

           outfile = process_pipe[1];
        }
        else {
            outfile = jb->outfile;
        }

        pid_t pid = fork();
        
        if (pid < (pid_t) 0) {
            /* fork failed */
            fprintf(stderr, "vsh: fork failed: %s\n",
                    strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (pid == (pid_t) 0) {
            /* child process */

            /* close the reading end of the pipe */
            if (is_pipe) close(process_pipe[0]);

            //printf("infile = %d, outfile = %d\n", infile, outfile);
            /* launch the process */
            launch_process(cur_process, jb->pgid, infile, outfile,
                    errfile, foreground);
        }
        else {
            /* parent process */

            /* set the job process group */
            if (jb->pgid == -1) {
                jb->pgid = pid;
            }

            /* change the process group of the child process */
            setpgid(pid, jb->pgid);

            /* update process info */
            cur_process->pid = pid;
            cur_process->status = RUNNING;
            
            /* clean up after pipes */
            if (infile != jb->infile) {
                close(infile);
            }
            if (outfile != jb->outfile) {
                close(outfile);
            }

            if (is_pipe) infile = process_pipe[0];
        }
    }

    int maxjid = get_maxjid(first_job);
    jb->jid = maxjid + 1;
    if (first_job == NULL) first_job = jb;
    else addjob(first_job, jb);

    //print_all_jobs(first_job);
    if (foreground) {
        put_job_in_foreground(jb, 0);
    }
    else {
        put_job_in_background(jb, 0);
    }
}

void put_job_in_foreground(job *jb, int cont) {
    /* Send SIGCONT signal if jb is stopped
     * wait for job to become stopped or completed
     * save terminal mode of the job
     * restore shell's terminal mode */

    //printf("put job in foreground\n");
    if (jb == NULL) return;

    /* send a continue signal if necessary */
    if (cont) {
        /* restore the shell terminal mode the process was in */
        tcsetattr(shell_terminal, TCSADRAIN, &jb->tmodes);

        if (kill(-jb->pgid, SIGCONT) != 0) {
            fprintf(stderr, "vsh: couldn't continue job with jid = %d: %s\n",
                    jb->jid, strerror(errno));
            return;
        }

        mark_job_as_running(jb);
    }

    /* put job in foreground */
    tcsetpgrp(shell_terminal, jb->pgid);

    wait_for_job(jb);
    //printf("out of wait\n");

    /* Put the shell back in the foreground */
    tcsetpgrp(shell_terminal, shell_pgid);

    /* Restore the shell's terminal modes */
    tcgetattr(shell_terminal, &jb->tmodes);
    tcsetattr(shell_terminal, TCSADRAIN, &shell_tmodes);

    /* print the job info along with exit code */
    //print_job_status(jb);

    /* delete job from the job list */
    if (is_job_completed(jb)) {
        deletejob_by_jid(first_job, jb->jid);
    }
    else if (is_job_stopped(jb)) {
        jb->notified = 1;
    }
}

void put_job_in_background(job *jb, int cont) {
    /* put job in background */
    if (cont) {
        if (kill(-jb->pgid, SIGCONT) != 0) {
            fprintf(stderr, "vsh: couldn't continue job with jid = %d: %s\n",
                    jb->jid, strerror(errno));
        }

        mark_job_as_running(jb);
    }
    
    /* notify the user that job is running is background */
    printf("[%d]+ %s &\n", jb->jid, jb->command);
}

int mark_process_status(pid_t pid, int wstatus) {
    /* mark the status of the process with process id "pid */

    /* returns the status of the process with pid = "pid"
     * RUNNING
     * STOPPED
     * SIGNALED
     * EXITED */

    /* returns -1 on any errors */

    if (pid <= 0) return -1;

    job *jb = get_job_by_pid(first_job, pid);
    if (jb == NULL) {
        /* job with pgid not found */
        return -1;
    }

    process *proc = get_process_by_pid(jb->first_process, pid);

    if (proc == NULL) {
        /* process with pid not found */
        return -1;
    }

    if (WIFEXITED(wstatus)) {
        /* process exited normally */
        proc->status = EXITED;
        proc->return_code = WEXITSTATUS(wstatus);

        jb->status = EXITED;
        jb->return_code = proc->return_code;

        return EXITED;
    }
    else if (WIFSIGNALED(wstatus)) {
        /* process was terminated by a signal */
        proc->status = SIGNALED;
        proc->return_code = WTERMSIG(wstatus);

        jb->status = SIGNALED;
        jb->return_code = proc->return_code;

        return SIGNALED;
    }
    else if (WIFSTOPPED(wstatus)) {
        /* process was stopped by a signal */
        proc->status = STOPPED;
        proc->return_code = WSTOPSIG(wstatus);

        jb->status = STOPPED;
        jb->return_code = proc->return_code;

        return STOPPED;
    }
    else if (WIFCONTINUED(wstatus)) {
        /* process continued by SIGCONT */
        proc->status = RUNNING;
        jb->status = RUNNING;

        return RUNNING;
    }
    else {
        return -1;
    }
}

void wait_for_job(job *jb) {
    /* do waitpid to reap processes
     * update process status accordingly
     * do this untill job is completed */

    if (jb == NULL) return;

    pid_t pgid = jb->pgid;
    int wstatus;
    pid_t pid;

    /* wait while even a single process in job pipeline is running */
    while (!is_job_stopped(jb) && !is_job_completed(jb)) {
        if ((pid = waitpid(-pgid, &wstatus, WUNTRACED)) > 0) {

            /* mark the process status */
            int ret_status = mark_process_status(pid, wstatus);

            if (ret_status == EXITED || ret_status == SIGNALED) {
                job *jb = get_job_by_pid(first_job, pid);
                if (jb == NULL) return;
                delete_process_by_pid(jb, pid);
            }

            //printf("pid = %d, stopped = %d, completed = %d\n",
                    //pid, is_job_stopped(jb), is_job_completed(jb));
        }
        else if (pid == -1) {
            /* pid = -1, error */
            if (errno == ECHILD) {
                /* no children are waiting, ignore */
                printf("no children are waiting\n");
                break;
            }
            else {
                fprintf(stderr, "vsh: waitpid: %s\n", strerror(errno));
                break;
            }
        }
    }
}

void update_status() {
    int wstatus;
    pid_t pid;

    while ((pid = waitpid(WAIT_ANY, &wstatus, WNOHANG 
                    | WUNTRACED | WCONTINUED)) > 0) {
        /* mark the process status */
        int ret_status = mark_process_status(pid, wstatus);

        /* delete process if it is completed */
        if (ret_status == EXITED || ret_status == SIGNALED) {
            job *jb = get_job_by_pid(first_job, pid);
            if (jb == NULL) return;
            delete_process_by_pid(jb, pid);
        }
        //printf("pid = %d, stopped = %d, completed = %d\n",
                //pid, is_job_stopped(jb), is_job_completed(jb));
    }
}

void do_job_notification() {
    job *cur_job = first_job;

    while (cur_job != NULL) {
        if (is_job_completed(cur_job)) {
            /* job is completed */
            print_job_status(cur_job);
            /* delete job from the job list */
            deletejob_by_jid(first_job, cur_job->jid);
        }
        else if (is_job_stopped(cur_job) && !cur_job->notified) {
            print_job_status(cur_job);
            cur_job->notified = 1;
        }

        cur_job = cur_job->next;
    }
}

void kill_all_jobs() {
    /* kill all the jobs which are children of shell */
    job *cur_job = first_job;

    while (cur_job != NULL) {
        first_job = cur_job->next;

        if (kill(-cur_job->pgid, SIGKILL) != 0) {
            fprintf(stderr, "vsh: sig: couldn't send Signal: SIGKILL to job with jid = %d\n",
                    cur_job->jid);
        }

        cur_job = cur_job->next;
    }
}
/* 3539th line , it's a prime number */

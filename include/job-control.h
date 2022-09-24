#ifndef _JOB_CONTROL_H
#define _JOB_CONTROL_H

void launch_job(job *jb, int foreground);
void wait_for_job(job *jb);
void put_job_in_foreground(job *jb, int cont);
void put_job_in_background(job *jb, int cont);
void update_status();
void do_job_notification();
#endif

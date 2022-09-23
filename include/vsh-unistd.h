#ifndef _VSH_UNISTD_H
#define _VSH_UNISTD_H

#define HOST_NAME_MAX 64 /* according to POSIX standards */
#define USER_NAME_MAX 100

/* Function to get the username of the system */
int getusername(char *name, size_t len);
#endif

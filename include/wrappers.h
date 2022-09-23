#ifndef _WRAPPERS_H
#define _WRAPPERS_H

/* Error handling wrappers */
void unix_error(char *msg);
void posix_error(int code, char *msg);
void app_error(char *msg);
void cmd_error(char *msg);

/* File handling wrappers */
int Open(const char *pathname, int flags, mode_t mode);
off_t Lseek(int fd, off_t offset, int whence);
ssize_t Write(int fd, const void *buf, size_t count);
ssize_t Read(int fd, void *buf, size_t count);
int Stat(const char *restrict pathname, struct stat *restrict statbuf);

/* system info wrappers */
char *Getcwd(char *buf, size_t size);
struct passwd *Getpwuid(uid_t uid);
int Setenv(const char *name, const char *value, int overwrite);

/* Process control wrappers */
pid_t Fork();
pid_t Wait(int *wstatus);

/* Signal wrappers */
int Kill(pid_t pid, int sig);
#endif

#include "../include/libs.h"

/* Error handling wrappers */
void unix_error(char *msg) { /* Unix-style error */
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}

void posix_error(int code, char *msg) { /* Posix-style error */
    fprintf(stderr, "%s: %s\n", msg, strerror(code));
    exit(0);
}

void app_error(char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(0);
}

void cmd_error(char *msg) {
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
}


/* File handling wrappers */
int Open(const char *pathname, int flags, mode_t mode) {
    int file_no = open(pathname, flags, mode);
    if (file_no == -1) {
        unix_error("open() error");
    }

    return file_no;
}

off_t Lseek(int fd, off_t offset, int whence) {
   off_t ret_val = lseek(fd, offset, whence);
   if (ret_val == -1) {
        unix_error("lseek error");
    }

    return ret_val;
}

ssize_t Write(int fd, const void *buf, size_t count) {
    ssize_t ret_val = write(fd, buf, count);

    if (ret_val == -1) {
        unix_error("write error");
    }

    return ret_val;
}

ssize_t Read(int fd, void *buf, size_t count) {
    ssize_t ret_val = read(fd, buf, count);

    if (ret_val == -1) {
        unix_error("read error");
    }

    return ret_val;
}

int Stat(const char *restrict pathname,
         struct stat *restrict statbuf) {
    int ret_val = stat(pathname, statbuf);

    if (ret_val == -1) {
        unix_error("Stat error");
    }

    return ret_val;
}

/* system info wrappers */
char *Getcwd(char *buf, size_t size) {
    char *ret_val = getcwd(buf, size);
    if (ret_val == NULL) {
        unix_error("Getcwd error");
    }

    return ret_val;
}

struct passwd *Getpwuid(uid_t uid) {
    struct passwd *pw = getpwuid(uid);
    if (pw == NULL) {
        unix_error("Getpwuid error");
    }

    return pw;
}

int Setenv(const char *name, const char *value, int overwrite) {
    int ret_val = setenv(name, value, overwrite);
    if (ret_val != 0) {
        unix_error("Setenv error");
    }

    return ret_val;
}


/* Process control wrappers */
pid_t Fork() {
    pid_t pid = fork();
    
    if (pid < 0) {
        unix_error("Fork error");
    }

    return pid;
}

pid_t Wait(int *wstatus) {
    pid_t pid = wait(wstatus);

    if (pid < 0) {
        unix_error("Wait error");
    }

    return pid;
}

int Kill(pid_t pid, int sig) {
    int ret_val = kill(pid, sig);

    if (ret_val != 0) {
        unix_error("kill error");
    }

    return ret_val;
}

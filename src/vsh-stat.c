#include "../include/libs.h"

int is_directory(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0) {
       return 0;
   }
   return S_ISDIR(statbuf.st_mode);
}

int does_path_exists(const char *path) {
    /* return 1 if the file pointed py "path" is valid
     * and -1 otherwise, sets errno on error */

    struct stat statbuf;
    if (stat(path, &statbuf) != 0) {
        /* error */
        return -1;
    }

    return 1;
}

char get_filetype_letter(mode_t mode) {
    char c;
    switch(mode & S_IFMT) {
        case S_IFBLK:
            /* block device */
            c = 'b';
            break;
        case S_IFCHR:
            /* character device */
            c = 'c';
            break;
        case S_IFDIR:
            /* directory */
            c = 'd';
            break;
        case S_IFIFO:
            /* FIFO/pipe */
            c = 'f';
            break;
        case S_IFLNK:
            /* symlink */
            c = 'l';
            break;
        case S_IFREG:
            /* regular file */
            c = '-';
            break;
        case S_IFSOCK:
            /* Socket file */
            c = 's';
            break;
        default:
            /* unknown file type */
            c = '?';
            break;
    }

    return c;
}

int is_executable(mode_t mode) {
    /* bit offsets for the executable bit in "mode" */
    int x_bit_offsets[] = {0, 3, 6};

    for (int i = 0; i < 3; i++) {
        if (mode & (1 << x_bit_offsets[i])) {
            return 1;
        }
    }

    return 0;
}

struct stat *get_lstatbuf(char *path) {
    if (path == NULL) return NULL;

    static struct stat statbuf;

    int ret_val = lstat(path, &statbuf);
    if (ret_val != 0) {
        return NULL;
    }

    return &statbuf;
}

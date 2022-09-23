#ifndef _VSH_STAT_H
#define _VSH_STAT_H

int is_directory(const char *path);
int does_path_exists(const char *path);
char get_filetype_letter(mode_t mode);
int is_executable(mode_t mode);
struct stat *get_lstatbuf(char *path);

#endif

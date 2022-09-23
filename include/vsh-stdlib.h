#ifndef _VSH_STDLIB_H
#define _VSH_STDLIB_H

/* Function to convert absolute path to relative */
char *getrelativepath(char *relative_path, size_t len, char *path);
/* Function to convert relative path to absolute */
char *getrealpath(char *resolved_path, size_t len, char *relative_path);
/* Function to resolve tilda in path */
char *resolve_tilda(char *resolved_path, size_t len, char *relative_path);
#endif

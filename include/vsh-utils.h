#ifndef _VSH_UTILS_H
#define _VSH_UTILS_H

#include "libs.h"

#define MAXLINE 1000 /* maxline for the command line input */
#define MAXARGS 1000 /* max number of arguments possible for a command */
extern char **environ;

/* functions to parse and execute the command line */
int getarguments(char *buf, char **argv);
void eval(char *cmdline);
void parseline(char *cmdline);
void execute_process_fg(int argc, char **argv);
void execute_process_bg(int argc, char **argv);

#endif

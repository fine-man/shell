#ifndef _VSH_BUILTINS_H
#define _VSH_BUILTINS_H

/* builtin commands of the shell */
int builtincommand(int argc, char **argv);
void pwd();
void echo(int argc, char **argv);
void cd(int argc, char **argv);
void print_realpath(int argc, char **argv);
#endif

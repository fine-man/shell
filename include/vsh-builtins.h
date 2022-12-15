#ifndef _VSH_BUILTINS_H
#define _VSH_BUILTINS_H

/* builtin commands of the shell */
int is_shell_builtin(char argv[]);
int execute_shell_builtin(int argc, char **argv);
int user_builtin_command(int argc, char **argv);
void pwd();
void echo(int argc, char **argv);
void cd(int argc, char **argv);
void print_realpath(int argc, char **argv);
#endif

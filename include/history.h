#ifndef __HISTORY_H
#define _HISTORY_H
#define MAX_HISTORY_SIZE 20

/* function to retrieve history */
int update_history(char cmdline[]);
void retrieve_history();
void print_history();
#endif

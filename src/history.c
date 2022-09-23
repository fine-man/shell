#include "../include/libs.h"

static const char history_file_name[] = ".vsh_history";
static char history_file_path[PATH_MAX];
static size_t history_path_len;
static int cur_history_size = 0;
static int MAX_PRINT_SIZE = 10;
/* flag to check if history is already retrieved or not */
static int history_retrieved = 0;
static char history[MAX_HISTORY_SIZE][MAXLINE];

static int get_history_file_path(){
    /* stores the history file path */
    char username[USER_NAME_MAX];
    getusername(username, USER_NAME_MAX); 
    sprintf(history_file_path, "/home/%s/%s\0",username,
            history_file_name);
    history_path_len = strlen(history_file_path);
    return history_path_len;
};

static void save_history() {
    /* save the current history to the file */
    FILE *file = fopen(history_file_path, "w+");

    for (int i = 0; i < cur_history_size; i++) {
        fprintf(file, "%s", history[i]);
    }

    fclose(file);
}

int update_history(char cmdline[]){
    if (cmdline == NULL) return -1;

    if (cur_history_size > 0) {
        if (strcmp(cmdline, history[cur_history_size-1]) == 0) {
            /* command line is same as last command */
            return 0;
        }
    }

    if (cur_history_size < MAX_HISTORY_SIZE) {
        /* if there is space, then store cmdline */
        strcpy(history[cur_history_size], cmdline);
        cur_history_size++;
        save_history(); /* write the updated history to history file */
        return 0;
    }
    
    /* if there is no space, then make space by deleting history[0] */
    for (int i = 1; i < cur_history_size; i++) {
        strcpy(history[i - 1], history[i]);
    }

    /* store the command line in the space made */
    strcpy(history[cur_history_size - 1], cmdline);
    save_history(); /* write the updated history to history file */
    return 0;
};

void print_history() {
    /* print the shell history of the user to stdout */    

    for (int i = MAX_PRINT_SIZE; i >= 1; i--) {
        int j = cur_history_size - i;
        if (j < 0) continue;
        printf("%s", history[j]);
    }
};

void retrieve_history() {
    /* retrieve the history from the history file of the user */

    get_history_file_path();

    cur_history_size = 0;
    FILE *file = fopen(history_file_path, "a+");
    fseek(file, 0, SEEK_SET); /* seeking to the start of file */
    
    while (fgets(history[cur_history_size], MAXLINE, file) != NULL) {
       cur_history_size++; 
       if (cur_history_size > MAX_HISTORY_SIZE) break;
    }

    /* add newline to the last command in history */
    if (cur_history_size > 0) {
        /* length of the last history */
        size_t history_len = strlen(history[cur_history_size-1]);
        if (history[cur_history_size-1][history_len-1] != '\n') {
            history[cur_history_size-1][history_len] = '\n';
        }
    }

    fclose(file);
}

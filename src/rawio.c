#include "../include/libs.h"

static struct termios original_tmode;
extern char cmdline[MAXLINE];
extern int cmd_len;

void enable_raw_mode() {
    int ret_val = tcgetattr(STDIN_FILENO, &original_tmode);
    if (ret_val != 0) {
        fprintf(stderr, "vsh: terminal setup error\n");
        exit(EXIT_FAILURE);
    }

    struct termios tmode = original_tmode;
    tmode.c_lflag &= (!ICANON & !ECHO);

    ret_val = tcsetattr(STDIN_FILENO, TCSAFLUSH, &tmode);
    if (ret_val != 0) {
        fprintf(stderr, "vsh: terminal setup error\n");
        exit(EXIT_FAILURE);
    }
}


void disable_raw_mode() {
    int ret_val = tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_tmode);
    if (ret_val != 0) {
        fprintf(stderr, "vsh: terminal setup error\n");
        exit(EXIT_FAILURE);
    }
}

void backspace() {
    printf("\b \b");
    cmdline[--cmd_len] = '\0';
}

static int is_word_char(int c) {
    if (isalnum(c) || c == '_') return 1;
    return 0;
}

void deleteword() {
    /* delete a word as it is done with c-w */
    int i;
    int word_found = 0;
    int non_word_char = 0;
    for (i = cmd_len - 1; i >= 0; i--) {
        unsigned char c = cmdline[i];
        if (word_found == 1) {
            if (is_word_char(c)) printf("\b \b");
            else if (isprint(c)) break;
        }
        else if (non_word_char == 1) {
            if (is_word_char(c) || isspace(c)) break;
            else printf("\b \b");
        }
        else if (is_word_char(c)) {
            printf("\b \b");
            word_found = 1;
        }
        else if (!isspace(c)) {
            printf("\b \b");
            non_word_char = 1;
        }
        else printf("\b \b");
    }

    cmdline[i+1] = '\0';
    cmd_len = i + 1;
}

void deleteline() {
    int i;
    for (i = cmd_len - 1; i >= 0; i--) {
        printf("\b \b");
    }
    cmdline[0] = '\0';
    cmd_len = 0;
}

void print_buffer() {
    for (int i = 0; i < cmd_len; i++) {
        printf("%c", cmdline[i]);
    }
}

int take_input() {
    cmdline[0] = '\0';
    cmd_len = 0;
    int is_eof = 0;
    enable_raw_mode();
    unsigned char c;
    while ((c = getchar()) != EOF) {
        if (isprint(c)) {
            /* any printable character */
            cmdline[cmd_len++] = c;
            cmdline[cmd_len] = '\0';
            printf("%c", c);
        }
        else if (c == '\n') {
            /* newline */
            cmdline[cmd_len++] = c;
            cmdline[cmd_len] = '\0';
            printf("%c", c);
            break;
        }
        else if (c == 127) {
            /* backspace */
            if (cmd_len > 0) backspace();
        }
        else if (c == 23) {
            /* c-w */
            deleteword();
        }
        else if (c == 21) {
            /* c-u */
            deleteline();
        }
        else if (c == 3) {
            /* c-c */
            cmdline[0] = '\0';
            cmd_len = 0;
            printf("\n");
            displayprompt();
        }
        else if (c == 4) {
            /* eof character */
            if (cmd_len == 0) {
                is_eof = 1;
            }
            else {
                printf("\n");
                cmdline[cmd_len++] = '\n';
            }
            break;
        }
        else if (c == 9) {
            /* tab */
            int matches = autocomplete();
            if (matches > 1) {
                displayprompt();
                print_buffer();
            }
        }
        else {
            /* ignore */
        }
    }

    disable_raw_mode();
    cmdline[cmd_len] = '\0';
    if (is_eof) return -1;
    return cmd_len;
}

/*
int main() {
    int ret_val = take_input();
    if (ret_val == -1) {
        exit(EXIT_SUCCESS);
    }
    for (int i = 0; i < cmd_len; i++) {
        printf("%c", cmdline[i]);
    }
}
*/

#include "../include/libs.h"
extern char cmdline[MAXLINE];
extern int cmd_len;

int extract_last_word(char dest[], char src[], size_t len) {
    /* extract the last word from src buffer and store in
     * dest buffer, a word is any printable character except for
     * tab and space */

    int i;
    int src_len = strlen(src);
    for (i = src_len - 1; i >= 0; i--) {
        /* loop till a space character comes */
        if (isspace(src[i])) break;
    }

    if (len < src_len - i + 1) {
        /* dest buffer length is too small */
        return -1;
    }

    int k = 0;
    for (int j = i + 1; j < src_len; j++) {
        /* copy the last word in the dest buffer */
        dest[k++] = src[j];
    }

    dest[k] = '\0';
    return k;
}

static int scandir_filter(const struct dirent *dir) {
    if (dir == NULL) return 0;
    if (dir->d_name[0] == '.') return 0;
    return 1;
}

int print_suggestions(char directory[], char file_name[]) {
    struct dirent **namelist;
    int n; /* number of elements in namelist */
    int directory_len = strlen(directory); 
    int file_name_len = strlen(file_name);

    n = scandir(directory, &namelist, scandir_filter, alphasort);
    if (n == -1) {
        return -1;
    }
    
    /* list of all the matching entries */
    struct dirent *match_list[n + 10];
    int match_list_size = 0;

    for (int i = 0; i < n; i++) {
        match_list[i] = NULL;
        if (strncmp(namelist[i]->d_name, file_name, file_name_len) == 0) {
            /* file_name is a prefix of d_name */
            match_list[match_list_size++] = namelist[i];
        }
    }
    
    if (match_list_size == 0) {
        /* no matches found */
        if (namelist != NULL) free(namelist);
        return 0;
    }

    if (match_list_size == 1) {
        /* there was only 1 match found */
        char *match_name = match_list[0]->d_name;
        int len = strlen(match_name);

        for (int i = file_name_len; i < len; i++) {
            cmdline[cmd_len++] = match_name[i];
            printf("%c", match_name[i]);
        }

        if (match_list[0]->d_type == DT_DIR) {
            /* match is a directory */
            cmdline[cmd_len++] = '/';
            printf("/");
        }
        cmdline[cmd_len] = '\0';
        return 1;
    }
    
    /* There are multiple matches */
    size_t prefix_len = strlen(match_list[0]->d_name);
    char prefix[prefix_len + 1];
    strcpy(prefix, match_list[0]->d_name);

    /* calculating the common prefix among all the matches */
    for (int i = 1; i < match_list_size; i++) {
        char *match_name = match_list[i]->d_name;
        int j = 0, k = 0;
        while (prefix[j] == match_name[k]) {
            j++; k++;
        }

        prefix[j] = '\0';
        prefix_len = j;
    }

    /* printing the common prefix */
    for (int i = file_name_len; i < prefix_len; i++) {
        cmdline[cmd_len++] = prefix[i];
        printf("%c", prefix[i]);
    }

    cmdline[cmd_len] = '\0';

    /* printing all the found matches */
    printf("\n");
    for (int i = 0; i < match_list_size; i++) {
        printf("%s\n", match_list[i]->d_name);
    }

    return match_list_size;
}

int get_dirname(char dest[], char path[], size_t len) {
    /* stores the dirname of a path in the dest buffer */
    /* returns the length of stored directory name in dest */

    char *dname; /* pointer to the directory name */

    size_t path_len = strlen(path);

    if (path_len == 1 && path[0] == '~') {
        /* path is just ~ */
        dest[0] = '~';
        dest[1] = '\0';
        return 1;
    }

    if (path[path_len - 1] == '/') {
        /* path is a directory */
        dname = path;
    }
    else dname = dirname(path); /* get the directory name */

    size_t dname_len = strlen(dname);
    if (len < dname_len + 1) {
        /* buffer is too small for directory path */
        return -1;
    }

    strcpy(dest, dname); /* copy dirname in the dest buffer */
    return dname_len;
}

int get_basename(char dest[], char path[], size_t len) {
    /* stores the basename of a path in the dest buffer */
    /* if path == '/', then result is '' */

    size_t path_len = strlen(path);
    if (path[path_len - 1] == '/' || strcmp(path, "~") == 0) {
        /* path is a directory or just ~ */
        dest[0] = '\0';
        return 0;
    }

    char *bname = basename(path);
    size_t bname_len = strlen(bname);
    if (len < bname_len + 1) {
        /* buffer is too small for directory path */
        return -1;
    }
    
    strcpy(dest, bname);
    return bname_len;
}

int autocomplete() {
    /* autocomplete command-line path */
    if (cmd_len <= 0) return -1;
    /* return if last character was a space */
    if (isspace(cmdline[cmd_len - 1])) return -1;

    /* extract the directory name and the partial file-name */
    char last_word[PATH_MAX], copy_last_word[PATH_MAX];
    int ret_val = extract_last_word(last_word, cmdline, PATH_MAX);
    if (ret_val <= 0) return -1;
    
    /* copy the last_word buffer as get_dirname
     * might change last_word */
    strcpy(copy_last_word, last_word);

    char dir_path[PATH_MAX], file_name[PATH_MAX];
    char resolved_path[PATH_MAX]; /* path with ~ resolved */

    /* Get the directory name */
    if (get_dirname(dir_path, last_word, PATH_MAX) == -1) {
        return -1;
    }

    /* resolve the ~ in the path if it is there */
    if (resolve_tilda(resolved_path, PATH_MAX, dir_path) == NULL) {
        return -1;
    }
    
    /* Get the basename */
    if (get_basename(file_name, copy_last_word, PATH_MAX) == -1) {
        return -1;
    }

    int matches = print_suggestions(resolved_path, file_name);

    /*
    printf("\ndirectory name = %s\nfile name = %s\n", resolved_path,
            file_name);
            */

    return matches;
}

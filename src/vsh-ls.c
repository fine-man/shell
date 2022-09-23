#include "../include/libs.h"

/* check out the NOTES section of man 3 getopt */
extern char *optarg; /* defined in getopt.h */
extern int optind, opterr, optopt; /* defined in getopt.h */

/* flags -a and -l for ls */
static int flag_a, flag_l, f_help,
f_multiple_arguments;

static void print_usage() {
    /* print the usage of the ls program */
    printf("Usage: ls [al]... [FILE]...\n");
}

static void print_help() {
    /* prints the help for ls */
    /* TODO : write a separate function for help */
    print_usage();
}

static void reset_ls_flags() {
    flag_a = 0, flag_l = 0, f_help = 0;
    f_multiple_arguments = 0;
}

static int ls_get_options(int argc, char **argv) {
    /* parses the command-line to get all the options
     * and set the appropriate flag */

    int opt; /* return value of getopt_long */

    /* struct option for long options */
    const struct option longopts[] = 
    {
        {"all", no_argument, NULL, 'a'},
        {"help", no_argument, &f_help, 1}
    };

    optind = 0; /* set this to 0 before every new getopt call */
    optarg = NULL;
    int longindex = 0; /* index for longopts struct array */

    /* getting the command line options */
    while ((opt = getopt_long(argc, argv, ":al", longopts, &longindex)) != -1) {
        switch(opt) {
            case 0:
                break;
            case 'a':
                flag_a = 1;
                break;
            case 'l':
                flag_l = 1;
                break;
            case '?':
                fprintf(stderr, "ls: invalid option -- \'%c\'\n", optopt);
                print_usage();
                //exit(EXIT_FAILURE);
                break;
            case ':':
                fprintf(stderr, "Missing option\n");
                print_usage();
                break;
            default:
                print_usage();
                //exit(EXIT_FAILURE);
        }
    }

    /* print help */
    if (f_help) {
        print_help();
        return -1;
        //exit(EXIT_SUCCESS);
    }

    return 0;
}

static char *get_fileperms_string(mode_t mode) {
    static const char perms[] = {'x', 'w', 'r'};

    static char buf[10]; 
    int k = 0; /* index for "buf" */

    for (int i = 2; i >= 0; i--) { /* user, group, others */
        for (int j = 2; j >= 0; j--) { /* read, write, execute */
            int bit_offset = (3 * i + j);
            
            if (mode & (1 << bit_offset)) {
                /* mode bit has that permission */
                buf[k++] = perms[j];
            }
            else buf[k++] = '-';
        }
    }

    buf[k] = '\0';

    return buf;
}

static char *get_file_username(uid_t uid) {
    struct passwd *pwd = getpwuid(uid);
    if (pwd == NULL) return NULL;
    return pwd->pw_name;
}

static char *get_file_groupname(gid_t gid) {
    struct group *grp = getgrgid(gid);
    if (grp == NULL) return NULL;
    return grp->gr_name;
}

static void print_file_color(mode_t mode, char *name) {
    /* prints the file name according to type of the file */

    if (S_ISDIR(mode)) {
        /* blue for directory */
        printf(COLOR_BOLD);
        printf(ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET, name);
        printf(COLOR_BOLD_OFF);
    }
#ifdef S_ISLNK
    else if (S_ISLNK(mode)) {
        /* cyan for symlinks */
        printf(COLOR_BOLD);
        printf(ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET, name);
        printf(COLOR_BOLD_OFF);
    }
#endif
    else if (is_executable(mode)) {
        /* green for executable */
        printf(COLOR_BOLD);
        printf(ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET, name);
        printf(COLOR_BOLD_OFF);
    }
    else {
        /* white for other files */
        printf("%s", name);
    }
}

static void print_file_info(struct stat *sb, char *name){
    if (sb == NULL) return;

    char perms[11];
    printf("%c", get_filetype_letter(sb->st_mode)); /* filetype letter */
    printf("%s ", get_fileperms_string(sb->st_mode)); /* file permissions */
    printf("%ju ", (uintmax_t) sb->st_nlink); /* Link count */

    char *username = get_file_username(sb->st_uid);
    if (username != NULL) {
        printf("%s ", username);
    }

    char *groupname = get_file_groupname(sb->st_gid);
    if (groupname != NULL) {
        printf("%s ", groupname);
    }

    printf("%jd ", (intmax_t) sb->st_size); /* file size */

    char date[21];
    strftime(date, 20, "%b %d %H:%M", localtime(&(sb->st_ctime)));
    printf("%s ", date);

    print_file_color(sb->st_mode, name);
};

static int ls_filter(const struct dirent *dir) {
    if (dir == NULL) return 0;
    if (dir->d_name[0] == '.') return 0;
    return 1;
}

static int ls_dir(char *dir_path) {
    struct dirent **namelist;
    int n; /* number of elements in namelist */

    /* filter function for filtering files/directories in dir_path */
    int (*filter)(const struct dirent *);
    if (flag_a) filter = NULL;
    else filter = ls_filter;

    n = scandir(dir_path, &namelist, filter, alphasort);
    if (n == -1) {
        return -1;
    }

    /* print the directory name if there are multiple arguments */
    if (f_multiple_arguments) printf("%s:\n", dir_path);

    size_t dir_path_len = strlen(dir_path);
    char file_path[PATH_MAX];
    size_t file_path_len = dir_path_len;
    char separator = '\0';
    if (dir_path[dir_path_len - 1] != '/') {
        file_path_len += 1;
        separator = '/';
    }

    sprintf(file_path, "%s%c", dir_path, separator);
    file_path[file_path_len] = '\0';

    int total = 0; /* total for ls -l */

    for (int i = 0; i < n; i++) {
        file_path[file_path_len] = '\0';
        strcat(file_path, namelist[i]->d_name);
        
        struct stat *sb = get_lstatbuf(file_path);
        if (sb == NULL) continue;

        if (flag_l) {
            total += sb->st_blocks;
        }
        else {
            print_file_color(sb->st_mode, namelist[i]->d_name);
            printf(" ");
        }
    }
    
    /* ls -l */
    if (flag_l) {
        total /= 2;
        printf("total: %d\n", total);
        
        for (int i = 0; i < n; i++) {
            file_path[file_path_len] = '\0';
            strcat(file_path, namelist[i]->d_name);
            
            struct stat *sb = get_lstatbuf(file_path);
            if (sb == NULL) continue;

            print_file_info(sb, namelist[i]->d_name);
            printf("\n");
        }
    }
    else printf("\n");

    for (int i = 0; i < n; i++) {
        if (namelist[i] == NULL) continue;
        free(namelist[i]);
    }
}

static int ls_non_dir(char *path) {
    if (path == NULL) return 0;

    /* making the struct stat buffer for file */
    struct stat *sb = (struct stat *) malloc(sizeof(struct stat));

    /* getting information about the file using lstat */
    int ret = lstat(path, sb);
    if (ret != 0) {
        if (sb != NULL) free(sb);
        return -1;
    }

    if (flag_l) {
        print_file_info(sb, path);
    }
    else {
        /* just print the file name with color coding */
        print_file_color(sb->st_mode, path);
    }

    if (sb != NULL) free(sb);
}

static void ls_list_files(char **files, int num_files) {
    /* prints info about a list of non directory files */

    if (files == NULL || num_files == 0) return;

    int flag_non_dir = 0; /* flag to see some non-dir was successfull*/
    for (int i = 0; i < num_files; i++) {
        if (files[i] == NULL) continue;

        /* print info about this non-directory file */
        int ret = ls_non_dir(files[i]);
        if (ret == -1) {
            fprintf(stderr, "ls: cannot access %s: %s\n",
                    files[i], strerror(errno));
            continue;
        }

        flag_non_dir = 1;
        if (flag_l) {
            printf("\n");
        }
        else printf(" ");
    }

    if (flag_non_dir) printf("\n\n");
}

int ls(int argc, char **argv) {
    
    reset_ls_flags(); /* reset the flags for ls */

    /* parse the command-line options */
    int ret_val = ls_get_options(argc, argv);
    if (ret_val != 0) {
        return -1;
    }

    char **directories = NULL; /* string array for all directory paths*/
    char **non_directories = NULL; /* string array for all non-directory paths */
    int num_non_directories = 0, num_directories = 0;

    if (optind == argc) {
        /* there are no arguments present */
        /* This is equivalent of ls . */
        /* add . to the list of directories */

        /* char array for current directory (.) */
        char *path = (char *) malloc(2 * sizeof(char));
        path[0] = '.', path[1] = '\0';

        /* increasing the size of directories array */
        /* +2 because of NULL and the new path */
        directories = (char **) reallocarray(directories,
                num_directories + 2, sizeof(char *));

        if (directories == NULL) {
            fprintf(stderr, "ls: reallocarray: %s\n", strerror(errno));
            /* freeing the memory in case of failure */
            for (int i = 0; i < num_directories; i++) free(directories[i]);
            return -1;
            //exit(EXIT_FAILURE);
        }
        
        directories[num_directories++] = path;
        directories[num_directories] = NULL;
    }

    /* getting the command line arguments and separating directories and
     * non-directories */
    while (optind < argc) {
        char *path = resolve_tilda(NULL, 0, argv[optind]); /* allocated on heap */

        if (does_path_exists(path) == -1) {
            fprintf(stderr, "ls: cannot access \'%s\': %s\n",
                    path, strerror(errno));
        }
        else {
            if (is_directory(path)) {
                /* path is a directory */

                /* increasing the size of directories array */
                /* +2 because of NULL and the new path */
                directories = (char **) reallocarray(directories,
                        num_directories + 2, sizeof(char *));

                if (directories == NULL) {
                    fprintf(stderr, "ls: reallocarray: %s\n", strerror(errno));
                    /* freeing the memory in case of failure */
                    for (int i = 0; i < num_directories; i++) free(directories[i]);
                    return -1;
                    //exit(EXIT_FAILURE);
                }
                
                directories[num_directories++] = path;
                directories[num_directories] = NULL;
            }
            else {
                /* path is not a directory */

                /* increasing the size of the char array */
                /* +2 because of NULL and the new path */
                non_directories = (char **) reallocarray(non_directories,
                        num_non_directories + 2, sizeof(char *));

                /* realloc error handling */
                if (non_directories == NULL) {
                    fprintf(stderr, "ls: reallocarray: %s\n", strerror(errno));
                    /* freeing the memory in case of failure */
                    for (int i = 0; i < num_non_directories; i++) {
                        free(non_directories[i]);
                    }
                    return -1;
                    //exit(EXIT_FAILURE);
                }
                
                /* storing pointer to new path */
                non_directories[num_non_directories++] = path;
                //printf("path: %s\n", non_directories[num_non_directories]
                non_directories[num_non_directories] = NULL;
            }
        }

        optind++;
    }

    if (num_directories + num_non_directories > 1) {
        f_multiple_arguments = 1;
    }

    /* print info about all the non-directory files first */
    ls_list_files(non_directories, num_non_directories);

    for (int i = 0; i < num_non_directories; i++) {
        if (non_directories[i] == NULL) continue;
        free(non_directories[i]);
    }

    /* print info about all the directories */
    for (int i = 0; i < num_directories; i++) {
        if (directories[i] == NULL) continue;

        int ret_val = ls_dir(directories[i]);
        if (ret_val == -1) {
            fprintf(stderr, "ls: cannot access %s: %s\n",
                    directories[i], strerror(errno));
        }
        else printf("\n");
        
        free(directories[i]);
    }

    return 0;
}

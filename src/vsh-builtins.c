#include "../include/libs.h"

void pwd() {
    char path[PATH_MAX + 1];
    char *ret_val = getcwd(path, PATH_MAX + 1);

    if (ret_val == NULL) {
       cmd_error("getcwd error"); 
       return;
    }

    printf("%s\n", path);
    return;
}

void echo(int argc, char **argv) {
    int i;
    for (i = 1; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n");
}

void cd(int argc, char **argv) {
    
    /* return if number of arguments is > 2 */
    if (argc > 2) {
        fprintf(stderr, "cd: expected 1 argument given %d\n", argc);
        return;
    }

    int ret_int; char *ret_char;
    char resolved_path[PATH_MAX];
    char cur_pwd[PATH_MAX];

    /* Get the home directory from the environment */
    const char *homedir = getenv("HOME");
    if (homedir == NULL) {
        fprintf(stderr, "cd: getenv: %s\n", strerror(errno));
        return;
    }

    /* Get the current pwd and store it in cur_pwd */
    ret_char = getcwd(cur_pwd, PATH_MAX);
    if (ret_char == NULL) {
        fprintf(stderr, "cd: getcwd: %s\n", strerror(errno));
        return;
    }

    /* if there are no additional arguments, then cd to 
     * home directory */
    if (argc == 1) {
        /* do cd ~ */
        strcpy(resolved_path, homedir);
    }
    else if (argc == 2 && (strcmp(argv[1], "-") == 0)) {
        /* cd - */
        /* Get the value of $OLDPWD environment variable */
        const char *old_pwd = getenv("OLDPWD");
        if (old_pwd == NULL) {
            fprintf(stderr, "cd: getenv: %s\n", strerror(errno));
            return;
        }
        strcpy(resolved_path, old_pwd);
    }
    else {
        /* store the absolute path in resolved_path buffer */
        ret_char = getrealpath(resolved_path, PATH_MAX, argv[1]);
        if (ret_char == NULL) {
            fprintf(stderr, "cd: getrealpath error\n");
            return;
        }
    }
    
    /* change the directory */
    ret_int = chdir(resolved_path);
    if (ret_int != 0) {
        fprintf(stderr, "cd: chdir: %s\n", strerror(errno));
        return;
    }
    
    /* change the $PWD environment variable to current path */
    ret_int = setenv("PWD", resolved_path, 1);
    if (ret_int != 0) {
        fprintf(stderr, "cd: setenv: %s\n", strerror(errno));
        return;
    }
    
    /* change the $OLDPWD environment variable to the old path */
    ret_int = setenv("OLDPWD", cur_pwd, 1);
    if (ret_int != 0) {
        fprintf(stderr, "cd: setenv: %s\n", strerror(errno));
        return;
    }

    /* printing the changed directory path */
    printf("%s\n", resolved_path);
}

void print_realpath(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "print_realpath: expected 2 argumets \
                given %d\n", argc);
        return;
    }

    char *path = getrealpath(NULL, 0, argv[1]);
    if(path == NULL) {
        fprintf(stderr, "print_realpath error\n");
        return;
    }
    else {
        printf("path: %s\n", path);
    }
}

int is_shell_builtin(char argv[]) {
    /* tells if a command is shell builtin */
    if (strcmp(argv, "cd") == 0) {
        return 1;
    }
    else if (strcmp(argv, "fg") == 0) {
        return 1;
    }
    else if (strcmp(argv, "bg") == 0) {
        return 1;
    }
    else if (strcmp(argv, "sig") == 0) {
        return 1;
    }
    else if (strcmp(argv, "exit") == 0) {
        return 1;
    }

    return 0;
}

int execute_shell_builtin(int argc, char **argv) {
    /* execute the shell builtins */
    if (strcmp(argv[0], "cd") == 0) {
        cd(argc, argv);
        return 1;
    }
    else if (strcmp(argv[0], "fg") == 0) {
        fg(argc, argv);
        return 1;
    }
    else if (strcmp(argv[0], "bg") == 0) {
        /* add the function for bg here */
        bg(argc, argv);
        return 1;
    }
    else if (strcmp(argv[0], "sig") == 0) {
        sig(argc, argv);
        return 1;
    }
    else if (strcmp(argv[0], "exit") == 0) {
        vsh_exit(argc, argv);
    }

    /* command was not a shell builtin */
    return 0;
}

int user_builtin_command(int argc, char **argv) {
    /* execute the command argv[0] if it a built in command */
    /* return value is 1 when argv[0] is a built in command and
     * 0 otherwise */

    if (strcmp(argv[0], "pwd") == 0) {
        pwd();
        return 1;
    }
    else if (strcmp(argv[0], "echo") == 0) {
        echo(argc, argv);
        return 1;
    }
    else if (strcmp(argv[0], "realpath") == 0) {
        print_realpath(argc, argv);
        return 1;
    }
    else if (strcmp(argv[0], "history") == 0) {
        print_history();
        return 1;
    }
    else if (strcmp(argv[0], "ls") == 0) {
        ls(argc, argv);
        return 1;
    }
    else if (strcmp(argv[0], "pinfo") == 0) {
        pinfo(argc, argv);
        return 1;
    }
    else if (strcmp(argv[0], "jobs") == 0) {
        list_jobs(argc, argv);
        return 1;
    }

    return 0;
}

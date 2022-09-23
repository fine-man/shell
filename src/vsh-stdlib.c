#include "../include/libs.h"

char *getrelativepath(char *buf, size_t len, char *absolute_path) {
    /* returns a char pointer to the path relative to home directory */
    /* returns null if len is not big enough to store the relative path */

    /* Note : if "buf" = NULL, then it allocates a buffer of appropriate size 
     * using malloc which the user has to free */
    
    /* check if absolute path string is NULL */
    if (absolute_path == NULL) {
        //cmd_error("getrelativepath error : absolute path string is NULL\n");
        return NULL;
    }

    int is_null = 0;
    /* checks if the buffer string is null or not */
    if (buf == NULL) {
        is_null = 1;
        len = 0; /* precautionary measure */
    }

    int prefix;
    
    /* Get the home directory from the environment */
    const char *homedir = getenv("HOME");
    if (homedir == NULL) {
        //cmd_error("getrelativepath: getenv: \"HOME\" environment is not \
                available\n");
        return NULL;
    }
    size_t homedir_len = strlen(homedir); /* length of the home directory */

    /* check if home directory is the prefix of absolute path */
    if ((strncmp(homedir, absolute_path, homedir_len) == 0) &&
        (absolute_path[homedir_len] == '/' || 
         absolute_path[homedir_len] == '\0')) {
        prefix = 1; /* home directory is prefix */
    }
    else prefix = 0; /* not a prefix */

    if (!prefix) {
        /* if home directory is not the prefix, then return absolute_path */
        size_t abs_len = strlen(absolute_path); /* length of the absolute path string */
        size_t required_len =  abs_len + 1; /* required length of the relative path */

        if (required_len > PATH_MAX) {
            //fprintf(stderr, "getrelativepath: required buffer size exceeds\
                    PATH_MAX: %lld\n", PATH_MAX);
            return NULL;
        }

        if (is_null) {
            len = required_len;
            buf = (char *) malloc(sizeof(char) * len);
        }

        if (len < abs_len + 1) {
            //fprintf(stderr, "getrelativepath : buffer size is smaller than\
                    path size, path : %s\n", absolute_path);
            return NULL;
        }

        strncpy(buf, absolute_path, abs_len);
        buf[abs_len] = '\0';
    }
    else {
        /* home directory is a prefix of absolute path */

        absolute_path = &absolute_path[homedir_len];
        if ((*absolute_path == '/')) absolute_path++;
        
        size_t abs_len = strlen(absolute_path); /* length of absolute path */

        char delimeter[2] = "\0\0";
        /* if absolute path != Home directory */
        if (abs_len) delimeter[0] = '/';
        
        size_t required_len = abs_len + 2 + strlen(delimeter);

        if (required_len > PATH_MAX) {
            //fprintf(stderr, "getrelativepath: required buffer size exceeds\
                    PATH_MAX: %lld\n", PATH_MAX);
            return NULL;
        }

        /* allocate a buffer of appropriate size if it was null */
        if (is_null) {
            len = required_len;
            buf = (char *) malloc(sizeof(char) * len);
        }

        /* print error message and return null if buffer size is not
         * big enough */
        if (len < required_len) {
            //fprintf(stderr, "getrelativepath : buffer size is smaller than\
                    path size, path : %s\n", absolute_path);
            return NULL;
        }

        sprintf(buf, "~%s%s", delimeter, absolute_path);
    }

    return buf;
}

char *getrealpath(char *resolved_path, size_t len, char *relative_path) {
    /* returns a char pointer to the resolved path of a relative path */
    /* returns null if len is not big enough to store the resolved path */

    /* Note : if "resolved_path" = NULL, then it allocates a buffer of 
     * appropriate size using malloc which the user has to free */
    
    /* check if relative path string is NULL */
    if (relative_path == NULL) {
        //fprintf(stderr, "getrealpath error : relative path string is NULL\n");
        return NULL;
    }
    size_t relative_len = strlen(relative_path); /* length of the relative path string */

    int is_null = 0;
    /* checks if the buffer string is null or not */
    if (resolved_path == NULL) {
        is_null = 1;
        len = 0; /* precautionary measure */
    }

    int has_homedir = 0;
    /* check if the path starts with home directory */
    if (relative_path[0] == '~') {
        has_homedir = 1;
        relative_path++;
    }

    /* Get the home directory from the environment */
    const char *homedir = getenv("HOME");
    if (homedir == NULL) {
        //fprintf(stderr, "getrealpath: getenv: %s\n", strerror(errno));
        return NULL;
    }
    size_t homedir_len = strlen(homedir); /* length of the home directory */
    
    char path[PATH_MAX]; /* buffer for ~ resolved path */
    size_t required_len = relative_len + 1;
    if (has_homedir) required_len += homedir_len;

    /* return NULL if required length is > PATH_MAX */
    if (required_len > PATH_MAX) {
        //fprintf(stderr, "Length of resolved path is longer than \
                PATH_MAX: %lld\n", PATH_MAX);
        return NULL;
    }

    /* resolve the ~ in the path */
    if (has_homedir) {
        /* path has ~ */
        sprintf(path, "%s%s", homedir, relative_path);
    }
    else {
        /* path doesn't have ~ */
        strncpy(path, relative_path, relative_len);
        path[relative_len] = '\0';
    }

    /* get the real path with . and .. resolved */
    char *real_path = realpath(path, NULL);
    if (real_path == NULL) {
        //fprintf(stderr, "getrealpath: realpath: %s\n", strerror(errno));
        return NULL;
    }

    if (is_null) {
        /* if resolved path is NULL */
        return real_path;
    }
    else {
        /* if resolved path is not NULL */
        size_t realpath_len = strlen(real_path);
        if (len < realpath_len + 1) {
            //fprintf(stderr, "getrealpath: buffer size is smaller than \
                    resolved path size, realpath: %s\n", real_path);
            free(real_path);
            return NULL;
        }

        strncpy(resolved_path, real_path, realpath_len);
        resolved_path[realpath_len] = '\0';
        free(real_path);
        return resolved_path;
    }
}

char *resolve_tilda(char *resolved_path, size_t len, char *path) {
    /* returns a char pointer to the resolved path of a relative path */
    /* returns null if len is not big enough to store the resolved path */

    /* Note : if "resolved_path" = NULL, then it allocates a buffer of 
     * appropriate size using malloc which the user has to free */
    
    /* check if relative path string is NULL */
    if (path == NULL) {
        //fprintf(stderr, "getrealpath error : relative path string is NULL\n");
        return NULL;
    }
    size_t path_len = strlen(path); /* length of the relative path string */

    int is_null = 0;
    /* checks if the buffer string is null or not */
    if (resolved_path == NULL) {
        is_null = 1;
        len = 0; /* precautionary measure */
    }

    int has_homedir = 0;
    /* check if the path starts with home directory */
    if (path[0] == '~') {
        has_homedir = 1;
        path++;
    }

    /* Get the home directory from the environment */
    const char *homedir = getenv("HOME");
    if (homedir == NULL) {
        //fprintf(stderr, "getrealpath: getenv: %s\n", strerror(errno));
        return NULL;
    }
    size_t homedir_len = strlen(homedir); /* length of the home directory */
    
    size_t required_len = path_len; /* required length of the resolved string */
    if (has_homedir) required_len += homedir_len;

    /* return NULL if required length is > PATH_MAX */
    if (required_len > PATH_MAX) {
        //fprintf(stderr, "Length of resolved path is longer than \
                PATH_MAX: %lld\n", PATH_MAX);
        return NULL;
    }

    /* allocating the buffer resolved_path if it is NULL */
    if (is_null) {
        /* if resolved_path variable is NULL */

        /* mallocing the buffer */
        resolved_path = (char *) malloc((required_len + 1) * sizeof(char));

        if (resolved_path == NULL) {
            /* bad alloc error */
            //fprintf(stderr, "resolve_tilda: bad alloc error: %s\n", strerror(errno));
            return NULL;
        }
    }
    else {
        /* if resolved_path variable is not NULL */
        if (len < required_len + 1) {
            /* length of buffer is not enough */
            return NULL;
        }
    }

    /* resolve the ~ in the path */
    if (has_homedir) {
        /* path has ~ */
        sprintf(resolved_path, "%s%s", homedir, path);
    }
    else {
        /* path doesn't have ~ */
        strncpy(resolved_path, path, path_len);
    }

    resolved_path[required_len] = '\0';

    return resolved_path;
}

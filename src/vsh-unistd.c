#include "../include/libs.h"

int getusername(char *name, size_t len) {
    /* stores the username in the buffer pointed by "name" */
    /* Return Value
     * on successfull run, it returns length of hostname, if
     * "len" is less than hostname, then it returns -1 */

    uid_t uid = geteuid();
    struct passwd *pw = Getpwuid(uid);
    size_t name_len;
    if (pw != NULL) {
        name_len = strlen(pw->pw_name);
        if (name_len > len) return -1;
        strcpy(name, pw->pw_name);
    }

    return name_len;
}

#ifndef _LIBS_
#define _LIBS_
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h> // for open
#include <sys/stat.h> // for mkdir, fstat, stat
#include <unistd.h> // for write, sleep, lseek
#include <errno.h> // for errno
#include <sys/wait.h> // for wait
#include <sys/types.h> // for system types like uid_t
#include <pwd.h> // for struct passwd
#include <limits.h> // for PATH_MAX
#include <stdint.h> // for uintmax_t
#include <grp.h>
#include <time.h>
#include <signal.h>
#include <dirent.h>
#include <getopt.h>
#include <termios.h>
#include <ctype.h>
#include <libgen.h>
#include "colors.h"
#include "wrappers.h"
#include "vsh-stdlib.h"
#include "vsh-unistd.h"
#include "vsh-signal.h"
#include "vsh-builtins.h"
#include "vsh-utils.h"
#include "vsh-stat.h"
#include "vsh-ls.h"
#include "history.h"
#include "pinfo.h"
#include "signal_handlers.h"
#include "process_signals.h"
#include "rawio.h"
#include "autocomplete.h"
#include "jobs.h"
#include "job-control.h"
#include "parser.h"
#include "fg.h"
#include "bg.h"
#include "sig.h"
#include "list-jobs.h"
#include "vsh-exit.h"
#include "shell.h"

#endif

# OSN Shell

## usefull syscalls/functions/links
- [Write your own shell - purdue](https://www.cs.purdue.edu/homes/grr/SystemsProgrammingBook/Book/Chapter5-WritingYourOwnShell.pdf)
- [Write a shell in C](https://brennan.io/2015/01/16/write-a-shell-in-c/)
- [list of all syscalls in C](https://man7.org/linux/man-pages/man2/syscalls.2.html)
- [colored output using C](https://stackoverflow.com/questions/3219393/stdlib-and-colored-output-in-c#comment3321207_3219471)

### System info
- [get the username in Linux- stackoverflow](https://stackoverflow.com/questions/8953424/how-to-get-the-username-in-c-c-in-linux)
- [get user and system name in Linux - stackoverflow](https://stackoverflow.com/questions/8723956/how-to-print-the-current-user-and-system-name-in-unix)
- uname - stores system information in a struct utsname buf
- gethostname - stores the system name in a buffer
- getuid - returns the user id
- getpwuid - returns a pointer to a structure containting user info
- getlogin_r - stores the username in the array "buf", not robust
- getcwd - stores the absolute pathname in "buf"
- chdir - takes in absolute path and changes directory

### Process control
- wait
- waitpid

### Signal handling
- signal - install a signal handler
- sigprocmask - block/unblock some signals
- sigemptyset - initialize an empty sigset_t
- sigfillset - fills the set sigset_t
- sigaddset - adds a signal number to the set
- sigdelset - deletes a signal number from the set
- sigismember - checks if a signal is part of the set
- sigaction - adds a signal handler

### Command-line arguments
- [Tokenize command line arguments in C]
- getopt, getopt_long

### File/directory info
- [checking if a file/directory exists in unix](https://stackoverflow.com/questions/3828192/checking-if-a-directory-exists-in-unix-system-call)
- stat/lstat
- scandir/opendir/readdir
- man 7 inode

### String parsing

Different header files are made separately for shell-builtin commands(like pwd, echo, cd) and ls has seperate header file.

For part-1 :
    I'm using utsname and getcwd commands.

For part-2 :
    For cd, I'm using chdir to chage directory.
    For pwd, I'm using getcwd command.
    For echo, I'm removing extra spaces and quotes and printing.

For part-3 :
    I'm using struct dirent to iterate over directory, struct stat for -l and -la flags.

For part-4 :
    I'm using fork to create a copy of process ad exec to replace child process with required process.

For part-5 :
    I'm printing details of a process from /proc/<pid>/status file and executable path from /proc/<pid>/cmdline.

For part-6 :
    I'm using waitpid command and signal to get a signal when child termiates with WNOHANG as third argumant in waitpid.

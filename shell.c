#include<stdio.h>
#include<string.h>
#include<sys/utsname.h>
#include<dirent.h>
#include<unistd.h>
#include<stdlib.h>
#include "executer.h"

int main(void) {
    char homepath[100];
    printf("\033c");
    struct utsname userData;
    uname(&userData);
    char input[100], temp, path[100], printpath[100];
    getcwd(homepath, 100);
    int homelen = strlen(homepath);
    while(1) {
        getcwd(path, 100);
        if(strlen(path) >= homelen) {
            int flag = 1;
            for(int i=0;i<homelen;i++) {
                if(path[i] != homepath[i]) {
                    flag = 0;
                    break;
                }
            }
            if(flag == 1) {
                printpath[0] = '~';
                int point = 1;
                for(int i=homelen;i<strlen(path);i++) {
                    printpath[point++] = path[i];
                }
                printpath[point] = '\0';
                printf("<%s@%s:%s> ", userData.nodename, userData.sysname, printpath);
            } else {
                printf("<%s@%s:%s> ", userData.nodename, userData.sysname, path);
            }
        } else {
            printf("<%s@%s:%s> ", userData.nodename, userData.sysname, path);
        }
        input[0] = '\0';
        scanf("%[^\n]s", input);
        scanf("%c", &temp);
        if(strlen(input) == 0) {
            continue;
        }
        int stdinfd = dup(0), stdoutfd = dup(1);
        char **splitpipe = splitpipes(input);
        char **inredsplit, **outredsplit;
        int pipecnt = 0, retval, wfd, rfd;
        while(strlen(splitpipe[pipecnt++]) != 0);
        pipecnt--;
        write(0, NULL, 0);
        for(int i=0;i<pipecnt-1;i++) {
            int fd[2];
            pipe(fd);
            if(strstr(splitpipe[i], "<")) {
                inredsplit = splitinred(splitpipe[i]);
                for(int j=0;j<2;j++) {
                    trim(inredsplit[j]);
                }
                rfd = open(inredsplit[1], O_RDONLY);
                dup2(rfd, 0);
                retval = executer(inredsplit[0], homepath, fd);
            } else {
                retval = executer(splitpipe[i], homepath, fd);
            }
            close(fd[1]);
            if(retval == 1) {
                break;
            }
            dup2(fd[0], 0);
        }
        if(retval == 1) {
            break;
        }
        int fd[2];
        fd[0] = 0;
        fd[1] = 1;
        if(strstr(splitpipe[pipecnt-1], "<")) {
            char splitcopy[1000];
            strcpy(splitcopy, splitpipe[pipecnt-1]);
            inredsplit = splitinred(splitcopy);
            for(int i=0;i<2;i++) {
                trim(inredsplit[i]);
            }
            if(strstr(inredsplit[1], ">")) {
                char splitcopy[1000];
                strcpy(splitcopy, inredsplit[1]);
                outredsplit = splitoutred(splitcopy);
                for(int i=0;i<2;i++) {
                    trim(outredsplit[i]);
                }
                rfd = open(outredsplit[0], O_RDONLY);
                char *p = strstr(inredsplit[1], ">");
                if(strstr(p+1, ">")) {
                    wfd = open(outredsplit[1], O_CREAT | O_APPEND | O_WRONLY, 0644);
                } else {
                    wfd = open(outredsplit[1], O_CREAT | O_TRUNC | O_WRONLY, 0644);
                }
                dup2(rfd, 0);
                dup2(wfd, 1);
                retval = executer(inredsplit[0], homepath, fd);
                dup2(stdinfd, 0);
                dup2(stdoutfd, 1);
            } else {
                rfd = open(inredsplit[1], O_RDONLY);
                dup2(rfd, 0);
                retval = executer(inredsplit[0], homepath, fd);
                dup2(stdinfd, 0);
                dup2(stdoutfd, 1);
            }
        } else {
            if(strstr(splitpipe[pipecnt-1], ">")) {
                char splitcopy[1000];
                strcpy(splitcopy, splitpipe[pipecnt-1]);
                outredsplit = splitoutred(splitcopy);
                for(int i=0;i<2;i++) {
                    trim(outredsplit[i]);
                }
                char* p;
                p = strstr(splitpipe[pipecnt-1], ">");
                if(strstr(p+1, ">")) {
                    wfd = open(outredsplit[1], O_CREAT | O_WRONLY | O_APPEND, 0644);
                } else {
                    wfd = open(outredsplit[1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
                }
                dup2(wfd, 1);
                retval = executer(outredsplit[0], homepath, fd);
                dup2(stdinfd, 0);
                dup2(stdoutfd, 1);
            } else {
                retval = executer(splitpipe[pipecnt-1], homepath, fd);
                dup2(stdinfd, 0);
                dup2(stdoutfd, 1);
            }
        }
        if(retval == 1) {
            break;
        }
    }
    return 0;
}
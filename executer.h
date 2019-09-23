#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "split.h"
#include "shellbuiltin.h"
#include "ls.h"
#include<sys/wait.h>
#include<sys/types.h>
#include<signal.h>
#include<math.h>
#include <syslog.h>

int bgcount = 0;
char background[1000][3][1000];

char **commandcomponents;

void childexit() {
    int a;
    pid_t pid = waitpid(-1, &a, WNOHANG);
    for(int i=0;i<bgcount;i++) {
        char strpid[100];
        sprintf(strpid, "%d", pid);
        if(strcmp(strpid, background[i][0]) == 0 && strcmp(background[i][2], "1") == 0) {
            fprintf(stderr, "%s with pid %s exited normally\n", background[i][1], background[i][0]);
            break;
        }
    }
}

void killprocess() {
    kill(getpid(), SIGINT);
}

int executer(char commandinput[], char homepath[], int fd[]) {
    commandcomponents = (char**)malloc(sizeof(char*)*100);
    for(int i=0;i<10;i++) {
        commandcomponents[i] = (char*)malloc(sizeof(char)*100);
    }
    char **copypath = (char**)malloc(sizeof(char*)*100);
    for(int i=0;i<10;i++) {
        copypath[i] = (char*)malloc(sizeof(char)*100);
    }
    char dupcommandinput[1000];
    strcpy(dupcommandinput, commandinput);
    char** commands = splitcommand(dupcommandinput);
    int count = 0, countofparts = 0;
    while(strlen(commands[count]) != 0) {
        char dupcommand[1000];
        int flag = 0;
        strcpy(dupcommand, commands[count]);
        char** commandparts = spliteachcommand(dupcommand);
        int bgp = 0;
        for(int i=0;i<100;i++) {
            if(strlen(commandparts[i]) == 0) {
                countofparts = i;
                break;
            } else {
                if(strcmp(commandparts[i], "&") == 0) {
                    bgp++;
                }
                strcpy(commandcomponents[i], commandparts[i]);
            }
        }
        if(strcmp(commandcomponents[0], "setenv") == 0) {
            if(countofparts == 1 || countofparts > 3) {
                fprintf(stderr, "Invalid number of arguments\n");
            } else {
                char setenvstr[1000];
                strcpy(setenvstr, commandcomponents[1]);
                strcat(setenvstr, "=");
                strcat(setenvstr, commandcomponents[2]);
                // printf("%s\n", setenvstr);
                putenv(setenvstr);
            }
        } else if(strcmp(commandparts[0], "getenv") == 0) {
            if(countofparts == 2) {
                printf("%s\n", getenv(commandcomponents[1]) ? getenv(commandcomponents[1]) : "");
            } else {
                fprintf(stderr, "Invalid number of arguments\n");
            }
        } else if(strcmp(commandcomponents[0], "unsetenv") == 0) {
            if(countofparts == 2) {
                unsetenv(commandcomponents[1]);
            } else {
                fprintf(stderr, "Invalid number of arguments\n");
            }
        } else if(strcmp(commandcomponents[0], "cd") == 0) {
            if(countofparts == 1) {
                cdcommand(homepath, homepath);
            } else if(countofparts == 2) {
                cdcommand(commandcomponents[1], homepath);
            } else {
                printf("Error: cd: too many arguments\n");
            }
        } else if(strcmp(commandcomponents[0], "quit") == 0 || strcmp(commandcomponents[0], "exit") == 0) {
            return 1;
        } else {
            pid_t pid = fork();
            if(pid < 0) {
                perror("FORK ERROR");
            } else if(pid == 0) {
                dup2(fd[1], 1);
                if(strcmp(commandcomponents[0], "echo") == 0) {
                    if(countofparts == 1) {
                        printf("\n");
                    } else {
                        echocommand(commands[count]);
                    }
                } else if(strcmp(commandcomponents[0], "pwd") == 0) {
                    pwdcommand();
                } else if(strcmp(commandcomponents[0], "ls") == 0) {
                    int l = 0, a = 0, la = 0;
                    for(int i=0;i<countofparts;i++) {
                        if(strcmp(commandcomponents[i], "-l") == 0) {
                            l++;
                        } else if(strcmp(commandcomponents[i], "-a") == 0) {
                            a++;
                        } else if((strcmp(commandcomponents[i], "-la") == 0) || (strcmp(commandcomponents[i], "-al") == 0)) {
                            la++;
                        }
                    }
                    if(bgp > 0) {
                        lsbgp(commandcomponents, countofparts, l, a, la, bgp, homepath);
                    } else {
                        ls(commandcomponents, countofparts, l, a, la, bgp, homepath);
                    }
                } else if(strcmp(commandcomponents[0], "pinfo") == 0) {
                    char pidstatus[100] = "\0", pidexe[100] = "\0", buf[100], charpid[50];
                    strcpy(pidstatus, "/proc/");
                    strcpy(pidexe, "/proc/");
                    if(countofparts > 1) {
                        strcpy(charpid, commandcomponents[1]);
                        strcat(pidstatus, charpid);
                        strcat(pidexe, charpid);
                    } else if(countofparts == 1) {
                        int count = 0, pid = getpid();
                        sprintf(charpid, "%d", pid);
                        strcat(pidstatus, charpid);
                        strcat(pidexe, charpid);
                    }
                    strcat(pidstatus, "/status");
                    strcat(pidexe, "/cmdline");
                    FILE *file1, *file2;
                    file1 = fopen(pidstatus, "r");
                    if(file1 == NULL) {
                        perror("FILE ERROR");
                    } else {
                        file2 = fopen(pidstatus, "r");
                        if(file2 == NULL) {
                            perror("FILE ERROR");
                        } else {
                            printf("pid -- %s\n", charpid);
                            while(fgets(buf, 100, file1) != NULL) {
                                if(strstr(buf, "State")) {
                                    int point = 0;
                                    while(buf[point++] != ':');
                                    printf("Process Status -- %s", buf+point+1);
                                }
                            }
                            fclose(file1);

                            while(fgets(buf, 100, file2) != NULL) {
                                if(strstr(buf, "VmSize")) {
                                    int point = 0;
                                    while(buf[point++] != ':');
                                    printf("memory -- %s", buf+point+5);
                                }
                            }
                            fclose(file2);
                            int fd = open(pidexe, O_RDONLY);
                            read(fd, buf, 100);
                            printf("Executable path -- %s\n", buf);
                        }
                    }
                } else {
                    int bgflag = 0;
                    if(strcmp(commandcomponents[countofparts-1], "&") == 0) {
                        bgflag = 1;
                    } else {
                        bgflag = 0;
                    }
                    pid_t p;
                    p = fork();
                    if(p == -1) {
                        printf("Error\n");
                    } else if(p == 0) {
                        int loop;
                        if(bgflag == 0) {
                            loop = countofparts;
                        } else {
                            loop = countofparts-1;
                        }
                        char **args = (char**)malloc(sizeof(char*)*loop);
                        for(int i=0;i<loop;i++) {
                            args[i] = (char*)malloc(sizeof(char)*strlen(commandcomponents[i]));
                            strcpy(args[i], commandcomponents[i]);
                        }
                        args[loop] = (char*)malloc(sizeof(char)*100);
                        args[loop] = NULL;
                        char temp[100] = "/usr/bin/";
                        strcat(temp, commandcomponents[0]);
                        if(execvp(temp, args) == -1) {
                            strcpy(temp, "/bin/");
                            strcat(temp, commandcomponents[0]);
                            if(execvp(temp, args) == -1) {
                                strcpy(temp, "/sbin/");
                                strcat(temp, commandcomponents[0]);
                                if(execvp(temp, args) == -1) {
                                    strcpy(temp, "/usr/sbin/");
                                    strcat(temp, commandcomponents[0]);
                                    if(execvp(temp, args) == -1) {
                                        if(execvp(commandcomponents[0], args) == -1) {
                                            perror("Error");
                                        }
                                    }
                                }
                            }
                        }
                        for(int i=0;i<countofparts;i++) {
                            free(args[i]);
                        }
                        free(args);
                        exit(1);
                    } else {
                        if(bgflag == 0) {
                            wait(NULL);
                        } else {
                            sprintf(background[bgcount][0], "%d", p);
                            strcpy(background[bgcount][1], commandcomponents[0]);
                            sprintf(background[bgcount++][2], "%d", bgflag);
                        }
                    }
                    signal(SIGCHLD, childexit);
                    signal(SIGINT, killprocess);
                }
                exit(1);
                close(fd[1]);
            } else {
                wait(NULL);
            }
        }
        count++;
    }
    return 0;
}
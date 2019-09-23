#include<stdio.h>
#include<unistd.h>
#include<string.h>

char pwdbuf[1000];

void cdcommand(char arg[], char homepath[]) {
    if(arg[0] == '~') {
        char path[100] = "\0";
        strcat(path, homepath);
        strcat(path, arg+1);
        if(chdir(path) == -1) {
            perror("Directory error");
        }
    } else {
        if(chdir(arg) == -1) {
            perror("Directory error");
        }
    }
}

void pwdcommand(void) {
    printf("%s\n", getcwd(pwdbuf, 1000));
}

void echocommand(char arg[]) {
    int start = 4;
    while(arg[start] == ' ') {
        start++;
    }
    for(int i=start;i<strlen(arg);i++) {
        if(arg[i] == '"') {
            continue;
        } else if(arg[i] == ' ') {
            while(arg[i] == ' ') {
                i++;
            }
            i--;
            printf(" ");
        } else {
            printf("%c", arg[i]);
        }
    }
    printf("\n");
}
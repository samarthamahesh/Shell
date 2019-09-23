#include<stdio.h>
#include<string.h>
#include<stdlib.h>

char** splitcommand(char arg[]) {
    char **commands = (char**)malloc(sizeof(char*)*100);
    for(int i=0;i<100;i++) {
        commands[i] = (char*)malloc(sizeof(char)*100);
    }
    char *splittoken = strtok(arg, ";");
    int count = 0;
    while(splittoken != NULL) {
        strcpy(commands[count++], splittoken);
        splittoken = strtok(NULL, ";");
    }

    return commands;
}

char** spliteachcommand(char arg[]) {
    char **commandparts=(char**)malloc(sizeof(char*)*100);
    for(int i=0;i<100;i++) {
        commandparts[i] = (char*)malloc(sizeof(char)*100);
    }
    char *splittoken = strtok(arg, " ");
    int count = 0;
    while(splittoken != NULL) {
        strcpy(commandparts[count++], splittoken);
        splittoken = strtok(NULL, " ");
    }

    return commandparts;
}

char** splitpipes(char arg[]) {
    char **commandpartspipe=(char**)malloc(sizeof(char*)*100);
    for(int i=0;i<100;i++) {
        commandpartspipe[i] = (char*)malloc(sizeof(char)*100);
    }
    char *splittoken = strtok(arg, "|");
    int count = 0;
    while(splittoken != NULL) {
        strcpy(commandpartspipe[count++], splittoken);
        splittoken = strtok(NULL, "|");
    }

    return commandpartspipe;
}

char** splitinred(char arg[]) {
    char **inredcommands = (char**)malloc(sizeof(char*)*2);
    inredcommands[0] = (char*)malloc(sizeof(char)*100);
    inredcommands[1] = (char*)malloc(sizeof(char)*100);
    char *splittoken = strtok(arg, "<");
    int count = 0;
    while(splittoken != NULL) {
        strcpy(inredcommands[count++], splittoken);
        splittoken = strtok(NULL, "<");
    }

    return inredcommands;
}

char** splitoutred(char arg[]) {
    char **outredcommands = (char**)malloc(sizeof(char*)*2);
    outredcommands[0] = (char*)malloc(sizeof(char)*100);
    outredcommands[1] = (char*)malloc(sizeof(char)*100);
    char *splittoken = strtok(arg, ">");
    int count = 0;
    while(splittoken != NULL) {
        strcpy(outredcommands[count++], splittoken);
        splittoken = strtok(NULL, ">");
    }

    return outredcommands;
}

void trim(char arg[]) {
    int start, end, i = 0;
    while(arg[i] == 32) {
        i++;
    }
    start = i;
    i = strlen(arg) - 1;
    while(arg[i] == 32) {
        i--;
    }
    end = i;
    char retchar[1000];
    strcpy(retchar, "");
    for(int i=start;i<=end;i++) {
        retchar[i-start] = arg[i];
    }
    retchar[end-start+1] = '\0';
    strcpy(arg, retchar);

    return;
}
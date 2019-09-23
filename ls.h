#include<stdio.h>
#include<fcntl.h>
#include<dirent.h>
#include<time.h>
#include<sys/stat.h>
#include<pwd.h>

void printdetails(struct stat filestat, int lcount, int acount, int lacount, int countofdir) {
    if(lcount > 0 || lacount > 0) {
        printf((S_ISDIR(filestat.st_mode)) ? "d" : "-");
        printf((filestat.st_mode & S_IRUSR) ? "r" : "-");
        printf((filestat.st_mode & S_IWUSR) ? "w" : "-");
        printf((filestat.st_mode & S_IXUSR) ? "x" : "-");
        printf((filestat.st_mode & S_IRGRP) ? "r" : "-");
        printf((filestat.st_mode & S_IWGRP) ? "w" : "-");
        printf((filestat.st_mode & S_IXGRP) ? "x" : "-");
        printf((filestat.st_mode & S_IROTH) ? "r" : "-");
        printf((filestat.st_mode & S_IWOTH) ? "w" : "-");
        printf((filestat.st_mode & S_IXOTH) ? "x " : "- ");

        if(S_ISDIR(filestat.st_mode) == 0) {
            printf("1 ");
        } else {
            printf("%d ", countofdir);
        }
        struct passwd *pws;
        pws = getpwuid(filestat.st_uid);
        printf("%10s ", pws->pw_name);
        pws = getpwuid(filestat.st_gid);
        printf("%10s ", pws->pw_name);
        printf("%6ld ", filestat.st_size);
        char datetime[100], printdatetime[100];
        strcpy(datetime, ctime(&filestat.st_mtime));
        for(int i=0;i<12;i++) {
            printdatetime[i] = datetime[i+4];
        }
        printdatetime[12] = '\0';
        printf("%s ", printdatetime);
    }
}

void ls(char **paths, int count, int lcount, int acount, int lacount, int bgp, char homepath[]) {
    struct dirent *dir;
    DIR *dr;

    int unnes = 1 + lcount + acount + lacount + bgp, countofdir = 0;
    int totalpaths = count - unnes;

    if(totalpaths == 0) {
        char path[100] = "\0";
        getcwd(path, 100);

        struct stat fileordir;
        stat(path, &fileordir);

        if(S_ISDIR(fileordir.st_mode) == 0) {
            printdetails(fileordir, lcount, acount, lacount, countofdir);
            printf("%s\n", path);
        } else {
            dr = opendir(path);

            if(dr == NULL) {
                perror("Error");
            }

            dir = readdir(dr);

            char refpath[1000];
            struct stat filestat;

            while(dir != NULL) {
                strcpy(refpath, path);
                if(refpath[strlen(refpath)-1] == '/') {
                    stat(strcat(refpath, dir->d_name), &filestat);
                } else {
                    stat(strcat(strcat(refpath, "/"), dir->d_name), &filestat);
                }

                if(dir->d_name[0] == '.' && (acount == 0 && lacount == 0)) {
                    dir = readdir(dr);
                    continue;
                }
                
                countofdir = 0;
                if(lcount > 0 || lacount > 0) {
                    if(S_ISDIR(filestat.st_mode)) {
                        struct dirent *dir1;
                        DIR *dr1;

                        struct stat indir;

                        dr1 = opendir(refpath);

                        dir1 = readdir(dr1);
                        char inrefpath[1000];
                        while(dir1 != NULL) {
                            strcpy(inrefpath, refpath);
                            stat(strcat(strcat(inrefpath, "/"), dir1->d_name), &indir);
                            if(S_ISDIR(indir.st_mode)) {
                                countofdir++;
                            }
                            dir1 = readdir(dr1);
                        }
                    } else {
                        countofdir++;
                    }
                }

                printdetails(filestat, lcount, acount, lacount, countofdir);
                
                if(lcount > 0 || lacount > 0) {
                    printf("%s\n", dir->d_name);
                } else {
                    printf("%s ", dir->d_name);
                }
                if(totalpaths > 1) {
                    printf("\n");
                }

                dir = readdir(dr);
            }
            printf("\n");
        }
    }

    int atleast1 = 0;

    for(int i=0;i<count;i++) {

        if(i == 0) {
            continue;
        }
        if(strcmp(paths[i], "-l") == 0 || strcmp(paths[i], "-a") == 0 || strcmp(paths[i], "-la") == 0 || strcmp(paths[i], "-al") == 0) {
            continue;
        }

        if(strcmp(paths[i], "&") == 0) {
            if(atleast1 == 0) {
                char path[100] = "\0";
                getcwd(path, 100);

                struct stat fileordir;
                stat(path, &fileordir);

                if(S_ISDIR(fileordir.st_mode) == 0) {
                    printdetails(fileordir, lcount, acount, lacount, countofdir);
                    printf("%s\n", path);
                } else {
                    dr = opendir(path);

                    if(dr == NULL) {
                        perror("Error");
                    }

                    dir = readdir(dr);

                    char refpath[1000];
                    struct stat filestat;

                    while(dir != NULL) {
                        strcpy(refpath, path);
                        if(refpath[strlen(refpath)-1] == '/') {
                            stat(strcat(refpath, dir->d_name), &filestat);
                        } else {
                            stat(strcat(strcat(refpath, "/"), dir->d_name), &filestat);
                        }

                        if(dir->d_name[0] == '.' && (acount == 0 && lacount == 0)) {
                            dir = readdir(dr);
                            continue;
                        }

                        countofdir = 0;
                        if(lcount > 0 || lacount > 0) {
                            if(S_ISDIR(filestat.st_mode)) {
                                struct dirent *dir1;
                                DIR *dr1;

                                struct stat indir;

                                dr1 = opendir(refpath);

                                dir1 = readdir(dr1);
                                char inrefpath[1000];
                                while(dir1 != NULL) {
                                    strcpy(inrefpath, refpath);
                                    stat(strcat(strcat(inrefpath, "/"), dir1->d_name), &indir);
                                    if(S_ISDIR(indir.st_mode)) {
                                        countofdir++;
                                    }
                                    dir1 = readdir(dr1);
                                }
                            } else {
                                countofdir++;
                            }
                        }

                        printdetails(filestat, lcount, acount, lacount, countofdir);
                        if(lcount > 0 || lacount > 0) {
                            printf("%s\n", dir->d_name);
                        } else {
                            printf("%s ", dir->d_name);
                        }

                        dir = readdir(dr);
                    }
                    printf("\n");
                }
            }
            for(int j=i+1;j<count;j++) {
                printf("%s: command not found\n", paths[j]);
            }
            break;
        }

        atleast1++;

        if(totalpaths > 1) {
            printf("%s:\n", paths[i]);
        }

        char path[100] = "\0";
        
        if(paths[i][0] == '~') {
            strcpy(path, homepath);
            strcat(path, paths[i]+1);
        } else {
            strcpy(path, paths[i]);
        }

        struct stat fileordir;
        stat(path, &fileordir);
        if(S_ISDIR(fileordir.st_mode) == 0) {
            if(open(path, O_RDONLY) == -1) {
                perror("Error");
                continue;
            }

            printdetails(fileordir, lcount, acount, lacount, countofdir);
            printf("%s\n", dir->d_name);
            continue;
        } else {
            dr = opendir(path);

            if(dr == NULL) {
                perror("Error");
                continue;
            }

            dir = readdir(dr);

            char refpath[1000];
            struct stat filestat;

            while(dir != NULL) {
                strcpy(refpath, path);
                if(refpath[strlen(refpath)-1] == '/') {
                    stat(strcat(refpath, dir->d_name), &filestat);
                } else {
                    stat(strcat(strcat(refpath, "/"), dir->d_name), &filestat);
                }

                if(dir->d_name[0] == '.' && (acount == 0 && lacount == 0)) {
                    dir = readdir(dr);
                    continue;
                }

                countofdir = 0;
                if(lcount > 0 || lacount > 0) {
                    if(S_ISDIR(filestat.st_mode)) {
                        struct dirent *dir1;
                        DIR *dr1;

                        struct stat indir;

                        dr1 = opendir(refpath);

                        dir1 = readdir(dr1);
                        char inrefpath[1000];
                        while(dir1 != NULL) {
                            strcpy(inrefpath, refpath);
                            stat(strcat(strcat(inrefpath, "/"), dir1->d_name), &indir);
                            if(S_ISDIR(indir.st_mode)) {
                                countofdir++;
                            }
                            dir1 = readdir(dr1);
                        }
                    } else {
                        countofdir++;
                    }
                }

                printdetails(filestat, lcount, acount, lacount, countofdir);
                if(lcount > 0 || lacount > 0) {
                    printf("%s\n", dir->d_name);
                } else {
                    printf("%s ", dir->d_name);
                }
                dir = readdir(dr);
            }
            if(lcount == 0 && lacount == 0) {
                printf("\n");
            }

            if(totalpaths > 1) {
                printf("\n");
            }
        }
    }
}

void lsbgp(char **paths, int count, int lcount, int acount, int lacount, int bgp, char homepath[]) {
    pid_t p = fork();
    if(p == -1) {
        perror("Fork error");
    } else if(p == 0) {
        ls(paths, count, lcount, acount, lacount, bgp, homepath);
        printf("%s with pid %d exited normally\n", paths[0], getpid());
    } else if(p == 1) {
        return;
    }
}
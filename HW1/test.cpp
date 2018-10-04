#include<unistd.h>
#include<cstdio>

int main(){

    char *cmd = "ls";
    char *argv[10];
    argv[0] = "ls";
    argv[1] = "-la";
    argv[2] = NULL;
    argv[3] = NULL;
    //printf("2 %s\n", argv[2]);
    execvp(cmd, argv); //This will run "ls -la" as if it were a command
    return 0;
}

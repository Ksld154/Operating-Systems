#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]){
    pid_t pid;
    pid_t parent_pid;
    pid = fork();        // fork1
    
    
    if(pid > 0){
        printf("Main process id : %d\n", getpid());
        wait(NULL);
        //exit(0);
    }
    else if(pid == 0){
        printf("Fork1, I am child: %d, my parent id: %d\n", getpid() ,getppid());
        pid = fork();   // fork2
        
        if(pid > 0){
            wait(NULL);
        }
        else if(pid == 0)
            printf("Fork2, I am child: %d, my parent id: %d\n", getpid() ,getppid());
    }


    pid = fork();      // fork3
    if(pid > 0){
        wait(NULL);
    }
    else if(pid == 0)
        printf("Fork3, I am child: %d, my parent id: %d\n", getpid() ,getppid());
    //printf("pid: %d, Finish\n", getpid());
    return 0;
}

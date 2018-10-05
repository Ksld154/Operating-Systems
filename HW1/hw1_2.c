#include <unistd.h>
#include <stdio.h>
int cnt = 0;

int main(int argc, char const *argv[]){
    pid_t pid;

    pid = fork();          //fork0
    printf("fork0, pid:%d\n", pid);

    for(int i = 0; i < 2; i++){
        
        if(pid == 0){                      /* child */
            pid = fork();  //fork1
            printf("fork1, pid:%d\n", pid);
            pid = fork();  //fork2  
            printf("fork2, pid:%d\n", pid);
        }
        else if(pid > 0){                   /* parent */ 
            pid = fork();  //fork3
            printf("fork3, pid:%d\n", pid);
        }
        else
            printf("Error!\n");
        printf("%3d Hello\n", pid);
    }
    
    //printf("%d\n", cnt);
    return 0;
}

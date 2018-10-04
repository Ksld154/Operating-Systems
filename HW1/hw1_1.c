#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MAX_LINE 80
char	*arg[MAX_LINE/2+1];      /*command line arguments*/

int main(void){

	int 	should_run = 1;           /*flag to determine when to exit program*/


	while(should_run){

		char 	InputBuffer[MAX_LINE+1];
		char	*tmp = NULL;
		int   	arg_len = 0;
		printf("osh>");
		fflush(stdout);
		
		read(STDIN_FILENO, InputBuffer, MAX_LINE);
		
		arg[0] = strtok(InputBuffer," \n");
		tmp = arg[0];
		
		
		for(int i = 1; tmp != NULL ;i++){
			tmp = strtok(NULL, " \n");
			arg[i] = tmp;
			//arg_len++;
		}
		for(int i = 0; i <= 40 ;i++){
			printf("%d %s\n", i, arg[i]);
		}
		printf("\n");
		
		int pid;
		pid = fork();

		int err = execvp(arg[0], arg);
		printf("err:%d\n", err);

		/**
		* your code!
		* After reading user input, the step are:
		* (1) fork a child process using fork()
		* (2) the child process will invoke execvp()
		* (3) if command included &, parent will not invoke wait()
		*/
	}

	return 0;
}


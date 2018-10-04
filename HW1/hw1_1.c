#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 80
char	*arg[MAX_LINE/2+1];      /*command line arguments*/

int main(void){

	int 	should_run = 1;           /*flag to determine when to exit program*/


	while(should_run){

		char 	*InputBuffer;
		char	*tmp = NULL;
		int   	arg_cnt = 0;
		int 	ampersand = 0;
		printf("osh>");
		fflush(stdout);
		
		read(STDIN_FILENO, InputBuffer, MAX_LINE);
		int len = strlen(InputBuffer);
		InputBuffer = strtok(InputBuffer, "\n");
		//InputBuffer[len-1] = '\0';


		if(InputBuffer[len-2] == '&') {
			ampersand = 1;
			InputBuffer[len-2] = '\0';
		}
		else ampersand = 0;
		//printf("%d\n", ampersand);


		if(strcmp(InputBuffer, "exit") == 0){
			should_run = 0;
			break;
		}

		arg[0] = strtok(InputBuffer," ");
		tmp = arg[0];
		for(int i = 1; tmp != NULL ;i++){
			tmp = strtok(NULL, " ");
			arg[i] = tmp;
			arg_cnt++;
		}
		

		pid_t pid = fork();

		if(pid < 0){         //error 
			fprintf(stderr, "Fork failed!");
		}
		else if(pid == 0){  //child process
			int forkmsg = execvp(arg[0], arg);



		}
		else{               //parent process   
			if(!ampersand)
				wait(NULL);
			printf("Child complete.\n");
		}
		






		//int err = execvp(arg[0], arg);
		//printf("err:%d\n", err);

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


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 80
char	*arg[MAX_LINE/2+1];           /*command line arguments*/

int main(void){
	int 	should_run = 1;           /*flag to determine when to exit program*/
	while(should_run){

		char 	*InputBuffer;
		char	*tmp = NULL;
		int 	ampersand = 0;

		/*clean up buffer*/
		for(int i = 0; i < MAX_LINE; i++)
			InputBuffer[i] = '\0';
		
		printf("osh>");
		fflush(stdout);

		/*read from std input*/
		read(STDIN_FILENO, InputBuffer, MAX_LINE);
		int len = strlen(InputBuffer);
		InputBuffer = strtok(InputBuffer, "\n");   // erase '\n' at end of line

		/*check if it's an ampersand command or not*/
		if(InputBuffer[len-2] == '&') {
			ampersand = 1;
			InputBuffer[len-2] = '\0';
		}
		else ampersand = 0;
		
		/*check if it's an "exit" command*/
		if(strcmp(InputBuffer, "exit") == 0){
			should_run = 0;
			break;
		}

		/*slice the input command*/
		arg[0] = strtok(InputBuffer," ");
		tmp = arg[0];
		for(int i = 1; tmp != NULL ;i++){
			tmp = strtok(NULL, " ");
			arg[i] = tmp;
		}
		
		/*fork*/
		pid_t pid = fork();
		if(pid < 0){         //error 
			fprintf(stderr, "Fork failed!");
		}
		else if(pid == 0){  //child process
			int forkmsg = execvp(arg[0], arg);
			exit(0);
		}
		else{               //parent process
			/* if it is an ampersand command, 
			the parent process does not have to wait for current child process. */   
			if(!ampersand)
				waitpid(pid, NULL, 0);
			//printf("Child complete.\n");
		}
		
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


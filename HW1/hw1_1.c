#include <stdio.h>
#include <unistd.h>

#define MAX_LINE 80

int main(void){
	char *arg[MAX_LINE/2+1];      /*command line arguments*/
	int should_run = 1;           /*flag to determine when to exit program*/
	
	while(should_run){
		print("osh>");
		fflush(stdout);
		
		



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


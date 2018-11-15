#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
using namespace std;

#define NUMBER_OF_THREADS 11 /* Hint */
#define PUZZLE_SIZE 9

/* Declare the threads */
void *Section_Checker(void *arg);

/* data structure for passing data to threads */
typedef struct{
	int thread_number;
	int x;
	int y;
    int sum;
}parameters;

/* example puzzle */
int puzzle[PUZZLE_SIZE+1][PUZZLE_SIZE+1] = {
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,0,0,0,0,0,0,0,0,0},
    {-1,0,0,0,0,0,0,0,0,0},
    {-1,0,0,0,0,0,0,0,0,0},
    {-1,0,0,0,0,0,0,0,0,0},
    {-1,0,0,0,0,0,0,0,0,0},
    {-1,0,0,0,0,0,0,0,0,0},
    {-1,0,0,0,0,0,0,0,0,0},
    {-1,0,0,0,0,0,0,0,0,0},
    {-1,0,0,0,0,0,0,0,0,0}
};


/* print puzzle */ 
#define LINE "====================== \n"
#define COLLINE "\n___________________________________ \n"
#define NAME "||  SUM   CHECKER  || \n"
void print_grid(int grid[10][10]){
    printf(LINE);
    printf(NAME);
    printf(LINE);
	for(int i = 1; i < 10; i++){
		for(int j = 1; j < 10; j++){
	        printf("|%1d |",grid[i][j]);
		}
        printf(COLLINE);
	}
    printf("\n");
}

// read file to check sudoku
void SetPuzzle(char filename[]){
    FILE *file = fopen(filename,"r");
    int ch,num;
    for(int i = 0; i <= 9; i++){
        for(int j = 0; j <= 9; j++){
            while((ch = getc(file)) != EOF){
                if(ch == EOF) break;
                if(ch == ',') break;
                if(ch == '\n') break;
                ungetc(ch,file);
                fscanf(file,"%d",&num);
                if(num != -1)
                    puzzle[i][j] = num;            
            } 
        }
    }
    print_grid(puzzle);
    return;
}


/* Multithread Version */

int main(int argc, char* argv[]){
	int rv = 1;                      // flag to check answer
    SetPuzzle((char *)"test2.txt");  // input the sudoku file


    /* Your code here */

    pthread_t	    tid [NUMBER_OF_THREADS + 1];
    parameters	    para[NUMBER_OF_THREADS + 1];
    void	        *ThreadStatus;

    // check the sum of row 1
    int sum_tmp = 0;
    for(int i = 1; i <= PUZZLE_SIZE; i++){    
        sum_tmp += puzzle[1][i];
    }
    
    // create threads
    for(int i = 1; i <= NUMBER_OF_THREADS; i++){
        para[i].thread_number = i;
        para[i].sum = sum_tmp;
        if(i <= 2){
            para[i].x = -1;
            para[i].y = -1;
        }
        else{
            // for 9 3*3 section checker
            // x: the row index of the 3*3 section (could be 0, 1, 2)
            // y: the column index of the 3*3 section (could be 0, 1, 2)
            para[i].x = i/3 - 1;
            para[i].y = i % 3;
        }
        pthread_create(&tid[i], NULL, Section_Checker, &para[i]);
    }

    // join threads
    for(int i = 1; i <= NUMBER_OF_THREADS; i++){
        pthread_join(tid[i], &ThreadStatus);

        // ThreadStatus: the return value from a thread,
        //               0 means the sum is right; 1 means the sum is Wrong!
        if(ThreadStatus != 0)
            rv = 0;
    }
    

    // output result
	if(rv == 1)
		printf("Successful :) \n");
	else
		printf("Must check again! :( \n");
	return 0;
}

/* Definition of a thread */
void *Section_Checker(void *arg){
    // get data from the argument
    parameters *para_ptr = (parameters *)arg;
    parameters data = *para_ptr;

    // row checker
    if(data.thread_number == 1){
        
        for(int i = 1; i <= PUZZLE_SIZE; i++){
            int sum_tmp = 0;
            for(int j = 1; j <= PUZZLE_SIZE; j++){
                sum_tmp += puzzle[i][j];
            }
            if(sum_tmp != data.sum){
                // thread return with 1 means the sum is not
                // the same as the puzzle's 1st row
                pthread_exit((void *)1);
            }
        }   
    }

    // column checker 
    if(data.thread_number == 2) {
        for(int i = 1; i <= PUZZLE_SIZE; i++){
            
            int sum_tmp = 0;
            for(int j = 1; j <= PUZZLE_SIZE; j++){
                sum_tmp += puzzle[j][i];
            }
            if(sum_tmp != data.sum)
                pthread_exit((void *)1);
        }
    }
    
    // 3*3 section checker
    if(data.thread_number >= 3){

        // a 3*3 sub-section
        int sum_tmp = 0;
            for(int m = 1; m <= 3; m++){
                for(int n = 1; n <= 3; n++)
                    sum_tmp += puzzle[3*data.x + m][3*data.y + n];    
        }
        if(sum_tmp != data.sum)
            pthread_exit((void *)1);
    }

    // thread return with 0 means that the thread sum
    // is the same as the sum of 1st row
    pthread_exit(0);
}
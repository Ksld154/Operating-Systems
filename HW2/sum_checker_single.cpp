#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

#define NUMBER_OF_THREADS 11 /* Hint */
#define PUZZLE_SIZE 9

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

/* data structure for passing data to threads */
typedef struct{
	int thread_number;
	int x;
	int y;
}parameters;


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


int main(int argc, char* argv[]){
	int rv = 0;                      // flag to check answer
    SetPuzzle((char *)"test2.txt");  // input the sudoku file

    /* Your code here */

    int PuzzleSum = 0;
    int sum_tmp = 0;
    int CorrectSection = 0;

    for(int i = 1; i <= 9; i++){     // check the sum of row 1
        sum_tmp += puzzle[1][i];
    }
    PuzzleSum = sum_tmp;

    // check rows    
    for(int i = 1; i <= 9; i++){
        int sum_tmp = 0;
        for(int j = 1; j <= 9; j++){
            sum_tmp += puzzle[i][j];
        }
        // check the sum of a row
        if(sum_tmp == PuzzleSum)
            CorrectSection++;
        else
            break;
    }
    
    // check columns
    for(int i = 1; i <= 9; i++){
        int sum_tmp = 0;
        for(int j = 1; j <= 9; j++){
            sum_tmp += puzzle[j][i];
        }
        if(sum_tmp == PuzzleSum)
            CorrectSection++;
        else 
            break;
    }

    // check each 3*3 sub-section
    for(int i = 0; i <= 2; i++){
        for(int j = 0; j <= 2; j++){
            
            // a 3*3 sub-section
            int sum_tmp = 0;
            for(int m = 1; m <= 3; m++){
                for(int n = 1; n <= 3; n++){
                    sum_tmp += puzzle[3*i+m][3*j+n];    
                }
            }
            if(sum_tmp == PuzzleSum)
                CorrectSection++;
            else
                break;
        }
    }
    
    //cout << "Correct Section: " << CorrectSection << endl;
    
    // Output the result
    if(CorrectSection == 27)
        rv = 1;
	if(rv == 1)
		printf("Successful :) \n");
	else
		printf("Must check again! :( \n");
	return 0;
}

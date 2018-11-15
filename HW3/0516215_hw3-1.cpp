#include<iostream>
#include<cstdio>
#include<iomanip>
#include<fstream>
#define INF 100000
using namespace std;

struct Process{
    int ArrivalTime;
    int BurstTime;
};

void PrintAvg(ofstream &f, int arg, int n){
    int arg_len = 0;
    int tmp1 = arg;
    while(tmp1 >= 1){
        tmp1 /= 10;
        arg_len++;
    }
    f << setprecision(arg_len+5) << (double)arg / (double) n << endl;
}

int main(int argc, char const *argv[]){
    int n;

    FILE *fp;
    ofstream fp2("ans1.txt", ios::out);
    fp = fopen(argv[1], "r");
    fscanf(fp, "%d", &n);
    
    struct Process process[n];
    bool done[n] = {0};
    int wait[n] = {0};
    int turnaround[n] = {0};

    int arrival_min = INF;
    int arrival_min_index = INF;

    for(int i = 0; i < n; i++){
        fscanf(fp, "%d", &process[i].ArrivalTime);
        if(process[i].ArrivalTime < arrival_min){
            arrival_min_index = i;
            arrival_min = process[i].ArrivalTime;
        }
    }
    for(int i = 0; i < n; i++){
        fscanf(fp, "%d", &process[i].BurstTime);
        turnaround[i] = process[i].BurstTime;
    }
    fclose(fp);

    int clk = 0;
    int finish_cnt = 0;

    // do the first arrive job
    clk = arrival_min + process[arrival_min_index].BurstTime;
    done[arrival_min_index] = 1;
    finish_cnt++;

    // update wait time and turnaround time of each job
    for(int i = 0; i <= n; i++){
        if(process[i].ArrivalTime <= clk && done[i] == 0){
            wait[i] = clk - process[i].ArrivalTime;
            turnaround[i] += wait[i];
        }
    }

    while(finish_cnt < n){  
        int ShortestJob = INF;
        int ShortestJob_index;
        
        for(int i = 0; i < n; i++){
            // job in the waiting queue
            if(process[i].ArrivalTime <= clk && done[i] == 0){
                
                // select the shortest job
                if(process[i].BurstTime < ShortestJob){
                    ShortestJob_index = i;
                    ShortestJob = process[i].BurstTime;
                }
                // if job length are the same, do earlier arrived one 
                if(process[i].BurstTime == ShortestJob){
                    if(process[i].ArrivalTime < process[ShortestJob_index].ArrivalTime){
                        ShortestJob_index = i;
                        ShortestJob = process[i].BurstTime;
                    }
                }
            }
        }
        
        // do the shortest job
        clk += process[ShortestJob_index].BurstTime;
        done[ShortestJob_index] = 1;
        
        // update wait time and turnaround time of each job
        for(int i = 0; i < n; i++){
            if(process[i].ArrivalTime <= clk && done[i] == 0){
                wait[i] += process[ShortestJob_index].BurstTime;
                turnaround[i] += process[ShortestJob_index].BurstTime;
            }
        }
        finish_cnt++;
    }

    // write output into ans1.txt
    int wait_total = 0;
    int turnaround_total = 0;
    for(int i = 0; i < n; i++){
        wait_total += wait[i];
        turnaround_total += turnaround[i];

        fp2 << wait[i] << " " << turnaround[i] << endl;
    }

    PrintAvg(fp2, wait_total, n);
    PrintAvg(fp2, turnaround_total, n);

    return 0;
}

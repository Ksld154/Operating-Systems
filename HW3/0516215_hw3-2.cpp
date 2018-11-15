#include<iostream>
#include<cstdio>
#include<iomanip>
#include<fstream>
#define INF 100000
using namespace std;

struct Process{
    int ArrivalTime;
    int BurstTime;
    bool done;
    int wait;
    int turnaround;
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
    fp = fopen(argv[1], "r");
    ofstream fp2("ans2.txt", ios::out);
    
    fscanf(fp, "%d", &n);
    struct Process job[n];


    int arrival_min = INF;
    int arrival_min_index;
    for(int i = 0; i < n; i++){
        fscanf(fp, "%d", &job[i].ArrivalTime);
        if(job[i].ArrivalTime < arrival_min){
            arrival_min_index = i;
            arrival_min = job[i].ArrivalTime;
        }
    }

    for(int i = 0; i < n; i++){
        fscanf(fp, "%d", &job[i].BurstTime);

        job[i].done = 0;
        job[i].wait = 0;
        job[i].turnaround = job[i].BurstTime;    
    }
    fclose(fp);
    
    int clk = 0;
    int finish_cnt = 0;

    bool inuse = 0;
    int inuse_index;

    int ShortestJob = INF;
    int ShortestJob_index;

    while(finish_cnt < n){

        if(inuse){
            job[ShortestJob_index].BurstTime--;
            if(job[ShortestJob_index].BurstTime == 0){
                finish_cnt++;
                job[ShortestJob_index].done = 1;
                inuse = 0;
                ShortestJob = INF;
            }
        }

        // cout << finish_cnt << " " << clk << endl;
        // if(clk > 50) break;

        // search next job
        for(int i = 0; i < n; i++){
            
            // jobs in queue
            if(job[i].ArrivalTime <= clk && job[i].done == 0){
                
                // select the shortest job
                if(job[i].BurstTime < ShortestJob){
                    ShortestJob_index = i;
                    ShortestJob = job[i].BurstTime;
                }
                // if job length are the same, do earlier arrived one 
                if(job[i].BurstTime == ShortestJob){
                    if(job[i].ArrivalTime < job[ShortestJob_index].ArrivalTime){
                        ShortestJob_index = i;
                        ShortestJob = job[i].BurstTime;
                    }
                }

                inuse = 1;  // next job is selected, so turn on machine
            }
        }

        for(int i = 0; i < n; i++){
            if(job[i].ArrivalTime <= clk && job[i].done == 0 && ShortestJob_index != i){
                job[i].wait++;
                job[i].turnaround++;
                //cout << job[i].wait << " " << job[i].turnaround << endl;
            }
        }
        
        clk++;
    }
    
    // write output into ans1.txt
    int wait_total = 0;
    int turnaround_total = 0;
    for(int i = 0; i < n; i++){
        wait_total += job[i].wait;
        turnaround_total += job[i].turnaround;

        fp2 << job[i].wait << " " << job[i].turnaround << endl;
    }

    PrintAvg(fp2, wait_total, n);
    PrintAvg(fp2, turnaround_total, n);

    return 0;
}

#include<iostream>
#include<cstdio>
#include<iomanip>
#include<fstream>
#include<queue>
#include<list>
#define INF 100000
using namespace std;

struct Process{
    int ArrivalTime;
    int BurstTime;
    int state;
    bool inqueue;
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
    int TimeQuantum;

    // read input from file, and initialize each job
    ifstream fin(argv[1]);
    fin >> n;
    
    struct Process job[n];

    for(int i = 0; i < n; i++)
        fin >> job[i].ArrivalTime;
    for(int i = 0; i < n; i++){
        fin >> job[i].BurstTime;
        job[i].inqueue = 0;
        job[i].state = -1;
        job[i].wait = 0;
        job[i].turnaround = job[i].BurstTime;
    }
    fin >> TimeQuantum;
    fin.close();

    int clk = 0;
    int finish_cnt = 0;
    int q = TimeQuantum;
    int inuse = -1;
    list<int> WaitingQueue;

    while(finish_cnt < n){  

        // update wait and turnaround time for each job in the waiting queue
        if(WaitingQueue.empty() == false){
            for(list<int>::iterator itr = WaitingQueue.begin(); itr != WaitingQueue.end(); itr++){
                job[*itr].wait++;
                job[*itr].turnaround++;
            }
        }

        // push newly arrived job into waiting queue
        for(int i = 0; i < n; i++){
            if(job[i].ArrivalTime == clk){
                
                // put this job in queue
                if(job[i].state == -1){
                    WaitingQueue.push_back(i);
                    job[i].state = 0;
                }
            }
        }

        // do current job
        if(inuse != -1){
            job[inuse].BurstTime--;
            q--;

            if(job[inuse].BurstTime == 0){
                q = TimeQuantum;
                job[inuse].state = 1;
                inuse = -1;
                finish_cnt++;
            }
            // run out of time quantum
            else if(q == 0){
                q = TimeQuantum;
                WaitingQueue.push_back(inuse);
                inuse = WaitingQueue.front();   //inuse: index of currently processing job
                WaitingQueue.pop_front();
            }
        }

        // if machine is idle, and some jobs are waiting
        if(inuse == -1){
            if(WaitingQueue.empty() == false){
                inuse = WaitingQueue.front();   //inuse: index of currently processing job
                WaitingQueue.pop_front();
            }
        }
        clk++;
    }
    
    // write output into ans3.txt
    ofstream fp2("ans3.txt", ios::out);
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

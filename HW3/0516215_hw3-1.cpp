#include<iostream>
#include<cstdio>
#include<iomanip>
#include<fstream>
#include<queue>
#define INF 100000
using namespace std;

typedef pair<int, int> pi; 

struct Process{
    int ArrivalTime;
    int BurstTime;
    int state;
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

    ifstream fin(argv[1]);
    fin >> n;

    struct Process job[n];
    priority_queue< pi, vector<pi>, greater<pi> > WQ;  //min-heap, ordered by first element

    for(int i = 0; i < n; i++)
        fin >> job[i].ArrivalTime;
    for(int i = 0; i < n; i++){
        fin >> job[i].BurstTime;

        job[i].state = -1;
        job[i].wait = 0;
        job[i].turnaround = job[i].BurstTime;
    }
    fin.close();

    int clk = 0;
    int finish_cnt = 0;
    int inuse  = -1;

    while(finish_cnt < n){  
        // cout << clk << " " << finish_cnt << endl;
        // if(clk >= 50) break;

        // update wait and turnaround time for each job in the waiting queue
        for(int i = 0; i < n; i++){
            if(job[i].state == 0){
                job[i].wait++;
                job[i].turnaround++;
            }
        }

        // push newly arrived job into waiting queue        
        for(int i = 0; i < n; i++){
            if(job[i].state == -1 && job[i].ArrivalTime == clk){
                WQ.push(make_pair(job[i].BurstTime, i));
                job[i].state = 0;
            }
        }

        // do current job
        if(inuse != -1){
            job[inuse].BurstTime--;

            if(job[inuse].BurstTime == 0){
                job[inuse].state = 2;
                inuse = -1;
                finish_cnt++;
            }
        }

        // if machine is idle, the select the next job from waiting queue.
        if(inuse == -1 && WQ.empty() != true){
            inuse = WQ.top().second;
            WQ.pop();
            job[inuse].state = 1;
        }

        clk++;
    }

    // write output into ans1.txt
    ofstream fp2("ans1.txt", ios::out);
    int wait_total = 0;
    int turnaround_total = 0;
    for(int i = 0; i < n; i++){
        wait_total += job[i].wait;
        turnaround_total += job[i].turnaround;
        
        //cout << job[i].wait << " " << job[i].turnaround << endl;
        fp2 << job[i].wait << " " << job[i].turnaround << endl;
    }

    PrintAvg(fp2, wait_total, n);
    PrintAvg(fp2, turnaround_total, n);

    return 0;
}

// state -1: not arrive yet
// state  0: in waiting queue
// state  1: executing, so not in waiting queue
// state  2: finished,  so not in waiting queue
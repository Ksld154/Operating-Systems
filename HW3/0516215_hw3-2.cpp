#include<iostream>
#include<cstdio>
#include<iomanip>
#include<fstream>
#include<list>
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
    int inuse = -1;

    while(finish_cnt < n){
        
        for(int i = 0; i < n; i++){
            if(job[i].state == 0){
                job[i].wait++;
                job[i].turnaround++;
            }
        }
        
        for(int i = 0; i < n; i++){
            if(job[i].state == -1 && job[i].ArrivalTime == clk){

                WQ.push(make_pair(job[i].BurstTime, i));
                job[i].state = 0;
            }
        }

        if(inuse != -1){
            job[inuse].BurstTime--;

            if(job[inuse].BurstTime == 0){
                job[inuse].state = 2;
                inuse = -1;
                finish_cnt++;
            }
            else if(WQ.empty() != true){
                
                if(WQ.top().first < job[inuse].BurstTime){
                    // put current job into waiting queue
                    job[inuse].state = 0;
                    WQ.push(make_pair(job[inuse].BurstTime, inuse));
                    
                    // grab the minimum remaining-time job from waiting queue
                    inuse = WQ.top().second;
                    WQ.pop();
                    job[inuse].state = 1;
                }
            }
        }

        // if machine is idle, and some jobs are waiting
        if(inuse == -1 && WQ.empty() != true){
            inuse = WQ.top().second;
            WQ.pop();
            job[inuse].state = 1;
        }

        clk++;
    }
    
    // write output into ans2.txt
    ofstream fp2("ans2.txt", ios::out);
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

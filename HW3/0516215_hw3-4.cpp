#include<iostream>
#include<fstream>
#include<cstdio>
#include<list>
#include<iomanip>
#include<cstdlib>
using namespace std;

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

int main(int argc, char *argv[]){
    int n;
    int TimeQuantum1, TimeQuantum2;

    ifstream fin(argv[1]);
    fin >> n;

    struct Process job[n];
    
    for(int i = 0; i < n; i++)
        fin >> job[i].ArrivalTime;
    for(int i = 0; i < n; i++){
        fin >> job[i].BurstTime;
        job[i].state = -1;
        job[i].wait = 0;
        job[i].turnaround = job[i].BurstTime;
    }
    fin >> TimeQuantum1 >> TimeQuantum2;
    fin.close();

    int clk = 0;
    int q1 = TimeQuantum1;
    int q2 = TimeQuantum2;
    int inuse = -1;

    list<int> WQ1;
    list<int> WQ2;
    list<int> WQ3;

    int finish_cnt = 0;
    while(finish_cnt < n){
        // cout << clk << " " << finish_cnt << " " << inuse << endl;
        // if(clk >= 50) break;

        // update wait and turnaround time for each job in wating queues
        if(WQ1.empty() == false){
            for(list<int>::iterator itr = WQ1.begin(); itr != WQ1.end(); itr++){
                job[*itr].wait++;
                job[*itr].turnaround++;
            }
        }
        if(WQ2.empty() == false){
            for(list<int>::iterator itr = WQ2.begin(); itr != WQ2.end(); itr++){
                job[*itr].wait++;
                job[*itr].turnaround++;
            }
        }
        if(WQ3.empty() == false){
            for(list<int>::iterator itr = WQ3.begin(); itr != WQ3.end(); itr++){
                job[*itr].wait++;
                job[*itr].turnaround++;
            }
        }

        // push newly arrived job into 1st waiting queue
        for(int i = 0; i < n; i++){
            if(job[i].ArrivalTime == clk){
                
                // put this job in 1st queue
                if(job[i].state == -1){
                    WQ1.push_back(i);
                    job[i].state = 1;
                }
            }
        }
        if(inuse != -1){
            
            // queue1: RR, q == q1
            if(job[inuse].state == 1){
                q1--;
                job[inuse].BurstTime--;

                // finished in time quantum
                if(job[inuse].BurstTime == 0){
                    q1 = TimeQuantum1;
                    job[inuse].state = 4;
                    inuse = -1;
                    finish_cnt++;
                }

                // does not finished in time quantum => move this job into 2nd queue  
                else if(q1 == 0){
                    q1 = TimeQuantum1;
                    job[inuse].state = 2;
                    WQ2.push_back(inuse);
                    inuse = -1;
                }
            }
            // queue2: RR, q == 2
            else if(job[inuse].state == 2){
                q2--;
                job[inuse].BurstTime--;

                if(job[inuse].BurstTime == 0){
                    q2 = TimeQuantum2;
                    job[inuse].state = 4;
                    inuse = -1;
                    finish_cnt++;
                }

                else if(q2 == 0){
                    q2 = TimeQuantum2;

                    // not just using push_back() because queue3 use SJF
                    // should select the right slot to insert the job
                    // queue3 is sorted with remaining burst time
                    if(job[WQ3.front()].BurstTime < job[inuse].BurstTime){
                    
                        for(list<int>::iterator itr = WQ3.begin(); itr != WQ3.end(); itr++){
                            if(job[inuse].BurstTime < job[*itr].BurstTime){
                                WQ3.insert(itr, inuse);
                                break;
                            }
                            if(itr == WQ3.end()){
                                WQ3.push_back(inuse);
                                break;
                            }
                        }
                    }
                    else
                        WQ3.push_front(inuse);  // either Q3 is empty, or job[inuse] has the smallest burst time

                    inuse = -1;

                }

            }
            else if(job[inuse].state == 3){
                job[inuse].BurstTime--;
                if(job[inuse].BurstTime == 0){
                    finish_cnt++;
                    job[inuse].state = 4;
                    inuse = -1;
                }
            }
        }

        // if no one is using the machine, the select the next job from each waiting queue in order.
        if(inuse == -1){
            if(WQ1.empty() == false){
                inuse = WQ1.front();   //inuse: index of currently processing job
                WQ1.pop_front();
            }
            else if(WQ2.empty() == false){
                inuse = WQ2.front();   //inuse: index of currently processing job
                WQ2.pop_front();
            }
            else if(WQ3.empty() == false){
                inuse = WQ3.front();   //inuse: index of currently processing job
                WQ3.pop_front();
            }
        }
        clk++;
    }


    // output result
    ofstream fp2("ans4.txt", ios::out);
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

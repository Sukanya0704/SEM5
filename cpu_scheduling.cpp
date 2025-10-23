#include<bits/stdc++.h>
using namespace std;

struct Process{
    int pid;
    int arrivalTime;
    int burstTime;
    int priority_no;
    int completionTime = 0;
    int turnAroundTime = 0;
    int waitingTime = 0;
    int remainingTime = 0;
};

void resetProcesses(vector<Process>& processes, const vector<Process>& original){
    for(int i=0; i<processes.size(); i++){
        processes[i] = original[i];
        processes[i].completionTime = 0;
        processes[i].turnAroundTime = 0;
        processes[i].waitingTime = 0;
        processes[i].remainingTime = original[i].burstTime;
        processes[i].priority_no = original[i].priority_no;
    }
}

//FCFS Non preemptive
void FCFS(vector<Process>& processes){
    int time = 0;
    sort(processes.begin(), processes.end(), [](auto a, auto b){
        return (a.arrivalTime == b.arrivalTime) ? (a.pid < b.pid) : (a.arrivalTime < b.arrivalTime);
    });

    for(auto &p : processes){
        if(time < p.arrivalTime) time = p.arrivalTime;
        time += p.burstTime;
        p.completionTime = time;
        p.turnAroundTime = p.completionTime - p.arrivalTime;
        p.waitingTime = p.turnAroundTime - p.burstTime;
    }
}

//SJF Preemptive
void SJF(vector<Process>& processes){
    int n = processes.size();
    int time = 0;
    int completed = 0;

    while(completed < n){
        int shortest = -1;
        for(int i=0; i<n; i++){
            if(processes[i].arrivalTime <= time && processes[i].remainingTime > 0){
                if(shortest == -1 || processes[i].remainingTime < processes[shortest].remainingTime || 
                   (processes[i].remainingTime == processes[shortest].remainingTime && processes[i].pid < processes[shortest].pid)){
                    shortest = i;
                }
            }
        }

        if(shortest == -1){
            time++;
            continue;
        }

        processes[shortest].remainingTime--;
        time++;

        if(processes[shortest].remainingTime == 0){
            completed++;
            processes[shortest].completionTime = time;
            processes[shortest].turnAroundTime = processes[shortest].completionTime - processes[shortest].arrivalTime;
            processes[shortest].waitingTime = processes[shortest].turnAroundTime - processes[shortest].burstTime;
        }
    }
}

//Round Robin Preemptive
void roundrobin(vector<Process>& processes, int quantum) {
    int n = processes.size();
    queue<int> q;
    vector<bool> inQueue(n, false);
    int time = 0;
    int completed = 0;

    for (int i = 0; i < n; i++) {
        if (processes[i].arrivalTime == 0) {
            q.push(i);
            inQueue[i] = true;
        }
    }

    while (completed < n) {
        if (q.empty()) {
            int nextArrival = INT_MAX;
            for (int i = 0; i < n; i++) {
                if (processes[i].remainingTime > 0 && processes[i].arrivalTime < nextArrival) {
                    nextArrival = processes[i].arrivalTime;
                }
            }
            time = nextArrival;
            for (int i = 0; i < n; i++) {
                if (processes[i].arrivalTime <= time && processes[i].remainingTime > 0 && !inQueue[i]) {
                    q.push(i);
                    inQueue[i] = true;
                }
            }
        }

        int idx = q.front();
        q.pop();
        inQueue[idx] = false;

        int runTime = min(quantum, processes[idx].remainingTime);
        processes[idx].remainingTime -= runTime;
        int startTime = time;
        time += runTime;

        for (int i = 0; i < n; i++) {
            if (processes[i].arrivalTime > startTime && processes[i].arrivalTime <= time && 
                processes[i].remainingTime > 0 && !inQueue[i]) {
                q.push(i);
                inQueue[i] = true;
            }
        }

        if (processes[idx].remainingTime > 0) {
            q.push(idx);
            inQueue[idx] = true;
        } else {
            completed++;
            processes[idx].completionTime = time;
            processes[idx].turnAroundTime = processes[idx].completionTime - processes[idx].arrivalTime;
            processes[idx].waitingTime = processes[idx].turnAroundTime - processes[idx].burstTime;
        }
    }
}


void priority(vector<Process>& processes){
    int n = processes.size();
    int time = 0;
    int completed = 0;

    vector<bool> isCompleted(n, false);

    while(completed < n){
        int idx = -1;
        int bestPriority = INT_MAX; // Lower number means higher priority

        for(int i=0; i<n; i++){
            if(processes[i].arrivalTime <= time && !isCompleted[i]){
                if(processes[i].priority_no < bestPriority || 
                   (processes[i].priority_no == bestPriority && (idx == -1 || processes[i].pid < processes[idx].pid))){
                    bestPriority = processes[i].priority_no;
                    idx = i;
                }
            }
        }

        if(idx == -1){
            time++;
            continue;
        }

        time += processes[idx].burstTime;
        processes[idx].completionTime = time;
        processes[idx].turnAroundTime = processes[idx].completionTime - processes[idx].arrivalTime;
        processes[idx].waitingTime = processes[idx].turnAroundTime - processes[idx].burstTime;
        isCompleted[idx] = true;
        completed++;
    }
}

void printTable(const vector<Process>& processes){
    cout << "PID\tArrival\tBurst\tPriority\tCompletion\tTurnAround\tWaiting\n";
    for(const auto &p : processes){
        cout << p.pid << "\t" << p.arrivalTime << "\t" << p.burstTime << "\t" << p.priority_no << "\t\t" 
             << p.completionTime << "\t\t" << p.turnAroundTime << "\t\t" << p.waitingTime << "\n";
    }
}

pair<double,double> printAverage(const vector<Process>& processes){
    double totalTAT = 0, totalWT = 0;
    for(const auto &p : processes){
        totalTAT += p.turnAroundTime;
        totalWT += p.waitingTime;
    }
    double avgTAT = totalTAT / processes.size();
    double avgWT = totalWT / processes.size();
    cout << fixed << setprecision(2);
    cout << "Average Turnaround Time: " << avgTAT << "\n";
    cout << "Average Waiting Time: " << avgWT << "\n";
    return {avgTAT, avgWT};
}

int main(){
    int n;
    cout << "Enter number of processes: ";
    cin >> n;

    vector<Process> original(n);
    cout << "Enter Arrival Time, Burst Time and Priority for each process:\n";
    for(int i=0; i<n; i++){
        original[i].pid = i+1;
        cin >> original[i].arrivalTime >> original[i].burstTime >> original[i].priority_no;
        original[i].remainingTime = original[i].burstTime;
    }

    vector<Process> processes = original;

    cout << "\n--- FCFS Non-Preemptive Scheduling ---\n";
    resetProcesses(processes, original);
    FCFS(processes);
    printTable(processes);
    auto fcfs_avg = printAverage(processes);

    cout << "\n--- SJF Preemptive Scheduling ---\n";
    resetProcesses(processes, original);
    SJF(processes);
    printTable(processes);
    auto sjf_avg = printAverage(processes);

    cout << "\n--- Round Robin Preemptive Scheduling ---\n";
    int quantum;
    cout << "Enter Time Quantum for Round Robin: ";
    cin >> quantum;
    resetProcesses(processes, original);
    roundrobin(processes, quantum);
    printTable(processes);
    auto rr_avg = printAverage(processes);

    cout << "\n--- Priority Non-Preemptive Scheduling ---\n";
    resetProcesses(processes, original);
    priority(processes);
    printTable(processes);
    auto priority_avg = printAverage(processes);

    cout << "\nSummary Table (Average Turnaround Time and Waiting Time):\n";
    cout << "Algorithm\t\tAvg Turnaround Time\tAvg Waiting Time\n";
    cout << "FCFS\t\t\t" << fcfs_avg.first << "\t\t\t" << fcfs_avg.second << "\n";
    cout << "SJF\t\t\t" << sjf_avg.first << "\t\t\t" << sjf_avg.second << "\n";
    cout << "Round Robin\t\t" << rr_avg.first << "\t\t\t" << rr_avg.second << "\n";
    cout << "Priority\t\t" << priority_avg.first << "\t\t\t" << priority_avg.second << "\n";

    return 0;
}

#include <iostream>
#include <algorithm>
using namespace std;

struct Process {
    int id;
    int burstTime;
    int waitingTime;
    int turnaroundTime;
};

// Comparison function to sort processes by burst time
bool compare(Process a, Process b) {
    return a.burstTime < b.burstTime;
}

int main() {
    int n;
    
    cout << "Enter the number of processes: ";
    cin >> n;
    
    Process processes[n];
    
    // Input burst times
    for (int i = 0; i < n; i++) {
        processes[i].id = i + 1;
        cout << "Enter burst time for Process " << processes[i].id << ": ";
        cin >> processes[i].burstTime;
    }
    
    // Sorting processes by burst time
    sort(processes, processes + n, compare);
    
    // Calculating waiting time
    processes[0].waitingTime = 0;
    for (int i = 1; i < n; i++) {
        processes[i].waitingTime = processes[i - 1].waitingTime + processes[i - 1].burstTime;
    }
    
    // Calculating turnaround time
    for (int i = 0; i < n; i++) {
        processes[i].turnaroundTime = processes[i].waitingTime + processes[i].burstTime;
    }
    
    // Calculating averages
    float totalWaitingTime = 0, totalTurnaroundTime = 0;
    for (int i = 0; i < n; i++) {
        totalWaitingTime += processes[i].waitingTime;
        totalTurnaroundTime += processes[i].turnaroundTime;
    }
    
    float avgWaitingTime = totalWaitingTime / n;
    float avgTurnaroundTime = totalTurnaroundTime / n;
    
    // Display results
    cout << "\nProcess\tBurst Time\tWaiting Time\tTurnaround Time" << endl;
    for (int i = 0; i < n; i++) {
        cout << "P" << processes[i].id << "\t"
             << processes[i].burstTime << "\t\t"
             << processes[i].waitingTime << "\t\t"
             << processes[i].turnaroundTime << endl;
    }
    
    cout << "\nAverage Waiting Time: " << avgWaitingTime << endl;
    cout << "Average Turnaround Time: " << avgTurnaroundTime << endl;
    
    return 0;
}
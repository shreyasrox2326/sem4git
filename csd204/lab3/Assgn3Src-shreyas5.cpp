#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>  // For setting precision

using namespace std;

struct Process {
    int pid;         // Process ID
    int arrival;     // Arrival time
    int burst;       // Burst time
    int priority;    // Priority
    int completion;  // Completion time
    int waiting;     // Waiting time
    int turnaround;  // Turnaround time
};

bool comparePriority(Process a, Process b) {
    // Compare processes based on priority, lower priority number means higher priority
    return a.priority < b.priority;
}

void calculateTimes(vector<Process>& processes) {
    int n = processes.size();
    int currentTime = 0;
    
    for (int i = 0; i < n; i++) {
        // Ensure the process arrives before the current time
        if (processes[i].arrival > currentTime) {
            currentTime = processes[i].arrival;
        }
        
        // Completion time is when the process finishes
        processes[i].completion = currentTime + processes[i].burst;
        
        // Turnaround time is the time from arrival to completion
        processes[i].turnaround = processes[i].completion - processes[i].arrival;
        
        // Waiting time is the time the process spent waiting in the queue
        processes[i].waiting = processes[i].turnaround - processes[i].burst;
        
        // Update the current time after process completion
        currentTime = processes[i].completion;
    }
}

void displayResults(const vector<Process>& processes) {
    cout << "\nPID\tArrival\tBurst\tPriority\tCompletion\tTurnaround\tWaiting\n";
    for (const auto& p : processes) {
        cout << p.pid << "\t"
             << p.arrival << "\t"
             << p.burst << "\t"
             << p.priority << "\t\t"
             << p.completion << "\t\t"
             << p.turnaround << "\t\t"
             << p.waiting << endl;
    }
}

int main() {
    int n;
    
    // Get the number of processes
    cout << "Enter number of processes: ";
    cin >> n;

    vector<Process> processes(n);
    
    // Input process details
    for (int i = 0; i < n; i++) {
        processes[i].pid = i + 1;
        cout << "\nEnter Arrival Time, Burst Time, and Priority for Process " << i + 1 << ": ";
        cin >> processes[i].arrival >> processes[i].burst >> processes[i].priority;
    }

    // Sort processes by priority
    sort(processes.begin(), processes.end(), comparePriority);

    // Calculate waiting time, turnaround time, and completion time
    calculateTimes(processes);

    // Display the results
    displayResults(processes);

    return 0;
}

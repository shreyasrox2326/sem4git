#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

using namespace std;

struct Process {
    int pid;         // Process ID
    int arrival;     // Arrival time
    int burst;       // Burst time
    int priority;    // Priority
    int remaining;   // Remaining burst time (for preemption)
    int completion;  // Completion time
    int waiting;     // Waiting time
    int turnaround;  // Turnaround time
};

bool compareArrival(Process a, Process b) {
    return a.arrival < b.arrival;
}

bool comparePriority(Process a, Process b) {
    return a.priority < b.priority;
}

void calculateTimes(vector<Process>& processes) {
    int n = processes.size();
    int currentTime = 0;
    int completed = 0;
    vector<bool> isCompleted(n, false);
    
    while (completed < n) {
        // Find the process with the highest priority that has arrived and not completed
        Process* currentProcess = nullptr;
        int highestPriority = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (processes[i].arrival <= currentTime && !isCompleted[i] && processes[i].priority < highestPriority) {
                currentProcess = &processes[i];
                highestPriority = processes[i].priority;
            }
        }

        // If no process is ready to execute, increment time
        if (currentProcess == nullptr) {
            currentTime++;
            continue;
        }

        // Execute the current process for 1 time unit
        currentProcess->remaining--;
        currentTime++;

        // If the process is completed
        if (currentProcess->remaining == 0) {
            currentProcess->completion = currentTime;
            currentProcess->turnaround = currentTime - currentProcess->arrival;
            currentProcess->waiting = currentProcess->turnaround - currentProcess->burst;
            isCompleted[currentProcess - &processes[0]] = true;
            completed++;
        }
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
        processes[i].remaining = processes[i].burst;  // Initialize remaining burst time
    }

    // Sort processes by arrival time
    sort(processes.begin(), processes.end(), compareArrival);

    // Calculate waiting time, turnaround time, and completion time
    calculateTimes(processes);

    // Display the results
    displayResults(processes);

    return 0;
}

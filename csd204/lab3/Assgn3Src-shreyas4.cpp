#include <iostream>
#include <queue>
using namespace std;

struct Process {
    int pid;
    int burst_time;
    int remaining_time;
    int waiting_time;
    int turnaround_time;
};

// Function to implement Round Robin Scheduling
void round_robin_scheduling(Process p[], int n, int time_quantum) {
    queue<int> q;
    int total_time = 0, completed = 0;
    int total_waiting_time = 0, total_turnaround_time = 0;
    int last_execution_time[n] = {0}; // Track last execution time for each process

    // Initialize processes and add them to queue
    for (int i = 0; i < n; i++) {
        q.push(i);
        p[i].waiting_time = 0;
        p[i].turnaround_time = 0;
        last_execution_time[i] = 0;
    }

    while (!q.empty()) {
        int i = q.front();
        q.pop();

        // Calculate waiting time before executing
        p[i].waiting_time += total_time - last_execution_time[i];

        // Execute the process for time_quantum or remaining_time
        int exec_time = min(time_quantum, p[i].remaining_time);
        total_time += exec_time;
        p[i].remaining_time -= exec_time;
        last_execution_time[i] = total_time;

        // If process still has remaining time, push it back into the queue
        if (p[i].remaining_time > 0) {
            q.push(i);
        } else {
            completed++;
            p[i].turnaround_time = total_time;
            total_waiting_time += p[i].waiting_time;
            total_turnaround_time += p[i].turnaround_time;
        }
    }

    // Display results
    cout << "\nProcess\tBurst Time\tWaiting Time\tTurnaround Time\n";
    for (int i = 0; i < n; i++) {
        cout << "P" << p[i].pid << "\t" << p[i].burst_time << "\t\t" << p[i].waiting_time << "\t\t" << p[i].turnaround_time << "\n";
    }

    cout << "\nAverage Waiting Time: " << (float)total_waiting_time / n;
    cout << "\nAverage Turnaround Time: " << (float)total_turnaround_time / n << endl;
}

int main() {
    int n, time_quantum;
    cout << "Enter the number of processes: ";
    cin >> n;
    cout << "Enter time quantum: ";
    cin >> time_quantum;

    Process p[n];
    for (int i = 0; i < n; i++) {
        cout << "Enter burst time for Process " << i + 1 << ": ";
        cin >> p[i].burst_time;
        p[i].pid = i + 1;
        p[i].remaining_time = p[i].burst_time;
    }

    round_robin_scheduling(p, n, time_quantum);
    return 0;
}

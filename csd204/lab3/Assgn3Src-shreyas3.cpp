#include <iostream>
#include <limits.h>
using namespace std;

struct Process {
    int pid;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int completion_time;
    int waiting_time;
    int turnaround_time;
};

void srtf_scheduling(Process p[], int n) {
    int completed = 0, time = 0, min_index, min_remaining_time;
    int total_waiting_time = 0, total_turnaround_time = 0;
    bool is_completed[n] = {false};

    while (completed != n) {
        min_index = -1;
        min_remaining_time = INT_MAX;

        // Find process with shortest remaining time at the current time
        for (int i = 0; i < n; i++) {
            if (p[i].arrival_time <= time && !is_completed[i] && p[i].remaining_time < min_remaining_time) {
                min_remaining_time = p[i].remaining_time;
                min_index = i;
            }
        }

        if (min_index == -1) {
            time++;
            continue;
        }

        // Execute the selected process for one unit of time
        p[min_index].remaining_time--;
        time++;

        // If the process is completed
        if (p[min_index].remaining_time == 0) {
            completed++;
            is_completed[min_index] = true;
            p[min_index].completion_time = time;
            p[min_index].turnaround_time = p[min_index].completion_time - p[min_index].arrival_time;
            p[min_index].waiting_time = p[min_index].turnaround_time - p[min_index].burst_time;

            total_waiting_time += p[min_index].waiting_time;
            total_turnaround_time += p[min_index].turnaround_time;
        }
    }

    // Display the results
    cout << "\nProcess\tArrival Time\tBurst Time\tCompletion Time\tWaiting Time\tTurnaround Time\n";
    for (int i = 0; i < n; i++) {
        cout << "P" << p[i].pid << "\t" << p[i].arrival_time << "\t\t"
             << p[i].burst_time << "\t\t" << p[i].completion_time << "\t\t"
             << p[i].waiting_time << "\t\t" << p[i].turnaround_time << "\n";
    }

    cout << "\nAverage Waiting Time: " << (float)total_waiting_time / n;
    cout << "\nAverage Turnaround Time: " << (float)total_turnaround_time / n << endl;
}

int main() {
    int n;
    cout << "Enter the number of processes: ";
    cin >> n;

    Process p[n];

    for (int i = 0; i < n; i++) {
        cout << "Enter arrival time and burst time for Process " << i + 1 << ": ";
        cin >> p[i].arrival_time >> p[i].burst_time;
        p[i].pid = i + 1;
        p[i].remaining_time = p[i].burst_time; // Initialize remaining time
    }

    srtf_scheduling(p, n);
    return 0;
}

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

// Bubble Sort - Parent Process
void bubbleSort(int arr[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

// Selection Sort - Child Process
void selectionSort(int arr[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        int minIndex = i;
        for (int j = i + 1; j < n; j++)
        {
            if (arr[j] < arr[minIndex])
            {
                minIndex = j;
            }
        }
        swap(arr[i], arr[minIndex]);
    }
}

int main()
{
    int n;
    cout << "Enter number of elements: ";
    cin >> n;
    int arr[n];

    cout << "Enter elements: ";
    for (int i = 0; i < n; i++)
    {
        cin >> arr[i];
    }

    for (int run = 0; run < 2; run++)
    {

        pid_t pid;
        pid = fork();

        if (pid < 0)
        {
            cerr << "Fork failed!" << endl;
            return 1;
        }

        if (pid > 0)
        { // Parent Process
            int arr2[n];
            for (int idx = 0; idx < n; idx++)
                arr2[idx] = arr[idx];

            cout << "Parent Process (PID: " << getpid() << ") sorting using Bubble Sort..." << endl;
            bubbleSort(arr2, n);

            cout << "Sorted by Parent: ";
            for (int i = 0; i < n; i++)
                cout << arr2[i] << " ";
            cout << endl;

            int status;

            // run 0: parent will wait
            // run 1: no waiting
            if (!run) 
            wait(&status); // Waiting to prevent zombie process

            cout << "Parent Process exiting.\n\n" << endl;
        }
        else
        { // Child Process
            int arr2[n];
            for (int idx = 0; idx < n; idx++)
                arr2[idx] = arr[idx];

            cout << "Child Process (PID: " << getpid() << ", PPID: " << getppid() << ") sorting using Selection Sort..." << endl;

            selectionSort(arr2, n);

            cout << "Sorted by Child: ";
            for (int i = 0; i < n; i++)
                cout << arr2[i] << " ";
            cout << endl;

            usleep(1000 * 100); // Sleep to demonstrate orphan process
            if (!run) //run #0
            cout << "Child Process (PID: " << getpid() << ", New PPID: " << getppid() << ") is not an orphan because parent waited" << endl;

            if (run) // run #1
            cout << "Child Process (PID: " << getpid() << ", New PPID: " << getppid() << ") is now orphan because parent exited early." << endl;
            
            
            return 0;
        }
    }
}

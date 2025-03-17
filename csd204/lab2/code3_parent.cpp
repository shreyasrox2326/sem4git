#include <iostream>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int main() {
    vector<int> arr;
    int n, key;

    cout << "Enter number of elements: ";
    cin >> n;

    cout << "Enter elements: ";
    for (int i = 0; i < n; i++) {
        int temp;
        cin >> temp;
        arr.push_back(temp);
    }

    cout << "Enter element to search: ";
    cin >> key;

    // Sort the array
    sort(arr.begin(), arr.end());

    pid_t pid = fork();

    if (pid < 0) {
        cerr << "Fork failed!" << endl;
        return 1;
    } else if (pid == 0) { // Child process
        // Convert sorted array to command-line arguments
        vector<string> args;
        args.push_back("./child"); // Child program executable
        args.push_back(to_string(n)); // First argument: size of array

        for (int num : arr) {
            args.push_back(to_string(num)); // Add sorted numbers as args
        }
        
        args.push_back(to_string(key)); // Add the search key
        
        // Convert vector<string> to char* array
        vector<char*> argv;
        for (auto &arg : args) {
            argv.push_back(&arg[0]);
        }
        argv.push_back(nullptr); // NULL termination for execvp()

        // Execute child program
        execvp(argv[0], argv.data());

        // If exec fails
        cerr << "Exec failed!" << endl;
        return 1;
    } else { // Parent process
        wait(nullptr); // Wait for child to complete
    }

    return 0;
}

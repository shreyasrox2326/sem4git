#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

// Function for binary search
int binarySearch(vector<int>& arr, int key) {
    int left = 0, right = arr.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (arr[mid] == key)
            return mid;
        else if (arr[mid] < key)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return -1;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: ./child <size> <sorted_array> <key>" << endl;
        return 1;
    }

    int n = atoi(argv[1]); // Convert first argument to size of array
    vector<int> arr;

    for (int i = 2; i < argc - 1; i++) {
        arr.push_back(atoi(argv[i])); // Convert remaining arguments to array elements
    }

    int key = atoi(argv[argc - 1]); // Last argument is the key to search

    int result = binarySearch(arr, key);

    if (result != -1)
        cout << "Element found at index: " << result << endl;
    else
        cout << "Element not found" << endl;

    return 0;
}

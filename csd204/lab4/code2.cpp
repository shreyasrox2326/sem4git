#include <iostream>
#include <pthread.h>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>

#define SIZE 10000
using namespace std;
using namespace chrono;

vector<int> arr(SIZE); // Global array to sort
struct ThreadData {
    int start, end;
};

// Function to generate random numbers
void generateRandomArray() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(1, 100000);
    for (int &num : arr) {
        num = dist(gen);
    }
}

// Function for each thread to sort a portion of the array
void* threadSort(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    sort(arr.begin() + data->start, arr.begin() + data->end);
    return nullptr;
}

// Function for multithreaded sorting
void multiThreadSort(int num_threads) {
    vector<pthread_t> threads(num_threads);
    vector<ThreadData> thread_data(num_threads);
    int chunk_size = SIZE / num_threads;

    // Create threads to sort sections of the array
    for (int i = 0; i < num_threads; ++i) {
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i == num_threads - 1) ? SIZE : (i + 1) * chunk_size;
        pthread_create(&threads[i], nullptr, threadSort, &thread_data[i]);
    }

    // Join threads
    for (pthread_t &thread : threads) {
        pthread_join(thread, nullptr);
    }

    // Merge sorted sections
    for (int size = chunk_size; size < SIZE; size *= 2) {
        for (int start = 0; start < SIZE - size; start += 2 * size) {
            int mid = start + size;
            int end = min(start + 2 * size, SIZE);
            inplace_merge(arr.begin() + start, arr.begin() + mid, arr.begin() + end);
        }
    }
}

int main() {
    vector<int> thread_counts = {2, 4, 6, 8, 10, 12, 16, 20};
    generateRandomArray();

    // Single-threaded sorting
    vector<int> single_threaded_arr = arr;
    auto start = high_resolution_clock::now();
    sort(single_threaded_arr.begin(), single_threaded_arr.end());
    auto end = high_resolution_clock::now();
    cout << "Single-threaded Sort Time: " << duration_cast<microseconds>(end - start).count() << " µs" << endl;

    // Multithreaded sorting
    for (int threads : thread_counts) {
        vector<int> temp_arr = arr;
        start = high_resolution_clock::now();
        multiThreadSort(threads);
        end = high_resolution_clock::now();
        cout << "Threads: " << threads << " -> Sort Time: " << duration_cast<microseconds>(end - start).count() << " µs" << endl;
    }

    return 0;
}

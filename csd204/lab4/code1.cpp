#include <iostream>
#include <pthread.h>
#include <vector>
#include <chrono>

#define NUMBERS 1000000

using namespace std;
using namespace chrono;

// Structure to hold arguments for threads
struct ThreadData {
    long start;
    long end;
    long long partial_sum;
};

// Function for summing in each thread
void* thread_sum(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    data->partial_sum = 0;
    for (long i = data->start; i <= data->end; ++i) {
        data->partial_sum += i;
    }
    return nullptr;
}

// Sequential sum function
long long sequential_sum() {
    long long sum = 0;
    for (long i = 1; i <= NUMBERS; ++i) {
        sum += i;
    }
    return sum;
}

// Multithreading sum function
long long parallel_sum(int num_threads) {
    vector<pthread_t> threads(num_threads);
    vector<ThreadData> thread_data(num_threads);
    long chunk_size = NUMBERS / num_threads;

    for (int i = 0; i < num_threads; ++i) {
        thread_data[i].start = i * chunk_size + 1;
        thread_data[i].end = (i == num_threads - 1) ? NUMBERS : (i + 1) * chunk_size;
        pthread_create(&threads[i], nullptr, thread_sum, &thread_data[i]);
    }

    long long total_sum = 0;
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], nullptr);
        total_sum += thread_data[i].partial_sum;
    }

    return total_sum;
}

int main() {
    vector<int> thread_counts = {1, 2, 3, 4, 6, 8, 10, 12, 16, 20};

    // Sequential execution
    auto start = high_resolution_clock::now();
    long long seq_sum = sequential_sum();
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    cout << "Sequential Sum: " << seq_sum << ", Time: " << duration << " µs" << endl;

    // Multithreaded execution
    for (int threads : thread_counts) {
        start = high_resolution_clock::now();
        long long par_sum = parallel_sum(threads);
        end = high_resolution_clock::now();
        duration = duration_cast<microseconds>(end - start).count();
        cout << "Threads: " << threads << " -> Sum: " << par_sum << ", Time: " << duration << " µs" << endl;
    }

    return 0;
}
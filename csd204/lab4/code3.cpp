#include <iostream>
#include <pthread.h>
#include <chrono>

using namespace std;
using namespace chrono;

const int ITERATIONS = 1000000;
int counter = 0;
pthread_mutex_t lock;

void* incrementWithoutLock(void*) {
    for (int i = 0; i < ITERATIONS; ++i) {
        counter++; // No lock, race condition occurs
    }
    return nullptr;
}

void* incrementWithLock(void*) {
    for (int i = 0; i < ITERATIONS; ++i) {
        pthread_mutex_lock(&lock);
        counter++; // Protected by lock
        pthread_mutex_unlock(&lock);
    }
    return nullptr;
}

void runThreads(void* (*func)(void*), const string& description) {
    counter = 0; // Reset counter
    pthread_t t1, t2;
    auto start = high_resolution_clock::now();
    pthread_create(&t1, nullptr, func, nullptr);
    pthread_create(&t2, nullptr, func, nullptr);
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    auto end = high_resolution_clock::now();
    cout << description << " Counter: " << counter << ", Time: " << duration_cast<microseconds>(end - start).count() << " µs" << endl;
}

int main() {
    pthread_mutex_init(&lock, nullptr);

    // Without Lock (Race Condition)
    runThreads(incrementWithoutLock, "Without Lock ->");
    
    // With Lock (Thread-Safe)
    runThreads(incrementWithLock, "With Lock ->");
    
    pthread_mutex_destroy(&lock);
    return 0;
}

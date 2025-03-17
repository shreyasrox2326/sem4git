#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <semaphore.h>  // POSIX semaphores
#include <random>
#include <chrono>

std::queue<int> buffer;
std::mutex mtx;
sem_t emptySlots, fullSlots;

int BUFFER_SIZE, num_producers, num_consumers;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> dist(1, 500);

void producer(int id) {
    while (true) {
        int item = dist(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen) + 200)); // Simulate work
        
        sem_wait(&emptySlots);  // Wait if buffer is full
        {
            std::lock_guard<std::mutex> lock(mtx);
            buffer.push(item);
            std::cout << "Producer " << id << " produced: " << item << "\n";
            
            if (buffer.size() == BUFFER_SIZE)
                std::cout << "[BUFFER FULL]\n";
        }
        sem_post(&fullSlots);  // Signal that data is available
    }
}

void consumer(int id) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen) + 200)); // Simulate work
        
        sem_wait(&fullSlots);  // Wait if buffer is empty
        int item;
        {
            std::lock_guard<std::mutex> lock(mtx);
            item = buffer.front();
            buffer.pop();
            std::cout << "Consumer " << id << " consumed: " << item << "\n";

            if (buffer.empty())
                std::cout << "[BUFFER EMPTY]\n";
        }
        sem_post(&emptySlots);  // Signal that space is available
    }
}

int main() {
    std::cout << "Enter buffer size: ";
    std::cin >> BUFFER_SIZE;
    std::cout << "Enter number of producers: ";
    std::cin >> num_producers;
    std::cout << "Enter number of consumers: ";
    std::cin >> num_consumers;

    sem_init(&emptySlots, 0, BUFFER_SIZE);  // Max buffer size
    sem_init(&fullSlots, 0, 0);  // Initially empty

    std::thread producers[num_producers], consumers[num_consumers];

    for (int i = 0; i < num_producers; ++i)
        producers[i] = std::thread(producer, i + 1);

    for (int i = 0; i < num_consumers; ++i)
        consumers[i] = std::thread(consumer, i + 1);

    for (auto& p : producers) p.join();
    for (auto& c : consumers) c.join();

    sem_destroy(&emptySlots);
    sem_destroy(&fullSlots);

    return 0;
}

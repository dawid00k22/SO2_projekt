#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <chrono>
#include <atomic>

// Custom Semaphore implementation
class Semaphore {
private:
    std::mutex mtx;
    std::condition_variable cv;
    int count;

public:
    Semaphore(int count = 0) : count(count) {}

    void acquire() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]() { return count > 0; });
        count--;
    }

    void release() {
        std::unique_lock<std::mutex> lock(mtx);
        count++;
        cv.notify_one();
    }
};

// Global variables
std::vector<std::unique_ptr<std::mutex>> forks;  // Mutexes representing forks
std::mutex cout_mutex;                           // Mutex for synchronized console output
std::vector<int> eat_counts;                     // Statistics: how many times each philosopher ate
std::atomic<bool> running{true};                 // Flag to control program execution

// Philosopher function
void philosopher(int id, int num_philosophers, Semaphore& table_semaphore) {
    int left_fork = id;
    int right_fork = (id + 1) % num_philosophers;

    while (running) {  // Check the running flag
        // Thinking
        {
            std::lock_guard<std::mutex> cout_lock(cout_mutex);
            std::cout << "Philosopher " << id << " is thinking." << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        // Acquire forks
        table_semaphore.acquire();  // Limit the number of philosophers trying to pick up forks
        forks[left_fork]->lock();
        forks[right_fork]->lock();

        // Eating
        {
            std::lock_guard<std::mutex> cout_lock(cout_mutex);
            std::cout << "Philosopher " << id << " is eating." << std::endl;
            eat_counts[id]++;  // Increment eat count for this philosopher
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        // Release forks
        forks[left_fork]->unlock();
        forks[right_fork]->unlock();
        table_semaphore.release();
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <number_of_philosophers>" << std::endl;
        return 1;
    }

    int num_philosophers = std::stoi(argv[1]);
    if (num_philosophers < 2) {
        std::cerr << "Number of philosophers must be at least 2." << std::endl;
        return 1;
    }

    // Initialize forks (mutexes)
    forks.resize(num_philosophers);
    for (int i = 0; i < num_philosophers; ++i) {
        forks[i] = std::make_unique<std::mutex>();
    }

    // Initialize eat counts
    eat_counts.resize(num_philosophers, 0);

    // Initialize semaphore to prevent deadlock
    Semaphore table_semaphore(num_philosophers - 1);

    // Create philosopher threads
    std::vector<std::thread> philosophers;
    for (int i = 0; i < num_philosophers; ++i) {
        philosophers.emplace_back(philosopher, i, num_philosophers, std::ref(table_semaphore));
    }

    // Let the program run for 20 seconds
    std::this_thread::sleep_for(std::chrono::seconds(10));

    // Stop the philosophers
    running = false;

    // Join threads
    for (auto& ph : philosophers) {
        ph.join();
    }

    // Print statistics
    std::cout << "\n--- Statistics ---\n";
    for (int i = 0; i < num_philosophers; ++i) {
        std::cout << "Philosopher " << i << " ate " << eat_counts[i] << " times.\n";
    }

    return 0;
}

#ifndef QUEUE_HPP
#define QUEUE_HPP
#include <condition_variable>   // std::condition_variable
#include <mutex>                // std::mutex, std::lock_guard, std::unique_lock
#include <queue>                // std::queue

template <class T>
class SynchronizedQueue {
    mutable std::mutex mutex;   // mutable - mutext needs to be able to lock, even when dealing with const references
    std::queue<T> queue;
    std::condition_variable condition;

public:
    //
    // Pushes a new value to the end of the queue
    //
    void push(T t) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(t);
        }

        // Notify the consumer, that something is waiting in the queue now
        condition.notify_one();
    }

    //
    // Try to receive a value from the queue - return false after the timeout expires
    //
    bool pop(T& t, std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mutex);

        // Wait for condition that something was added to the queue
        if (!condition.wait_for(lock, timeout, [this] { return !queue.empty(); })) {
            return false;
        }

        t = queue.front();
        queue.pop();
        return true;
    }
};

#endif
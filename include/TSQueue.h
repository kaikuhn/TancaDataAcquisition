#pragma once

#include <condition_variable>
#include <optional>
#include <iostream>
#include <mutex>
#include <queue>

// Thread-safe queue
template <typename T>
class TSQueue {
private:
    // Underlying queue
    std::queue<T> m_queue;

    // mutex for thread synchronization
    std::mutex m_mutex;

    // Condition variable for signaling
    std::condition_variable m_cond;

public:
    // Pushes an element to the queue
    void push(T item)
    {
        // Acquire lock
        std::unique_lock<std::mutex> lock(m_mutex);

        // Add item
        m_queue.push(std::move(item));
    }

    // remove element from the queue
    std::optional<T> pop() {
        // acquire lock
        std::unique_lock<std::mutex> lock(m_mutex);

        // return nullopt, if queue is empty
        if (m_queue.empty()) return std::nullopt;
        
        // else retrieve item
        T item = std::move(m_queue.front());
        m_queue.pop();

        // return item
        return std::move(item);
    }
};
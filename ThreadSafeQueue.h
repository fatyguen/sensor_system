#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class ThreadSafeQueue {
private:
    std::queue<T> queue;
    std::mutex mtx;
    std::condition_variable cv;
    bool closed = false;

public:
    void push(const T& value) {
        std::lock_guard<std::mutex> lock(mtx);
        if (closed) return;

        queue.push(value);
        cv.notify_one();
    }

    // returns false if queue is closed and empty
    bool pop(T& result) {
        std::unique_lock<std::mutex> lock(mtx);

        cv.wait(lock, [&]() {
            return !queue.empty() || closed;
        });

        if (queue.empty() && closed)
            return false;

        result = queue.front();
        queue.pop();
        return true;
    }

    void close() {
        std::lock_guard<std::mutex> lock(mtx);
        closed = true;
        cv.notify_all();
    }
};
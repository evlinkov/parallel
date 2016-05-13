#ifndef syncQueue_h
#define syncQueue_h

#include <mutex>
#include <queue>
#include <iostream>
#include <exception>
#include <condition_variable>


template <typename T>
class SyncQueue {
    
private:
    
    std::mutex mut;
    std::queue<T> elements;
    std::condition_variable condition;
    
public:
    
    T front() {
        std::unique_lock<std::mutex> mutLock(mut);
        while (elements.empty()) {
            condition.wait(mutLock);
        }
        return elements.front();
    }
    void pop() {
        std::unique_lock<std::mutex> mutLock(mut);
        if (elements.empty()) {
            condition.wait(mutLock);
        }
        elements.pop();
    }
    void push(const T& element) {
        std::unique_lock<std::mutex> mutLock(mut);
        elements.push(element);
        mutLock.unlock();
        condition.notify_one();
    }
    ~SyncQueue() {
        while (!elements.empty()) {
            elements.pop();
        }
    }
};

#endif

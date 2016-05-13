#ifndef futex_h
#define futex_h

#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>
#include <exception>

class Exception: public std::exception
{
    virtual const char* message() const throw()
    {
        return "bad unlock";
    }
};

class Futex {
    
private:

    int hash;
    std::atomic<int> futex;
    
public:
    
    Futex() {
        futex.store(-1);
        hash = -1;
    }
    void lock(int data) {
        int current = hash;
        while (!futex.compare_exchange_strong(current, data)) {
            current = hash;
            std::this_thread::yield();
        }
    }
    void unlock(int data) {
        int current = data;
        hash = -1;
        if (!futex.compare_exchange_strong(current, hash)) {
            throw Exception();
        }
    }
};

#endif

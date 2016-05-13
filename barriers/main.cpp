#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>
#include <vector>
#include <thread>
#include <atomic>

class Futex {
    
public:
    
    Futex();
    bool lock(int);
    bool unlock(int);
    Futex (const Futex&) = delete;
    
private:
    
    int hash;
    std::atomic<int> id;
};

class Exception: public std::exception
{
    virtual const char* message() const throw()
    {
        return "bad unlock";
    }
};

Futex::Futex() {
    id.store(-1);
    hash = -1;
}

bool Futex::lock(int idHash)
{
    int desired = hash;
    while (!id.compare_exchange_strong(desired, idHash, std::memory_order_acquire)) {
        desired = hash;
        std::this_thread::yield();
    }
    return true;
}

bool Futex::unlock(int idHash)
{
    int desired = idHash;
    if (!id.compare_exchange_strong(desired, hash, std::memory_order_release)) {
        throw Exception();
    }
    return true;
}

Futex futex;
std::mutex mut;
int ans, to = 1e8;
std::ofstream out("output.txt");

void increaseFutex(int &value) {
    int thisHash = int(std::hash<std::thread::id>()(std::this_thread::get_id()));
    while (true) {
        futex.lock(thisHash);
        if (ans >= to) {
            futex.unlock(thisHash);
            break;
        }
        ++ans;
        ++value;
        futex.unlock(thisHash);
    }
}

void testFutex(int threadsNumber) {
    ans = 0;
    Futex futex;
    std::vector <int> values(threadsNumber, 0);
    std::vector <std::thread *> t(threadsNumber);
    std::chrono::time_point<std::chrono::system_clock> start, end;
    
    start = std::chrono::system_clock::now();
    
    for (int i = 0; i < threadsNumber; ++i) {
        t[i] = new std::thread(increaseFutex, std::ref(values[i]));
    }
    
    for (int i = 0; i < threadsNumber; ++i) {
        t[i]->join();
    }
    
    end = std::chrono::system_clock::now();
    
    int sum = 0;
    std::cout << "Futex, " << threadsNumber << " threads : ";
    for (int i = 0; i < threadsNumber; ++i) {
        std :: cout << values[i] << " ";
        sum += values[i];
    }
    std::cout << "\n";
    if (sum != to) {
        out << "WTF" << "\n";
        exit(0);
    }
    out << "Futex, " << threadsNumber << " threads : ";
    out << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "\n";
}


int main() {
    std::ios_base::sync_with_stdio(false);
    for (int i = 2; i <= 8; i*=2) {
        testFutex(i);
    }
    return 0;
}

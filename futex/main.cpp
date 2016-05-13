#include <mutex>
#include <vector>
#include "futex.h"
#include <fstream>

std::ofstream out("output.txt");

Futex futex;
std::mutex mut;
int ans, to = 3e5;

void increaseFutex(int &value) {
    int thisHash = std::hash<std::thread::id>()(std::this_thread::get_id());
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

void increaseMutex(int &value) {
    while (true) {
        mut.lock();
        if (ans >= to) {
            mut.unlock();
            break;
        }
        ++ans;
        ++value;
        mut.unlock();
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

void testMutex(int threadsNumber) {
    ans = 0;
    std::vector <int> values(threadsNumber, 0);
    std::vector <std::thread *> t(threadsNumber);
    std::chrono::time_point<std::chrono::system_clock> start, end;
    
    start = std::chrono::system_clock::now();
    
    for (int i = 0; i < threadsNumber; ++i) {
        t[i] = new std::thread(increaseMutex, std::ref(values[i]));
    }
    
    for (int i = 0; i < threadsNumber; ++i) {
        t[i]->join();
    }
    
    end = std::chrono::system_clock::now();
    
    int sum = 0;
    std::cout << "Mutex, " << threadsNumber << " threads : ";
    for (int i = 0; i < threadsNumber; ++i) {
        std :: cout << values[i] << " ";
        sum += values[i];
    }
    std::cout << "\n";
    if (sum != to) {
        out << "WTF" << "\n";
        exit(0);
    }
    out << "Mutex, " << threadsNumber << " threads : ";
    out << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "\n";
}


int main()
{
    std::ios_base::sync_with_stdio(false);
    for (int i = 1; i <= 8; i*=2) {
        testFutex(i);
        testMutex(i);
    }
    return 0;
}


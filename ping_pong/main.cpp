#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>
#include <thread>

char array[65];
static const int iterations = 1e9;

int justdoit(const int a) {
    return (a + 5) % 33 + a * a + (a + a + 1) % (a + 7 + (a + 7 == 0));
}

void f(const int pos) {
    for (int i = 0; i < iterations; ++i) {
        ++array[pos];
    }
}

int main() {
    std::vector<std::thread *> t(2);
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    t[0] = new std::thread(f, 0);
    t[1] = new std::thread(f, 1);
    for (int i = 0; i < 2; ++i) {
        t[i]->join();
    }
    end = std::chrono::system_clock::now();
    std::ofstream out("timer.txt");
    out << "with ping pong : ";
    out << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "\n";
    
    start = std::chrono::system_clock::now();
    t[0] = new std::thread(f, 0);
    t[1] = new std::thread(f, 64);
    for (int i = 0; i < 2; ++i) {
        t[i]->join();
    }
    end = std::chrono::system_clock::now();
    out << "without ping pong : ";
    out << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "\n";
    
    
    return 0;
}

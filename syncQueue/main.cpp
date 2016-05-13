#include <thread>
#include "syncQueue.h"

SyncQueue <int> syncQueue;

void justDoIt(int type) {
    if (type == 0) {
        std::cout << syncQueue.front();
    }
    else {
        if (type == 1) {
            syncQueue.pop();
        }
        else {
            syncQueue.push(type);
        }
    }
}

void checkSyncQueue() {
    std::vector<std::thread *> t(8);
    int a[8] = {0, 0, 2, 3, 1, 1, 4, 2};
    for (int i = 0; i < 8; ++i) {
        t[i] = new std::thread(justDoIt, a[i]);
    }
    for (int i = 0; i < 8; ++i) {
        t[i]->join();
    }
}

int main()
{
    checkSyncQueue();
    return 0;
}


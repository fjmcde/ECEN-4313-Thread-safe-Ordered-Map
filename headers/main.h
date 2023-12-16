#include <iostream>

#include "map.h"
#include <barrier>
#include <mutex>
#include <thread>
#include <vector>


#define MAIN_THREAD     numThreads

int numThreads = 2;
std::vector<std::thread*> workers;
std::barrier<>* bar;
std::mutex lk;

Map map;

namespace myThread
{
    template<typename TF>
    void fork();
    void threadMain(int threadID);
    void join(void);
}


namespace testing
{
    template<typename TF>
    void fork(TF threadFunc);
    void print2DMap(void);
    void threadRange(int threadID);
}


namespace myThread
{
    void fork()
    {
        // resize the worker vector
        workers.resize(numThreads);

        // Create barriers for worker threads and the main thead
        bar = new std::barrier<>(numThreads + 1);

        for(int threadIdx = 0; threadIdx < numThreads; threadIdx++)
        {
            // std::cout << "Created Thread: " << threadIdx << std::endl;
            workers[threadIdx] = new std::thread(threadMain, threadIdx);
        }
    }

    void threadMain(int threadID)
    {
        printf("Thread%d: Called threadMain", threadID);
    }


    void join(void)
    {
        for(int i = 0; i < numThreads; i++)
        {
            // std::cout << "Joined Thread: " << i << std::endl;
            workers[i]->join();
        }
    }


    void cleanup(void)
    {
        delete bar;

        for(int i = 0; i < numThreads; i++)
        {
            // std::cout << "Deleted Thread: " << i << std::endl;
            delete workers[i];
        }

        workers.clear();
    }
}


namespace testing
{
    template<typename TF>
    void fork(TF threadFunc)
    {
        // resize the worker vector
        workers.resize(numThreads);

        // Create barriers for worker threads and the main thead
        bar = new std::barrier<>(numThreads + 1);

        for(int threadIdx = 0; threadIdx < numThreads; threadIdx++)
        {
            // std::cout << "Created Thread: " << threadIdx << std::endl;
            workers[threadIdx] = new std::thread(threadFunc, threadIdx);
        }
    }

    void threadRange(int threadID)
    {
        Range pairs;
        int start;
        int end;

        if(threadID == MAIN_THREAD)
        {
            // Both values exist in map
            start = 2;
            end = 9;
        }
        else if(threadID == 1)
        {
            // End value far exceeds any value in the map
            start = 73;
            end = 512;
        }
        else
        {
            // Neither values exist in the map
            start = -5;
            end = 11;
        }

        pairs = map.getRange(start, end);

        lk.lock();

        printf("Thread%d Value Range: [%d:%d]\n [", threadID, start, end);

        for(size_t i = 0; i < pairs.size(); i++)
        {
            printf(" {%d / %d} ", pairs[i].first, pairs[i].second);
        }

        printf("]\n\n");

        lk.unlock();
    }


    void print2DMap(void)
    {
        std::cout << "\n\nmap.size(): " << map.size() << std::endl;

        std::cout << "Ordered: [";
        for(int i = 0; i < map.size(); i++)
        {
            std::cout << " " << map[i] << " ";
        }
        std::cout << "]\n";
    }
}



// std::cout << "Pairs {k / v}: [";
// for(size_t i = 0; i < pairs.size(); i++)
// {
//     std::cout << " {" << pairs[i].first << " / " << pairs[i].second << "} ";
// }
// std::cout << "]\n";

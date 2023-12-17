#include <iostream>

#include "map.h"
#include <barrier>
#include <cstdio>
#include <mutex>
#include <string>
#include <thread>
#include <typeinfo>
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
    void threadInsert(int threadID);
    void threadRange(int threadID);
    void threadRemove(int threadID);
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
        bar->arrive_and_wait();

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
            end = 2101;
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

        bar->arrive_and_wait();
    }


    void threadInsert(int threadID)
    {
        bar->arrive_and_wait();

        if(threadID == MAIN_THREAD)
        {
            // Start timeing
        }

        bar->arrive_and_wait();


        if(threadID == MAIN_THREAD)
        {
            DEBUG_PRINT("Thread%d: ", threadID);
            map.put(40);
            DEBUG_PRINT("Thread%d: ", threadID);
            map.put(5);
            DEBUG_PRINT("Thread%d: ", threadID);
            map.put(2);
            DEBUG_PRINT("Thread%d: ", threadID);
            map.put(-1);
            DEBUG_PRINT("Thread%d: ", threadID);
            map.put(6);
        }
        else if(threadID == 1)
        {
            DEBUG_PRINT("Thread%d: ", threadID);
            map.put(4);
            DEBUG_PRINT("Thread%d: ", threadID);
            map.put(200);
            DEBUG_PRINT("Thread%d: ", threadID);
            map.put(8);
            DEBUG_PRINT("Thread%d: ", threadID);
            map.put(1);
        }
        else
        {
            DEBUG_PRINT("Thread%d: ", threadID);
            map.put(73);
            DEBUG_PRINT("Thread%d: ", threadID);
            map.put(3);
            DEBUG_PRINT("Thread%d: ", threadID);
            map.put(7);
            DEBUG_PRINT("Thread%d: ", threadID);
            map.put(9);
            DEBUG_PRINT("Thread%d: ", threadID);
            map.put(0);
            DEBUG_PRINT("Thread%d: ", threadID);
            map.put(20);
        }

        bar->arrive_and_wait();

        if(threadID == MAIN_THREAD)
        {
            // Stop timer
        }
    }

    void threadRemove(int threadID)
    {
        bar->arrive_and_wait();

        if(threadID == MAIN_THREAD)
        {
            map.remove(20);
            map.remove(2);
            map.remove(7);
        }
        else if(threadID == 1)
        {
            map.remove(40);
            map.remove(9);
            map.remove(200);
            map.remove(6);
        }
        else
        {
            map.remove(73);
            map.remove(1);
            map.remove(-1);
        }

        bar->arrive_and_wait();
    }

    void print2DMap(void)
    {
        std::cout << "\nmap.size(): " << map.size() << std::endl;

        std::cout << "Ordered: [";
        for(int i = 0; i < map.size(); i++)
        {
            std::cout << " " << map[i] << " ";
        }
        std::cout << "]\n";
    }
}

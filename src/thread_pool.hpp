#ifndef THREAD_POLL_HPP
#define THREAD_POLL_HPP


#include <iostream>
#include <thread>
#include <mutex>
#include <functional>
#include <vector>
#include <queue>
#include <condition_variable>


/*
    SIMPLE THREAD_POOL 
*/


class ThreadPool
{

public:
    ThreadPool(short int pool_count)
    {

        for(short int i = 0; i < pool_count; i++)
        {
            threads.emplace_back([this]
            {
                while(true)
                {
                    std::function<void()> task;
                    
                    {
                        std::unique_lock<std::mutex> lock(mx);
                        condition.wait(lock, [this]{ return stop || !tasks.empty(); });
                        if(stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();

                    }
                    task();

                }
            }
            );
        }
    }

    template <typename F>
    void add_task(F&& task)
    {   
        {
            std::unique_lock<std::mutex> lock(mx);
            tasks.emplace(std::forward<F>(task));
        }
        condition.notify_one();
    }


    ~ThreadPool()
    {
        {
        std::unique_lock<std::mutex> lock(mx);
        stop = true;
        }
        condition.notify_all();

        for(auto& th : threads)
        {
            th.join();
        }
    }


private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex mx;

    std::condition_variable condition;
    bool stop = false;

};


#endif
#pragma once
#include<vector>
#include<thread>
#include<mutex>
#include<queue>
#include<future>
#include<functional>
#include<condition_variable>


class ThreadPool{

    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;
        std::mutex queue_mutex;
        std::condition_variable cv;
        bool stop;

    public:
        ThreadPool(size_t num_threads) : stop(false) {
            for(size_t i = 0; i<num_threads; i++ ){
                    workers.emplace_back([this]{workerLoop();});
            }
        }

        ~ThreadPool(){
            {
            std::unique_lock<std::mutex>lock (queue_mutex);
            stop = true;
            }
           cv.notify_all();     // wake up all threads
           for(auto& t : workers) 
                t.join() ;  // wait for threads to finish

        }

        void enqueue(std::function<void()>task){
            {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.push(std::move(task));
            }

            cv.notify_one();

        }


    private:
        void workerLoop(){
            while(true){
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex>lock(queue_mutex);
                    cv.wait(lock,[this]{return stop || !tasks.empty();
                    });

                    if(stop && tasks.empty()) 
                      {return;}
                    task =std::move(tasks.front()) ;
                    tasks.pop();

                }
                task();

            }
        }

      
};
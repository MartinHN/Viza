/*
 * This file is licensed under the zlib/libpng license, included in this
 * distribution in the file COPYING.
 */


#include <thread>
#include <vector>
#include <utility>
#include <chrono>
#include <functional>
#include <type_traits>

#ifndef WORKQUEUE_threadpool_hpp
#define WORKQUEUE_threadpool_hpp

using namespace std;

template<typename WorkerType,typename workInfo>
class threadpool {
    std::vector<std::thread> pool;
    bool stop;
    
    std::mutex access;
    std::condition_variable cond;
    std::deque<workInfo*> tasks;
    
    
    float * progress;
    int totalTask;
    
    
public:
    explicit threadpool(float * _progress = nullptr) :
    stop(false),
    totalTask(0),
    progress(_progress) {
    }
    ~threadpool() {
        joinAll();

        // cout << "deleting poool" << endl;
    }
    
    void joinAll(){
        // cout << "finnishing work" << endl;
        int left = 1;
        while(left>0){
            
            cond.notify_all();
            std::this_thread::sleep_for(std::chrono::microseconds(10));
            {
                std::unique_lock<std::mutex> lock(access);
                left = tasks.size();
            }
            
            
        }
        stop = true;

        // cout << "joining" << endl;
        for(std::thread &t : pool) {
            cond.notify_all();
            if(t.joinable())t.join();
        }
        pool.clear();
    }
    
    
    void addTask(workInfo *  pt) {
        std::unique_lock<std::mutex> lock(access);
        tasks.push_back(pt);
        totalTask++;
        cond.notify_one();
        
        // cout << "added Task" << endl;
    }
    
    void addWorker(WorkerType * worker) {
        std::thread t([this,worker]() {
            
            while(!stop) {
                workInfo *task = nullptr;
                {
                     // cout << "entering : " << worker->name<< endl;
                    std::unique_lock<std::mutex> lock(access);
                     // cout << "waiting: " << worker->name<<endl;
                    cond.wait(lock);//, std::chrono::duration<int, std::milli>(5));
                     // cout << "unlocked: " << worker->name<< endl; ;
                    
                    if(tasks.empty()) {
                        if(stop){
                            // cout << "stoped: " << worker->name<< endl;
                            continue;
                        }
                        // cout << "empty task: " << worker->name<< endl;
                        
                    }
                    task = std::move(tasks.front());
                    tasks.pop_front();
                    if(progress!=nullptr)*progress = 1.0 - tasks.size()*1.0f/totalTask;
                }
                if(task!=nullptr){
                    // cout << "executing: " << worker->name<< endl;
                    worker->runTask(task);
                }
            }
            // cout << "ended: " << worker->name<< endl;
            delete worker;

        });
        pool.push_back(std::move(t));
        // cout << "added; " << worker->name <<  endl;
    }
};



#endif//WORKQUEUE_threadpool_hpp



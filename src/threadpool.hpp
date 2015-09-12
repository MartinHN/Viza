/*
 * This file is licensed under the zlib/libpng license, included in this
 * distribution in the file COPYING.
 */

#include <future>
#include <thread>
#include <deque>
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
    
public:
    explicit threadpool() : stop(false) {
    }
    ~threadpool() {
        
        

        //        cond.notify_all();
        cout << "finnishing work" << endl;

        while(tasks.size()>0){
            cond.notify_one();
            sleep(1);
            
        }
        stop = true;
        cond.notify_all();
        cout << "joining" << endl;
        for(std::thread &t : pool) {
            t.join();
        }
        pool.clear();
        cout << "deleting poool" << endl;
    }
    
    
    void addTask(workInfo *  pt) {
        std::unique_lock<std::mutex> lock(access);
        tasks.push_back(pt);
        
        cond.notify_one();
        
        cout << "added Task" << endl;
        
        
    }
    
    void addWorker(WorkerType * worker) {
        std::thread t([this,worker]() {
            
            while(!stop) {
                workInfo *task = nullptr;
                {
                     cout << "entering : " << worker->name<< endl;
                    std::unique_lock<std::mutex> lock(access);
                     cout << "waiting: " << worker->name<<endl;
                    cond.wait(lock);//, std::chrono::duration<int, std::milli>(5));
                     cout << "unlocked: " << worker->name<< endl; ;
                    if(tasks.empty()) {
                        if(stop){
                            cout << "stoped: " << worker->name<< endl;
                            continue;
                        }
                        cout << "empty task: " << worker->name<< endl;
                        
                    }
                    task = std::move(tasks.front());
                    tasks.pop_front();
                }
                if(task!=nullptr){
                    cout << "executing: " << worker->name<< endl;
                    worker->runTask(task);
                }
            }
            cout << "ended: " << worker->name<< endl;
            delete worker;

        });
        pool.push_back(std::move(t));
        cout << "added; " << worker->name <<  endl;
    }
};



#endif//WORKQUEUE_threadpool_hpp



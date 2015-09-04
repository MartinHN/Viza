//
//  ExtractorQ.h
//  esstst
//
//  Created by martin hermant on 07/05/15.
//  Copyright (c) 2015 martin hermant. All rights reserved.
//

#ifndef __esstst__ExtractorQ__
#define __esstst__ExtractorQ__

#include "BaseExtractor.h"
#include <map>
#include <unistd.h>


#include "workingQ.h"


//#include <boost/timer/timer.hpp>

using namespace std;

template <typename ExtractorType,typename ExtractorParams,typename Consumable>
class ExtractorQ{
    
public:
    
    ExtractorQ(ExtractorParams * p,int _numThreads):threadPool(_numThreads),numThreads(_numThreads){
        for(int i = 0 ; i < numThreads ; i++){
            ExtractorType * newE = new ExtractorType(*p);
            extractorPool.push_back(newE);
            newE->initFile();
            newE->buildMe();
            
            
            
            //            std::function<void(ExtractorBase*)> cB =std::bind(&ExtractorQ::ended,this, std::placeholders::_1);
            //            extractorPool.back()->endCBFunction = cB;
        }
        if(numThreads>1){

        }
    };
    
    
    vector<BaseExtractor  *> extractorPool;
    
    void pushQueueInfos(const Consumable &  e);
    void ended(BaseExtractor * e){
        if(todoList.size()>0){
            //            float pctDone = todoList.size()*100.0/WIdx;
            if(todoList.size()%(WIdx/20) == 0){
                cout<< '*' ;
            }
            std::unique_lock<std::mutex> lk(todoListMutex);
            Consumable info = todoList.front();
            todoList.pop_front();
            results.emplace_back(
                                 threadPool.enqueue([this,e,info]{
                e->setInput(info);
                e->compute();
                this->ended(e);
            }));
        }
    }
    
    
    void join(){
        while(todoList.size()){};
        for(auto && r:results)
            r.get();
    }
    
    
private:
    
    ThreadPool threadPool;
    int numThreads = 0;
    int WIdx=0;
    std::vector< std::future<void> > results;
    std::deque<Consumable> todoList;
    std::mutex todoListMutex;
    
    
};


template <typename ExtractorType,typename ExtractorParams,typename Consumable>
void ExtractorQ<ExtractorType,ExtractorParams,Consumable>::pushQueueInfos(const Consumable & e){
    
    int numT = WIdx;
    
    
    if(WIdx < numThreads){
        results.emplace_back(
                             threadPool.enqueue([this,e,numT]{
            this->extractorPool[numT]->setInput(e);
            this->extractorPool[numT]->threadedFunction();
            this->ended(this->extractorPool[numT]);
        }));
        
    }
    else{
        
        std::unique_lock<std::mutex> lk(todoListMutex);
        todoList.push_back(e);
    }
    
    
    WIdx++;
    //    WIdx%=numThreads;
}

#endif
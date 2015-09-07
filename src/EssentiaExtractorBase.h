//
//  EssentiaExtractorBase.h
//  esstst
//
//  Created by martin hermant on 06/05/15.
//  Copyright (c) 2015 martin hermant. All rights reserved.
//

#ifndef __esstst__EssentiaExtractorBase__
#define __esstst__EssentiaExtractorBase__

#include <iostream>
#include <map>
#include "essentia.h"
#include "essentia/pool.h"


#include "algorithmfactory.h"
#include "scheduler/network.h"
#include "streaming/streamingalgorithmcomposite.h"
#include "algorithms/io/ringbufferinput.h"


//// utility includes for derived class
//#include "streaming/algorithms/poolstorage.h"
//#include "streaming/algorithms/copy.h"
//#include "streaming/sourceproxy.h"
#include "streaming/algorithms/vectoroutput.h"
#include "streaming/algorithms/vectorinput.h"
//#include "streaming/algorithms/ringbuffervectoroutput.h"
#include "streaming/algorithms/poolstorage.h"

#include "tnt/tnt.h"
#include "tnt/jama_qr.h"


//#include "workingQ.h"

#include <thread>
#include <regex>
//#include "FileUtils.h"


using namespace essentia ;

using namespace streaming ;

using namespace std;


class EssentiaExtractorBase  {
public:
    

    
    typedef std::pair<std::vector<Real>,Pool*> RTInfo;
    typedef std::function<void(EssentiaExtractorBase *)> EndCBFunction;
    
    
    
    ////////////////////////////////
    // constructor
    
    EssentiaExtractorBase();
    // TODO : Destroy everything
    virtual ~EssentiaExtractorBase();
    
    ///////////////////////////////////////////////
    // members
    

    
    Pool outPool,aggregatedPool;
    EndCBFunction endCBFunction;
    
    
    
    
    string outputPath;
    map<string,string> classes;
    void initFile();
    void setInput(RTInfo & v_input);
    
    ///////////////////////////////////////////////
    // intern
    void buildMe(){
        configureIt();
        createNetwork();
    };
    void threadedFunction();
    

    
    void setInput(string audioPath,string _outputPath , map<string,string> _classes);
    void saveIt(string & p);
    
    protected :
    ///////////////////////////////////////////////
    // overridable functions
    //
    // mandatory
    virtual void configureIt() =0;
    virtual void createNetwork() =0 ;
    
    // optional
    virtual void aggregate(){};
    void produceLast();
    
    
    
    
    
    enum AlgoUsage{
        RT=0,
        FILE};
    
    AlgoUsage  algoUsage;
    

    
    essentia::scheduler::Network * network = NULL;
    essentia::streaming::Algorithm * inputAlgo = NULL;
    essentia::streaming::Algorithm * outputAlgo = NULL;
    
    
private:
    essentia::streaming::Algorithm * metaReader = NULL;
    essentia::scheduler::Network * metaNetwork = NULL;
    essentia::streaming::Algorithm * durAlgo = NULL;
    
    public :
    
    
    class TimeCell:public std::map<std::string,double>{
        void add(TimeCell & other){for(std::map<std::string,double>::iterator it = begin(); it != end() ; ++it){it->second +=other[it->first];}}
        void printout(){std::cout << "Times : \n";for(std::map<std::string,double>::iterator it = begin(); it != end() ; ++it){std::cout << it->first << " : " <<it->second << "\n";}}
    };
    
    static int default_sR;
    static int default_frameSize;
    static int default_hopSize ;
    
    
    
    static bool inited ;
    
    
};








#endif /* defined(__esstst__EssentiaExtractorBase__) */

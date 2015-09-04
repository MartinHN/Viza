//
//  EssentiaExtractorBase.cpp
//  esstst
//
//  Created by martin hermant on 06/05/15.
//  Copyright (c) 2015 martin hermant. All rights reserved.
//

#include "EssentiaExtractorBase.h"



 int EssentiaExtractorBase::default_sR = 44100;
 int EssentiaExtractorBase::default_frameSize = 2048;
 int EssentiaExtractorBase::default_hopSize = 2048;
bool EssentiaExtractorBase::inited = false;

EssentiaExtractorBase::EssentiaExtractorBase(){
    if(!inited){
        essentia::init();
        inited = true;
    }
    // essentia has a non thread safe log , so desactivate it
    essentia::warningLevelActive = false;
    


};
EssentiaExtractorBase::~EssentiaExtractorBase(){
    // delete every algos
    delete network;
    delete metaReader;
};


    

    

void EssentiaExtractorBase::initFile(){
    inputAlgo = essentia::streaming::AlgorithmFactory::create("MonoLoader");
    algoUsage = FILE;
    metaReader = essentia::streaming::AlgorithmFactory::create("MetadataReader");
    for (auto cname:metaReader->outputNames()){
       if(cname!="duration") metaReader->output(cname)>> PC(outPool,"metadata."+cname);
       else metaReader->output(cname) >> DEVNULL;
    }
    
    if(durAlgo==NULL)durAlgo = essentia::streaming::AlgorithmFactory::create("Duration");
    inputAlgo->output(0) >> durAlgo->input(0);
    durAlgo->output(0) >> PC(outPool,"metadata.duration");
    
}

void EssentiaExtractorBase::threadedFunction(){

    metaNetwork->run();
    durAlgo->configure("sampleRate",outPool.value<Real>("metadata.sampleRate"));
    network->run();
    
    aggregate();
    produceLast();
    if(endCBFunction)endCBFunction(this);
}

void EssentiaExtractorBase::setInput(string audioPath,string _outputPath , map<string,string> _classes){

    
    classes = _classes;
    outputPath = _outputPath;
    
    inputAlgo->reset();
    inputAlgo->configure("filename",audioPath );
    metaReader->configure("filename",audioPath);
    
    if(network==NULL)network = new essentia::scheduler::Network(inputAlgo);
    else network->reset();
    
    
    if(metaNetwork==NULL)metaNetwork = new essentia::scheduler::Network(metaReader);
    else metaNetwork->reset();

    
    
}







void EssentiaExtractorBase::setInput(RTInfo & RTinput){
    essentia::streaming::RingBufferInput * inVec = dynamic_cast<essentia::streaming::RingBufferInput *>(inputAlgo);
    
    // recreate only if we need
    if(inVec==NULL ){
        // support only one type of input
        if(inputAlgo!=NULL)delete inputAlgo;
        
        inVec = new essentia::streaming::RingBufferInput();
    }
    inVec->add(&RTinput.first[0],RTinput.first.size());
    inputAlgo = inVec;
    
    
    algoUsage = RT;
    if(network==NULL)network = new essentia::scheduler::Network(inputAlgo);
}

void EssentiaExtractorBase::produceLast(){
    if(algoUsage==FILE){
        for(auto m:classes){
            aggregatedPool.set(m.first, m.second);
        }
        
//        essentia::standard::Algorithm * jsonOut = essentia::standard::AlgorithmFactory::create("YamlOutput","filename" , outputPath,"format","json","writeVersion",false);
//        jsonOut->input("pool").set(aggregatedPool);
//        jsonOut->compute();
//        delete jsonOut;
    }
    else if (algoUsage == RT){
        
    }
}




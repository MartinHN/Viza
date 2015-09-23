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
//        essentia::init();
        inited = true;
    }

    
    network = nullptr;
    metaNetwork = nullptr;
    


};
EssentiaExtractorBase::~EssentiaExtractorBase(){
    // delete every algos
    delete network;
    delete metaNetwork;

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
 
}

void EssentiaExtractorBase::setInput(string audioPath,string _outputPath ){

    
//    classes = _classes;
    outputPath = _outputPath;
    
 

    if(network==nullptr) network = new essentia::scheduler::Network(inputAlgo);
     network->reset();
    
    
    if(metaNetwork==nullptr)metaNetwork = new essentia::scheduler::Network(metaReader);
     metaNetwork->reset();
    

    
    
    inputAlgo->reset();
    inputAlgo->configure("filename",audioPath );
    metaReader->configure("filename",audioPath);
    outPool.clear();
    aggregatedPool.clear();
    
    
}






void EssentiaExtractorBase::produceLast(){
    if(algoUsage==FILE){
        for(auto m:classes){
            aggregatedPool.set(m.first, m.second);
        }
//        
        aggregatedPool.set("metadata.audioPath",inputAlgo->parameter("filename").toString());
    }
    else if (algoUsage == RT){
        
    }
}

void EssentiaExtractorBase::saveIt(string & p){
    outputPath = p!=""?p:outputPath;
    
    // Slice onsets
    if(aggregatedPool.contains<vector<Real>>("onsets")){
        vector <Real> ons = aggregatedPool.value<vector<Real>>("onsets");
        aggregatedPool.removeNamespace("onsets");
        aggregatedPool.remove("onsets");
        for(int i = 1 ; i < ons.size() ; i++){
            vector<Real> slice;
            slice.push_back(ons[i-1]);
            slice.push_back(ons[i]);
            aggregatedPool.add("slice.time", slice);
        }
        if(aggregatedPool.contains<Real>("metadata.duration")){
            vector<Real> slice;
            float start = (ons.size()>0)?ons[(int)ons.size()-1]:0;
            slice.push_back(start);
            slice.push_back(aggregatedPool.value<Real>("metadata.duration"));
            aggregatedPool.add("slice.time", slice);
        }
        
        
    }

    
    essentia::standard::Algorithm * jsonOut = essentia::standard::AlgorithmFactory::create("YamlOutput","filename" , outputPath,"format","json","writeVersion",false);
    jsonOut->input("pool").set(aggregatedPool);
    jsonOut->compute();
    delete jsonOut;
}


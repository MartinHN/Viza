//
//  SimpleEssentiaExtractor.cpp
//  esstst
//
//  Created by martin hermant on 06/05/15.
//  Copyright (c) 2015 martin hermant. All rights reserved.
//

#include "SimpleEssentiaExtractor.h"



void SimpleEssentiaExtractor::createNetwork() {
    // link all audio algos acording to audio function infos
    for(int  i = 0 ; i < audioFunctions.size() ; i++){
        if(audioFunctions[i].framecut.first!=0){
            Algorithm *  usedFC = NULL;
            // try to use same frameCutters instances if same window / hop size
            for( auto f:FC){
                if(f->parameter("frameSize")==audioFunctions[i].framecut.first &&
                   f->parameter("hopSize")==audioFunctions[i].framecut.second){
                    usedFC = f;
                    
                }
            }
            
            //create FrameCutter if needed
            if(usedFC == NULL){
                usedFC = essentia::streaming::AlgorithmFactory::create("FrameCutter","frameSize",audioFunctions[i].framecut.first,"hopSize",audioFunctions[i].framecut.second);
                FC.push_back(usedFC);
            }
            inputAlgo->output(0) >> usedFC->input("signal");
            
            usedFC->output("frame") >> audioAlgos[i]->input(audioFunctions[i].inputName);
        }
        // dont use Frame Cutter ...
        else{
            inputAlgo->output(0) >> audioAlgos[i]->input(audioFunctions[i].inputName);
        }
        essentia::streaming::Algorithm::OutputMap outputs = audioAlgos[i]->outputs();
        for(int j = 0 ; j < outputs.size() ; j++){
            if(std::find(audioFunctions[i].outputs.begin(), audioFunctions[i].outputs.end(), outputs[j].first)!= audioFunctions[i].outputs.end()){
                audioAlgos[i]->output(j) >> PC(outPool,"values." +outputs[j].first);
            }
            else{
                audioAlgos[i]->output(j)>>DEVNULL;
            }
        }
        
    }
    
    
    
    
};


void SimpleEssentiaExtractor::configureIt(){
    for(int  i = 0 ; i < audioFunctions.size() ; i++){
        essentia::streaming::Algorithm * a =essentia::streaming::AlgorithmFactory::create(audioFunctions[i].name);
        audioAlgos.push_back(a);
        a->configure(audioFunctions[i].parameters);
        
    }
    //        for(int  i = 0 ; i < spectralFunctions.size() ; i++){
    //            spectralAlgos.push_back(essentia::streaming::AlgorithmFactory::create(audioFunctions[i].name));
    //            audioAlgos.back()->configure(audioFunctions[i].parameters);
    //        }
    
};




void SimpleEssentiaExtractor::aggregate(){
    essentia::standard::Algorithm * myaggregator = essentia::standard::AlgorithmFactory::create("PoolAggregator");
    const char* statsToCompute[] = {"mean"};//,"median","var","skew","kurt","dmean","dvar"};
    myaggregator->configure("defaultStats", arrayToVector<string>(statsToCompute));
    myaggregator->input("input").set(outPool);
    myaggregator->output("output").set(aggregatedPool);
    myaggregator->compute();
    delete myaggregator;
    
}





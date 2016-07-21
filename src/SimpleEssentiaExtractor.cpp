//
//  SimpleEssentiaExtractor.cpp
//  esstst
//
//  Created by martin hermant on 06/05/15.
//  Copyright (c) 2015 martin hermant. All rights reserved.
//

#include "SimpleEssentiaExtractor.h"

bool SimpleEssentiaExtractor::spliceIt = false;
float SimpleEssentiaExtractor::onsetThresh = 6.;
void SimpleEssentiaExtractor::createNetwork() {
    outPool->clear();
    FC.clear();


    // link all audio algos acording to audio function infos
    for(int  i = 0 ; i < audioFunctions.size() ; i++){
        if(audioFunctions[i].framecut.first!=0
           && audioFunctions[i].inputName.substr(0,5)!="Algo."
           ){
            Algorithm *  usedFC = NULL;
            Algorithm * usedSpec = NULL;
            // try to use same frameCutters instances if same window / hop size
            for( auto & f:FC){
                if(f->parameter("frameSize")==audioFunctions[i].framecut.first &&
                   f->parameter("hopSize")==audioFunctions[i].framecut.second){
                    usedFC = f;
                    
                }
            }
            
            //create FrameCutter if needed and connect it to spectrum if needed
            if(usedFC == NULL){
                usedFC = essentia::streaming::AlgorithmFactory::create("FrameCutter","frameSize",audioFunctions[i].framecut.first,"hopSize",audioFunctions[i].framecut.second);
                inputAlgo->output(0) >> usedFC->input("signal");
                FC.push_back(usedFC);
                
            }
            
            
            // do we need to insert spectrum
            if(audioFunctions[i].inputName=="spectrum"){
                usedSpec =  essentia::streaming::AlgorithmFactory::create("Spectrum");
                usedFC->output("frame") >> usedSpec->input(0);
                usedSpec->output("spectrum") >> audioAlgos[i]->input("spectrum");
                
                spectrumAlgo.push_back(usedSpec);
            }
            
            else {
                usedFC->output("frame") >> audioAlgos[i]->input(audioFunctions[i].inputName);
            }

        }
        // dont use Frame Cutter but from audio directly
        else if(audioFunctions[i].inputName.substr(0,5) != "Algo."){
            if(audioFunctions[i].inputName=="spectrum"){
                cout << "cant connect non cutted to spectrum" << endl;
            }
            else{
                inputAlgo->output(0) >> audioAlgos[i]->input(audioFunctions[i].inputName);
            }
        }
        
        
        // conect outputs
        
        essentia::streaming::Algorithm::OutputMap outputs = audioAlgos[i]->outputs();
        for(int j = 0 ; j < outputs.size() ; j++){
            
            // interconnect
            if(audioFunctions[i].outputs[0].substr(0,5) == "Algo."){
                string algoName = audioFunctions[i].outputs[0].substr(5);
                int afIdx = 0;
                for(auto k:audioFunctions){
                    if(k.name == algoName){
                        for( string outputA:audioAlgos[i]->outputNames()){
                            cout << outputA << endl;
                            if(std::find(audioAlgos[afIdx]->inputNames().begin(),audioAlgos[afIdx]->inputNames().end(),outputA)!= audioAlgos[afIdx]->inputNames().end()){
//                                cout << "interConnecting : " << audioAlgos[afIdx]->name() << " and " << audioAlgos[i]->name() << "by : " << outputA << endl;
                                audioAlgos[i]->output(outputA) >> audioAlgos[afIdx]->input(outputA);
                            }
                            else{
                                audioAlgos[afIdx]->output(outputA) >> DEVNULL;
                            }
                        }
                        break;
                    }
                    afIdx++;
                }
                break;
            }
            
            // connect To Pool
            else if(std::find(audioFunctions[i].outputs.begin(), audioFunctions[i].outputs.end(), outputs[j].first)!= audioFunctions[i].outputs.end()){
//                cout << " connecting : "+ audioAlgos[i]->name()+" to Pool : "+outputs[j].first << endl;
                audioAlgos[i]->output(j) >> PC(*outPool,"values." +outputs[j].first);
            }
            else{
                audioAlgos[i]->output(j)>>DEVNULL;
            }
        }
        
    }
    
    
    aggregatedPool.clear();
    onsetAlgo = nullptr;
    if(spliceIt){
        
        onsetAlgo = essentia::streaming::AlgorithmFactory::create("SuperFluxExtractor","ratioThreshold",onsetThresh);
        highPassAlgo = essentia::streaming::AlgorithmFactory::create("HighPass","cutoffFrequency",400);
        if(onsetAlgo!=nullptr){
            inputAlgo->output(0) >>
          highPassAlgo->input(0);highPassAlgo->output(0) >>
          onsetAlgo->input(0);
            onsetAlgo->output(0) >> PC(aggregatedPool,"onsets");
            
        }
    }
    
    
};


void SimpleEssentiaExtractor::configureIt(){
  audioAlgos.clear();
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
    
    //    if(onsetAlgo!=nullptr){

    map<string,vector<Real> > res = outPool->getRealPool();

    for( auto & kv: outPool->getVectorRealPool()){
        int vIdx =0;
        int vsize = kv.second[0].size();
        int numV =  kv.second.size();
        
        for(int i=0;i< vsize ; i++){
            string vName = kv.first+"_"+std::to_string(i);
            res[vName] = vector<Real>(numV);
            for(int j=0; j < numV ; j++){
                res[vName][j] =kv.second[j][i] ;
            }
        }
        
    }
 cout  << res.size() << endl;    
    vector<Real> onsets;
    if(aggregatedPool.contains<vector<Real> >("onsets")){
    onsets = aggregatedPool.value<vector<Real> >("onsets");
    }
    if(!onsets.size()){
        onsets.push_back(0);
        aggregatedPool.set("onsets", vector<Real>(1,0));
    }
    for(map<string,vector<Real> >::iterator it = res.begin(); it!=res.end() ; ++it){
        int firstIdx = 0;
        float frameRate = 44100/1024.0;
        for(auto aF:audioFunctions){
            if(std::find(aF.outputs.begin(),aF.outputs.end(),it->first)!=aF.outputs.end()){
                if(aF.framecut.first!=0){
                    frameRate = 44100.0/aF.framecut.second;
                }
            }
        }
        
        
        for(int i = 0 ; i < onsets.size() ; i++){
            float myVal = 0;
            int begin = onsets[i]*frameRate;
            float end = 0;
            if(i>=((int)onsets.size()-2)){
                end = it->second.size();
            }
            else{
                end = onsets[i+1]*frameRate;
            }
            
            for(int j = begin ; j < end ; j++){
                myVal+=it->second[j];

            }
            if(end==begin){
                cout <<"fuck : " << i << " : " << it->first << " / " <<  it->second.size() << endl;;
                end = begin+1;
              myVal = 0;
            }
          if(!std::isfinite(myVal)){
            cout << "Nan or inf: " << it->first << " for : " << it->second[begin]<< endl;
            myVal = 0;
          }
          else{
            int a = 0;
            a++;
          }
            myVal/=(end-begin);

            //                cout << myVal << "," << it->first << endl;
            aggregatedPool.add(it->first+".mean",myVal);
        }
        
    }
    
    

    
    map<string , Real >  unique = outPool->getSingleRealPool();
    for(map<string , Real >::iterator it = unique.begin() ; it !=unique.end() ; ++it){
        aggregatedPool.set(it->first, it->second);
    }

    
}





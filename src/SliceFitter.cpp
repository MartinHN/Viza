//
//  SliceFitter.cpp
//  ViZa
//
//  Created by martin hermant on 16/12/14.
//
//

#include "SliceFitter.h"


#include "Physics.h"
#include "Container.h"

SliceFitter::SliceFitter(){
    
    ofAddListener(ofEvents().update , this, &SliceFitter::update);
    //get a list of all algorithms we could use for the test

    
    
}
SliceFitter::~SliceFitter(){
    
}

void SliceFitter::fitFor(float s){
    maxFitTime = s;
    startTime =  ofGetElapsedTimef();
    
    
    
    fitThread.dataset.resize(Physics::vs.size());
    for(int i = 0 ; i < fitThread.dataset.size() ; i ++){
        fitThread.dataset[i].xdash = Physics::vs[i];
        fitThread.dataset[i].descriptors = Container::containers[i].attributes;
//        cout << fitThread.dataset[i].descriptors.size()<< endl;
    }
    fitThread.model.size = fitThread.dataset[0].descriptors.size();
    vector<double> randParam(fitThread.model.size);
    for(int i = 0 ; i < randParam.size() ; i ++){
        randParam[i] = ofRandom(1);
    
    }
    fitThread.model.setParameters(&randParam[0]);
    if(fitThread.model.size>0){
        startThread = true;
        fitThread.startThread();
    }
    else{
        cout << "no parameters to fit" << endl;
    }
    
}


void SliceFitter::update(ofEventArgs &a){
    
    if(fitThread.isThreadRunning()){
        
    if(ofGetElapsedTimef()-startTime>maxFitTime){
        
        fitThread.fitter->forceStop();
        fitThread.waitForThread(true);
        
        cout << "stopped Fit" << endl;
    }
    
    
    }
    else if(startThread == true)
    {
        for(int i = 0 ; i < fitThread.model.size ; i++){
            if(fitThread.model.getParameters()[i]>0){
            cout << "param " << Container::attributeNames[i] <<": "<< fitThread.model.getParameters()[i] << endl;
            }
        }
        
        startThread = false;
    }
    
    
}
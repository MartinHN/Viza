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

#include <Accelerate/Accelerate.h>

SliceFitter::SliceFitter(){
    
    ofAddListener(ofEvents().update , this, &SliceFitter::update);
    //get a list of all algorithms we could use for the test

    
    
}
SliceFitter::~SliceFitter(){
    if(fitThread.fitter)fitThread.fitter->forceStop();
    fitThread.waitForThread(true);
}

void SliceFitter::fitFor(float s){
//    maxFitTime = s;
    
    
    
    
    int elemSize =Physics::vs.size();
    fitThread.dataset.resize(elemSize);
    int dimSize = Container::attributeNames.size();
    for(int i = 0 ; i < elemSize ; i ++){
        int i2 = (int)ofRandom(elemSize-1);
        ofVec3f d = Physics::vs[i]- Physics::vs[i2];
        fitThread.dataset[i].angle =fitThread.model->getAngle(d);
//        cout << fitThread.dataset[i].angle<< endl;
        fitThread.dataset[i].descriptorsDiff.resize(dimSize);
        
        
         vDSP_vsub(&Container::normalizedAttributes[i2][0],1,&Container::normalizedAttributes[i][0],1,&fitThread.dataset[i].descriptorsDiff[0],1,dimSize);
        
    }
    fitThread.model = new FitThread::Model();
    fitThread.model->size = dimSize;
    int paramSize = fitThread.model->getParameterCount();
    vector<double> randParam(paramSize);
    for(int i = 0 ; i <paramSize ; i ++){
        randParam[i] = 0;//(double)(ofRandom(1));
    
    }
    fitThread.model->setParameters(&randParam[0]);
    cout << elemSize << " : " << paramSize <<" : "<< dimSize << endl;
//    for (int i = 0 ; i < dimSize*3 ; i++){
//        if(fitThread.model.getParameters()[i]>0){
//            cout <<fitThread.model.getParameters()[i] << endl;
//        }
//    }
    fitThread.init();
    fitThread.fitter->upperBound = 1;
    fitThread.fitter->lowerBound = 0;
    fitThread.fitter->maxTime = s;

    if(fitThread.model->size>0){
//        startThread = true;
        fitThread.startThread();
    }
    else{
        cout << "no parameters to fit" << endl;
    }
    
}


void SliceFitter::update(ofEventArgs &a){
    
    if(!fitThread.isThreadRunning() && fitThread.ended == true){
        
        for(int i = 0 ; i < fitThread.model->size ; i++){
            if(fitThread.model->getParameters()[i*3]>0.1   ||
               fitThread.model->getParameters()[i*3+1]>0.1 ||
               fitThread.model->getParameters()[i*3+2]>0.1
               ){
                
            
            cout
                <<": "<< fitThread.model->getParameters()[i*3]
                <<": "<< fitThread.model->getParameters()[i*3+1]
               << ": "<< fitThread.model->getParameters()[i*3+2]
                << " " << Container::attributeNames[i]
                << endl;
            }
        }
        
        fitThread.clear();
    }
    else if(fitThread.model!=NULL && fitThread.model->getParameters()!=NULL ){
        int totalNum = Physics::vs.size();
        int attrNum = Container::normalizedAttributes[0].size();
        if(totalNum!= outPoints.size()){
            outPoints.resize(totalNum);
            cout << "resize to" << totalNum << endl;
        }
        curParams.resize(fitThread.model->getParameterCount());
        for(int  i = 0 ; i < fitThread.model->getParameterCount();i++){
            curParams[i] = fitThread.model->getParameters()[i];
        }
        vDSP_mmul(&Container::normalizedAttributes[0][0],1,&curParams[0],1,&outPoints[0].x,1,totalNum,3,attrNum);
        Physics::setFits(outPoints);
    }
    
    
}
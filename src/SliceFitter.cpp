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
//   fitting function
    
    int elemSize =Physics::vs.size();
    fitThread.dataset.resize(elemSize);
    int dimSize = Container::attributeNames.size();
    
    // choose random pairs of point to build dataset
    // the data is the vector of differences of each pair
    for(int i = 0 ; i < elemSize ; i ++){
        int i2 = (int)ofRandom(elemSize-1);
        ofVec3f d = Physics::vs[i]- Physics::vs[i2];
        fitThread.dataset[i].angle =   fitThread.model->getAngle(d);
        fitThread.dataset[i].descriptorsDiff.resize(dimSize);
        vDSP_vsub(&Container::normalizedAttributes[i2*Container::attrSize],1,&Container::normalizedAttributes[i*Container::attrSize],1,&fitThread.dataset[i].descriptorsDiff[0],1,dimSize);
        
    }
    
    
    fitThread.model = new FitThread::Model();
    fitThread.model->size = dimSize;
    int paramSize = fitThread.model->getParameterCount();
    
    // init search space

    vector<double> randParam(paramSize);
//    if(curParams.size()==paramSize){
//        for(int i = 0 ; i <paramSize ; i ++){
//            randParam[i] = curParams[i];
//            
//        }
//    }
//    else{
    for(int i = 0 ; i <paramSize ; i ++){
        randParam[i] = 0;//(ofRandom(.01));
    
//    }
    }
    
    
    fitThread.model->setParameters(&randParam[0]);
    fitThread.init();
    fitThread.fitter->upperBound = 1;
    fitThread.fitter->lowerBound = 0;
    fitThread.fitter->maxTime = s;

    if(fitThread.model->size>0){
        fitThread.startThread();
    }
    else{
        cout << "no parameters to fit" << endl;
    }
    
}


void SliceFitter::update(ofEventArgs &a){
    
    
    if(fitThread.model!=NULL && fitThread.model->getParameters()!=NULL ){
        int totalNum = Physics::vs.size();
        if(totalNum!= outPoints.size()){
            outPoints.resize(totalNum);
            
            cout << "resize to" << totalNum << endl;
        }
        curParams.resize(fitThread.model->getParameterCount());
        for(int  i = 0 ; i < fitThread.model->getParameterCount();i++){
            curParams[i] = fitThread.model->getParameters()[i];
        }
        vDSP_mmul(&Container::normalizedAttributes[0],1,&curParams[0],1,&outPoints[0].x,1,totalNum,3,Container::attrSize);

        outPointsReshape();
        
        Physics::setFits(outPoints);
    }
    
    
    
    //print out end report
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

    
    
}


void SliceFitter::outPointsReshape(){
    
    

    ofVec3f scale(0,0,0);
    int numElements = 40;
    for(int i = 0 ; i < numElements ;i++){
        int curIdx = ofRandom(Physics::vs.size()-1);
        int curIdx2 = (int)(curIdx + ofRandom(Physics::vs.size()-1))%Physics::vs.size();
        
        
        ofVec3f scaleNum = (Physics::vs[curIdx] - Physics::vs[curIdx2]);
        ofVec3f scaleDen = (outPoints[curIdx]-outPoints[curIdx2]);
        
        scale.x+=(scaleNum.x*scaleDen.x)!=0?scaleNum.x/scaleDen.x:1;
        scale.y+=(scaleNum.y*scaleDen.y)!=0?scaleNum.y/scaleDen.y:1;
        scale.z+=(scaleNum.z*scaleDen.z)!=0?scaleNum.z/scaleDen.z:1;
        
    }
    
    scale/= numElements;
    //scale.set(1,1,1);
    
    
    ofVec3f translation(0,0,0);
    for(int i = 0 ; i < numElements ;i++){
        int curIdx = ofRandom(Physics::vs.size()-1);
        translation+=Physics::vs[curIdx]/scale - outPoints[curIdx];
    }

    translation/= numElements;
    
    //cout << "scale" <<  scale << endl;
    vDSP_vsmsa(&outPoints[0].x, 3, &scale.x, &translation.x, &outPoints[0].x, 3, outPoints.size());
    vDSP_vsmsa(&outPoints[0].y, 3, &scale.y, &translation.y, &outPoints[0].y, 3, outPoints.size());
    vDSP_vsmsa(&outPoints[0].z, 3, &scale.z, &translation.z, &outPoints[0].z, 3, outPoints.size());
    
    
    
    
}






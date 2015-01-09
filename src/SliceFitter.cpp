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
        for(int k = 0 ; k< Container::fixAttributes.size() ; k++){
//            cout <<
            fitThread.dataset[i].descriptorsDiff[Container::fixAttributes[k]]= 10;
        }
    }
    
    
    fitThread.model = new FitThread::Model();
    fitThread.model->size = dimSize;
    int paramSize = fitThread.model->getParameterCount();
    
    // init search space

    vector<double> randParam(paramSize);

    for(int i = 0 ; i <paramSize ; i ++){
        randParam[i] = 0;//ofRandom(.01);

    }
    
    
    fitThread.model->setParameters(&randParam[0]);
    fitThread.init();
    fitThread.fitter->upperBound = 1;
    fitThread.fitter->lowerBound = -1;

    fitThread.fitter->maxTime = s;
    fitThread.fitter->stopval = pow(.1 ,2);
    cout << "stopval : " << fitThread.fitter->stopval << endl;
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
        
        
//    }
    
    

    
    
    //print out end report

        
        fitEquation.clear();
        fitEquation.paramNames = Container::attributeNames;
        
        // iterate over parameters :  saving equation
        for(int i = 0 ; i < fitThread.model->size ; i++){
            for(int j = 0 ; j< 3 ; j++){
                float curParam = fitThread.model->getParameters()[i*3+j];
                if(abs(curParam)>0){
                    fitEquation.equation[j][curParam] = i;
                }
            }
            
            
            
            }
        GUI::instance()->LogIt(ofToString(fitThread.fitter->lowerResidual/Container::attrSize)+"\n"+fitEquation.toString(3));
        if(!fitThread.isThreadRunning() && fitThread.ended == true){       fitThread.clear();}
    }
    }

    
    




void SliceFitter::outPointsReshape(){
    
    

    ofVec3f scale(0,0,0);
    int numElements = 400;
#ifdef ANGLE_DIST
    int validEl=0;
    int maxWatch = 0;
    while(validEl < numElements && maxWatch < Physics::vs.size()){
        int curIdx = ofRandom(Physics::vs.size()-1);
        int curIdx2 = (int)(curIdx + ofRandom(Physics::vs.size()-1))%(Physics::vs.size()-1);
        
        
        ofVec3f scaleNum = (Physics::vs[curIdx] - Physics::vs[curIdx2]);
        ofVec3f scaleDen = (outPoints[curIdx]-outPoints[curIdx2]);
        if(
           (scaleNum.x*scaleDen.x)!=0 &&
           (scaleNum.y*scaleDen.y)!=0 &&
           (scaleNum.z*scaleDen.z)!=0
            
            
            ){
        scale.x+=abs(scaleNum.x/scaleDen.x);
        scale.y+=abs(scaleNum.y/scaleDen.y);
        scale.z+=abs(scaleNum.z/scaleDen.z);
            validEl++;
        }
        maxWatch++;
    }
    
    scale/= validEl;
#else
    
    scale.set(1,1,1);
#endif
    
    ofVec3f translation(0,0,0);
    for(int i = 0 ; i < numElements ;i++){
        int curIdx = ofRandom(Physics::vs.size()-1);
        translation+=Physics::vs[curIdx] - outPoints[curIdx]*scale;
    }

    translation/= numElements;
    
    //cout << "scale" <<  scale << endl;
    vDSP_vsmsa(&outPoints[0].x, 3, &scale.x, &translation.x, &outPoints[0].x, 3, outPoints.size());
    vDSP_vsmsa(&outPoints[0].y, 3, &scale.y, &translation.y, &outPoints[0].y, 3, outPoints.size());
    vDSP_vsmsa(&outPoints[0].z, 3, &scale.z, &translation.z, &outPoints[0].z, 3, outPoints.size());
    
    
    
    
}





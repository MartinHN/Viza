//
//  GUIPhysics.cpp
//  ViZa
//
//  Created by martin hermant on 24/04/15.
//
//

#include "GUIPhysics.h"


GUIPhysics::GUIPhysics(string name): GUICanvasBase(name){
    
    
    ////////////////CLUSTER </////////////////////
    
    setName("Physics");
    runPhysics = new ofxUIToggle("run",false,10,10);
    minDistance = new ofxUISlider("distance",0.0f,100.0f,&PhysicalDistribution::i()->minDist,100,10);
    kSpring = new ofxUISlider("k",0.0f,.10f,&PhysicalDistribution::i()->kSpring,100,10);
    
    addWidgetDown(minDistance);
    addWidgetDown(kSpring);
    addWidgetDown(runPhysics);
    
    setDimensions(getRect()->width, getRect()->height*1.3);
    
    
    
}


void GUIPhysics::init(){
    
}



void GUIPhysics::guiEvent(ofxUIEventArgs & e){
    if(e.widget == runPhysics ){
        if(runPhysics->getValue()){
            if(!PhysicalDistribution::i()->isThreadRunning()){
                PhysicalDistribution::i()->init();
                PhysicalDistribution::i()->startThread();
            }
        }
        else{
            PhysicalDistribution::i()->stopThread();
        }
        
        
    }
    
//    if(e.widget == minDistance){
//        PhysicalDistribution::i()->minDist = minDistance->getValue();
//    }
//    
//    else if (e.widget == kSpring){
//        PhysicalDistribution::i()->kSpring = kSpring->getValue();
//    }
}



//
//  GUIFitter.cpp
//  ViZa
//
//  Created by martin hermant on 24/04/15.
//
//

#include "GUIFitter.h"


GUIFitter::GUIFitter(string name): GUICanvasBase(name) {
  
    //// FITTER ///////////////
    setName("Fitter");
    samplingPct = new ofxUISlider("sampling%",.05,2,1,100,10);
    typeOfFit = new ofxUIDropDownList("typeOfFit",SliceFitter::DistanceType::types,150,0,0,OFX_UI_FONT_SMALL);
    keepResults = new ofxUIToggle("keep Results",false,10,10);
    fitIt = new ofxUIToggle("FitIt",false,10,10);
    applyIt = new ofxUIButton("apply",false,10,10);
    
    addWidgetDown(typeOfFit);
    addWidgetDown(samplingPct);
    addWidgetDown(keepResults);
    addWidgetDown(fitIt);
    addWidgetDown(applyIt);
    
    
    
    vector<ofxUIWidget*> ddls;
    ddls.push_back(typeOfFit);
    for(int i = 0 ; i < ddls.size(); i++){
        ((ofxUIDropDownList*) ddls[i])->setAutoClose(true);
        ((ofxUIDropDownList*) ddls[i])->setShowCurrentSelected(true);
    }
    
    ofAddListener(this->newGUIEvent, this, &GUIFitter::guiEvent);
    
}



void GUIFitter::guiEvent(ofxUIEventArgs &e){
    if(e.widget == keepResults){
        SliceFitter::i()->keepResult = keepResults->getValue();
    }
    else if (e.widget == typeOfFit){
        if(typeOfFit->getSelectedIndeces().size()>0){
            SliceFitter::i()->type.idx = typeOfFit->getSelectedIndeces()[0];
        }
    }
    else if(e.widget == samplingPct){
        SliceFitter::i()->samplePct = samplingPct->getValue();
    }
    else if(e.widget == fitIt ){
        if(SliceFitter::i()->fitThread.isThreadRunning()){
            SliceFitter::i()->fitThread.fitter->forceStop();
        }
        else if (fitIt->getValue()){
            SliceFitter::i()->fitFor();
        }
    }
    else if (e.widget == applyIt && !applyIt->getValue()){
        Physics::applyEquation(SliceFitter::i()->fitEquation);
        for(int i = 0 ; i < 3 ; i++){
            ofMessage msg("Axes:setLabels,"+ofToString(i)+",fitterEq");
            ofNotifyEvent(sharedMessage, msg,this);
        }
    }
}

void GUIFitter::fitProgress(ofMessage & a){
    
    if(getGUIMsgDest(a)=="Fitter"){
        vector<string> r = getGUIMsgArgs(a);
        if(r[0]=="progress")progress = ofToFloat(r[1]);
        if(progress >= 100 && fitIt->getValue()){
            fitIt->setValue(false);
        }
    }
    
    
}


void GUIFitter::init(){

}


void GUIFitter::setup(){

    
}
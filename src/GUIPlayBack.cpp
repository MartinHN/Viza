//
//  GUIPlayBack.cpp
//  ViZa
//
//  Created by martin hermant on 19/02/16.
//
//

#include "GUIPlayBack.h"


GUIPlayBack::GUIPlayBack(string name):GUICanvasBase(name){
    
    setName("PlayBack");
    playBack =new ofxUISuperCanvas("playBack");
    playBack->setName("playBack");
    
    continuousPB = new ofxUIToggle("continuousPlayBack",true,10,10);
    holdPB = new ofxUIToggle("hold",false,10,10);
    stopAll = new ofxUIButton("stopAll",false,10,10);
    maxPBTime = new ofxUINumberDialer(0,10,0.f,1,"max time" , OFX_UI_FONT_SMALL);
    
    
    addWidgetDown(continuousPB);
    addWidgetDown(holdPB);
    addWidgetDown(stopAll);
    addWidgetDown(maxPBTime);
    
    
    setDimensions(getRect()->width, getRect()->height*1.3);
    
}

void GUIPlayBack::init(){
    
}

void GUIPlayBack::guiEvent(ofxUIEventArgs & e){
    
        if(e.widget == stopAll){
            AudioPlayer::stopAll();
        }
        else if (e.widget == maxPBTime){
            AudioPlayer::maxTime = maxPBTime->getValue();
        }

        
    
}
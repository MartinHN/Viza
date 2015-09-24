//
//  GUI.cpp
//  ViZa
//
//  Created by martin hermant on 14/10/14.
//
//

#include "GUI.h"


GUI * GUI::inst;
ofEvent<ofMessage> GUI::GUISharedEvent;


GUI::GUI():
guiLoad("Load"),
guiStatistics("Statistics"),
guiAxe("Axes"),
guiClass("Class"),
//guiFitter("Fitter"),
guiCluster("Cluster"),
guiView("View"),
guiPhysics("Physics"),
guiMidi("Midi")


{
    


    ofAddListener(ofEvents().update, this, &GUI::update);
    ///LOGGER///////////
    logCanvas = new ofxUISuperCanvas("Log",0,700,900,100,OFX_UI_FONT_SMALL);
    logCanvas->setName("Log");
    logCanvas->setTheme(OFX_UI_THEME_MACOSX);
    Logger = new ofxUITextArea("Logger","Log",900,0,0,0,OFX_UI_FONT_SMALL);
    Logger->setVisible(true);
    



    

    //// PLAYBACK /////////////
    playBack =new ofxUISuperCanvas("playBack");
    playBack->setName("playBack");
    
    continuousPB = new ofxUIToggle("continuousPlayBack",true,10,10);
    holdPB = new ofxUIToggle("hold",false,10,10);
    stopAll = new ofxUIButton("stopAll",false,10,10);
    maxPBTime = new ofxUINumberDialer(0,10,0.f,1,"max time" , OFX_UI_FONT_SMALL);
    
    
    ///PLACING//////////////


    logCanvas->addWidgetDown(Logger);
    
    

    
    playBack->addWidgetDown(continuousPB);
    playBack->addWidgetDown(holdPB);
    playBack->addWidgetDown(stopAll);
    playBack->addWidgetDown(maxPBTime);
    
    
    //GLOBAL TAB
    global = new ofxUITabBar();
    
    global->setName("Global");
    global->setTheme(OFX_UI_THEME_MACOSX);
    
    global->addCanvas(&guiLoad);
    global->addCanvas(&guiStatistics);
    global->addCanvas(&guiAxe);
    global->addCanvas(&guiClass);
    global->addCanvas(&guiView);
//    global->addCanvas(&guiFitter);
    global->addCanvas(&guiCluster);
    global->addCanvas(&guiPhysics);
    global->addCanvas(&guiMidi);
    global->addCanvas(playBack);
    
    
    
    
    
    init();
}


GUI::~GUI(){
    //    delete global;
}

void GUI::init(){
    ofAddListener(global->newGUIEvent, this, &GUI::guiEvent);
    guiLoad.init();
    guiStatistics.init();
    guiView.init();
//    guiFitter.init();
    guiCluster.init();
    guiAxe.init();
    guiClass.init();
    guiPhysics.init();
    guiMidi.init();
    
    ofAddListener(ofEvents().draw,this,&GUI::draw);

    
    ofAddListener(playBack->newGUIEvent,this , &GUI::guiEvent);
}

void GUI::setup(){
    guiLoad.setup();
    guiStatistics.setup();
    guiAxe.setup();
    guiClass.setup();
    guiMidi.setup();
    
    logCanvas->autoSizeToFitWidgets();
    
    
}

void GUI::draw(ofEventArgs & a){

}



void GUI::guiEvent(ofxUIEventArgs &e){



    if (e.getParent()==playBack){
        if(e.widget == stopAll){
            AudioPlayer::stopAll();
        }
        else if (e.widget == maxPBTime){
            AudioPlayer::maxTime = maxPBTime->getValue();
        }

    }

    lastFramenum = ofGetFrameNum();
    
}



void GUI::LogIt(string s){
    i()->Logger->setTextString(s);
    
}

bool GUI::isOver(int x,int y){
    bool res = global->isHit(x,y);
//    
//    ofxUICanvas * c = global->getActiveCanvas();
//    if(c)res |= c->isHit(x, y);
    res |= logCanvas->isHit(x,y);
    return res;
    //    global->getActiveCanvas();
}





void GUI::update(ofEventArgs &a){
    guiClass.async(a);
//    guiAxe.async(a);
}
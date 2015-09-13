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
guiPhysics("Physics")


{
    


    ofAddListener(ofEvents().update, this, &GUI::update);
    ///LOGGER///////////
    logCanvas = new ofxUISuperCanvas("Log",0,700,900,100,OFX_UI_FONT_SMALL);
    logCanvas->setName("Log");
    logCanvas->setTheme(OFX_UI_THEME_MACOSX);
    Logger = new ofxUITextArea("Logger","Log",900,0,0,0,OFX_UI_FONT_SMALL);
    Logger->setVisible(true);
    

    //MIDI//////////
    midiCanvas = new ofxUISuperCanvas("Midi");
    midiCanvas->setName("Midi");
    
    
    midiPorts = new ofxUIDropDownList("MidiPorts", Midi::instance()->getPorts(),150,0,0,OFX_UI_FONT_SMALL);
    midiVel = new ofxUIRangeSlider("VelocityRange",0,1,0,1,100,10);
    
    midiRadius = new ofxUISlider("Radius",0,.5,0.05,100,10);
    midiHold = new ofxUIToggle("Hold",false,10,10);
    
    midiLink2Cam = new ofxUIToggle("link2Cam",true,10,10);
    link2x = new ofxUIToggle("link2X",false,10,10);
    midiSpots = new ofxUIToggle("midiSpots",false,10,10);
    randomMidi = new ofxUISlider("randomMidi",0.0f,1.f,0.0f,150,10);
    

    //// PLAYBACK /////////////
    playBack =new ofxUISuperCanvas("playBack");
    playBack->setName("playBack");
    
    continuousPB = new ofxUIToggle("continuousPlayBack",true,10,10);
    holdPB = new ofxUIToggle("hold",false,10,10);
    stopAll = new ofxUIButton("stopAll",false,10,10);
    
    
    ///PLACING//////////////

    midiCanvas->addWidgetDown(midiPorts);
    midiCanvas->addWidgetDown(midiVel);
    midiCanvas->addWidgetDown(midiRadius);
    midiCanvas->addWidgetDown(midiHold);
    midiCanvas->addWidgetDown(midiLink2Cam);
    midiCanvas->addWidgetDown(link2x);
    midiCanvas->addWidgetDown(midiSpots);
    midiCanvas->addWidgetDown(randomMidi);

    logCanvas->addWidgetDown(Logger);
    
    

    
    playBack->addWidgetDown(continuousPB);
    playBack->addWidgetDown(holdPB);
    playBack->addWidgetDown(stopAll);
    
    
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
    global->addCanvas(midiCanvas);
    global->addCanvas(playBack);
    
    
    
    vector<ofxUIWidget*> ddls;
    ddls.push_back(midiPorts);
    for(int i = 0 ; i < ddls.size(); i++){
        ((ofxUIDropDownList*) ddls[i])->setAutoClose(true);
        ((ofxUIDropDownList*) ddls[i])->setShowCurrentSelected(true);
    }
    
    
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
    
    
    ofAddListener(ofEvents().draw,this,&GUI::draw);

    ofAddListener(midiCanvas->newGUIEvent,this , &GUI::guiEvent);
    ofAddListener(playBack->newGUIEvent,this , &GUI::guiEvent);
}

void GUI::setup(){
    guiLoad.setup();
    guiStatistics.setup();
    guiAxe.setup();
    guiClass.setup();
  
    logCanvas->autoSizeToFitWidgets();
    
    
}

void GUI::draw(ofEventArgs & a){

}



void GUI::guiEvent(ofxUIEventArgs &e){

    
    ofxUICanvas * root,*parent;
    
    
    
    root = (ofxUICanvas*)e.widget;
    
    while(root->getParent()!=NULL ){root= (ofxUICanvas*)root->getParent();}
    if(e.widget->getParent()!=NULL){ parent = (ofxUICanvas*)e.widget->getParent();}
    else{parent = root;}
    
    


    
//    
//    if(parent == NULL){
//        ofLogError("GUI", "orphan !!! : " +e.widget->getName() );
//        return;
//    }
    //ID for GUI Controls
    string rootName = root->getName();
    string parentName = parent->getName();
    
    
    
    if(root!=NULL){
        ofLogVerbose("GUI",root->getName() + "//" + e.getParent()->getName() + "//" + e.getName());
    }
    

    

    
    // Midi
    
    if(rootName == "Midi" ){
        if(parentName == "MidiPorts"){
            Midi::instance()->midiIn.closePort();
            Midi::instance()->midiIn.openPort(e.getName());
        }
        if(e.getName() == "VelocityRange"){
            Midi::velScale.set(((ofxUIRangeSlider*)e.widget)->getValueLow(),((ofxUIRangeSlider*)e.widget)->getValueHigh());
        }
        if(e.getName() == "Radius"){
            Midi::radius = ((ofxUISlider*)e.widget)->getValue();
        }
        if(e.getName()=="Hold"){
            Midi::hold=((ofxUIToggle*)e.widget)->getValue();
        }
        if(e.getName() == "link2Cam"){
            Midi::link2Cam = ((ofxUIToggle*)e.widget)->getValue();
        }
        if(e.widget == link2x ){
            if(link2x->getValue())
                Midi::midiModulo = Physics::maxs->x - Physics::mins->x + 1;
            else{
                Midi::midiModulo = 12;
            }
        }
        if(e.widget == midiSpots){
            Midi::bMidiSpot = e.getBool();
        }
        if(e.widget==randomMidi){
            Midi::random = e.getDouble();
        }
        
        
        
        
    }
    else if (rootName=="playBack"){
        if(e.widget == stopAll){
            AudioPlayer::stopAll();
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
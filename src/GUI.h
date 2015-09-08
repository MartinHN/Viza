//
//  GUI.h
//  ViZa
//
//  Created by martin hermant on 14/10/14.
//
//

#ifndef __ViZa__GUI__
#define __ViZa__GUI__

#define MYPARAM(nom,init,min,max) nom.setName(#nom);nom.setMin(min);nom.setMax(max);settings.add(nom);nom=init;


#include "GUIUtils.h"
#include "GUIAxes.h"
#include "GUIClass.h"
#include "GUIView.h"
#include "GUIFitter.h"
#include "GUICluster.h"
#include "GUIPhysics.h"
#include "GUILoad.h"
#include "GUIStatistics.h"

#include "Camera.h"
#include "Midi.h"
#include "SliceFitter.h"
#include "ofxTSNE.h"




class GUI{
public:

    GUI();
    ~GUI();
    
    
    GUILoad guiLoad;
    GUIAxes guiAxe;
    GUIClass guiClass;
    GUIView guiView;
    GUIFitter guiFitter;
    GUICluster guiCluster;
    GUIPhysics guiPhysics;
    GUIStatistics guiStatistics;
    
    ofxUITabBar * global;
    

    ofxUISuperCanvas * logCanvas;
    ofxUITextArea * Logger;



    ofxUISuperCanvas * midiCanvas;
    ofxUIDropDownList * midiPorts;
    ofxUIRangeSlider * midiVel;
    ofxUISlider * midiRadius;
    ofxUIToggle * midiHold;
    ofxUIToggle * midiLink2Cam;
    ofxUIToggle * link2x;
    ofxUIToggle * midiSpots;
    ofxUISlider * randomMidi;
    
    
    ofxUISuperCanvas * playBack;
    ofxUIToggle * continuousPB;
    ofxUIToggle * holdPB;
    ofxUIButton * stopAll;
    
    
    
    int pad=50;
    
    
     void guiEvent(ofxUIEventArgs &e);
    
    static GUI * i(){if(inst == NULL){inst = new GUI;} return inst;   }
    
    void setup();

    void draw (ofEventArgs & a);


    
    int lastFramenum;
    string lastName;
    
    
    void checkMinsMaxsChanged(bool updateVal);

    static void LogIt(string s );
    bool isOver(int x,int y);
    
    void update(ofEventArgs & a);
    
    static ofEvent<ofMessage> GUISharedEvent;
    

private:
    
    static GUI * inst;

    void asyncGUI();


    void init();
    
};



#endif /* defined(__ViZa__GUI__) */

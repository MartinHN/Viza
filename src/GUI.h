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

#include"ofxUI.h"
#include "ofxGui.h"
#include <iostream>
#include "Container.h"
#include "ofApp.h"

class GUI{
public:

    GUI();
    ~GUI();
    
    ofxUITabBar * global;
    
    ofxUISuperCanvas *guiconf;
    ofxUIDropDownList * attr[3], *aggr[3];
    ofxUIDropDownList *scaleType[3];
    ofxUINumberDialer * min[3],*max[3];
    ofxUIDropDownList * coordinateType;
    ofxUIToggle* clampValues;

    ofxUIScrollableCanvas * scrollNames;

    ofxUISuperCanvas * logCanvas;
    ofxUITextArea * Logger;
    
    ofxUISuperCanvas * viewCanvas;
    ofxUISlider * alphaView;
    ofxUIToggle * selBrightest;
    ofxUIToggle * linkSongs;
    ofxUIToggle * orthoCam;
    ofxUISlider * pointSize;
    ofxUIToggle * isClipping;
    ofxUIToggle * show2dViews;
    
    
    ofxUISuperCanvas * midiCanvas;
    ofxUIDropDownList * midiPorts;
    ofxUIRangeSlider * midiVel;
    ofxUISlider * midiRadius;
    ofxUIDropDownList * songNames;
    ofxUIToggle * midiHold;
    ofxUIToggle * midiLink2Cam;
    
    
    ofxUISuperCanvas * playBack;
    ofxUIToggle * continuousPB;
    ofxUIToggle * holdPB;
    
    
    int scrollW = 700;
    int pad=50;
    
    
     void guiEvent(ofxUIEventArgs &e);
    
    static GUI * instance(){if(inst == NULL){inst = new GUI;} return inst;   }
    
    void setup();
    void registerListener();

    
    ofParameter<bool> isModifiying;
    vector<string> attrNames;
    vector<string> aggrNames;
    vector<string> songnames;
    static GUI * inst;
    int lastFramenum;
    string lastName;
    
    
    void checkMinsMaxsChanged(bool updateVal);
    static string numToAxe(int i);
    static int axeToNum(char s);
    static void LogIt(string s );
    bool isOver(int x,int y);
    
    
private:

    vector<string> coordinateTypeNames;
};



#endif /* defined(__ViZa__GUI__) */

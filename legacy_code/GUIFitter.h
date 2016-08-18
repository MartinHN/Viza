//
//  GUIFitter.h
//  ViZa
//
//  Created by martin hermant on 24/04/15.
//
//

#ifndef __ViZa__GUIFitter__
#define __ViZa__GUIFitter__

#include "GUICanvasBase.h"

#include"SliceFitter.h"

class GUIFitter : public GUICanvasBase{
public:
    
    GUIFitter(string name);
    ~GUIFitter(){};
    
    
//    ofxUISuperCanvas * canvas;
    ofxUISlider * samplingPct;
    ofxUIDropDownList * typeOfFit;
    ofxUIToggle * keepResults;
    ofxUIToggle * fitIt;
    ofxUIButton * applyIt;
    

    void init() override;
    void setup();
    void async();
    void messageRecieved(ofMessage & msg) override{};

    private:
    
    void guiEvent(ofxUIEventArgs &a);
    void fitProgress(ofMessage & a);
    
    
    float progress;

    
};

#endif /* defined(__ViZa__GUIFitter__) */

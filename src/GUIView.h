//
//  GUIView.h
//  ViZa
//
//  Created by martin hermant on 24/04/15.
//
//

#ifndef __ViZa__GUIView__
#define __ViZa__GUIView__

#include "GUICanvasBase.h"


#include "Camera.h"

class GUIView : public GUICanvasBase{
    public :
    GUIView(string name);
    ~GUIView(){};
    
    
    void setup();
    void messageRecieved(ofMessage & msg) override{};

    ofxUISlider * alphaView;
    ofxUIToggle * selBrightest;
    ofxUIToggle * linkClasses;
    ofxUIToggle * orthoCam;
    ofxUISlider * pointSize;
    ofxUIToggle * isClipping;
    ofxUIToggle * show2dViews;
    ofxUISlider *fishEyeStrength;
    ofxUISlider *fishEyeRadius;
    
    void guiEvent(ofxUIEventArgs & e);
    
};

#endif /* defined(__ViZa__GUIView__) */

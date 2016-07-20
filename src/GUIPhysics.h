//
//  GUIPhysics.h
//  ViZa
//
//  Created by martin hermant on 24/04/15.
//
//

#ifndef __ViZa__GUIPhysics__
#define __ViZa__GUIPhysics__

#include "GUICanvasBase.h"

#include "PhysicalDistribution.h"


class GUIPhysics:  public GUICanvasBase{
    public :

    
    GUIPhysics(string name);
    
    ofxUIToggle * runPhysics;
    ofxUISlider * minDistance;
    ofxUISlider * kSpring;

    
    
    ofEvent<ofMessage> * sharedGUIEvent;
    void init() override;
    void setup() override {};
    
    void guiEvent(ofxUIEventArgs & a)override;
    
};

#endif /* defined(__ViZa__GUIPhysics__) */

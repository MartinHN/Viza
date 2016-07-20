//
//  GUIPlayBack.h
//  ViZa
//
//  Created by martin hermant on 19/02/16.
//
//

#ifndef __ViZa__GUIPlayBack__
#define __ViZa__GUIPlayBack__

#include "GUICanvasBase.h"


class GUIPlayBack:  public GUICanvasBase{
    public :
    
    
    GUIPlayBack(string name);

    
    
    
    ofEvent<ofMessage> * sharedGUIEvent;
    void init() override;
    void setup() override {};
    
    void guiEvent(ofxUIEventArgs & a)override;
    
    
    
    ofxUISuperCanvas * playBack;
    ofxUIToggle * continuousPB;
    ofxUIToggle * holdPB;
    ofxUIButton * stopAll;
    ofxUINumberDialer * maxPBTime;

};

#endif /* defined(__ViZa__GUIPlayBack__) */

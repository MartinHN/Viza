//
//  GUIMidi.h
//  ViZa
//
//  Created by martin hermant on 24/04/15.
//
//

#ifndef __ViZa__GUIMidi__
#define __ViZa__GUIMidi__

#include "GUICanvasBase.h"
#include "Midi.h"



class GUIMidi:  public GUICanvasBase{
    public :
    
    
    GUIMidi(string name);
    
    ofxUIDropDownList * midiPorts;
    ofxUISlider * midiVelRange,*midiVelCenter;
    ofxUIButton * resetNoteMap;
    ofxUISlider * midiRadius;
    ofxUIToggle * midiHold;
    ofxUIToggle * midiLink2Cam;
    ofxUIRangeSlider * vel2Vol;
    ofxUIToggle * midiSpots;
    ofxUISlider * randomMidi;
    
    
    ofEvent<ofMessage> * sharedGUIEvent;
    void init() override;
    void setup() override {};
    
    void guiEvent(ofxUIEventArgs & a);
    
};

#endif /* defined(__ViZa__GUIMidi__) */

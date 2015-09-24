//
//  GUIMidi.cpp
//  ViZa
//
//  Created by martin hermant on 24/04/15.
//
//

#include "GUIMidi.h"


GUIMidi::GUIMidi(string name): GUICanvasBase(name){
    
    
    ////////////////CLUSTER </////////////////////
    
    setName("Midi");

    
    
    midiPorts = new ofxUIDropDownList("MidiPorts", Midi::i()->getPorts(),150,0,0,OFX_UI_FONT_SMALL);
    midiVelRange = new ofxUISlider("VelocityRange",0,1.,0.,100,10);
    midiVelCenter = new ofxUISlider("VelocityCenter",0,1.,0.5,100,10);
    vel2Vol = new ofxUIRangeSlider("velocity2Volume",0.01,1,0.5,1,100,10);
    
    midiRadius = new ofxUISlider("Radius",0,.5,0.05,100,10);
    midiHold = new ofxUIToggle("Hold",false,10,10);
    
    midiLink2Cam = new ofxUIToggle("link2Cam",true,10,10);
    
    midiSpots = new ofxUIToggle("midiSpots",false,10,10);
    randomMidi = new ofxUISlider("randomMidi",0.0f,1.f,0.0f,150,10);
    
    
    addWidgetDown(midiPorts);
    addWidgetDown(midiVelRange);
    addWidgetDown(midiVelCenter);
    addWidgetDown(vel2Vol);
    addWidgetDown(midiRadius);
    addWidgetDown(midiHold);
    addWidgetDown(midiLink2Cam);
    addWidgetDown(midiSpots);
    addWidgetDown(randomMidi);

    
    setDimensions(getRect()->width, getRect()->height*1.3);

    
    vector<ofxUIWidget*> ddls;
    ddls.push_back(midiPorts);
    for(int i = 0 ; i < ddls.size(); i++){
        ((ofxUIDropDownList*) ddls[i])->setAutoClose(true);
        ((ofxUIDropDownList*) ddls[i])->setShowCurrentSelected(true);
    }

    
    
    
}


void GUIMidi::init(){
    
}



void GUIMidi::guiEvent(ofxUIEventArgs & e){
    
    if(e.getParent() == midiPorts){
        Midi::i()->midiIn.closePort();
        Midi::i()->midiIn.openPort(e.getName());
    }
    
    else if(e.widget == midiVelCenter || e.widget == midiVelRange){
        Midi::velScale.set(midiVelCenter->getValue() - midiVelRange->getValue()/2.0,midiVelCenter->getValue() + midiVelRange->getValue()/2.0);
    }
    else if(e.widget == resetNoteMap && !resetNoteMap->getValue()){
        Midi::midiNotes.clear();
        
    }
    else if(e.widget == midiRadius){
        Midi::radius = midiRadius->getValue();
    }
    else if(e.widget == midiHold){
        Midi::hold=midiHold->getValue();
    }
    else if(e.widget == midiLink2Cam){
        Midi::link2Cam = midiLink2Cam->getValue();
    }
    else if(e.widget == vel2Vol ){
        Midi::vel2VolScale.set(vel2Vol->getValueLow(),vel2Vol->getValueHigh());
    }
    else if(e.widget == midiSpots){
        Midi::bMidiSpot = e.getBool();
    }
    else if(e.widget==randomMidi){
        Midi::random = e.getDouble();
    }
}



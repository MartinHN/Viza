//
//  Midi.h
//  ViZa
//
//  Created by martin hermant on 21/10/14.
//
//

#ifndef __ViZa__Midi__
#define __ViZa__Midi__

#include <iostream>

#include "ofxMidi.h"
#include "Physics.h"
#include "Container.h"
//#include "ofApp.h"
class Container;


typedef struct swaplist{
    swaplist(){
        front = new vector<ofxMidiMessage>;
        back = new vector<ofxMidiMessage>;
    }
    
    void swap(){
        vector<ofxMidiMessage> * tmp = front;
        front = back;
        back = tmp;
    }
    vector<ofxMidiMessage> * front;
    vector<ofxMidiMessage> * back;
    
}swaplist;

class Midi:public ofxMidiListener {
    public :
    
    Midi(){
        ofAddListener(ofEvents().mouseReleased, this, &Midi::mouseReleased);
        ofAddListener(ofEvents().mousePressed, this, &Midi::mousePressed);
        ofAddListener(ofEvents().mouseDragged, this, &Midi::mouseDragged);

        midiIn.addListener(this);getPorts();midiIn.openPort(0);}
    ~Midi(){midiIn.closePort();
        midiIn.removeListener(this);}
    
    static Midi* i(){
        if(midi==NULL){midi = new Midi;}
        return midi;
    };
    
    
    void setup();
    void draw();
    void updateMidi(ofxMidiMessage & msg);
    vector<string> getPorts();
    void newMidiMessage(ofxMidiMessage& msg);
    void mouseReleased(ofMouseEventArgs & e);
    void mousePressed(ofMouseEventArgs & e);
    void mouseDragged(ofMouseEventArgs & e);
    static int midiModulo;
    static int midiRoot;
    static int midiMax;
    static float radius;
    static ofVec2f velScale;
    
    
    static map<int,ofPoint> curpoints;
    static map<int,Container * > curCont;
    static ofxMidiIn midiIn;
    static int draggedNum;

    static swaplist msg;
    static bool isReading;
    static bool hold;
    static bool link2Cam;
    
    
    static bool bMidiSpot;
    static bool bActive;
    

    static vector<ofVec3f > midiSpots;
    static vector<int> midiNotes;
    static float random;
    
    static ofVec2f vel2VolScale;
    


private:
    static Midi* midi;
    ofMutex mutex;

};




#endif /* defined(__ViZa__Midi__) */

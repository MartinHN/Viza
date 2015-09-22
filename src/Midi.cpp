//
//  Midi.cpp
//  ViZa
//
//  Created by martin hermant on 21/10/14.
//
//

#include "Midi.h"
#include "Camera.h"

#define MAX_MIDI_QUEUE 60

Midi * Midi::midi;
ofxMidiIn Midi::midiIn;
map<int,ofPoint> Midi::curpoints;
int Midi::midiModulo=12;
int Midi::midiRoot=24;
int Midi::midiMax = 60;
float Midi::radius = .05;
ofVec2f Midi::velScale(0.5,.5);
bool Midi::hold;
bool Midi::link2Cam=true;

bool Midi::isReading=false;
swaplist Midi::msg;
map<int,Container*> Midi::curCont;
float Midi::random=0;
int Midi::draggedNum = -1;

bool Midi::bMidiSpot = false;
vector<ofVec3f> Midi::midiSpots;
vector<int> Midi::midiNotes;



vector<string> Midi::getPorts(){
    midiIn.listPorts();
    
    return midiIn.getPortList();
    
}

void Midi::newMidiMessage(ofxMidiMessage& msgin){
    if(!isReading){
        ofScopedLock(mutex);
        if(msg.back->size()<MAX_MIDI_QUEUE){
            msg.back->push_back(msgin);
            ofLogVerbose("Midi",  "midicallback at " + ofToString(ofGetElapsedTimef()));
            update();
        }
    }
    
    else {
        ofLogWarning("Midi", "dropping MIDI" );
        
    }
    
    
    
    
}

void Midi::update(){
    {
        ofScopedLock(mutex);
        
        isReading = true;
        msg.swap();
        isReading = false;
        
        
        
        
        for(vector<ofxMidiMessage>::iterator it = msg.front->begin() ; it!=msg.front->end() ; ++it){
            
            // trigger new midi
            if(it->status==MIDI_NOTE_ON && it->velocity!=0){
                ofLogVerbose("Midi", "Midi pitch " + ofToString(it->pitch) );
                ofVec3f v (((it->pitch-midiRoot)%(midiModulo) )*1.0/((midiModulo-1)) , //
                           ((int)((it->pitch-midiRoot)/(midiModulo)) + 0.5)*1.0/((midiMax-midiRoot)/midiModulo),
                           ofMap(it->velocity, 127, 0, velScale.x, velScale.y));
                Container* cc = NULL;
                
                // trigger in spot : compute equivalent screen coordinates
                if(bMidiSpot){
                    
                    int curIdx  = ofFind(midiNotes,it->pitch);
                    if(curIdx>=midiNotes.size()){
                        curIdx = midiNotes.size();
                        midiNotes.push_back(it->pitch);
                    }
                    
                    ofVec3f v2;
                    
                    // midi spot already mapped to pitch
                    if(curIdx<midiSpots.size()){
                        v2 = midiSpots[curIdx];
                        v.x = v2.x;
                        v.y = v2.y;
                    }
                    
                    // todo handle non mapped midi Notes ?
                    else{
                        continue;
                    }
                    
                }
                
                
                
                //                randomize
                v.x+=random*(0.5-ofRandom(100)/100.0);
                v.z+=random*(0.5-ofRandom(100)/100.0);
                v.y+=random*(0.5-ofRandom(100)/100.0);
                
                
                
                
                // change to world coordinates
                // camera dependent mode
                if(link2Cam){
                    
                    Camera * locCam = Camera::mainCam;
                    v.z = locCam->toCamZ(v.z-.5);
                    ofRectangle viewPort =locCam->viewPort;
                    v*=ofVec3f(viewPort.width,viewPort.height,1);
                    
                    v=locCam->screenToWorld(v,viewPort);
                    
                }
                
                // absolute mode
                else{
                    v-= ofVec3f(.5);
                    v.y*=-1;
                    ofRectangle viewPort = Camera::mainCam->viewPort;
                    v.x*=viewPort.width*1.0/viewPort.height;
                }
                
                
                
                // store point
                curpoints[it->pitch] = v;
                ofLogVerbose("Midi") << it->pitch <<":" << v;
                
                
                
                // find triggered sample
                cc =Physics::nearest(v ,radius);
                if(cc!=NULL ){
                    cc->state=0;
                    ofLogNotice("Midi","call play " + ofToString(Physics::vs[cc->globalIdx]));
                    cc->state = 1;
                    curCont[it->pitch]=cc;
                }
                
            }
            
            
            
            // delete
            else if(it->status==MIDI_NOTE_OFF || (it->status==MIDI_NOTE_ON && it->velocity==0)   ){
                ofLogVerbose("Midi") << it->pitch <<" off" ;
                curpoints.erase(it->pitch);
                
                // stop if necessary
                if(curCont[it->pitch]!=NULL){
                    if(!hold)curCont[it->pitch]->state=0;
                    
                    curCont.erase(it->pitch);
                }
            }
            
        }
        
        
        
        msg.front->clear();
    }
    
}




void Midi::draw(){
    ofPushMatrix();
    ofPushStyle();
    ofPushView();
    ofNoFill();
    ofSetLineWidth(5);
    ofSetCircleResolution(160);
    //    ofVec3f r = ofApp::cam.getOrientationEuler();
    if(bMidiSpot){
        ofColor c = ofColor::green;
        c.a = 100;
        ofSetColor(c);
        
        
        for(vector<ofVec3f>::iterator it = midiSpots.begin(); it!= midiSpots.end() ; ++it){
            ofVec3f v;
            Camera * locCam = Camera::mainCam;
            ofRectangle viewPort =locCam->viewPort;
            if(link2Cam){
                v = *it;
                v.z = locCam->toCamZ(v.z-.5);
                v*=ofVec3f(viewPort.width,viewPort.height,1);
                v=locCam->screenToWorld(v,viewPort);
            }
            else{
                v =*it ;
                v.x=( v.x-.5 )* viewPort.width*1.0/viewPort.height;
                v.y = (.5-v.y );
            }
            
            ofDrawCircle(v,radius);
            
        }
    }
    
    if(radius>0){
        ofColor c = ofColor::blue;
        c.a = 100;
        ofSetColor(c);
        {
            
            ofScopedLock sl(mutex);
            for(map<int,ofVec3f>::iterator it = curpoints.begin(); it!= curpoints.end() ; ++it){
                
                ofDrawCircle(it->second,radius);
                
            }
        }
    }
    ofPopStyle();
    ofPopMatrix();
    ofPopView();
}



void Midi::mousePressed(ofMouseEventArgs & a){
    
    draggedNum = -1;
    if(a.button!= 1) return;
    
    for(auto & s:midiSpots){
        ofRectangle viewPort = Camera::mainCam->viewPort;
        ofVec2f screenPos;
        if( link2Cam) screenPos = s*ofVec2f(viewPort.x,viewPort.y);
        else {
            screenPos =s ;
            screenPos.x=( screenPos.x-.5 );//* viewPort.width*1.0/viewPort.height;
            screenPos.y = (.5-screenPos.y );
            screenPos = ofVec2f(Camera::mainCam->worldToScreen(screenPos));
            cout << screenPos << endl;
        }
        if(abs(a.x -screenPos.x)<radius*viewPort.height &&
           abs(a.y -screenPos.y)<radius * viewPort.height)
        {
            
            draggedNum = &s - &midiSpots[0];
            
            break;
        }
        
    }
    
    
}

void Midi::mouseDragged(ofMouseEventArgs & a){
    if(draggedNum!=-1){
        midiSpots[draggedNum].set(a.x/ofGetWidth(),a.y/ofGetHeight());
        
    }
}


void Midi::mouseReleased(ofMouseEventArgs & a){
    
    int hoverIdx  = -1;
    for(auto & s:midiSpots){
        ofRectangle viewPort = Camera::mainCam->viewPort;
        ofVec2f screenPos;
        if( link2Cam) screenPos = s*ofVec2f(viewPort.x,viewPort.y);
        else {
            screenPos =s ;
            screenPos.x=( screenPos.x-.5 );//* viewPort.width*1.0/viewPort.height;
            screenPos.y = (.5-screenPos.y );
            screenPos = ofVec2f(Camera::mainCam->worldToScreen(screenPos));
            cout << screenPos << endl;
        }
        if(abs(a.x -screenPos.x)<radius*viewPort.height &&
           abs(a.y -screenPos.y)<radius * viewPort.height)
        {
            hoverIdx = &s - &midiSpots[0];
            break;
        }
    }
    
    if(hoverIdx>=0 && a.button == 0){
        midiSpots.erase(midiSpots.begin() + hoverIdx);
        if(hoverIdx < midiNotes.size()){
            midiNotes.erase(midiNotes.begin() + hoverIdx);
        }
    }
    
    if(hoverIdx<0 && a.button == 2 ){
        midiSpots.push_back(ofVec3f(a.x/ofGetWidth(),a.y/ofGetHeight()));
        
    }
    
    
}
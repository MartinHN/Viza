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
ofVec2f Midi::velScale(0,1);
bool Midi::hold;
bool Midi::link2Cam=true;

bool Midi::isReading=false;
swaplist Midi::msg;
map<int,Container*> Midi::curCont;

bool Midi::bMidiSpot = false;
multimap<int,ofVec3f> Midi::midiSpots;



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
            
            if(it->status==MIDI_NOTE_ON && it->velocity!=0){
                bool canAddSpot = false;
                ofLogVerbose("Midi", "Midi pitch " + ofToString(it->pitch) );
                ofVec3f v (((it->pitch-midiRoot)%(midiModulo) )*1.0/((midiModulo-1)) , //
                           ((int)((it->pitch-midiRoot)/(midiModulo)) + 0.5)*1.0/((midiMax-midiRoot)/midiModulo),
                           ofMap(it->velocity, 127, 0, velScale.x, velScale.y));
                Container* cc = NULL;
                if(bMidiSpot){
                    canAddSpot = true;
                    ofVec3f v2;
                    multimap<int,ofVec3f>::iterator mit;
                    mit = midiSpots.find(it->pitch);
                    if(mit!=midiSpots.end()){
                        v2 = mit->second;
                        v.x = v2.x;
                        v.y = v2.y;
                        canAddSpot = false;
                    }
                    else if((mit = midiSpots.find(0))!= midiSpots.end()){
                        v2 = mit->second;
                        v.x = v2.x;
                        v.y = v2.y;
                        canAddSpot = true;
                        midiSpots.insert(std::pair<int,ofVec3f>(it->pitch,v2));
                        midiSpots.erase(mit);
                    }
                    

                }
                
                
                
                
                
                 if(link2Cam){
                    
                     
                    Camera * locCam = Camera::mainCam;
                    v.z = locCam->toCamZ(v.z-.5);
                    ofRectangle viewPort =locCam->viewPort;
                    v*=ofVec3f(viewPort.width,viewPort.height,1);
                    
                    v=locCam->screenToWorld(v,viewPort);

                }
                else{
                    v-= ofVec3f(.5);
                    v.y*=-1;
                    ofRectangle viewPort = Camera::mainCam->viewPort;
                    v.x*=viewPort.width*1.0/viewPort.height;
                }
                curpoints[it->pitch] = v;
                
                //            v+= ofVec3f(0.5,0.5,0);
                ofLogVerbose("Midi") << it->pitch <<":" << v;

                cc =Physics::nearest(v ,radius);
                if(cc!=NULL ){
                    cc->state=0;
                    ofLogNotice("Midi","call play " + ofToString(Physics::vs[cc->globalIdx]));
                    cc->state = 1;
                    curCont[it->pitch]=cc;
                }
                
            }
            else if(it->status==MIDI_NOTE_OFF || (it->status==MIDI_NOTE_ON && it->velocity==0)   ){
                ofLogVerbose("Midi") << it->pitch <<" off" ;
                curpoints.erase(it->pitch);
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
    //    ofVec3f r = ofApp::cam.getOrientationEuler();
    if(bMidiSpot){
        ofColor c = ofColor::green;
        c.a = 100;
        ofSetColor(c);
        for(multimap<int,ofVec3f>::iterator it = midiSpots.begin(); it!= midiSpots.end() ; ++it){
            ofVec3f v;
            Camera * locCam = Camera::mainCam;
            ofRectangle viewPort =locCam->viewPort;
            if(link2Cam){
            v = it->second;
            
            v.z = locCam->toCamZ(v.z-.5);
            
            v*=ofVec3f(viewPort.width,viewPort.height,1);
            
            v=locCam->screenToWorld(v,viewPort);
            }
            else{
                v =it->second ;
                v.x=( v.x-.5 )* viewPort.width*1.0/viewPort.height;
                v.y = (.5-v.y );

            }

            ofDrawSphere(v,radius);
            
        }
    }
    if(radius>0){
        ofColor c = ofColor::blue;
        c.a = 100;
        ofSetColor(c);
        for(map<int,ofVec3f>::iterator it = curpoints.begin(); it!= curpoints.end() ; ++it){
            
            ofDrawSphere(it->second,radius);
            
        }
    }
    ofPopStyle();
    ofPopMatrix();
}


void Midi::mouseReleased(ofMouseEventArgs & a){
    
    bool hover = false;
    for(auto s:midiSpots){
        ofRectangle viewPort = Camera::mainCam->viewPort;
        ofVec2f screenPos;
        if( link2Cam) screenPos = s.second*ofVec2f(viewPort.x,viewPort.y);
            else {
                screenPos =s.second ;
                screenPos.x=( screenPos.x-.5 );//* viewPort.width*1.0/viewPort.height;
                screenPos.y = (.5-screenPos.y );
                screenPos = ofVec2f(Camera::mainCam->worldToScreen(screenPos));
                cout << screenPos << endl;
            }
        if(abs(a.x -screenPos.x)<radius*viewPort.height &&
           abs(a.y -screenPos.y)<radius * viewPort.height)
        {
            if(a.button == 0){
                midiSpots.erase(s.first);
            }
            hover = true;
            
            break;
        }
    }
    if(!hover && a.button == 2 ){
        midiSpots.insert(std::pair<int,ofVec3f>(0,ofVec3f(a.x/ofGetWidth(),a.y/ofGetHeight())));
        
    }
    
    
}
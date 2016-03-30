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
bool Midi::link2Cam=false;

bool Midi::isReading=false;

map<int,Container*> Midi::curCont;
float Midi::random=0;
int Midi::draggedNum = -1;

bool Midi::bMidiSpot = true;
vector<MidiSpot> Midi::midiSpots;
vector<int> Midi::midiNotes;

ofVec2f Midi::vel2VolScale = ofVec2f(0,1);


bool Midi::bActive = true;

ofMutex Midi::mutex;



vector<string> Midi::getPorts(){
    midiIn.listPorts();
    
    return midiIn.getPortList();
    
}

void Midi::newMidiMessage(ofxMidiMessage& msgin){
    if(bActive)
        updateMidi(msgin);

    
    
    
}

void Midi::updateMidi(ofxMidiMessage & msg){
    {
        ofScopedLock sl(mutex);
        
        // trigger new midi
        if(msg.status==MIDI_NOTE_ON && msg.velocity!=0){
            ofLogVerbose("Midi", "Midi pitch " + ofToString(msg.pitch) );
            ofVec3f normalizedV (((msg.pitch-midiRoot)%(midiModulo) )*1.0/((midiModulo-1)) , //
                                 ((int)((msg.pitch-midiRoot)/(midiModulo)) + 0.5)*1.0/((midiMax-midiRoot)/midiModulo),
                                 ofMap(msg.velocity, 127, 0, velScale.x, velScale.y));
            Container* cc = NULL;
            
            // trigger in spot : compute equivalent screen coordinates
            if(bMidiSpot){
                int curPitch = msg.pitch;
                int curIdx  =  std::distance(
                                              midiSpots.begin(),
                                              find_if(midiSpots.begin(),
                                                      midiSpots.end(),
                                                      [curPitch](MidiSpot & m){return m.pitch == curPitch;})
                                            );
                
                // assign to empty midi spot
                cout << curIdx << endl;
                if((curIdx < 0) || curIdx>=midiSpots.size()){
                    vector<MidiSpot>::iterator miit = find_if(midiSpots.begin(),
                                                              midiSpots.end(),
                                                              [](MidiSpot & m){return m.pitch == -1;});
                    if(miit!= midiSpots.end()){
                        cout << "set : " << curPitch << endl;
                        miit->setPitch(curPitch);
                        normalizedV = miit->normalizedPos;
                    }
                    else{
                        return;
                    }
                }
                
                else{
                        normalizedV = midiSpots[curIdx].normalizedPos;

                }
                
                
            }
            
            
            
            //                randomize
            normalizedV.x+=random*(0.5-ofRandom(100)/100.0);
            normalizedV.z+=random*(0.5-ofRandom(100)/100.0);
            normalizedV.y+=random*(0.5-ofRandom(100)/100.0);
            

            
            
            // change to world coordinates
            // camera dependent mode
            if(link2Cam){
                
                Camera * locCam = Camera::mainCam;
                normalizedV.z = locCam->toCamZ(normalizedV.z-.5);
                ofRectangle viewPort =locCam->viewPort;
                normalizedV*=ofVec3f(viewPort.width,viewPort.height,1);
                
                normalizedV=locCam->screenToWorld(normalizedV,viewPort);
                
            }
            
            // absolute mode
            else{
                normalizedV-= ofVec3f(.5,.5,0);
                normalizedV.y*=-1;
                ofRectangle viewPort = Camera::mainCam->viewPort;
                normalizedV.x*=viewPort.width*1.0/viewPort.height;
                cout << normalizedV << endl;
            }
            
            // find triggered sample
            cc =Physics::nearestVisible(normalizedV,radius);
            
            // store point
            curpoints[msg.pitch] = normalizedV;
            ofLogVerbose("Midi") << msg.pitch <<":" << normalizedV;
            
            
            

            if(cc!=NULL ){
                cc->setState(0);
                ofLogNotice("Midi") <<"call play " << ofToString(Physics::vs[cc->globalIdx]);
                cout << cc->getPos() << endl;
                cc->setState(ofMap(msg.velocity, 0, 127, vel2VolScale.x,vel2VolScale.y));
                curCont[msg.pitch]=cc;
            }
            
            else{
                ofLogVerbose("Midi") << "not found : " << msg.pitch;
            }
            
        }
        
        
        
        // delete
        else if(msg.status==MIDI_NOTE_OFF || (msg.status==MIDI_NOTE_ON && msg.velocity==0)   ){
            ofLogVerbose("Midi") << msg.pitch <<" off" ;
            
            
            // stop if necessary
            if(curCont[msg.pitch]!=NULL){
                if(!hold)curCont[msg.pitch]->setState(0);
                
                curCont.erase(msg.pitch);
            }
        }
        
    }

    
}




void Midi::draw(){
    //    ofPushMatrix();
    ofPushStyle();
    
    ofNoFill();
    ofSetLineWidth(5);
    ofSetCircleResolution(160);
    
    // check curpoints integrity
    {
        
        ofScopedLock sl(mutex);
        for(auto & kv:curCont){
            if(curpoints.count(kv.first)!=0){
                curpoints.erase(kv.first);
            }
        }
    }
    //    ofVec3f r = ofApp::cam.getOrientationEuler();
    if(bMidiSpot){
        ofColor c = ofColor::red;
        c.a = 255;
        ofSetColor(c);
        
        Camera * locCam = Camera::mainCam;

        for(vector<MidiSpot>::iterator it = midiSpots.begin(); it!= midiSpots.end() ; ++it){
                it->draw(link2Cam,locCam);                //

        }
    }
    
    if(radius>0){
        ofColor c = ofColor::blue;
        c.a = 100;
        ofSetColor(c);
        {
            
            for(map<int,ofVec3f>::iterator it = curpoints.begin(); it != curpoints.end() ; ++it){
                
                ofDrawCircle(it->second,radius);
                
            }
        }
    }
    ofPopStyle();
    //    ofPopMatrix();
    
}



void Midi::mousePressed(ofMouseEventArgs & a){
    if(!bMidiSpot)return;
    draggedNum = -1;
    if(a.button!= 0 || !ofGetKeyPressed('m')) return;
    
    for(auto & s:midiSpots){
        ofRectangle viewPort = Camera::mainCam->viewPort;
        ofVec2f screenPos;
        if( link2Cam){
            screenPos = s.normalizedPos*ofVec2f(viewPort.width,viewPort.height);
        }
        else {
            screenPos =s.normalizedPos ;
            screenPos.x=( screenPos.x-.5 )* viewPort.width*1.0/viewPort.height;
            screenPos.y = (.5-screenPos.y );
            screenPos = ofVec2f(Camera::mainCam->worldToScreen(screenPos));
            
        }
        if(abs(a.x -screenPos.x)<radius * viewPort.height &&
           abs(a.y -screenPos.y)<radius * viewPort.height)
        {
            
            draggedNum = &s - &midiSpots[0];
            
            break;
        }
        
    }
    
    
}

void Midi::mouseDragged(ofMouseEventArgs & a){
    if(!bMidiSpot)return;
    if(draggedNum!=-1){
        midiSpots[draggedNum].normalizedPos.set(a.x/ofGetWidth(),a.y/ofGetHeight());
        
    }
}


void Midi::mouseReleased(ofMouseEventArgs & a){
    int hoverIdx  = -1;
    if(draggedNum!=-1) {
        draggedNum = -1;
        return;}
    
    if(!bMidiSpot){return;}
    
    
    for(auto & s:midiSpots){
        ofRectangle viewPort = Camera::mainCam->viewPort;
        ofVec2f screenPos;
        if( link2Cam) screenPos = s.normalizedPos*ofVec2f(viewPort.width,viewPort.height);
        else {
            screenPos =s.normalizedPos ;
            screenPos.x=( screenPos.x-.5 )* viewPort.width*1.0/viewPort.height;
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
    

    if(ofGetKeyPressed('m')){

        if(hoverIdx>=0 && a.button == 2 ){
                    ofScopedLock lk(mutex);
            midiSpots.erase(midiSpots.begin() + hoverIdx);
            if(hoverIdx < midiNotes.size()){
                midiNotes.erase(midiNotes.begin() + hoverIdx);
            }
        }
        

        if(hoverIdx<0 && a.button == 0){
            ofScopedLock lk(mutex);
            midiSpots.push_back(MidiSpot(ofVec2f(a.x/ofGetWidth(),a.y/ofGetHeight()),radius));
            
        }
    }
    
}
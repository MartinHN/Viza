#include "ofApp.h"
//#include <omp.h>
#include <stdio.h>
Camera ofApp::cam;
ofVec3f ofApp::scrS;

static const     double clipPlanes[] = {
    1,  0,  0,  .51,
    -1, 0,  0,  .51,
    0,  1,  0,  .51,
    0,  -1, 0,  .51,
    0,  0,  1,  .51,
    0,  0,  -1, .51
    
    
};

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(50);
    //    ofEnableAlphaBlending();
    //    ofDisableSmoothing();
    //    ofEnableSmoothing();
    //    ofEnableAntiAliasing();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    
    //    ofDisableDepthTest();
    
    
    

    cam.setRelativeViewPort(0,0,1,1);
    cam.setup();
    Camera::mainCam = &cam;
    
    for(int i = 0; i < 2 ; i++){
        cam2ds.push_back(new Camera());
        cam2ds[i]->setRelativeViewPort(.75, i>=1?.5:0,.25,.5);
        cam2ds[i]->setup();
        cam2ds[i]->isVisible = false;
    }
    
    
    cam.setTranslationKey('a');
    cam.setZoomKey('s');
    
    Casttime=ofGetElapsedTimeMillis();
    
    
    loadFiles();
    
    
    ofBackground(0);
    
    ofShowCursor();
    
    
    
    
    
    GUI::instance()->isModifiying.addListener(this, &ofApp::isGUIing);
    AudioPlayer::instance();
    windowResized(ofGetWindowWidth(), ofGetWindowHeight());
    
    lastCamPos =cam.getPosition();
    
    
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    Midi::update();
    
    if((cam.getPosition()-lastCamPos).length()>0 ){
        isCamSteady = false;
        if(ofGetElapsedTimef()-lastCamUpdate>.3){
            Physics::updateVScreen();
            lastCamUpdate = ofGetElapsedTimef();
        }
    }
    else if (!isCamSteady ){
        Physics::updateVScreen();
        isCamSteady = true;
        //        cout << "steadyCam" << endl;
    }
    lastCamPos = cam.getPosition();
    
    
    
}

void ofApp::loadFiles(string audiopath,string segpath){
    
    
    AudioPlayer::UnloadAll();
    Container::clearAll();
    jsonLoader::instance()->loadSegments(audiopath,segpath);
    Physics::resizeVBO();
    GUI::instance()->registerListener();
    GUI::instance()->setup();
    Container::registerListener();
    for(map<string,vector<Container* > >::iterator it = Container::songs.begin() ; it != Container::songs.end() ; ++it ){
        for(int i = 0 ; i <POLYPHONY ; i++){
            //           AudioPlayer::Load(*it->second[i], true);
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    cam.begin();
    draw3d();
    cam.end();
    
    if(GUI::instance()->show2dViews->getValue()){
        for(int i = 0 ; i< cam2ds.size(); i ++) {
            cam2ds[i]->begin();
            draw3d();
            cam2ds[i]->end();
        }
    }
    
    
    if(abs(selectRect.width)>0){
        ofSetColor(0,0,255,80);
        ofRect(selectRect);
    }
    
    
    
    
    
    
    
}

void ofApp:: draw3d(){
    if(GUI::instance()->isClipping->getValue()){
        for(int i = 0 ; i < 6 ; i++) {
            glClipPlane(GL_CLIP_PLANE0+i,&clipPlanes[4*i]);
            glEnable(GL_CLIP_PLANE0+i);
        }
    }
    
    
    Physics::draw();
    if(GUI::instance()->isClipping->getValue()){
        for(int i = 0 ; i < 6 ; i++) {
            glDisable(GL_CLIP_PLANE0+i);
        }
    }
    
    
    
    Midi::draw();
    
    
    
}










void ofApp::isGUIing(bool & t){
    //    if(t){
    //        cam.disableMouseInput();
    //
    //    }
    //    else{
    //        cam.enableMouseInput();
    //
    //    }
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    switch (key) {
        case 'x':
            Camera::getActiveCam()->orbit(-90,0,cam.getDistance());
            break;
        case 'y':
            Camera::getActiveCam()->orbit(0,90,cam.getDistance());
            break;
        case 'z':
            Camera::getActiveCam()->orbit(0,0,cam.getDistance());
            break;
            
        case 'l':{
            ofFileDialogResult f = ofSystemLoadDialog("analysisFiles",true);
            loadFiles("",f.filePath);
            break;
        }
        case ' ':
            ofFmodSoundStopAll();
            break;
            
        case 'f':
            if(SliceFitter::i()->fitThread.isThreadRunning()){
                SliceFitter::i()->fitThread.fitter->forceStop();
            }
            else
                SliceFitter::i()->fitFor();
            break;
        case 'h':
            Physics::drawFits = !Physics::drawFits;
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
    if(ofGetElapsedTimeMillis()-Casttime>10){
        if(isSelecting){
            
        }
        else{
            Container * cc = Physics::hoveredOnScreen( ofVec3f(x,y,0));
            bool change = Container::hoverContainer(cc == NULL?-1:cc->index);
            Casttime = ofGetElapsedTimeMillis();
            string log = "";
            if (change ){
                if(cc!=NULL){
                    log+=cc->filename +"\n";
                    for(int i = 0 ; i < 3; i++){
                        log+=ofToString(cc->getAttributes(Physics::curAttributesIndex[i]),4) + " ";
                    }
                }
                GUI::LogIt(log);
                
            }
            
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
    // select instances
    if(isSelecting){
        
        selectRect.width =  x -selectRect.x;
        selectRect.height=  y -selectRect.y;
        //        selectRect.standardize();
        
    }
    
    // drag instances
    else if(button==1){
        if(Physics::updateDrag(ofVec2f(x,y))){
            
        }
        
    }
    
    // play instances
    else if (button ==2 && GUI::instance()->continuousPB->getValue()){
        if(ofGetElapsedTimeMillis()-Casttime>70){
            Container * cc = Physics::nearestOnScreen( ofVec3f(x,y,0));
            int oldIdx = Container::hoverIdx;
            bool change = Container::hoverContainer(cc == NULL?-1:cc->index);
            Casttime = ofGetElapsedTimeMillis();
            if (change){
                GUI::LogIt(cc == NULL?"":cc->filename +"\n"+ ofToString((cc->getPos()*(Physics::maxs.get()-Physics::mins)+Physics::mins)));
                if(cc!=NULL)cc->state =1;
                if(oldIdx>=0 && !GUI::instance()->holdPB->getValue())Container::containers[oldIdx].state=0;
            }
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    // start selecting multiple
    if(ofGetKeyPressed('d') && button == 1){
        Physics::dragged.clear();
        selectRect.set(x,y,0,0);
        cam.disableMouseInput();
        isSelecting = true;
        selectRect.x = x;
        selectRect.y = y;
        return;
    }
    else{
        isSelecting = false;
    }
    
    Container * cc = Container::hoverIdx!=-1? &Container::containers[Container::hoverIdx]:NULL;// if (cc == NULL) {cout<< "error : no Container hovered" << endl;return;}
    
    // play
    if(button == 2 && cc)cc->state =1;//cc->state==0?1:0;
    
    // drag
    if(button == 1){
        
        // will drag multiple
        if (selectRect.inside(x,y)){
            
            Physics::originDrag.clear();
            
            for(int i = 0 ; i < Physics::dragged.size() ; i++){
                int curI = Physics::dragged[i]->index;
                Physics::originDrag.push_back(cam.worldToScreen(Physics::vs[curI]) - ofVec2f(x,y));
            }
            selectRect.set(x,y,0,0);
            
        }
        
        
        
        
        
        else{
            selectRect.set(x,y,0,0);
            Physics::dragged.clear();
            // one selected
            if(cc){
                
                Physics::originDrag.clear();
                Physics::dragged.push_back(cc);
                ofVec3f screenD = cam.worldToScreen(Physics::vs[cc->index])-ofVec3f(x,y);
                Physics::originDrag.push_back(screenD);
            }
            
            
        }
        
        
    }
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    if(isSelecting){
        selectRect.width =  x - selectRect.x;
        selectRect.height =  y -selectRect.y;
        selectRect.standardize();
        Physics::dragged = Physics::containedInRect(selectRect);
        isSelecting = false;
        cam.enableMouseInput();
    }
    
    Physics::updateVScreen();
    //    Physics::dragged.clear();
    //    cam.enableMouseMiddleButton();
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
    scrS.x =w;
    scrS.y =h;
    scrS.z = cam.getFarClip()-cam.getNearClip();
    cam.updateViewPort();
    
    for(int i = 0 ; i < cam2ds.size() ; i++){
        cam2ds[i]->updateViewPort();
    }
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

void ofApp::exit(){
    for(int i = 0 ; i < cam2ds.size() ; i++){
        delete cam2ds[i];
    }
}






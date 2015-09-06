#include "ofApp.h"
//#include <omp.h>
#include <stdio.h>

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
    
    ofSetLogLevel(OF_LOG_WARNING);
    ofSetLogLevel("GUI",OF_LOG_NOTICE);
//    ofSetLogLevel("Container",OF_LOG_VERBOSE);
    ofSetLogLevel("FileImporter",OF_LOG_VERBOSE);

//    ofSetLogLevel("FileLoader",OF_LOG_VERBOSE);
//    ofSetLogLevel("ofxUI",OF_LOG_VERBOSE);
    
    
    ofSetFrameRate(50);
    
    //    ofEnableAlphaBlending();
    //    ofDisableSmoothing();
    //        ofEnableSmoothing();
    //        ofEnableAntiAliasing();
    ofDisablePointSprites();
    glEnable(GL_POINT_SMOOTH);
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
        cam2ds[i]->setTranslationKey('a');
    }
    
    
    cam.setTranslationKey('a');
    //cam.setZoomKey('s');
    
    Casttime=ofGetElapsedTimeMillis();
    
//    ofSetLogLevel(OF_LOG_VERBOSE);
//    loadFiles();
//    ofEvents().disable();
//    ofEvents().update.enable();
//    ofEvents().draw.enable();
//    ofEvents().keyReleased.enable();
//    ofEvents().exit.enable();
    ofBackground(0);
    
    ofShowCursor();
    
    
    
    
    

    AudioPlayer::instance();
    windowResized(ofGetWindowWidth(), ofGetWindowHeight());
    
    lastCamPos =cam.getPosition();
    
    fishEye.load("shaders/fishEye");
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    if(FileImporter::i()->hasLoaded){
        
//        if(!ofEvents().mouseMoved.isEnabled()){
//            onCompletion();
//        }
    Midi::update();
    
    if((cam.getPosition()-lastCamPos).length()>0 ){
        isCamSteady = false;
        //        if(ofGetElapsedTimef()-lastCamUpdate>.3){
        //            Physics::updateVScreen();
        //            lastCamUpdate = ofGetElapsedTimef();
        //        }
    }
    else if (!isCamSteady ){
        Physics::updateVScreen();
        isCamSteady = true;
        ofLogNotice("ofApp" , "steadyCam");
    }
    lastCamPos = cam.getPosition();
    
    
    
    fishEye.setUniform1f("BarrelPower",1);
    }

    
}







//--------------------------------------------------------------
void ofApp::draw(){
    if(FileImporter::i()->hasLoaded){
//        if(!ofEvents().mouseMoved.isEnabled()){
//            onCompletion();
//        }
    cam.begin();
    if(GUI::i()->guiView.fishEyeRadius->getValue()>0){
        
        
        fishEye.begin();
        fishEye.setUniform1f("maxradius", (float)GUI::i()->guiView.fishEyeRadius->getValue());
        fishEye.setUniform1f("strength", 1/(1.001 -float(GUI::i()->guiView.fishEyeStrength->getValue())));
        fishEye.setUniform2f("mouse", 2*(mouseX*1.0/cam.viewPort.width-.5),2*(-mouseY*1.0f/cam.viewPort.height+.5) );
        
    }
    draw3d();
    if(GUI::i()->guiView.fishEyeRadius->getValue()>0){
        fishEye.end();
    }
    cam.end();
    
    if(GUI::i()->guiView.show2dViews->getValue()){
        for(int i = 0 ; i< cam2ds.size(); i ++) {
            cam2ds[i]->begin();
            draw3d();
            cam2ds[i]->end();
        }
    }
    
    
    if(abs(selectRect.width)>0){
        ofSetColor(0,0,255,80);
        ofDrawRectangle(selectRect);
    }
    
    
    
    }
    
    else{
        ofVec2f barSize (ofGetWidth()/3,200);
        ofRectangle bar;
        bar.setFromCenter(ofGetWidth()/2, ofGetHeight()/2, ofGetWidth()*3/4, ofGetHeight()/10);
        ofSetColor(ofColor::gray);
        ofDrawRectangle(bar);
        ofSetColor(ofColor::red);
        ofDrawRectangle(bar.getMinX(),bar.getMinY(),bar.width*FileImporter::i()->progressPct,bar.height);
        ofSetColor(255);
        ofDrawBitmapString(ofToString(FileImporter::i()->progressPct * 100) + " %",bar.getCenter());
        
    }
    
    
    
}

void ofApp:: draw3d(){
    
    //    ofDisablePointSprites();
    //    ofEnableAlphaBlending();
    //    ofEnableAntiAliasing();
    //    ofEnableSmoothing();
    
    if(GUI::i()->guiView.isClipping->getValue()){
        for(int i = 0 ; i < 6 ; i++) {
            glClipPlane(GL_CLIP_PLANE0+i,&clipPlanes[4*i]);
            glEnable(GL_CLIP_PLANE0+i);
        }
    }
    
    
    Physics::draw();
    if(GUI::i()->guiView.isClipping->getValue()){
        for(int i = 0 ; i < 6 ; i++) {
            glDisable(GL_CLIP_PLANE0+i);
        }
    }
    
    
    
    Midi::draw();
    
    
    
}











//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){

    switch (key) {
        case 'x':
            Camera::getActiveCam()->orbit(-90,0,cam.getDistance());
            Physics::updateVScreen();
            break;
        case 'y':
            Camera::getActiveCam()->orbit(0,90,cam.getDistance());
            Physics::updateVScreen();
            break;
        case 'z':
            Camera::getActiveCam()->orbit(0,0,cam.getDistance());
            Physics::updateVScreen();
            break;
            
        case 'l':{
            ofFileDialogResult f = ofSystemLoadDialog("analysisFiles",true);
            FileImporter::loadAnalysisFiles(f.filePath);
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
            
        case 's':
            if(ofGetKeyPressed(OF_KEY_LEFT_SUPER))FileImporter::i()->savePosition();
            break;
            
            case 'a':
            Physics::applyFit();
            break;
            


            
            case 'p':
#ifdef PROTOBUF_SUPPORT
            FileImporter::i()->saveProto();
#endif
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
            bool change = Container::hoverContainer(cc == NULL?-1:cc->globalIdx);
            Casttime = ofGetElapsedTimeMillis();
            string log = "";
            if (change ){
                if(cc!=NULL){
                    log+=cc->getFilename() +"\n";
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
    else if (button ==2 && GUI::i()->continuousPB->getValue()){
        if(ofGetElapsedTimeMillis()-Casttime>70){
            Container * cc = Physics::nearestOnScreen( ofVec3f(x,y,0));
            int oldIdx = Container::hoverIdx;
            bool change = Container::hoverContainer(cc == NULL?-1:cc->globalIdx);
            Casttime = ofGetElapsedTimeMillis();
            if (change){
                GUI::LogIt(cc == NULL?"":cc->getFilename() +"\n"+ ofToString((cc->getPos()*(Physics::maxs.get()-Physics::mins)+Physics::mins)));
                if(cc!=NULL)cc->state =1;
                if(oldIdx>=0 && !GUI::i()->holdPB->getValue())Container::containers[oldIdx]->state=0;
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
    
    Container * cc = Container::hoverIdx!=-1? Container::containers[Container::hoverIdx]:NULL;
    
    // play
    if(button == 2 && cc)cc->state =1;//cc->state==0?1:0;
    
    // drag
    if(button == 1){
        
        // will drag multiple
        if (selectRect.inside(x,y)){
            
            Physics::originDrag.clear();
            
            for(int i = 0 ; i < Physics::dragged.size() ; i++){
                int curI = Physics::dragged[i]->globalIdx;
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
                if(ofGetKeyPressed('u')){

                    vector<string > paths;
                    paths.push_back(cc->getAudioPath());
                    DragOut::i()->performExternalDragDropOfFiles(paths,ofGetCocoaWindow(),ofGetMouseX(),ofGetWindowHeight() - ofGetMouseY());
                }
                else{
                Physics::dragged.push_back(cc);
                ofVec3f screenD = cam.worldToScreen(Physics::vs[cc->globalIdx])-ofVec3f(x,y);
                Physics::originDrag.push_back(screenD);
                }
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
    if(dragInfo.files.size()==1 && ofFile(dragInfo.files[0]).isDirectory()){
        ofLogWarning("App") << "dragged : " << dragInfo.files[0];
    FileImporter::loadAnalysisFiles(dragInfo.files[0],"");
    }
    else{
        ofLogWarning("App") << "dragged non supported : " << dragInfo.files[0];
    }
}

void ofApp::exit(){
    for(int i = 0 ; i < cam2ds.size() ; i++){
        delete cam2ds[i];
    }
}






#include "ofApp.h"
//#include <omp.h>
#include <stdio.h>

#include "SimpleEssentiaExtractor.h"

ofVec3f ofApp::scrS;

static const     double clipPlanes[] = {
  1,  0,  0,  .51,
  -1, 0,  0,  .51,
  0,  1,  0,  .51,
  0,  -1, 0,  .51,
  0,  0,  1,  .51,
  0,  0,  -1, .51


};



void ofApp::execCmd(const string &cmd,const vector<string> & args){

  if(cmd == "loadDir"){
    if(args.size()==1){
      if(! FileImporter::loadAnalysisFiles(args[0],"")){
        cout << "cannot load files at : "<< args[0]  << endl;
      }
    }
    else{
      cout << "Usage : -loadDir /path/To/Be/Loaded " << endl;
    }
  }
  else if(cmd=="splice"){
    SimpleEssentiaExtractor::spliceIt = true;
  }
  else if(cmd=="forcePreloaded"){
    FileImporter::forcePreloaded = true;
  }
  else if(cmd=="forceReload"){
    FileImporter::forceReload = true;
  }
  cout <<"commandArg :"<<  cmd << " : ";
  for (auto & s : args){
    cout  << s<< ",";
  }
  cout << endl;
}

void ofApp::parseCmd(int argc, char ** argv ){
  char lastCmd[30];
  lastCmd[0] = '\0';
  struct cmdD{
    cmdD(string c,vector<string> a):cmd(c),args(a){}
    string cmd;
    vector<string> args;
  };
  vector<cmdD> cmds;
  vector<string> cmdArgs;
  for(int i = 0 ; i < argc ; i++){
    char * c = argv[i];
    bool isCmd = c[0] == '-';
    if(isCmd){
      if(lastCmd[0]!='\0'){
        cmds.push_back(cmdD(lastCmd,cmdArgs));

      }
      cmdArgs.clear();
      strcpy(lastCmd,c +1);
    }
    else if(lastCmd[0]!='\0'){
      cmdArgs.push_back(c);
    }

  }
  if(lastCmd[0]!='\0'){
    cmds.push_back(cmdD(lastCmd,cmdArgs));

  }
  for(auto & c:cmds){
    if(c.cmd=="loadDir"){
      std::swap(c, cmds.back());
    }
  }

  for(auto & c:cmds){
    execCmd(c.cmd,c.args);
  }
}

//--------------------------------------------------------------
void ofApp::setup(){


#if defined TARGET_OSX
  try{
    ofSetDataPathRoot(std::filesystem::canonical(ofFilePath::join(ofFilePath::getCurrentExeDir(),  "../../data/")).string());
  }catch(...){
    ofFilePath::join(ofFilePath::getCurrentExeDir(),  "../../../data/");
  }

#endif
  essentia::init();
  //  Eigen::setNbThreads(4);
  ofLogWarning("Eigen") << "using "<< Eigen::nbThreads( )<< " threads";

  ofSetLogLevel(OF_LOG_WARNING);
  ofSetLogLevel("GUI",OF_LOG_NOTICE);
  //    ofSetLogLevel("Container",OF_LOG_VERBOSE);
  ofSetLogLevel("FileImporter",OF_LOG_NOTICE);
  ofSetLogLevel("ofShader",OF_LOG_VERBOSE);
  //    ofSetLogLevel("Midi",OF_LOG_NOTICE);
  //    ofSetLogLevel("Audio",OF_LOG_VERBOSE);

  ofSetLogLevel("FileLoader",OF_LOG_ERROR);
  //    ofSetLogLevel("ofxUI",OF_LOG_VERBOSE);


  ofSetFrameRate(60);


  ofDisablePointSprites();
  glEnable(GL_POINT_SMOOTH);
  ofEnableBlendMode(OF_BLENDMODE_ALPHA);






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
  ofBackground(0);
  ofShowCursor();

  AudioPlayer::i();
  windowResized(ofGetWindowWidth(), ofGetWindowHeight());

  lastCamPos =cam.getPosition();

  fishEye.load("shaders/fishEye");
  tmpFolder=ofToDataPath("tmpSlices");
  if(tmpFolder.size()> 12){
    ofDirectory::removeDirectory(tmpFolder, true);

    ofDirectory::createDirectory(tmpFolder);

  }

  try{
    oscSender.setup("127.0.0.1", 8000);
    oscSenderIsConnected = true;
  }
  catch(...){
    ofLogError("ofApp","can't connect OSC");
    oscSenderIsConnected = false;
  }
  GUI::i()->setup();

  parseCmd( argc, argv );

}

//--------------------------------------------------------------
void ofApp::update(){
  if(FileImporter::i()->hasLoaded){

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
      nearestFromMouse.clear();
    }
    lastCamPos = cam.getPosition();



    fishEye.setUniform1f("BarrelPower",1);
    //        ofMatrix4x4 mat;
    //        mat = Camera::mainCam->getModelViewProjectionMatrix();
    //        fishEye.setUniformMatrix4f("modelViewProj",mat);
  }

  if(!FileImporter::i()->isThreadRunning())Container::updateContainerView();

}





void ofApp::onCompletion(){
  nearestFromMouse.clear();
}

//--------------------------------------------------------------
void ofApp::draw(){
  if(FileImporter::i()->hasLoaded){
    if(!wasLoaded){
      onCompletion();
    }

    ofBackground(GUI::i()->guiView.graphMode->getValue()?255:0);

    if(isCamSteady && GUI::i()->guiView.triangulate->getValue()){
      if(nearestFromMouse.size()>0){
        ofSetColor(255);
        ofSetLineWidth(1);
        ofVec2f ori (ofGetMouseX(),ofGetMouseY());
        for(auto c : nearestFromMouse){
          ofDrawLine(ori,c->getScreenPos());
        }

      }
      ofNoFill();
      ofSetLineWidth(1);
      ofSetColor(255,255,255,255);
      Physics::delaunay.draw();
    }
    ofFill();
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
    cam.end(!GUI::i()->guiView.graphMode->getValue());

    if(GUI::i()->guiView.miniViews->getValue()){
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
    ofPushStyle();
    ofPushMatrix();
    ofPushView();
    ofSetColor(255);
    ofVec2f center (ofGetWidth()/2, ofGetHeight()/2);
    float radius = ofGetHeight()/10;

    ofNoFill();
    ofSetLineWidth(10);
    float originA = 90;
    ofPolyline path;
    path.arc(center, radius,radius,originA,originA+FileImporter::i()->progressPct*360,360);
    path.draw();

    ofDrawBitmapString(ofToString(FileImporter::i()->progressPct * 100,1) + " %",center -ofVec2f(20,-5));
    ofPopView();
    ofPopStyle();
    ofPopMatrix();

  }


  wasLoaded = FileImporter::i()->hasLoaded;
}

void ofApp:: draw3d(){


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



  Midi::i()->draw();



}











//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  if(ofGetKeyPressed(OF_KEY_SHIFT)){
    Camera::hoveredCam->enableMouseInput();
    Camera::hoveredCam->disableMouseMiddleButton();
  }
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
      if(f.bSuccess)  { FileImporter::loadAnalysisFiles(f.filePath); }
      break;
    }
    case ' ':
      ofFmodSoundStopAll();
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


    case OF_KEY_SHIFT:
      Camera::hoveredCam->disableMouseInput();
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

  if(ofGetElapsedTimeMillis()-Casttime>100){
    if(isSelecting){

    }
    else{
      Container * cc = Physics::hoveredOnScreen( ofVec3f(x,y,0));
      bool change = Container::hoverContainer(cc == NULL?-1:cc->globalIdx);
      Casttime = ofGetElapsedTimeMillis();
      string log = "";
      sendInterpolated();
      if (change ){
        if(cc!=NULL){

          sendContainerViaOsc(cc);
          log+=cc->getFilename() + "\ntime : "+ ofToString(cc->begin,2) + " "+ofToString(cc->end,2)+"\ndimensions : ";
          for(int i = 0 ; i < 3; i++){
            log+=ofToString(cc->getAttribute(Physics::curAttributesIndex[i]),4) + " ";
          }

        }
        GUI::LogIt(log);

      }

    }
  }

}

void ofApp::sendContainerViaOsc(Container * c){
  if(c!=nullptr && oscSenderIsConnected){
    ofxOscMessage msg;
    msg.setAddress("/sample");
    msg.addStringArg(c->getFilename());
    oscSender.sendMessage(msg);
  }
}




void ofApp::sendInterpolated(){

  int numToSend = 3;
  ofVec2f mouse( ofGetMouseX(),ofGetMouseY());

  nearestFromMouse= Physics::getTriangleForPos(mouse);//Physics::nearestOnScreen(ofVec3f(mouse),maxToSearch);

  vector<float> contributions;
  float totalWeight=0;

  ofxOscMessage msg;
  msg.setAddress("/interpolated");

  std::sort(nearestFromMouse.begin(), nearestFromMouse.end(),
            [](Container * a, Container * b) {
              return b->getFilename() < a->getFilename();
            });


  for(auto cc:nearestFromMouse){
    float contribution = 1;
    ofVec2f O= cc->getScreenPos();
    ofVec2f OM = mouse - O;



    //    specific triangle interpolation if delaunay
    //    distance with opposed side
    if(nearestFromMouse.size()==3){
      ofVec2f AB[2];
      int idx = 0;
      for(auto ccc:nearestFromMouse){
        if(ccc!=cc){
          AB[idx] = ccc->getScreenPos()-O;
          idx+=1;
        }
      }
      ofVec2f norm = (AB[0] - AB[1]).getPerpendicular();
      float Odist = abs(AB[0].dot(norm));
      float Mdist = Odist - abs(OM.dot(norm));
      contribution = Mdist;

    }
    else{
      for(auto ccc:nearestFromMouse){
        if(ccc!=cc){
          ofVec2f OA = ccc->getScreenPos() - O;
          float dot =   MAX(0,1 -OA.getNormalized().dot(OM) / OA.length());

          contribution *= dot;
        }
      }
      contribution /= nearestFromMouse.size()  - 1;
    }


    contributions.push_back(contribution);
    totalWeight+= contribution;

  }

  for(int i = 0 ; i < nearestFromMouse.size() ; i ++){
    msg.addStringArg(nearestFromMouse[i]->getFilename());
    msg.addFloatArg(contributions[i]*1.0/totalWeight);
  }
  oscSender.sendMessage(msg);

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
  if(ofGetKeyPressed(OF_KEY_SHIFT) && button == 0)return;
  // select instances
  if(isSelecting){

    selectRect.width =  x -selectRect.x;
    selectRect.height=  y -selectRect.y;
    //        selectRect.standardize();

  }
  //    else if(ofGetElapsedTimeMillis()-Casttime>10){
  //
  //            Container * cc = Physics::hoveredOnScreen( ofVec3f(x,y,0));
  //            bool change = Container::hoverContainer(cc == NULL?-1:cc->globalIdx);
  //            Casttime = ofGetElapsedTimeMillis();
  //            string log = "";
  //            sendInterpolated();
  //            if (change ){
  //                if(cc!=NULL){
  //
  //                    sendContainerViaOsc(cc);
  //                    log+=cc->getFilename() + "\ntime : "+ ofToString(cc->begin,2) + " "+ofToString(cc->end,2)+"\ndimensions : ";
  //                    for(int i = 0 ; i < 3; i++){
  //                        log+=ofToString(cc->getAttribute(Physics::curAttributesIndex[i]),4) + " ";
  //                    }
  //
  //                }
  //                GUI::LogIt(log);
  //
  //            }
  //
  //
  //    }
  //

  // drag instances
  else if(button==2){
    if(Physics::updateDrag(ofVec2f(x,y))){

    }

  }

  // play instances
  else if (button ==0 && GUI::i()->guiPlayBack.continuousPB->getValue() && !GUI::i()->isOver(x,y) ){
    if(ofGetElapsedTimeMillis()-Casttime>70){
      Container * cc = Physics::nearestOnScreen( ofVec3f(x,y,0));
      int oldIdx = Container::hoverIdx;
      bool change = Container::hoverContainer(cc == NULL?-1:cc->globalIdx);
      Casttime = ofGetElapsedTimeMillis();
      if (change){
        GUI::LogIt(cc == NULL?"":cc->getFilename() +"\n"+ ofToString((cc->getPos()*(Physics::maxs.get()-Physics::mins)+Physics::mins)));
        if(cc!=NULL)cc->setState(1);
        if(oldIdx>=0 && !GUI::i()->guiPlayBack.holdPB->getValue())Container::containers[oldIdx]->setState(0);
      }
    }
  }



}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
  // dont want to triggerAction if overGUI
  if(GUI::i()->isOver(x,y))return;
  // or if we are changing camera
  if(ofGetKeyPressed(OF_KEY_SHIFT) && button == 0)return;
  cout << ofGetKeyPressed('d') << endl;
  // start selecting multiple
  if(ofGetKeyPressed(OF_KEY_SHIFT) && button == 2){
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
  if(button == 0 && cc){
    cc->setState(1);//cc->state==0?1:0;
    return;
  }


  // inside selection zone
  if (selectRect.inside(x,y)){

    // drag
    if(button == 2){

      // will drag multiple


      Physics::originDrag.clear();

      for(int i = 0 ; i < Physics::dragged.size() ; i++){
        int curI = Physics::dragged[i]->globalIdx;
        Physics::originDrag.push_back(cam.worldToScreen(Physics::vs[curI]) - ofVec2f(x,y));
      }
      selectRect.set(x,y,0,0);

    }
  }





  else{
    selectRect.set(x,y,0,0);
    Physics::dragged.clear();

    // one selected
    if(cc){

      Physics::originDrag.clear();


      Physics::dragged.push_back(cc);
      ofVec3f screenD = cam.worldToScreen(Physics::vs[cc->globalIdx])-ofVec3f(x,y);
      Physics::originDrag.push_back(screenD);

    }


  }


}



void ofApp::mouseExited( int x, int y){
  if( Physics::dragged.size()>0 ){
    for (auto & c:Physics::dragged){
      Physics::vs[c->globalIdx] = cam.screenToWorld(Physics::originDrag[(int)(&c - &Physics::dragged[0])]);
    }

    Container * cc = Physics::dragged[0];
    vector<string > paths;
    string firstPath = cc->getAudioPath();
    if(firstPath!=""){
      paths.push_back(firstPath);
    vector<float> starts(1,cc->begin);
    vector<float> ends(1,cc->end);

    DragOut::i()->performExternalDragDrop(paths,tmpFolder,starts,ends,ofGetCocoaWindow(),ofGetMouseX(),ofGetWindowHeight() - ofGetMouseY());
    }
    Physics::dragged.clear();
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

  }
  else{
    Physics::dragged.clear();
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
    ofLogNotice("App") << "dragged : " << dragInfo.files[0];
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






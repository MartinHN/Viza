#pragma once


#include "Physics.h"
#include "FileImporter.h"
#include "GUI.h"
#include "Midi.h"

//#include "SliceFitter.h"
#include "Camera.h"


#include "DragOut.h"


#include "ofMain.h"
#include "ofxOsc.h"

class ofApp : public ofBaseApp{
    
public:
  ofApp(int _argc, char ** _argv ):argc(_argc),argv(_argv){}
  int argc;
  char**argv;
    Camera  cam;
    
    vector<Camera*> cam2ds;
    
    ofVec3f lastCamPos;
    bool isCamSteady;
    float lastCamUpdate;
    unsigned long long Casttime;
    
    
    static ofVec3f scrS;
    
    bool drawGUI;
    
    
    
    
    void setup();
    void update();
    void draw();
  void parseCmd(int argc, char ** argv );
  void execCmd(string cmd,vector<string> args);
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseExited( int x, int y);

    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void isGUIing(bool & t);
    void exit();
    
    

    
    
    void draw3d();
    

    string tmpFolder;
    
    bool isSelecting;
  bool wasLoaded;
  void onCompletion();
    ofRectangle selectRect;
    ofShader fishEye;
    
    ofxOscSender oscSender;
    bool oscSenderIsConnected;
    void sendContainerViaOsc(Container * c);
    void sendInterpolated();
    vector<Container*> nearestFromMouse;

};

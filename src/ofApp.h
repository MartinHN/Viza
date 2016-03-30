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
    ofRectangle selectRect;
    ofShader fishEye;
    
    ofxOscSender oscSender;
    bool oscSenderIsConnected;
    void sendContainerViaOsc(Container * c);
    void sendInterpolated();
    vector<Container*> nearestFromMouse;

};

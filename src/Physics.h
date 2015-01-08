//
//  Physics.h
//  ViZa
//
//  Created by martin hermant on 09/10/14.
//
//

#ifndef __ViZa__Physics__
#define __ViZa__Physics__


#include "ofMain.h"
#include "Container.h"
#include "ofApp.h"
#include "ofxNearestNeighbour.h"

#include <iostream>

//#define COMPLEX_DESCRIPTOR_TEST
#define COMPLEX_DESCRIPTOR_TEST_NOISE .9

class Container;

class Physics{
    
public:
   
    
    static vector<ofVec3f> vs;
    static vector<ofVec3f> vScreen;
    static vector<ofFloatColor> cols;
    static vector<unsigned int> idxs;
    
    static vector<ofVec3f> * fits;
    static ofVbo vbo;
    static ofVbo fitsVbo;
    
    static ofParameter<ofVec3f> mins;
    static ofParameter<ofVec3f> maxs;
    
    static void updateVBO();
    static void resizeVBO();
    static void freeVbo();
    static void updateAllColors();
    static void updateOneColor(int idx,ofColor col);
    static void updateOnePos(int idx,ofVec3f & pos);
    static void orderBy(string attr,int axe,int type);
    static void updateVScreen();
    
    static Container * NearestCam( ofVec3f mouse,float sphereMult=1,bool brightest=false);
    static Container* Nearest(ofVec3f point,float radius = 0 );
    static float distanceVanish(ofCamera cam);
    
    static Container* dragged;
    static float originDrag;
    static void draw();
    vector<float> ks;
    
    static void setSelected(int s,int end);
    static int startLines;
    static int amountLines;
    static bool linksongs;

    
    
//    
//    ofFbo pos;
//    ofFbo vel;
    
    
    string curAttribute;
    
    Physics * instance(){if(inst == NULL){inst = this;}    }
    
    void buildNetwork();
    void updatePhy(float time);
    static bool updateDrag(ofVec2f mouse);
    static void setFits(vector<ofVec3f> & fi);
    
private:
     static Physics* inst;
    
    static ofxNearestNeighbour3D kNN;
    static ofxNearestNeighbour3D kNNCam;
    
};
#endif /* defined(__ViZa__Physics__) */

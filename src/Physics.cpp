//
//  Physics.cpp
//  ViZa
//
//  Created by martin hermant on 09/10/14.
//
//

#include "Physics.h"



vector<ofVec3f> Physics::vs;
vector<ofVec3f> Physics::vScreen;
vector<ofFloatColor> Physics::cols;
vector<unsigned int> Physics::idxs;
ofVbo Physics::vbo;
int Physics::startLines;
int Physics::amountLines;

ofParameter<ofVec3f> Physics::maxs = ofVec3f(1,1,1);
ofParameter<ofVec3f> Physics::mins = ofVec3f(0,0,0);
Container* Physics::dragged;
float Physics::originDrag;
bool Physics::linksongs = false;
ofxNearestNeighbour3D Physics::kNN;
ofxNearestNeighbour3D Physics::kNNCam;

void updatePhy(float time){
    
    
}

void buildNetwork(){
    
}


void Physics::draw(){
    ofPushMatrix();
    float ratio = 1;//ofGetScreenHeight()*1.0/ofGetScreenWidth();
//    if(ofApp::cam.getOrtho())ofScale(1.0/ofApp::cam.getDistance(),1.0/ofApp::cam.getDistance(),1.0/ofApp::cam.getDistance());
    ofDisableDepthTest();
    vbo.drawElements(GL_POINTS,Physics::vbo.getNumVertices());
    if(linksongs&&amountLines>0){
        vbo.draw(GL_LINE_STRIP, startLines, amountLines);
    }
    ofPopMatrix();
}






Container * Physics::NearestCam( ofVec3f mouse , float sphereMult,bool brightest){

    ofEasyCam cam = ofApp::cam;
    vector<size_t> resI;
    vector<float>  resD;
    float radmult = sphereMult*cam.getDistance()*Container::radius* 1.0/((cam.getOrtho()?60.0:1)*distanceVanish(cam));
    
    kNNCam.findNClosestPoints(mouse, 1, resI,resD);
    
    if(resI.size()>0){
        return &Container::containers[resI[0]];
    }
    else{
        return NULL;
    }
    

    
}


Container * Physics::Nearest(ofVec3f point,float radius ){
    
    vector<pair<size_t, float> > res;
    
    kNN.findPointsWithinRadius(point, radius, res);
    
    if(res.size()>0){
        return &Container::containers[res[0].first];
    }
    else{
        return NULL;
    }
    
}




void Physics::orderBy(string _attr,int axe,int type){
    bool found = false;
    string attr = _attr;
    for (vector<string> ::iterator it = Container::attributeNames.begin(); it!=Container::attributeNames.end(); ++it) {
        if(*it==attr){
            found = true;
            break;
        }
    }
    if(!found){
        attr = ofSplitString(_attr, ".")[0];
    }
    float max = Container::maxs[attr];
    float min = Container::mins[attr];
    float mean = Container::means[attr];
    int idxAttr = ofFind(Container::attributeNames, attr);
    if(type==2){
        max = Physics::maxs.get()[axe];
        min = Physics::mins.get()[axe];
    }
    else if(type==1){
        ofVec2f stddev(0);
        ofVec2f stdlength(0);

        for(vector<Container>::iterator it = Container::containers.begin() ; it!=Container::containers.end();++it){
            float delta = it->attributes[idxAttr]-mean;
            stddev[delta>0?1:0]+= delta*delta;
            stdlength[delta>0?1:0]++;
        }
        
        stddev/=stdlength;
        
        min = mean - sqrt(stddev.x);
        max = mean + sqrt(stddev.y);

    }
 
    
    
    for(vector<Container>::iterator it = Container::containers.begin() ; it!=Container::containers.end();++it){
               vs[it->index][axe] = (it->attributes[idxAttr]-min)/(max-min)-.5;
    }
    
    ofVec3f mask(axe==0?1:0,axe==1?1:0,axe==2?1:0);
    maxs = max*mask + (-mask+ofVec3f(1))*maxs;
    mins = min*mask + (-mask+ofVec3f(1))*mins;
    Physics::updateVBO();
    Physics::updateVScreen();
}



void Physics::resizeVBO(){
    int newSize = Container::containers.size();
    if(vs.size() !=newSize){
        vs .resize(newSize);
        cols.resize(newSize);
        idxs.resize(newSize);
        
        vbo.setVertexData(&vs[0], newSize, GL_DYNAMIC_DRAW);
        vbo.setIndexData(&idxs[0], newSize, GL_DYNAMIC_DRAW);
        vbo.setColorData(&cols[0], newSize, GL_DYNAMIC_DRAW);
         Container::registerListener();
    }
}

void Physics::updateVBO(){



    int curSize = vs.size();
#pragma omp parallel for
    for(int i = 0 ; i < curSize ; i++){
//        vs[i] = Container::containers[i].pos - ofVec3f(.5);
        cols[i]= Container::containers[i].getColor();
        idxs[i] = Container::containers[i].index;
    }

    vbo.updateVertexData(&vs[0], curSize);
    vbo.updateIndexData(&idxs[0], curSize);
    vbo.updateColorData(&cols[0], curSize);
    
   
    if(curSize>0){
    kNN.buildIndex(vs);
    kNNCam.buildIndex(vScreen);
    }
}


void Physics::updateVScreen(){
    vScreen.resize(vs.size());
    int i = 0;
    ofCamera cam = ofApp::cam;
    for(vector<ofVec3f>::iterator it = vs.begin() ; it!=vs.end();++it){
        vScreen[i] = cam.worldToScreen(*it);
                i++;
    }
    kNNCam.buildIndex(vScreen);
}




void Physics::updateOneColor(int idx,ofColor col){

    ofFloatColor* c = new ofFloatColor[1];
    c[0] = (ofFloatColor)col;
    vbo.updateOneColorData(c,idx);
    Physics::cols[idx] = col;
    
    delete [] c;
}

void Physics::updateAllColors(){
for(vector<Container>::iterator it = Container::containers.begin() ; it != Container::containers.end();++it){

    Physics::cols[it->index] = Container::stateColor[it->isSelected?2:it->isHovered?3:(int)it->state];
        }
        vbo.updateColorData(&cols[0],Container::containers.size());
}

void Physics::updateOnePos(int idx,ofVec3f & pos){
    
    
    vbo.updateOneVertexData(&pos, idx);
    Physics::vs[idx] = pos;
    
}

float Physics::distanceVanish(ofCamera cam){
    if(cam.getOrtho())return 1.0;
    else return 2.0f/tan(ofDegToRad(cam.getFov()/2.0f));
}


bool Physics::updateDrag(ofVec2f mouse){
    if(dragged!=NULL){
        ofVec3f v =ofApp::cam.screenToWorld(ofVec3f(mouse.x,mouse.y,originDrag));
        
     updateOnePos(dragged->index,v);
        return true;
    }
    return false;
}


void Physics::setSelected(int s, int end){
    startLines = s;
    amountLines = end-s;
}


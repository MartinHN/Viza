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

vector<ofVec3f> * Physics::fits = NULL;
ofVbo Physics::fitsVbo;

ofParameter<ofVec3f> Physics::maxs = ofVec3f(1,1,1);
ofParameter<ofVec3f> Physics::mins = ofVec3f(0,0,0);
vector<Container*> Physics::dragged;
vector<ofVec3f> Physics::originDrag;
bool Physics::linksongs = false;
ofxNearestNeighbour3D Physics::kNN;
ofxNearestNeighbour3D Physics::kNNScreen;

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
    if(fits!=NULL){
        
        fitsVbo.drawElements(GL_POINTS, fitsVbo.getNumVertices());
    }
    
    ofPopMatrix();
}






Container * Physics::nearestOnScreen( ofVec3f mouse ){
    
    ofEasyCam cam = ofApp::cam;
    vector<size_t> resI;
    vector<float>  resD;
    
    kNNScreen.findNClosestPoints(mouse, 1, resI,resD);
    
    if(resI.size()>0 ){
        return &Container::containers[resI[0]];
    }
    else{
        return NULL;
    }
    
    
    
}


Container * Physics::hoveredOnScreen( ofVec3f mouse , float addRadius){
    
    ofEasyCam cam = ofApp::cam;
    vector<size_t> resI;
    vector<float>  resD;
    // equation corresponding to GL_POINTS Radius Computation
    float radmult = cam.getDistance()*Container::radius* 1.0/((cam.getOrtho()?60.0:1)*distanceVanish(cam));
    
    kNNScreen.findNClosestPoints(mouse, 1, resI,resD);
    
    if(resI.size()>0){
        
        if ( (vScreen[resI[0]]-mouse).length()<radmult + addRadius ){
            return &Container::containers[resI[0]];
        }
    }
    return NULL;
    
    
    
    
}

vector<Container *> Physics::containedInRect( ofRectangle rect){
    ofVec2f center = rect.getCenter();
    float radius = (rect.getTopLeft() - center).length();
    ofEasyCam cam = ofApp::cam;
    vector<pair<size_t, float> > matches;
    
    kNNScreen.findPointsWithinRadius(center, radius, matches);
    
    vector<Container*> res;
    for(int i = 0 ; i < matches.size() ; i++){
        
        ofVec2f screenLoc = Physics::vScreen[(int)matches[i].first];
        if(rect.inside(screenLoc)){
            res.push_back(&Container::containers[(int)matches[i].first]);
        }
    }
    
    return res;
    
}


Container * Physics::nearest(ofVec3f point,float radius ){
    
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
    
    int idxAttr = ofFind(Container::attributeNames, attr);
    float max = Container::maxs[idxAttr];
    float min = Container::mins[idxAttr];
    float mean = Container::means[idxAttr];
    float stddev = Container::stddevs[idxAttr];
    
    // get user defined range
    if(type==2){
        max = Physics::maxs.get()[axe];
        min = Physics::mins.get()[axe];
    }
    
    // get standard dev value
    else if(type==1){
        min = mean - stddev/2;
        max = mean + stddev/2;
        
    }
    
    
    bool printout = true;
    for(vector<Container>::iterator it = Container::containers.begin() ; it!=Container::containers.end();++it){
        
#ifdef COMPLEX_DESCRIPTOR_TEST
        float value = 0;
        float totalW = 0;
        for (int i = 0;i< 6;i++){
            int curAttri = (idxAttr+4*i)%Container::attributeNames.size();
            float w =(i+1)/8.0;
            value+= w* ( (1-COMPLEX_DESCRIPTOR_TEST_NOISE)*(it->getAttributes(curAttri,true)) + COMPLEX_DESCRIPTOR_TEST_NOISE*ofRandom(-.5,.5));//-Container::mins[curAttri])/(Container::maxs[curAttri]-Container::mins[curAttri])-.5 );
            totalW+=w;
            if(printout)cout << Container::attributeNames[curAttri] << " : " << w << " / ";
        }
        if(printout)cout << endl;
        printout = false;
        vs[it->index][axe] = value/totalW;
        
#else
        vs[it->index][axe] =  min!=max?(it->getAttributes(idxAttr)-min)/(max-min)-.5:0;
#endif
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
        kNNScreen.buildIndex(vScreen);
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
    kNNScreen.buildIndex(vScreen);
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
    if(dragged.size()>0){
        
        for(int i = 0 ; i < originDrag.size() ; i++ ){
            ofVec3f v =ofApp::cam.screenToWorld(ofVec3f(mouse.x,mouse.y,0)+originDrag[i]);
            
            updateOnePos(dragged[i]->index,v);
        }
        return true;
    }
    return false;
}


void Physics::setSelected(int s, int end){
    startLines = s;
    amountLines = end-s;
}

void Physics::setFits(vector<ofVec3f> & fi){
    
    fits = &fi;
    
    
    if( fits!=NULL)
    {
        //init Vbo
        if(fitsVbo.getNumVertices()==0){
            fitsVbo.setVertexData(&fi[0], fits->size(), GL_DYNAMIC_DRAW);
            fitsVbo.setIndexData(&idxs[0], fits->size(), GL_DYNAMIC_DRAW);
        }
        // update
        else{
            fitsVbo.updateVertexData(&fi[0], fits->size());
        }
    }
    else{
        fitsVbo.clearVertices();
    }
    
    
    
    
    
}
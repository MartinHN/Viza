//
//  Physics.cpp
//  ViZa
//
//  Created by martin hermant on 09/10/14.
//
//

#include "Physics.h"



vector<ofVec3f> Physics::vs;
vector<ofVec2f> Physics::vScreen;
vector<ofFloatColor> Physics::cols;
vector<unsigned int> Physics::idxs;
ofVec3f Physics::curAttributesIndex;
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
bool Physics::drawFits = true;
ofxNearestNeighbour3D Physics::kNN;
ofxNearestNeighbour2D Physics::kNNScreen;

void updatePhy(float time){
    
    
}

void buildNetwork(){
    
}


void Physics::draw(){
    ofPushMatrix();
    ofDisableDepthTest();

    vbo.drawElements(GL_POINTS,Physics::vbo.getNumVertices());
    if(linksongs&&amountLines>0){
        vbo.draw(GL_LINE_STRIP, startLines, amountLines);
    }
    if(fits!=NULL &&drawFits){
        ofSetColor(0,0,255,100);
        fitsVbo.drawElements(GL_POINTS, fitsVbo.getNumVertices());
    }
    
    ofPopMatrix();
}






Container * Physics::nearestOnScreen( ofVec3f mouse ){
    
    
    vector<size_t> resI;
    vector<float>  resD;
    Container * res = NULL;
    if(vScreen.size()>0){
        kNNScreen.findNClosestPoints(mouse, 1, resI,resD);
        
        for(int i=0 ; i<resI.size() ; i++){
            
            res = Container::containers[resI[i]] ;
            break;
            
        }
    }
    return res;
    
    
    
    
}


Container * Physics::hoveredOnScreen( ofVec2f mouse , float addRadius){
    
    Camera * cam = Camera::getActiveCam();
    vector<size_t> resI;
    vector<float>  resD;
    Container * res = NULL;
    int numToSearch = 10;
    kNNScreen.findNClosestPoints(mouse, numToSearch, resI,resD);
    
    
    for(int j = 0 ; j< resI.size() ; j++){
        // equation corresponding to GL_POINTS Radius Computation
        float radmult ;
        if(cam->getOrtho()){
            float vertDist = (vs[resI[j]] *cam->getModelViewMatrix() ).length();

            radmult = Container::radius* .50/sqrt((ofVec3f(1,vertDist,pow(vertDist,2)).dot(cam->distanceVanish())));

        }
        else{
            radmult = cam->getDistance()*Container::radius* 1.0/cam->distanceVanish().y;
        }
        
        
        
        if( (vScreen[resI[j]]-mouse).length()<radmult+ addRadius    ){
            res = Container::containers[resI[j]] ;
            break;
        }
        
        
        
    }
    
    return res;
    
}

vector<Container *> Physics::containedInRect( ofRectangle rect){
    ofVec2f center = rect.getCenter();
    float radius = (rect.getTopLeft() - center).length();
    vector<pair<size_t, float> > matches;
    
    kNNScreen.findPointsWithinRadius(center, radius, matches);
    
    vector<Container*> res;
    for(int i = 0 ; i < matches.size() ; i++){
        
        ofVec2f screenLoc = Physics::vScreen[(int)matches[i].first];
        if(rect.inside(screenLoc)){
            res.push_back(Container::containers[(int)matches[i].first]);
        }
    }
    
    return res;
    
}


Container * Physics::nearest(ofVec3f point,float radius ){
    
    vector<pair<size_t, float> > res;
    
    kNN.findPointsWithinRadius(point, radius, res);
    
    if(res.size()>0){
        return Container::containers[res[0].first];
    }
    else{
        return NULL;
    }
    
}




void Physics::orderByAttributes(string _attr,int axe,int type){
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
    curAttributesIndex[axe] = idxAttr;
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
    
    ofVec3f mask(axe==0?1:0,axe==1?1:0,axe==2?1:0);
    maxs = max*mask + (-mask+ofVec3f(1))*maxs;
    mins = min*mask + (-mask+ofVec3f(1))*mins;
    
    bool printout = true;
    int coordType = GUI::instance()->coordinateType->getSelectedIndeces()[0];
    bool clampIt = GUI::instance()->clampValues->getValue();
    for(vector<Container*>::iterator it = Container::containers.begin() ; it!=Container::containers.end();++it){
        
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
        if(coordType == 0){
            vs[(*it)->index][axe] =  min!=max?((*it)->getAttributes(idxAttr)-min)/(max-min)-.5:0;
        }
        
        // need to get all 3 coords to compute new value
        else{
            ofVec3f sph;
            for(int iii = 0 ; iii < 3; iii++){
                sph[iii] = Physics::mins.get()[iii]!=Physics::maxs.get()[iii]? ofMap((*it)->getAttributes(curAttributesIndex[iii]),Physics::mins.get()[iii],Physics::maxs.get()[iii],0,1) : .5;
            }
            
            if(clampIt){
                for(int j = 0;j< 3;j++){
                    if(sph[j]>1 || sph[j]<0){
                        sph[j]=30;
                        
                    }
                }
            }
            
            
            switch(coordType){
                    
                    // cilindrical
                case 1:{
                    vs[(*it)->index].set(sph.x*.5,0,sph.y-.5);
                    vs[(*it)->index].rotate(0,0,ofMap(sph.z,0,1,0,360));
                    
                }
                    break;
                    // Toroidal
                case 2:{
                    
                    vs[(*it)->index].set((sph.x*.25+.25),0,0);
                    
                    vs[(*it)->index].rotate(ofMap(sph.z,0,1,0,360), ofVec3f(.25,0,0),ofVec3f(0,0,1));
                    vs[(*it)->index].rotate(0,ofMap(sph.y,0,1,0,360),0);
                    
                }
                    break;
                    // spherical
                case 3:{
                    vs[(*it)->index].set(sph.x*.5,0,0);
                    vs[(*it)->index].rotate(0,0,ofMap(sph.z,0,1,-90,90));
                    vs[(*it)->index].rotate(0,ofMap(sph.y,0,1,-180,180),0);
                }
                    break;
                    
            }
        }
#endif
    }
    
    
    
    Physics::updateVBO();
    Physics::updateVScreen();
}

void Physics::orderByClass(string className,int axe){
    bool found = false;

   int numClassMember =  Container::classeMap[className].size() ;
    float max = numClassMember;
    float min =0;
    int cIdx = 0;
    for (map<string,vector<int> > ::iterator it = Container::classeMap[className].begin(); it!=Container::classeMap[className].end(); ++it) {
        float pos = cIdx/numClassMember -.5;
        
        for(int i = 0 ; i< it->second.size() ; i++){
            vs[it->second[i]][axe] =  pos;
        }

        cIdx++;
    }

    
    ofVec3f mask(axe==0?1:0,axe==1?1:0,axe==2?1:0);
    maxs = max*mask + (-mask+ofVec3f(1))*maxs;
    mins = min*mask + (-mask+ofVec3f(1))*mins;

    Physics::mins = mins;
    Physics::maxs = maxs;
    
    
    Physics::updateVBO();
    Physics::updateVScreen();

    
    
    
}



void Physics::resizeVBO(){
    int newSize = Container::containers.size();
    if(vs.size() !=newSize){
        vs .resize(newSize);
        cols.resize(newSize);
        idxs.resize(newSize);
        
        vbo.setVertexData(&vs[0]  , newSize, GL_DYNAMIC_DRAW);
        vbo.setIndexData (&idxs[0], newSize, GL_DYNAMIC_DRAW);
        vbo.setColorData (&cols[0], newSize, GL_DYNAMIC_DRAW);
        Container::registerListener();
    }
}

void Physics::updateVBO(){
    
    
    
    int curSize = vs.size();
    
    for(int i = 0 ; i < curSize ; i++){
        //        vs[i] = Container::containers[i].pos - ofVec3f(.5);
        cols[i]= Container::containers[i]->getColor();
        idxs[i] = Container::containers[i]->index;
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
    Camera* cam = Camera::getActiveCam();
    cout << cam << endl;
    for(vector<ofVec3f>::iterator it = vs.begin() ; it!=vs.end();++it){
        if((GUI::instance()->alphaView->getValue() !=0 || Container::containers[i]->isSelected) && cam->isPointVisible(*it)){
         vScreen[i] = cam->worldToScreen(*it,cam->viewport);
            
        }
        else vScreen[i] = ofVec2f(-1,-1);
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
    for(vector<Container*>::iterator it = Container::containers.begin() ; it != Container::containers.end();++it){
        
        Physics::cols[(*it)->index] = Container::stateColor[(*it)->isSelected?2:(*it)->isHovered?3:(int)(*it)->state];
    }
    vbo.updateColorData(&cols[0],Container::containers.size());
}

void Physics::updateOnePos(int idx,ofVec3f & pos){
    
    
    vbo.updateOneVertexData(&pos, idx);
    Physics::vs[idx] = pos;
    
}




bool Physics::updateDrag(ofVec2f mouse){
    if(dragged.size()>0){
        
        for(int i = 0 ; i < originDrag.size() ; i++ ){
            ofVec3f v =Camera::getActiveCam()->screenToWorld(ofVec3f(mouse.x,mouse.y,0)+originDrag[i]);
            
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
//
//  Physics.cpp
//  ViZa
//
//  Created by martin hermant on 09/10/14.
//
//

#include "Physics.h"
#include "Camera.h"
#include "GUI.h"


vector<ofVec3f> Physics::vs;
vector<ofVec2f> Physics::vScreen;
vector<ofFloatColor> Physics::cols;
vector<unsigned int> Physics::idxs;
ofVec3f Physics::curAttributesIndex;
ofVbo Physics::vbo;

vector<unsigned int> * Physics::selectedIdx;
ofVec3f * Physics::fits = NULL;
unsigned int Physics::fitsSize = 0;;
ofVbo Physics::fitsVbo;

ofParameter<ofVec3f> Physics::maxs = ofVec3f(1,1,1);
ofParameter<ofVec3f> Physics::mins = ofVec3f(0,0,0);
vector<Container*> Physics::dragged;
vector<ofVec3f> Physics::originDrag;
bool Physics::linkClasses = false;
bool Physics::drawFits = true;
ofxNearestNeighbour3D Physics::kNN;
ofxNearestNeighbour2D Physics::kNNScreen;

ofMutex Physics::staticMutex;

ofxDelaunay Physics::delaunay;
int Physics::maxToDelaunaySearch = 300;


void updatePhy(float time){


}

void buildNetwork(){

}


void Physics::draw(){
  //    ofPushMatrix();
  ofDisableDepthTest();
  //    glEnable(GL_POINT_SMOOTH);

  //    ofDisablePointSprites();


  //    vbo.disableNormals();
  //    vbo.disableTexCoords();

  vbo.drawElements(GL_POINTS, Physics::vbo.getNumVertices());



  if(linkClasses && selectedIdx!= NULL){

    ofSetLineWidth(1);

    vbo.setIndexData (&selectedIdx->at(0), selectedIdx->size(), GL_DYNAMIC_DRAW);

    vbo.drawElements(GL_LINE_STRIP,selectedIdx->size());


  }
  if(fits!=NULL &&drawFits){
    ofSetColor(0,113,251,180);
    fitsVbo.drawElements(GL_POINTS, fitsVbo.getNumVertices());
  }

  //    ofPopMatrix();
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

vector<Container * > Physics::nearestOnScreen( ofVec3f mouse ,int num){


  vector<size_t> resI;
  vector<float>  resD;
  vector<Container *> res ;
  if(vScreen.size()>0){
    kNNScreen.findNClosestPoints(mouse, num, resI,resD);

    for(int i=0 ; i<resI.size() ; i++){

      res.push_back( Container::containers[resI[i]]) ;


    }
  }
  return res;




}

void Physics::clearAll(){

  kNN.clear();
  kNNScreen.clear();

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
  matches.reserve(50);
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
  vector<size_t> indices;
  vector<float> dists;
  res.reserve(50);
  kNN.findNClosestPoints(point, 1, indices , dists);
  if(indices.size()>0 && dists[0]<radius){

    return Container::containers[indices[0]];
  }
  else{
    return NULL;
  }

}

Container * Physics::nearestVisible(ofVec3f point,float radius ){

  vector<pair<size_t,float> > matches;
  kNN.findPointsWithinRadius(point,(radius), matches);
  for (auto & m:matches){
    if(Physics::cols[m.first].a>0.2){
      return Container::containers[m.first];
    }
  }



  return NULL;




}





void Physics::orderByAttributes(string _attr,int axe,int type){

  ofScopedLock sl(staticMutex);
  bool found = false;
  string attr = _attr;


  for (vector<string> ::iterator it = Container::attributeNames.begin(); it!=Container::attributeNames.end(); ++it) {
    if(*it==attr){
      found = true;
      break;
    }

  }
  if(!found){attr = ofSplitString(_attr, ".")[0];}

  int idxAttr = ofFind(Container::attributeNames, attr);
  if(idxAttr>=Container::attributeNames.size()){ofLogError("Physics") << "can't find attribute "+ attr;return;}

  curAttributesIndex[axe] = idxAttr;
  float max = Container::maxs[idxAttr];
  float min = Container::mins[idxAttr];
  float mean = Container::means[idxAttr];
  float stddev = Container::stddevs[idxAttr];

  // get user defined range
  if(type==2){
    max = maxs.get()[axe];
    min = mins.get()[axe];
  }

  // get standard dev value
  else if(type==1){
    min = mean - stddev;
    max = mean + stddev;

  }

  ofVec3f tmp = maxs;
  tmp[axe] = max;
  maxs = tmp;
  tmp = mins;
  tmp[axe] = min;
  mins = tmp;


  bool printout = true;
  int coordType = GUI::i()->guiAxe.coordinateType->getSelectedIndeces()[0];
  bool clampIt = GUI::i()->guiAxe.clampValues->getValue();
  for(vector<Container*>::iterator it = Container::containers.begin() ; it!=Container::containers.end();++it){

#ifdef COMPLEX_DESCRIPTOR_TEST
    float value = 0;
    float totalW = 0;
    for (int i = 0;i< 6;i++){
      int curAttri = (idxAttr+4*i)%Container::attributeNames.size();
      float w =(i+1)/8.0;
      value+= w* ( (1-COMPLEX_DESCRIPTOR_TEST_NOISE)*(it->getAttributes(curAttri,true)) + COMPLEX_DESCRIPTOR_TEST_NOISE*ofRandom(-.5,.5));//-Container::mins[curAttri])/(Container::maxs[curAttri]-Container::mins[curAttri])-.5 );
      totalW+=w;
      //            if(printout)cout << Container::attributeNames[curAttri] << " : " << w << " / ";
    }
    //        if(printout)cout << endl;
    printout = false;
    vs[it->globalIdx][axe] = value/totalW;

#else
    if(coordType == 0){
      vs[(*it)->globalIdx][axe] =  min!=max?((*it)->getAttribute(idxAttr)-min)/(max-min)-.5:0;
    }

    // need to get all 3 coords to compute new value
    else{
      ofVec3f sph;
      for(int iii = 0 ; iii < 3; iii++){
        sph[iii] = Physics::mins.get()[iii]!=Physics::maxs.get()[iii]? ofMap((*it)->getAttribute(curAttributesIndex[iii]),Physics::mins.get()[iii],Physics::maxs.get()[iii],0,1) : .5;
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
          vs[(*it)->globalIdx].set(sph.x*.5,0,sph.y-.5);
          vs[(*it)->globalIdx].rotate(0,0,ofMap(sph.z,0,1,0,360));

        }
          break;
          // Toroidal
        case 2:{

          vs[(*it)->globalIdx].set((sph.x*.25+.25),0,0);
          vs[(*it)->globalIdx].rotate(ofMap(sph.z,0,1,0,360), ofVec3f(.25,0,0),ofVec3f(0,0,1));
          vs[(*it)->globalIdx].rotate(0,ofMap(sph.y,0,1,0,360),0);

        }
          break;
          // spherical
        case 3:{
          vs[(*it)->globalIdx].set(sph.x*.5,0,0);
          vs[(*it)->globalIdx].rotate(0,0,ofMap(sph.z,0,1,-90,90));
          vs[(*it)->globalIdx].rotate(0,ofMap(sph.y,0,1,-180,180),0);
        }
          break;

      }
    }
#endif
  }



  updateVBO();
  updateVScreen();
}

void Physics::orderByClass(string className,int axe){
  bool found = false;

  int numClassMember =  Container::classeMap[className].size() ;
  float max = numClassMember-1;
  float min =0;
  int cIdx = 0;
  for (map<string,vector<unsigned int> > ::iterator it = Container::classeMap[className].begin(); it!=Container::classeMap[className].end(); ++it) {
    float pos = cIdx*1.0/(max) -.5;

    for(int i = 0 ; i< it->second.size() ; i++){
      vs[it->second[i]][axe] =  pos;
    }

    cIdx++;
  }


  ofVec3f tmp = maxs;
  tmp[axe] = max;
  maxs = tmp;
  tmp = mins;
  tmp[axe] = min;
  mins = tmp;




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
    //        cols[i]= Container::containers[i]->getColor();
    idxs[i] = Container::containers[i]->globalIdx;
  }

  vbo.updateVertexData(&vs[0], curSize);
  vbo.updateIndexData(&idxs[0], curSize);
  vbo.updateColorData(&cols[0], curSize);


  if(curSize>0){
    kNN.buildIndex(vs);
    kNNScreen.buildIndex(vScreen);
  }
}


vector<Container * > Physics::getTriangleForPos(ofVec2f &mouse){
//  retriangulate neighboring parts in bigdatasets
if(vScreen.size()>maxToDelaunaySearch){
  vector< Container * > neigh = nearestOnScreen(mouse,maxToDelaunaySearch);
  vector<int > ind (neigh.size());
  for(int i = 0; i < neigh.size() ; i ++){
    ind[i]  = neigh[i]->globalIdx;
  }
  delaunay.triangulate(&ind);
}
  ITRIANGLE t = delaunay.getTriangleForPos(mouse);
  vector<Container * > res;
  int size = Container::containers.size();
  if(t.p1>=0&& t.p1< size && t.p2>=0&& t.p2< size && t.p3>=0 && t.p3< size&& t.p1*t.p2*t.p3>0 ){
    res.push_back(Container::containers[t.p1]);
    res.push_back(Container::containers[t.p2]);
    res.push_back(Container::containers[t.p3]);
  }
  return res;
}


void Physics::updateVScreen(){
  vScreen.resize(vs.size());
  int i = 0;
  Camera* cam = Camera::getActiveCam();

  for(vector<ofVec3f>::iterator it = vs.begin() ; it!=vs.end();++it){

//    if(true ||(Container::stateColor[0].a !=0 || Container::containers[i]->isSelected) && cam->isPointVisible(*it) && cols[i].a>.02){
      vScreen[i] = cam->worldToScreen(*it,cam->viewPort);

//    }
//    else vScreen[i] = ofVec2f(-1,-1);
    i++;
  }
  if(vs.size()>0){
    kNNScreen.buildIndex(vScreen);
    if(delaunay.getNumPoints()== vScreen.size()){
      for(int i = 0 ; i < vScreen.size() ; i++){
        delaunay.setPointAtIndex(vScreen[i], i,false);
      }

    }
    else{
      delaunay.reset();
      for(auto & v:vScreen){
        delaunay.addPoint(v);
      }
    }
//    triangulate only once on little datasets
    if(vScreen.size()<maxToDelaunaySearch){
    delaunay.triangulate();
    }
  }
  
}

void Physics::applyEquation(FitEquation feq) {

  for(int i = 0; i < 3 ; i++){
    bool begin = true;
    for(FitEquation::eqStruct::iterator it = feq.equation[i].begin(); it!= feq.equation[i].end() ; ++it){
      int id = Container::getAttributeId(feq.paramNames[it->first]);
      float factor = it->second;
      if(begin){

        //                Physics::vs[0][i] = Container::normalizedAttributes.row(id)*factor;
        //                DSP_vsmul(&Container::normalizedAttributes[id],Container::attrSize,&factor,&Physics::vs[0][i],3,Physics::vs.size());
        begin = false;
      }
      else{
        //                DSP_vsma(&Container::normalizedAttributes[id],Container::attrSize,&factor,&Physics::vs[0][i],3,&Physics::vs[0][i],3,Physics::vs.size());
      }
    }


  }

  ofLogNotice("Physics") <<"applying Eq : "<< feq.toString(4) ;

  updateVBO();
  updateVScreen();
}

bool Physics::applyFit(){
  if(!fits){
    ofLogWarning("Physics") << "no fits to apply" ;
    return false;

  }
  if(fitsSize!=vs.size()){
    ofLogWarning("Physics") << "wrong fitsSize" ;
    return false;

  }
  memcpy(&vs[0],fits,sizeof(ofVec3f)*fitsSize);
  updateVBO();
  updateVScreen();
  GUI::i()->guiAxe.attr[0]->setLabelText("custom_x");
  GUI::i()->guiAxe.attr[1]->setLabelText("custom_y");
  GUI::i()->guiAxe.attr[2]->setLabelText("custom_z");
  return true;
}


void Physics::updateOneColor(int idx,ofColor col,bool temp,bool callback){
  ofFloatColor* c = new ofFloatColor[1];
  if(callback){
    c[0] = cols[idx];
  }
  else{
    c[0] = (ofFloatColor)col;
  }
  static int colorStride  = sizeof(ofFloatColor);
  vbo.getColorBuffer().updateData(idx*colorStride, colorStride, &c[0].r);

  if(!temp){cols[idx] = col;}
  delete [] c;
}

void Physics::updateAllColorsAlpha(){
  for(vector<Container*>::iterator it = Container::containers.begin() ; it != Container::containers.end();++it){

    cols[(*it)->globalIdx].a = Container::stateColor[0].a;// = Container::stateColor[(*it)->isSelected?2:(*it)->isHovered?3:(int)(*it)->state];
  }
  vbo.updateColorData(&cols[0],Container::containers.size());
}

void Physics::reinitAllColors(){
  for(vector<Container*>::iterator it = Container::containers.begin() ; it != Container::containers.end();++it){
    cols[(*it)->globalIdx] =  Container::stateColor[(*it)->isSelected?2:(*it)->isHovered?3:(int)(*it)->state];
    cols[(*it)->globalIdx].a = Container::stateColor[0].a;// = Container::stateColor[(*it)->isSelected?2:(*it)->isHovered?3:(int)(*it)->state];
  }
  vbo.updateColorData(&cols[0],Container::containers.size());
}


void Physics::updateAllColors(){

  vbo.updateColorData(&cols[0],Container::containers.size());
}

void Physics::updateOnePos(int idx,ofVec3f & pos){

  static int vertStride  = sizeof(ofVec3f);
  vbo.getVertexBuffer().updateData(idx*vertStride, vertStride, &pos[0]);
  //    vbo.updateOneVertexData(&pos, idx);
  Physics::vs[idx] = pos;

}




bool Physics::updateDrag(ofVec2f mouse){
  if(dragged.size()>0){

    for(int i = 0 ; i < originDrag.size() ; i++ ){
      ofVec3f v =Camera::getActiveCam()->screenToWorld(ofVec3f(mouse.x,mouse.y,0)+originDrag[i]);

      updateOnePos(dragged[i]->globalIdx,v);
    }
    return true;
  }
  return false;
}


void Physics::setSelected(vector<unsigned int> * selected){

  selectedIdx = selected;
  if(selectedIdx)
    vbo.setIndexData(&selectedIdx->at(0), selectedIdx->size(),GL_DYNAMIC_DRAW);
  else
    vbo.setIndexData(&idxs[0], vs.size(),GL_DYNAMIC_DRAW);
}

void Physics::setFits(vector<ofVec3f> & fi){

  fits = &fi[0];
  fitsSize = fi.size();
  updateFits();
}
void Physics::setFits(float * fi,unsigned int size){
  
  fits = (ofVec3f * )fi;
  fitsSize = size;
  updateFits();
  
}

void Physics::updateFits(){
  if( fits!=NULL)
  {
    //init Vbo
    if(fitsVbo.getNumVertices()==0){
      fitsVbo.setVertexData(fits, fitsSize, GL_DYNAMIC_DRAW);
      fitsVbo.setIndexData(&idxs[0], fitsSize, GL_DYNAMIC_DRAW);
    }
    // update
    else{
      fitsVbo.updateVertexData(fits, fitsSize);
    }
  }
  else{
    fitsVbo.clearVertices();
  }
}



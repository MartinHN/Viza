//
//  Container.cpp
//  ViZa
//
//  Created by martin hermant on 09/10/14.
//
//

#include "Container.h"
//#include "ofxTweener.h"





vector<Container> Container::containers;
map<string,vector<Container*> > Container::songs;
string Container::selectedSong;
vector<string> Container::attributeNames;
int Container::hoverIdx;
bool Container::colorInit = true;

vector< vector< float> > Container::normalizedAttributes;
vector< vector< float> > Container::attributesCache;

float Container::radius = 10;
ofFloatColor Container::stateColor[4];

 vector<float > Container::mins;
 vector<float > Container::maxs;
 vector<float > Container::means;
 vector<float > Container::stddevs;
 vector<unsigned int > Container::total;



void Container::registerListener(){

    
    for(int i = 0 ; i < containers.size() ; i++){
        containers[i].state = ofParameter<float>();
        containers[i].state.addListener(&containers[i], &Container::setState);
        containers[i].state = 0;
        
        containers[i].isSelected = ofParameter<bool>();
        containers[i].isSelected.addListener(&containers[i], &Container::setSelected);
        containers[i].isSelected = false;

        
        containers[i].isHovered = ofParameter<bool>();
        containers[i].isHovered.addListener(&containers[i], &Container::setHovered);
        containers[i].isHovered = false;
        
        
    }
    songs.clear();
    for(vector<Container>::iterator it = containers.begin() ; it!= containers.end() ; ++it){
        songs[it->filename].push_back(&(*it));
    }
    
   
}

void Container::setSelected(bool & s){
Physics::updateOneColor(index,getColor());

}


void Container::setState(float & s){
Physics::updateOneColor(index,getColor());
    if(s<=1){AudioPlayer::instance()->Play(*this,(int)s);}
    
    
}

void Container::setHovered(bool & s){

    
Physics::updateOneColor(index,getColor());

}

void Container::selectSong(string name){
    if(selectedSong!=""){
        vector<Container*> conts = Container::songs[selectedSong];
        for(vector<Container*>::iterator it = conts.begin() ; it!=conts.end() ; ++it){
            (*it)->isSelected = false;
            
        }
    }
    selectedSong = name;
    if(selectedSong!=""){
        vector<Container*> conts = Container::songs[selectedSong];
        for(vector<Container*>::iterator it = conts.begin() ; it!=conts.end() ; ++it){
            (*it)->isSelected = true;
        }
        Physics::setSelected(conts[0]->index,conts.back()->index);
    }
}

bool Container::hoverContainer(int  idx){
    if(idx!=hoverIdx){
    if(hoverIdx!=-1)containers[hoverIdx].isHovered= false;
        hoverIdx=idx;
    if(hoverIdx!=-1)containers[hoverIdx].isHovered= true;
        return true;
    }
    return false;
}


void Container::clearAll(){
    containers.clear();
    attributeNames.clear();

    maxs.clear();
    mins.clear();
    means.clear();
    total.clear();
    
  
}
void Container::setAttribute(const string n,const float v){
        bool found = false;

    int foundIdx = ofFind(attributeNames,n);

    
    if(foundIdx>=0 && foundIdx < attributeNames.size()){

                mins[foundIdx] = MIN(mins[foundIdx], v);
                maxs[foundIdx] = MAX(maxs[foundIdx], v);

//                float t = total[foundIdx];
//                means[foundIdx]=(means[foundIdx]*t+v)*1.0/(t+1.0);

                total[foundIdx]++;
                
                found = true;
            
        }
    
    else{
        attributeNames.push_back(n);
        int attrIdx = attributeNames.size()-1;

        mins.resize(attrIdx+1);
        maxs.resize(attrIdx+1);
//        means.resize(attrIdx+1);
        total.resize(attrIdx+1);
        
        mins[attrIdx]=v;
        maxs[attrIdx]=v;
//        means[attrIdx]=v;
        total[attrIdx]++;
        
        foundIdx = attrIdx;
        

    
    }
    

    if(foundIdx>=getAttributes()->size()){
     getAttributes()->resize(attributeNames.size());
    }

    
    (*getAttributes())[foundIdx] = v;

    
}

void Container::CacheNormalized(){
    
    int numCont = containers.size();
    normalizedAttributes.resize(numCont);
    int idx=0;
    int attrSize = attributesCache[0].size();
    means.resize(attrSize);
    stddevs.resize(attrSize);
//    for(vector <Container >::iterator it =  containers.begin();it != containers.end();++it){
//        normalizedAttributes[idx].resize(attrSize);
//        vector<float> tmpBuf(attrSize);
////        vDSP_normalize(&it->getAttributes()->at(0), 1, &normalizedAttributes[idx][0], 1, &means[idx], &stddevs[idx], attrSize);
//        vDSP_vsub(&mins[0],1,&it->getAttributes()->at(0),1,&normalizedAttributes[idx][0],1,attrSize);
//        vDSP_vsub(&mins[0],1,&maxs[0],1,&tmpBuf[0],1,attrSize);
//        vDSP_vdiv(&tmpBuf[0], 1, &normalizedAttributes[idx][0], 1, &normalizedAttributes[idx][0], 1, attrSize);
////
//        idx++;
//    }

    
    for(int i = 0 ; i < numCont;i++){
        normalizedAttributes[i].resize(attrSize);
    }
    for(int i = 0 ; i < attrSize;i++){
        //        vector<float> tmpBuf(attrSize);
        vDSP_normalize(&attributesCache[0][i],attrSize, &normalizedAttributes[0][i], attrSize, &means[i], &stddevs[i], numCont);
        //        vDSP_vsub(&mins[0],1,&it->getAttributes()->at(0),1,&normalizedAttributes[idx][0],1,attrSize);
        //        vDSP_vsub(&mins[0],1,&maxs[0],1,&tmpBuf[0],1,attrSize);
        //        vDSP_vdiv(&tmpBuf[0], 1, &normalizedAttributes[idx][0], 1, &normalizedAttributes[idx][0], 1, attrSize);
        //
        
    }
}


ofFloatColor Container::getColor(){
    if(colorInit){
    Container::stateColor[0] = ofFloatColor::white;
    Container::stateColor[0].a=.3;
    Container::stateColor[1] =ofFloatColor::red;
    Container::stateColor[1].a=.6;
    Container::stateColor[2] =ofFloatColor::green;
    Container::stateColor[2].a=.5;
    Container::stateColor[3] =ofFloatColor::white;
    Container::stateColor[3].a=1;
        colorInit = false;
}
    
    return ofColor(stateColor[(int)state==1?1:isHovered?3:isSelected?2:0]);
}



ofVec3f Container::getPos(){
    return Physics::vs[index]+.5;
}

vector< float>* Container::getAttributes(){
    
    return &attributesCache[index];
};
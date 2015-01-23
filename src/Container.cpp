//
//  Container.cpp
//  ViZa
//
//  Created by martin hermant on 09/10/14.
//
//

#include "Container.h"
//#include "ofxTweener.h"





vector<Container*> Container::containers;
map<string,vector<Container*> > Container::songs;
string Container::selectedSong;

int Container::hoverIdx;
bool Container::colorInit = true;


map<string, map<string,vector<int> > > Container::classes;



float Container::radius = 10;
ofFloatColor Container::stateColor[4];





void Container::registerListener(){
    
    
    for(int i = 0 ; i < containers.size() ; i++){
        containers[i]->state = ofParameter<float>();
        containers[i]->state.addListener(containers[i], &Container::setState);
        containers[i]->state = 0;
        
        containers[i]->isSelected = ofParameter<bool>();
        containers[i]->isSelected.addListener(containers[i], &Container::setSelected);
        containers[i]->isSelected = false;
        
        
        containers[i]->isHovered = ofParameter<bool>();
        containers[i]->isHovered.addListener(containers[i], &Container::setHovered);
        containers[i]->isHovered = false;
        
        
    }
    songs.clear();
    for(vector<Container*>::iterator it = containers.begin() ; it!= containers.end() ; ++it){
        songs[(*it)->filename].push_back(*it);
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
        if(hoverIdx!=-1)containers[hoverIdx]->isHovered= false;
        hoverIdx=idx;
        if(hoverIdx!=-1)containers[hoverIdx]->isHovered= true;
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
    if( Container::stateColor[0].a>.5){
        int stop =54;
        cout << "stop" << endl;
    }
    return ofColor(stateColor[(int)state==1?1:isHovered?3:isSelected?2:0]);
}



ofVec3f Container::getPos(){
    return Physics::vs[index]+.5;
}


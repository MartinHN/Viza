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
vector< vector<unsigned int> > Container::songsContainers;
vector<Container::SongMeta > Container::songMeta;
int Container::numContainer = 0;

pair<string,string> Container::selectedClass;
ofMutex Container::staticContainerMutex;

int Container::hoverIdx = -1;
bool Container::colorInit = true;


vector<int> Container::containerToUpdate;

float Container::radius = 10;
ofFloatColor Container::stateColor[4];





void Container::registerListener(){
    
    
    for(int i = 0 ; i < containers.size() ; i++){
        containers[i]->setState(0);
        containers[i]->setSelected( false);
        containers[i]->setHovered(false);
        
        
    }

    
    
    
}

void Container::setSelected(bool  s){
    isSelected = s;
    Physics::updateOneColor(globalIdx,getColor(),true,s==0);
    
}


void Container::setState(float s){
    state = s;
    containerToUpdate.push_back(globalIdx);
    if(s<=1){AudioPlayer::Play(*this,s);}
    
    
}

void Container::setHovered(bool  s){
    isHovered = s;
    Physics::updateOneColor(globalIdx,getColor(),true,!s);
    
}

void Container::selectClass(string _name,string _value){
    if(selectedClass!=pair<string,string>("","")){
        vector<unsigned int> conts = Container::classeMap[selectedClass.first][selectedClass.second];
        for(vector<unsigned int>::iterator it = conts.begin() ; it!=conts.end() ; ++it){
            containers[(*it)]->setSelected( false);
            
        }
    }

    if(_name!=""&& _value!="" && classeMap.count(_name)>0 && classeMap[_name].count(_value)>0){
        selectedClass = pair<string,string>(_name,_value);
        vector<unsigned int> conts = classeMap[_name][_value];
        for(vector<unsigned int>::iterator it = conts.begin() ; it!=conts.end() ; ++it){
            containers[(*it)]->setSelected(true);
        }

        Physics::setSelected(&Container::classeMap[_name][_value]);
        Physics::updateAllColorsAlpha();
    }
    else{

        Physics::setSelected(NULL);
        Physics::reinitAllColors();
        selectedClass=pair<string,string>("","");
    }
}

bool Container::hoverContainer(int  idx){
    if(idx!=hoverIdx){
        if(hoverIdx!=-1)containers[hoverIdx]->setHovered( false);
        hoverIdx=idx;
        if(hoverIdx!=-1)containers[hoverIdx]->setHovered( true);
        return true;
    }
    return false;
}


void Container::clearAll(){
    hoverIdx = -1;
    AttributeContainer::clearAll();
    ClassContainer::clearAll();
    for(auto c:containers){
        delete c;
    }
    containers.clear();

    songsContainers.clear();
    songMeta.clear();
    numContainer = 0;

    

    
    
}

string Container::getFilename() const{
   return songMeta[songIdx].name;
}

string Container::getAudioPath() const{
    return songMeta[songIdx].audioPath;
}
string Container::getAnnotationPath()const{
    return songMeta[songIdx].annotationPath;
}



ofFloatColor Container::getColor() const{
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
    ofColor c =ofColor(stateColor[(int)state==1?1:isHovered?3:isSelected?2:0]);

    return c;
}



ofVec3f Container::getPos() const{
    return Physics::vs[globalIdx]+.5;
}
ofVec2f Container::getScreenPos() const{
    return Physics::vScreen[globalIdx]+.5;
}

void Container::updateContainerView(){
    for(auto & c:containerToUpdate){
        int s = containers[c]->state;
        Physics::updateOneColor(c,containers[c]->getColor(),s==1,s==0);
    }
    containerToUpdate.clear();
}

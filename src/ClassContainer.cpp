//
//  ClassContainer.cpp
//  ViZa
//
//  Created by martin hermant on 23/01/15.
//
//

#include "ClassContainer.h"


#include "Container.h"
map < string,map<string,vector <unsigned int> > > ClassContainer::classeMap;
vector<ofColor> ClassContainer::classColor;


ofMutex ClassContainer::ClassStaticMutex;




void ClassContainer::setClass(const string &name,const string & value){
    ofScopedLock lock(ClassStaticMutex);
    classeMap[name][value].push_back(((Container*)this)->index);


}


const vector<string> ClassContainer::getClassValues(string cName){
    vector<string > res;
    for(ClassValueStruct::iterator it = classeMap[cName].begin() ; it!=classeMap[cName].end() ; ++it){
        res.push_back(it->first);
    }
    
        return res;
}
const vector<string> ClassContainer::getClassNames(){
    vector<string > res;
    for(ClassMapStruct::iterator it = classeMap.begin() ; it!=classeMap.end() ; ++it){
        res.push_back(it->first);
    }
    
    return res;
}


ofColor ClassContainer::getColorForId(int id){
    if(classColor.size()==0){
        initColors();
    }
    return classColor[id%classColor.size()];
}
void ClassContainer::initColors(){
    classColor.push_back(ofColor::red);
    classColor.push_back(ofColor::blue);
    classColor.push_back(ofColor::violet);
    classColor.push_back(ofColor::brown);
    classColor.push_back(ofColor::purple);
    classColor.push_back(ofColor::gray);
    classColor.push_back(ofColor::hotPink);
    classColor.push_back(ofColor::gold);
    
}
void ClassContainer::clearAll(){
 
    classeMap.clear();
    
}
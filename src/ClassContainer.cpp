//
//  ClassContainer.cpp
//  ViZa
//
//  Created by martin hermant on 23/01/15.
//
//

#include "ClassContainer.h"


#include "Container.h"
map < string,map<string,vector <int> > > ClassContainer::classeMap;



string & ClassContainer::getClass(const string &name){
    return curClass[name];

}


void ClassContainer::setClass(const string &name,const string & value){
    classeMap[name][value].push_back(((Container*)this)->index);
    curClass[name] = value;

}
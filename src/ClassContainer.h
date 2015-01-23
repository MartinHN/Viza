//
//  ClassContainer.h
//  ViZa
//
//  Created by martin hermant on 23/01/15.
//
//

#ifndef __ViZa__ClassContainer__
#define __ViZa__ClassContainer__


#include "ofMain.h"


class ClassContainer{
public:
    
    
    static map < string,map<string,vector <int> >  > classeMap;

    ClassContainer(){};
    virtual  ~ClassContainer(){};
    
    string & getClass(const string &cl);
    void setClass(const string &cl,const string & v);
    
    map<string,string> curClass;

    
    
    
    
    
    
    
};

#endif /* defined(__ViZa__ClassContainer__) */

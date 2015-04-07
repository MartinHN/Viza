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
    
    typedef map<string,vector <unsigned int> > ClassValueStruct;
    typedef map < string,ClassValueStruct  >  ClassMapStruct;
    
    static ClassMapStruct classeMap;

    ClassContainer(){};
    virtual  ~ClassContainer(){};
    
    string & getClass(const string &cl);
    void setClass(const string &cl,const string & v);

    static const vector<string> getClassValues(string cName);
    static const vector<string> getClassNames();
    
    static vector<ofColor> classColor;
    static ofColor getColorForId(int id);
    static void initColors();
    
    static void clearAll();
    
    
};

#endif /* defined(__ViZa__ClassContainer__) */

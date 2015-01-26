//
//  Container.h
//  ViZa
//
//  Created by martin hermant on 09/10/14.
//
//

#ifndef __ViZa__Container__
#define __ViZa__Container__

#include <iostream>
#include "ofMain.h"
#include "AudioPlayer.h"
#include "Physics.h"
#include "AttributeContainer.h"
#include "ClassContainer.h"





class Container : public AttributeContainer,public ClassContainer{
public:
    
    Container(){
    path="";
    filename = "";
    
    
    };
    
    static vector<Container *> containers;
    static map<string,vector<Container*> > songs;


    
    static float radius;
    static ofFloatColor stateColor [];
    static void registerListener();
    static void orderBy(string attr,int axe,bool norm);
    static void selectClass(string name,string value);
    static bool hoverContainer(int idx);
    static int hoverIdx;
    static pair<string,string> selectedClass;
    static bool colorInit;

    static void clearAll();
    
    
    Container(string path,float begin,float end,unsigned int _idx,int level=0):index(_idx),AttributeContainer(_idx),path(path),begin(begin),end(end),level(level){
        
        state = 0;
        filename = path.substr(path.find_last_of("/")+1);
//        if(((map<string,vector<Container*> >::iterator)songs.find(filename))==songs.end())
        songs[filename].push_back(this);
        setClass("songName",filename);
        
    };
    
    

    
    
    ofVec3f getPos();
    string path;
    string filename;
    float begin;
    float end;
    int level;
    unsigned int index;

    ofParameter<float> state;
    ofParameter<bool> isSelected;
    ofParameter<bool> isHovered;
    
    
    void setState(float & a);
    void setSelected(bool & s);
    void setHovered(bool & s);

    ofFloatColor getColor();
    
    
    
};



#endif /* defined(__ViZa__Container__) */

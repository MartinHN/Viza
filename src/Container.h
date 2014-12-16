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



class Container{
public:
    
    Container(){
    path="";
    filename = "";
    
    
    };
    
    static vector<Container> containers;
    static map<string,vector<Container*> > songs;
    static vector<string> attributeNames;
    
    static map<string,float > mins;
    static map<string,float> maxs;
    static map<string,float > means;
    static map<string,unsigned int > total;
    
    static float radius;
    static ofFloatColor stateColor [];
    static void registerListener();
    static void orderBy(string attr,int axe,bool norm);
    static void selectSong(string name);
    static bool hoverContainer(int idx);
    static int hoverIdx;
    static string selectedSong;
    static bool colorInit;

    static void clearAll();
    
    
    Container(string path,float begin,float end,int idx,int level=0): path(path),begin(begin),end(end),level(level),index(idx){
        
        state = 0;
        filename = path.substr(path.find_last_of("/")+1);
//        if(((map<string,vector<Container*> >::iterator)songs.find(filename))==songs.end())
        songs[filename].push_back(this);
        
    };
    
    
    void init(string path,float begin,float end,int idx,int level=0);
    
    
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
    map<int, float> attributes;
    
    void setState(float & a);
    void setSelected(bool & s);
    void setHovered(bool & s);

    
    void setAttribute(const string n,const float v);
    ofFloatColor getColor();
    
    
    
};



#endif /* defined(__ViZa__Container__) */

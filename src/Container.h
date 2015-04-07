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
    
    
    
    
    };
    
    static vector<Container *> containers;
    static map<string,vector<Container*> > songs;
    static map<string, string > audioPaths;
    static map<string, string > annotationPaths;


    
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
    
    
    Container(string path,string audioPath,float begin,float end,unsigned int _idx,int level=0):index(_idx),AttributeContainer(_idx),begin(begin),end(end),level(level){
        
        state = 0;
        string audioFileName = audioPath.substr(audioPath.find_last_of("/")+1);
        songs[audioFileName].push_back(this);
        audioPaths[audioFileName] = audioPath;
        annotationPaths[audioFileName] = path;
        setClass("songName",audioFileName);
        
        
    };
    
    

    
    
    ofVec3f getPos() const;
    string getFilename() const;
    string getAudioPath() const;
    string getAnnotationPath() const;
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

    ofFloatColor getColor() const;
    
    
    
};



#endif /* defined(__ViZa__Container__) */

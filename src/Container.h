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
//        multi thread creation would have to lock, we assign later, see FileImporter.cpp
//        numContainer++;
    };
    
    ~Container(){
        
        ofLogVerbose("Container") << "deleting : " << numContainer;
        numContainer--;
    }
    
    
    
    
    typedef struct{
        string name = "";
        string audioPath= "";
        float length=0;
        int numSlices=0;
        
        //filled automaticly
        string annotationPath="";
        int idx;
    }SongMeta;
    
    
    static vector<Container *> containers;
    static vector< vector<unsigned int> > songsContainers;
    static vector<SongMeta> songMeta;

    static vector<int> containerToUpdate;
    
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
    
    static ofMutex staticContainerMutex;
    
    Container(float begin,float end,unsigned int _idx,unsigned int _sliceIdx=0):globalIdx(_idx),AttributeContainer(_idx),begin(begin),end(end),sliceIdx(_sliceIdx){
        state = 0;
        
//        multi thread creation would have to lock numContainer , we assign later, see FileImporter.cpp
//        {
//        ofScopedLock lock (Container::staticContainerMutex);
//        numContainer++;
//        }
//        ofLogVerbose("Container") << "creating : " << numContainer;
    };
    
    

    static int numContainer;
    ofVec2f getScreenPos() const;
    ofVec3f getPos() const;
    string getFilename() const;
    string getAudioPath() const;
    string getAnnotationPath() const;
    float begin;
    float end;
    int level;
    unsigned int globalIdx;
    unsigned int songIdx;
    unsigned int sliceIdx;

    float state;
    bool isSelected;
    bool isHovered;
    
    
    void setState(float  a);
    void setSelected(bool  s);
    void setHovered(bool  s);

    ofFloatColor getColor() const;
    
    static void updateContainerView();
    
};



#endif /* defined(__ViZa__Container__) */

//
//  GUIAxes.h
//  ViZa
//
//  Created by martin hermant on 22/04/15.
//
//

#ifndef __ViZa__GUIAxes__
#define __ViZa__GUIAxes__

#include "ofMain.h"
#include "GUICanvasBase.h"

class GUIAxes : public GUICanvasBase{
    public :
//    GUIAxes();
    GUIAxes(string name);
    ~GUIAxes(){};
    

    ofxUIDropDownList * attr[3], *aggr[3];
    ofxUIDropDownList *scaleType[3];
    ofxUINumberDialer * min[3],*max[3];
    ofxUIDropDownList * coordinateType;
    ofxUIToggle* clampValues;
    
    
    void guiEvent(ofxUIEventArgs &e) ;
    void setup() override;
    void async(ofEventArgs & e,bool init = false) ;
    
    void checkMinsMaxsChanged(bool updateVal);

    
    set<string> attrNames;
    set<string> aggrNames;
    
    private :
    
    vector<string> coordinateTypeNames;
    vector<string> typeScales;
    int shouldUpdateAggregator;
    
    bool isAnAttributeList(ofxUIWidget * w);
    int getNumAxe(ofxUIWidget* w);
    string numToAxe(int i);
    int getFullAttrIdx(const string & attr,const string &  aggr );
    void recievedMessage(ofMessage & msg);
    void reorderAxe(int a);
    void checkOverlapingDDL(ofxUIEventArgs & e);
    
};

#endif /* defined(__ViZa__GUIAxes__) */

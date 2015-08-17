//
//  GUIclass.h
//  ViZa
//
//  Created by martin hermant on 22/04/15.
//
//

#ifndef __ViZa__GUIClass__
#define __ViZa__GUIClass__

#include "GUICanvasBase.h"


class GUIClass : public GUICanvasBase {
public:
    
    GUIClass(string name);
    ~GUIClass(){};
    
    void setup();

    
    void guiEvent(ofxUIEventArgs & e);
    void async(ofEventArgs & a);
    
private:

//    
//    ofxUIScrollableCanvas * classNamesScroll;
//    ofxUIScrollableCanvas * classValueScroll;
    ofxUIScrollableList * classNamesDDList;
    ofxUIScrollableList * classValueDDList;
    ofxUIToggle * classColor;
        bool updateClassValue = false;
    int scrollW = 700;
};


#endif /* defined(__ViZa__GUIclass__) */

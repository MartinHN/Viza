//
//  GUILoad.h
//  ViZa
//
//  Created by martin hermant on 24/04/15.
//
//

#ifndef __ViZa__GUILoad__
#define __ViZa__GUILoad__

#include "GUICanvasBase.h"
#include "FileImporter.h"

class GUILoad : public GUICanvasBase{
    public :
    GUILoad(string name);
    ~GUILoad(){};
    
    
    void setup()override;
    void messageRecieved(ofMessage & msg) override{};
    
    
    ofxUIButton * loadAnal;
    ofxUIButton * savePos,*loadPos;
    ofxUIToggle * splice;
    ofxUINumberDialer * onsetThreshold;
    ofxUIDropDownList * types;

    
    void guiEvent(ofxUIEventArgs & e)override;
    
};

#endif /* defined(__ViZa__GUILoad__) */

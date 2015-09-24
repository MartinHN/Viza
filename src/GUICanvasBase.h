//
//  GUICanvasBase.h
//  ViZa
//
//  Created by martin hermant on 29/04/15.
//
//

#ifndef __ViZa__GUICanvasBase__
#define __ViZa__GUICanvasBase__
#include"ofxUI.h"
#include "ofxGui.h"
#include "ofxUIScrollableList.h"
#include "ofxUINumberD.h"
#include "ofxUIOwnedDDList.h"
#include "Container.h"





class GUICanvasBase : public ofxUISuperCanvas{
public:
    
    
    GUICanvasBase(const string & name):ofxUISuperCanvas(name,getShared()){
        ofLogVerbose("GUI") << "creating GUI : " << name ;
        ofAddListener(sharedMessage, this, &GUICanvasBase::_messageRecieved);
       ofAddListener(this->newGUIEvent  , this, &GUICanvasBase::guiEvent);
        
    };
    
    
    virtual ~GUICanvasBase(){
    ofLogVerbose("GUI") << "destroying GUI : " << name ;
    
    };
    
    static ofxUICanvas * getShared(){static  ofxUICanvas * sh = new ofxUICanvas();return sh;}
    
    virtual void init(){};
    virtual void setup(){};
    virtual void guiEvent(ofxUIEventArgs &e){};
    
    void _messageRecieved(ofMessage & msg){};
    virtual void messageRecieved(ofMessage & msg){};
    
    
    
    
    static ofEvent<ofMessage>  sharedMessage;
    // hack for having one font only shared between instances
    static ofxUICanvas sharedRessource;
    
    static inline string getGUIMsgDest(ofMessage & msg){
        return ofSplitString(msg.message,":")[0];
    }
    static inline vector<string> getGUIMsgArgs(ofMessage & msg){
        return ofSplitString(ofSplitString(msg.message,":")[1],",");
    }
    
};

#endif /* defined(__ViZa__GUICanvasBase__) */

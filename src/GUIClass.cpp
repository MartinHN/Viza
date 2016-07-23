//
//  GUIclass.cpp
//  ViZa
//
//  Created by martin hermant on 22/04/15.
//
//

#include "GUIClass.h"


GUIClass::GUIClass(string name): GUICanvasBase(name){

    
    //Classes////////////////////

    setName("Class");


    setDimensions(scrollW, 500);
    
    
    vector<string> dumb;
//    classNamesScroll = new ofxUIScrollableCanvas(this);
//    classValueScroll = new ofxUIScrollableCanvas(this);
    classNamesDDList = new ofxUIScrollableList("ClassNames",dumb,100,ofGetHeight(),0,0,OFX_UI_FONT_SMALL);
    classValueDDList = new ofxUIScrollableList("ClassValues",dumb ,100,ofGetHeight(),0,0,OFX_UI_FONT_SMALL);
//    classNamesDDList = new ofxUIDropDownList("ClassNames",dumb);
//    classValueDDList = new ofxUIDropDownList("ClassValues",dumb);
    classColor = new ofxUIToggle("ColorIt",false,10,10);
    
//    classNamesDDList->setSnapping(true);
//    classNamesDDList->setScrollableDirections(false, true);
    
    
    
//    classNamesScroll->addWidgetDown(classNamesDDList);
//    classValueScroll->addWidgetDown(classValueDDList);
//    addWidgetDown(classNamesScroll);
//    addWidgetRight(classValueScroll);
//    

    addWidgetDown(classNamesDDList);
    addWidgetRight(classValueDDList);
    addWidgetRight(classColor);
    
    classNamesDDList->getSRect()->setParent(rect);
    classValueDDList->getSRect()->setParent(rect);
    classValueDDList->getSRect()->y = rect->y;
    


    
}


void GUIClass::setup(){
//    ofAddListener(this->newGUIEvent, this, &GUIClass::guiEvent);
    setDrawBack(false);
    if(Container::getClassNames().size()>0){
        classValueDDList->setValue(true);
        classNamesDDList->clearToggles();
        vector <string> tmpName = Container::getClassNames();
        
        classNamesDDList->addToggles(tmpName);
      int selIdx = 0;
//      select first not songName if available
      if(classNamesDDList->getToggles().size()>1){selIdx = 1;}
        classNamesDDList->getToggles()[selIdx]->setValue(true);
        classNamesDDList->getToggles()[selIdx]->triggerSelf();

      if(selIdx!=0){
        classColor->setValue(true);
        classColor->triggerSelf();
      }

//        
//        classNamesScroll->setScrollAreaHeight(500);
//        
//        classValueScroll->setScrollAreaHeight(500);
        
    }
}


void GUIClass::guiEvent(ofxUIEventArgs & e){
    
    if(e.getParent() ==classNamesDDList){
        updateClassValue = true;
    }
    
    else if(e.getParent() == classValueDDList){
        Container::selectClass(e.getName() =="None"?"":classNamesDDList->getSelectedNames()[0],e.getName());
    }
    
    else if(e.widget == classColor ){
        
        Container::ClassValueStruct *curV = &Container::classeMap[classNamesDDList->getSelectedNames()[0]];
        int idx =0;
        for(Container::ClassValueStruct::iterator it = curV->begin() ; it != curV->end(); ++it ){
            
            ofColor nC;
            if(classColor->getValue())
                nC = ofColor(Container::getColorForId(idx),255*Container::stateColor[0].a);
            else
                nC = ofColor(ofColor::white,255*Container::stateColor[0].a);
            
            
            for(int i = 0 ; i < it->second.size() ; i++){
                Physics::updateOneColor(it->second[i],nC);
            }
            idx++;
        }
        
    }
}


void GUIClass::async(ofEventArgs & a){
    if(updateClassValue){
        classValueDDList->clearToggles();
        vector<string> tmpC = Container::getClassValues(classNamesDDList->getSelected()[0]->getName());
        if(tmpC.size() > 100){
            tmpC.resize(100);
        }
        tmpC.insert(tmpC.begin(), "None");
        classValueDDList->addToggles(tmpC);
        
        classValueDDList->open();
        classNamesDDList->open();
        ofxUIRectangle * r =classValueDDList->getRect();
        
//        classValueScroll->setDimensions(scrollW,(MAX(classValueDDList->getToggles().size(),classNamesDDList->getToggles().size())+2)*r->height);
        
        updateClassValue = false;
    }
    
}
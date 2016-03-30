//
//  GUIView.cpp
//  ViZa
//
//  Created by martin hermant on 24/04/15.
//
//

#include "GUIView.h"



GUIView::GUIView(string name): GUICanvasBase(name){
    
    //VIEWWWW/////////////
    
    setName("View");
    
    alphaView = new ofxUISlider("alphaView",0,1,0.3f,100,10);
    selBrightest = new ofxUIToggle("SelectBrightest",false,10,10);
    linkClasses = new ofxUIToggle("linkClasses",false,10,10);
    orthoCam = new ofxUIToggle("orthoCam",true,10,10);
    pointSize = new ofxUISlider("pointSize",0,30,1,100,10);
    isClipping = new ofxUIToggle("isClipping",false,10,10);
    show2dViews = new ofxUIToggle("2dViews",false,10,10);
    fishEyeRadius = new ofxUISlider("fishEyeRadius",0.0,1.0,0.,100,10);
    fishEyeStrength = new ofxUISlider("fishEyeStrength",0.01,1,.5,100,10);
    graphMode = new ofxUIToggle("graphMode",false,10,10);
    
    
    // placing
    
    addWidgetDown(alphaView);
    addWidgetDown(selBrightest);
    addWidgetDown(linkClasses);
    addWidgetDown(orthoCam);
    addWidgetDown(pointSize);
    addWidgetDown(isClipping);
    addWidgetDown(show2dViews);
    addWidgetDown(fishEyeRadius);
    addWidgetDown(fishEyeStrength);
    addWidgetDown(graphMode);
    
    autoSizeToFitWidgets();
    
}
void GUIView::guiEvent(ofxUIEventArgs & e){
    
    if(e.widget == alphaView){
        Container::stateColor[0].a = pow((alphaView)->getValue(),2);
        Physics::updateAllColorsAlpha();
    }
    else if(e.widget == linkClasses){
        Physics::linkClasses = linkClasses->getValue();
    }
    else if(e.widget == orthoCam){
        Camera::mainCam->setcamOrtho(orthoCam->getValue());
    }
    else if(e.widget == pointSize){
        Container::radius = pointSize->getValue();
        //            glPointSize(Container::radius);
    }
    else if(e.widget == show2dViews){
        
        Camera::mainCam->setRelativeViewPort(0, 0,show2dViews->getValue()? .75:1, 1);
        Camera::mainCam->updateViewPort();
        Camera::setSecondaryVisible(show2dViews->getValue());
        Physics::updateVScreen();
        
    }
}


void GUIView::setup(){
//    ofAddListener(this->newGUIEvent, this, &GUIView::guiEvent);
    
}
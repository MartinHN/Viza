//
//  GUIStatistics.cpp
//  ViZa
//
//  Created by martin hermant on 22/04/15.
//
//

#include "GUIStatistics.h"



GUIStatistics::GUIStatistics(string name): GUICanvasBase(name){
    
    
    //Classes////////////////////
    
    setName("Statistics");
    
    
    setDimensions(scrollW, 500);
    
    
    vector<string> dumb;
    statisticList = new ofxUIScrollableList("statistics",dumb ,scrollW,ofGetHeight(),0,0,OFX_UI_FONT_SMALL);
    //    classNamesDDList = new ofxUIDropDownList("ClassNames",dumb);
    //    statisticList = new ofxUIDropDownList("ClassValues",dumb);

    
    //    classNamesDDList->setSnapping(true);
    //    classNamesDDList->setScrollableDirections(false, true);
    
    
    
    //    classNamesScroll->addWidgetDown(classNamesDDList);
    //    classValueScroll->addWidgetDown(statisticList);
    //    addWidgetDown(classNamesScroll);
    //    addWidgetRight(classValueScroll);
    //
    

    addWidgetDown(statisticList);


    statisticList->getSRect()->setParent(rect);
    statisticList->getSRect()->y = rect->y;
    statisticList->open();
//    statisticList->setDrawBack(true);
    
    
    
    
}


void GUIStatistics::setup(){
    //    ofAddListener(this->newGUIEvent, this, &GUIStatistics::guiEvent);
    setDrawBack(false);
    
    
    Statistics::i()->setMatrix(&Container::attributesCache[0],Container::attrSize,Container::numContainer);
    Statistics::i()->computePCA();
    
    if(Container::attributeNames.size()>0){
        statisticList->clearToggles();
        vector <string> tmpName;
        int atId=0;
        for(auto & a : Container::attributeNames){
            tmpName.emplace_back(getFormattedStat(atId));
            atId++;
        }

        
        statisticList->addToggles(tmpName);
        statisticList->setSingleSelected(0);
//        statisticList->getToggles()[0]->setValue(true);
//        statisticList->getToggles()[0]->triggerSelf();
        
        //
        //        classNamesScroll->setScrollAreaHeight(500);
        //
        //        classValueScroll->setScrollAreaHeight(500);
        
    }
        statisticList->open();
}

string GUIStatistics::getFormattedStat(int i){
    string res = Container::attributeNames[i];
    if(Statistics::i()->stats.count("PCARank")){
        res += "    ";
        res+= std::to_string(Statistics::i()->stats["PCARank"][i]);
    }
    return res;
}


void GUIStatistics::guiEvent(ofxUIEventArgs & e){
    

}


void GUIStatistics::async(ofEventArgs & a){

}
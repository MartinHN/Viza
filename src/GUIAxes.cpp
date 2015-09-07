//
//  GUIAxes.cpp
//  ViZa
//
//  Created by martin hermant on 22/04/15.
//
//

#include "GUIAxes.h"



void GUIAxes::guiEvent(ofxUIEventArgs &e){
    // events based on mouse released
    if(ofGetMousePressed()){return;}
    
    
    
    string name = e.getName();
    
    
    int axe = getNumAxe(e.getParent());
    ofLogVerbose("GUI") << e.getParent()->getName() << " // " << e.getName() << "// " <<  axe ;
    
    // attributes and aggregator modification
    if( axe!=-1 && attr[axe]->getSelected().size()>0 ){
        if(isAnAttributeList(e.getParent())){
            shouldUpdateAggregator = axe;
            if(shouldUpdateAggregator!=-1){
                int axe = shouldUpdateAggregator;
                string attrtmp =attr[axe]->getSelected()[0]->getName();
                string oldAggr ="";
                if(aggr[axe]->getSelected().size()>0) oldAggr =aggr[axe]->getSelected()[0]->getName();
                aggr[axe]->clearSelected();
                aggr[axe]->clearToggles();
                vector<string>  newAggr = Container::getAggregators(attrtmp);
                
                int idx = ofFind(newAggr, oldAggr);
                if(oldAggr == "" || idx == newAggr.size())idx = 0;
                aggr[axe]->addToggles(newAggr);
                aggr[axe]->getToggles()[idx]->setValue(true);
                aggr[axe]->getToggles()[idx]->triggerSelf();
                aggr[axe]->setSingleSelected(idx);
                shouldUpdateAggregator = -1;
            }
        }
        else if (aggr[axe]->getSelected().size()>0 && scaleType[axe]->getSelectedIndeces().size()>0) {
            reorderAxe(axe);
        }
        
    }
    else if(e.getParent() == coordinateType){
        for(int i = 0 ; i <3; i++){
            reorderAxe(i);
        }
    }
    
    
    // mins maxs modifications
    else if (e.getKind() == OFX_UI_WIDGET_NUMBERDIALER){
        axe = getNumAxe(e.widget);
        float s =((ofxUINumberDialer*)e.widget)->getValue();
        
        ofVec3f mask(axe==0?1:0,axe==1?1:0,axe==2?1:0);
        if(name.substr(0,name.length()-1)=="min"){
            Physics::mins = s*mask + (-mask+ofVec3f(1))*Physics::mins;
        }
        if(name.substr(0,name.length()-1)=="max"){
            Physics::maxs = s*mask + (-mask+ofVec3f(1))*Physics::maxs;
        }
        scaleType[axe]->getToggles()[2]->setValue(true);
        scaleType[axe]->getToggles()[2]->triggerSelf();
        Physics::orderByAttributes(attr[axe]->getSelected()[0]->getName()+"."+aggr[axe]->getSelected()[0]->getName(), axe, scaleType[axe]->getSelectedIndeces()[0]);
        
    }
    
    
    checkOverlapingDDL(e);
        
}

void GUIAxes::reorderAxe(int axe){
    string attrtmp =attr[axe]->getSelected()[0]->getName();
    if(aggr[axe]->getSelected().size()>0){
    string aggrtmp = aggr[axe]->getSelected()[0]->getName();
    if(aggrtmp=="None"){
        aggrtmp = "";
    }
    int scaletmp =scaleType[axe]->getSelectedIndeces()[0];
    Physics::orderByAttributes(attrtmp+"."+aggrtmp, axe, scaletmp);
    checkMinsMaxsChanged(name != "range"  );
    }
}

// Async GUI modifications
void GUIAxes::async(ofEventArgs & e,bool init){
    
    //update Aggregator list for Selected Attribute in given Axe
    if(shouldUpdateAggregator!=-1){
        int axe = shouldUpdateAggregator;
        string attrtmp =attr[axe]->getSelected()[0]->getName();
        string oldAggr ="";
        if(aggr[axe]->getSelected().size()>0) oldAggr =aggr[axe]->getSelected()[0]->getName();
        aggr[axe]->clearSelected();
        aggr[axe]->clearToggles();
        vector<string>  newAggr = Container::getAggregators(attrtmp);
        
        int idx = ofFind(newAggr, oldAggr);
        if(oldAggr == "" || idx == newAggr.size())idx = 0;
        aggr[axe]->addToggles(newAggr);
        aggr[axe]->getToggles()[idx]->setValue(true);
        if(!init)aggr[axe]->getToggles()[idx]->triggerSelf();
        aggr[axe]->setSingleSelected(idx);
        shouldUpdateAggregator = -1;
    }
}


// update numboxes for min max  of each attributes/aggregator
void GUIAxes::checkMinsMaxsChanged(bool updateVal){
    
    for(int i = 0 ; i < 3 ; i++){
        
        if(attr[i]->getSelected().size()*aggr[i]->getSelected().size()>0  ){
            int idxAttr =getFullAttrIdx(attr[i]->getSelected()[0]->getName(),aggr[i]->getSelected()[0]->getName());
            
            max[i]->setMin(Container::mins[idxAttr] - Container::stddevs[idxAttr]);
            max[i]->setMax(Container::maxs[idxAttr]+ Container::stddevs[idxAttr]);
            
            min[i]->setMin(Container::mins[idxAttr] - Container::stddevs[idxAttr]);
            min[i]->setMax(Container::maxs[idxAttr]+ Container::stddevs[idxAttr]);
            

            if(updateVal){
                max[i]->setValue(Physics::maxs.get()[i]);
                min[i]->setValue(Physics::mins.get()[i]);
            }
        }
    }
    
}

// get Attr idx in Data world
int GUIAxes::getFullAttrIdx(const string & attr,const string &  aggr ){
    string attrN = attr+"."+aggr;
    bool found = false;
    for (vector<string> ::iterator it = Container::attributeNames.begin(); it!=Container::attributeNames.end(); ++it) {
        if(*it==attrN){
            found = true;
            break;
        }
        
    }
    if(!found){
        attrN = ofSplitString(attrN, ".")[0];
    }
    return ofFind(Container::attributeNames, attrN );;
}



bool GUIAxes::isAnAttributeList(ofxUIWidget * w){
    for(int i = 0 ; i < 3 ; i++){
        if(w == attr[i])return true;
    }
    return false;
}

int GUIAxes::getNumAxe(ofxUIWidget* w){
    for (int i = 0 ; i < 3 ; i++){
        if(w == attr[i]||
           w == aggr[i]||
           w == max[i]||
           w == min[i]||
           w == scaleType[i]
           
           ){
            return i;
        }
    }
    return -1;
}

string GUIAxes::numToAxe(int i){
    switch(i){
        case 0:
            return "X";
        case 1:
            return "Y";
        case 2:
            return "Z";
        default:
            break;
    }
    return "";
}




GUIAxes::GUIAxes(string name): GUICanvasBase(name){
    
    shouldUpdateAggregator = -1;
    

    setName("Axes");
    
    
    coordinateTypeNames.push_back("cartesian");
    coordinateTypeNames.push_back("cylindrical");
    coordinateTypeNames.push_back("Toroidal");
    coordinateTypeNames.push_back("spherical");
    
    //Hack for memory corruption
    attrNames.clear();
    aggrNames.clear();
    
    
    vector<string> dumb;
    
    typeScales.push_back("min/max");
    typeScales.push_back("standard deviation");
    typeScales.push_back("range");
    
    for(int i = 0 ; i < 3 ; i++){
        attr[i] =           new ofxUIDropDownList("Attribute"+numToAxe(i), dumb,150,0,0,OFX_UI_FONT_SMALL);
        aggr[i] =         new ofxUIDropDownList("Aggregate"+numToAxe(i), dumb,150,0,0,OFX_UI_FONT_SMALL);
        scaleType[i] =    new ofxUIDropDownList("scaleType"+numToAxe(i), typeScales,150,0,0,OFX_UI_FONT_SMALL);
        min[i] =          new ofxUINumberD(4,"min"+numToAxe(i),OFX_UI_FONT_SMALL);
        max[i] =          new ofxUINumberD(4,"max"+numToAxe(i),OFX_UI_FONT_SMALL);
        
    }
    
    coordinateType =  new ofxUIDropDownList("coordinateType",coordinateTypeNames,150,0,0,OFX_UI_FONT_SMALL);
    coordinateType->activateToggle("cartesian");
    
    clampValues = new ofxUIToggle("clampValues",false,10,10);
    
    
    // place
    
    
    
    for(int i=0;i<3;i++){
       
        addWidgetDown(attr[i]);
        addWidgetRight(aggr[i] );
        addWidgetRight(scaleType[i] );
        addWidgetRight(min[i] );
        addWidgetRight(max[i] );
    }
    addWidgetDown(coordinateType);
    addWidgetRight(clampValues);
    
    
    
    
    vector<ofxUIWidget*> ddls = getWidgetsOfType(OFX_UI_WIDGET_DROPDOWNLIST);
    for(int i = 0 ; i < ddls.size(); i++){
        ((ofxUIDropDownList*) ddls[i])->setAutoClose(true);
        ((ofxUIDropDownList*) ddls[i])->setShowCurrentSelected(true);
    }
    setDimensions(700, 140);
    
    
}


void GUIAxes::setup(){


    
    attrNames.clear();
    aggrNames.clear();

    if(Container::attributeNames.size()>0){
        
        for(vector<string>::iterator it = Container::attributeNames.begin() ; it != Container::attributeNames.end() ;++it){
            vector <string> nnn =ofSplitString(*it, ".");
            if(nnn.size()>0){
                attrNames.emplace(nnn[0]);
            }
            else{
                cout << "WTF : " << *it << endl;
            }
            if(nnn.size()>1){
                aggrNames.emplace(nnn[1]);
            }
            

        }
  
        for(int i = 0 ; i < 3 ; i++){
            
//            attr[i]->removeToggles();
            attr[i]->clearSelected();
            attr[i]->clearEmbeddedWidgets();
            attr[i]->clearToggles();

            {
            vector<string> tmp(attrNames.begin(),attrNames.end());
            attr[i]->addToggles(tmp);
            }
            attr[i]->setSingleSelected(i);
//            attr[i]->getToggles()[i]->setValue(true);
//            attr[i]->getToggles()[i]->triggerSelf();
            scaleType[i]->getToggles()[1]->triggerSelf();
            
        }
        
    }
    
    
    
    for(int i = 0 ; i < 3 ;i++){
    shouldUpdateAggregator = i;
    ofEventArgs dumbA;
    attr[i]->setSingleSelected(i);
    async(dumbA,true);
    }
    
    float ddSize = 100;
    


    
    coordinateType->triggerEvent(coordinateType->getToggles()[0]);
    
    for(int i = 0 ; i < 3;i++){
        attr[i]->close();
        aggr[i]->close();
        scaleType[i]->close();
    }


    
}


void GUIAxes::recievedMessage(ofMessage & msg){
//    if(getGUIMsgDest(msg) == "Axes"){
//        vector < string > args = getGUIMsgArgs(msg);
//        if(args[0] == "setLabels"){
//            int i = ofToInt(args[1]);
//            attr[i]->setLabelText(args[2]);
//            aggr[i]->setLabelText(args[2]);
//        }
//    }
    
 }


void GUIAxes::checkOverlapingDDL(ofxUIEventArgs & e){

    switch (e.getKind()) {
        case OFX_UI_WIDGET_DROPDOWNLIST:
        {bool hideothers = ((ofxUIDropDownList*)e.widget)->getValue();
            
            if(e.getCanvasParent()!=NULL){
                
                vector<ofxUIWidget*> vv = e.getCanvasParent()->getWidgetsOfType(OFX_UI_WIDGET_DROPDOWNLIST);
                for(vector<ofxUIWidget*>::iterator it = vv.begin() ; it !=vv.end() ; ++it){
                    if(e.widget->getRect()->x ==  (*it)->getRect()->x && e.widget->getRect()->y <  (*it)->getRect()->y &&(*it)!=e.widget){
                        
                        ((ofxUIDropDownList*)*it)->close();
                        ((ofxUIDropDownList*)*it)->setVisible(!hideothers);
                    }
                }
            }
        }
            break;
            
            
        default:
            break;
    }
    
}

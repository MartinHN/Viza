//
//  GUI.cpp
//  ViZa
//
//  Created by martin hermant on 14/10/14.
//
//

#include "GUI.h"


GUI * GUI::inst;


GUI::GUI(){
    int ch = 0;
    vector<string> dumb;
    dumb.push_back("lol");
    
    
    ///LOGGER///////////

    
    logCanvas = new ofxUISuperCanvas("Log",0,0,900,100,OFX_UI_FONT_SMALL);
    logCanvas->setName("Log");
    
    
    Logger = new ofxUITextArea("Logger","Log",900,0,0,0,OFX_UI_FONT_SMALL);
    Logger->setVisible(true);
    
    //AXES/////////////
    guiconf = new ofxUISuperCanvas("Axes",0,0,700,100);
    guiconf->setName("Axes");
    
    
    ch+=guiconf->getRect()->height+pad;
    
    vector<string> typescales;
    typescales.push_back("min/max");
    typescales.push_back("standard deviation");
    typescales.push_back("range");
    
    
    //Hack for memory corruption
    attrNames.clear();
    aggrNames.clear();
    for(int i = 0  ;i< 10 ; i++){
    attrNames.push_back(ofToString(i)+" attr");
    aggrNames.push_back(ofToString(i)+" aggr");

    }
    
    
    
    
    for(int i = 0 ; i < 3 ; i++){
        attr[i] =           new ofxUIDropDownList("Attribute"+numToAxe(i), attrNames,150,0,0,OFX_UI_FONT_SMALL);
        aggr[i] =         new ofxUIDropDownList("Aggregate"+numToAxe(i), aggrNames,150,0,0,OFX_UI_FONT_SMALL);
        scaleType[i] =    new ofxUIDropDownList("scaleType"+numToAxe(i), typescales,150,0,0,OFX_UI_FONT_SMALL);
        min[i] =          new ofxUITextInput("min"+numToAxe(i),"",50);
        max[i] =          new ofxUITextInput("max"+numToAxe(i),"",50);
        
    }
    
    
    
    //SONGSNAMES////////////////////
    scrollNames = new ofxUIScrollableCanvas(0,ch,scrollW,400);
    scrollNames->setName("Songs");
    scrollNames->setScrollableDirections(false, true);
    
    songNames = new ofxUIDropDownList("songNames",dumb ,0,0,0,OFX_UI_FONT_SMALL);
    
    scrollNames->addWidgetDown(songNames);
    scrollNames->setSnapping(true);
    
    
    
    
    //MIDI//////////
    midiCanvas = new ofxUISuperCanvas("Midi");
    midiCanvas->setName("Midi");
    
    
    midiPorts = new ofxUIDropDownList("MidiPorts", Midi::instance()->getPorts(),150,0,0,OFX_UI_FONT_SMALL);
    midiVel = new ofxUIRangeSlider("VelocityRange",0,1,0,1,100,10);
    
    midiRadius = new ofxUISlider("Radius",0,.5,0.05,100,10);
    midiHold = new ofxUIToggle("Hold",false,10,10);
    
    midiLink2Cam = new ofxUIToggle("link2Cam",true,10,10);
    
    //VIEWWWW/////////////
    
    viewCanvas = new ofxUISuperCanvas("View");
    viewCanvas->setName("View");
    
    alphaView = new ofxUISlider("alphaView",0,1,0.3f,100,10);
    selBrightest = new ofxUIToggle("SelectBrightest",false,10,10);
    linkSongs = new ofxUIToggle("linkSongs",false,10,10);
    orthoCam = new ofxUIToggle("orthoCam",true,10,10);
    pointSize = new ofxUISlider("pointSize",0,2,1,100,10);
    
    //// PLAYBACK /////////////
    playBack =new ofxUISuperCanvas("playBack");
    playBack->setName("playBack");
    
    continuousPB = new ofxUIToggle("continuousPlayBack",true,10,10);
    holdPB = new ofxUIToggle("hold",false,10,10);
    
    
    ///PLACING//////////////
    viewCanvas->addWidgetDown(alphaView);
    viewCanvas->addWidgetDown(selBrightest);
    viewCanvas->addWidgetDown(linkSongs);
    viewCanvas->addWidgetDown(orthoCam);
    viewCanvas->addWidgetDown(pointSize);
    
    
    midiCanvas->addWidgetDown(midiPorts);
    midiCanvas->addWidgetDown(midiVel);
    midiCanvas->addWidgetDown(midiRadius);
    midiCanvas->addWidgetDown(midiHold);
    midiCanvas->addWidgetDown(midiLink2Cam);

    logCanvas->addWidgetDown(Logger);
    
    
    for(int i=0;i<3;i++){
    guiconf->addWidgetDown(attr[i]);
    guiconf->addWidgetRight(aggr[i] );
    guiconf->addWidgetRight(scaleType[i] );
    guiconf->addWidgetRight(min[i] );
    guiconf->addWidgetRight(max[i] );
    }

    
    playBack->addWidgetDown(continuousPB);
    playBack->addWidgetDown(holdPB);
    
    
    //GLOBAL TAB
    global = new ofxUITabBar();
    
    global->setName("Global");
    global->addCanvas(scrollNames);
    global->addCanvas(guiconf);
    global->addCanvas(logCanvas);
    global->addCanvas(viewCanvas);
    global->addCanvas(midiCanvas);
    global->addCanvas(playBack);
    
    vector<ofxUIWidget*> ddls= guiconf->getWidgetsOfType(OFX_UI_WIDGET_DROPDOWNLIST);
    ddls.push_back(midiPorts);
    for(int i = 0 ; i < ddls.size(); i++){
        ((ofxUIDropDownList*) ddls[i])->setAutoClose(true);
        ((ofxUIDropDownList*) ddls[i])->setShowCurrentSelected(true);
    }
    
}


GUI::~GUI(){
//    delete global;
}

void GUI::setup(){
    

   
    
    
    
    attrNames.clear();
    aggrNames.clear();
    
    

    if(Container::attributeNames.size()>0){
        
        for(vector<string>::iterator it = Container::attributeNames.begin() ; it != Container::attributeNames.end() ;++it){
            vector <string> nnn =ofSplitString(*it, ".");
            {
                bool found = false;
                for(vector<string>::iterator itt = attrNames.begin() ; itt!= attrNames.end() ; ++itt){
                    if(*itt==nnn[0]){
                        found = true;
                        break;
                    }
                }
                if(!found)attrNames.push_back(nnn[0]);
            }
            if(nnn.size()>1 ){
                bool found = false;
                for(vector<string>::iterator itt = aggrNames.begin() ; itt!= aggrNames.end() ; ++itt){
                    if(*itt==nnn[1]){
                        found = true;
                        break;
                    }
                }
             if(!found)aggrNames.push_back(nnn[1]);
            }
        }
        float ddSize = 100;
        



            
          for(int i = 0 ; i < 3 ; i++){
            
//            attr[i]->clearEmbeddedWidgets();
            attr[i]->clearToggles();
            
            attr[i]->addToggles(attrNames);
            aggr[i]->clearToggles();
            aggr[i]->addToggles(aggrNames);

            attr[i]->getToggles()[i]->setValue(true);
            aggr[i]->getToggles()[0]->setValue(true);
            scaleType[i]->getToggles()[i==0?0:1]->setValue(true);
              
              
            scaleType[i]->getToggles()[i==0?0:1]->triggerSelf();
              attr[i]->getToggles()[i]->triggerSelf();
              aggr[i]->getToggles()[0]->triggerSelf();
            min[i]->setAutoClear(false);
            min[i]->setTriggerOnClick(false);
            max[i]->setAutoClear(false);
            max[i]->setTriggerOnClick(false);
        }


        
        
        songnames.clear();
        for(map<string,vector<Container*> > ::iterator it = Container::songs.begin() ; it!=Container::songs.end() ; ++it){
            songnames.push_back(it->first);

        }
        songNames->clearToggles();
        songNames->addToggles(songnames);
        
        
        
        
        
        
        
        ((ofxUIDropDownList*)scrollNames->getWidgetsOfType(OFX_UI_WIDGET_DROPDOWNLIST)[0])->open();
        ofxUIRectangle * r =((ofxUIDropDownList*)scrollNames->getWidgetsOfType(OFX_UI_WIDGET_DROPDOWNLIST)[0])->getRect();
        
        scrollNames->setDimensions(scrollW,songnames.size()*r->height);

    }


    for(int i = 0 ; i < 3;i++){
        attr[i]->getToggles()[i]->triggerSelf();
    }
    
    
    logCanvas->autoSizeToFitWidgets();
    

}



void GUI::registerListener(){
    
    map< ofxUIToggle*,ofxUICanvas* > w = global->canvases;
    for(map< ofxUIToggle*,ofxUICanvas* > ::iterator it = w.begin() ; it!=w.end() ; ++it){
//        it->second->setParent(global);
        ofAddListener(((ofxUICanvas*)(it->second))->newGUIEvent,this,&GUI::guiEvent);
    }
    

}

void GUI::guiEvent(ofxUIEventArgs &e){
    string name = e.getName();
	int kind = e.getKind();
    
    ofxUICanvas * root,*parent;


    
    root = (ofxUICanvas*)e.widget;
    
    while(root->getParent()!=NULL ){root= (ofxUICanvas*)root->getParent();}
    if(e.widget->getParent()!=NULL){ parent = (ofxUICanvas*)e.widget->getParent();}
    else{parent = root;}

    
    //Check modifications
    isModifiying = ofGetMousePressed();
    
    
    // hack for avoiding double hits on opened DDLists
    switch (kind) {
        case OFX_UI_WIDGET_DROPDOWNLIST:
        {bool hideothers = ((ofxUIDropDownList*)e.widget)->getValue();
            
            if(parent!=NULL){
                vector<ofxUIWidget*> vv = parent->getWidgetsOfType(OFX_UI_WIDGET_DROPDOWNLIST);
                for(vector<ofxUIWidget*>::iterator it = vv.begin() ; it !=vv.end() ; ++it){
                    if(e.widget->getRect()->x ==  (*it)->getRect()->x && e.widget->getRect()->y <  (*it)->getRect()->y &&((ofxUIDropDownList*)*it)!=e.widget){

                        ((ofxUIDropDownList*)*it)->setVisible(!hideothers);
                    }
                }
            }
        }
            break;
        
        
        default:
            break;
    }
    
    
    
    if(parent == NULL){
        cout << "orphan !!! : " <<e.widget->getName() << endl;
     return;
    }
    //ID for GUI Controls
    string rootName = root->getName();
    string parentName = parent->getName();
    
    
    
    if(root!=NULL){
     //cout << root->getName() << "//" << parent->getName() << "//" << name<< endl;
    }
    
    
    // Axes
    if(rootName == "Axes"){
    
    int axe = axeToNum(parentName[parentName.length()-1]);
    
    // attributes and aggregator modification
        if(axe!=-1 && attr[axe]->getSelected().size()>0 && aggr[axe]->getSelected().size()>0 && scaleType[axe]->getSelectedIndeces().size()>0){
            string attrtmp =attr[axe]->getSelected()[0]->getName();
            string aggrtmp = aggr[axe]->getSelected()[0]->getName();
            int scaletmp =scaleType[axe]->getSelectedIndeces()[0];
        Physics::orderBy(attrtmp+"."+aggrtmp, axe, scaletmp);
        }
    // mins maxs modifications
    else if (kind == OFX_UI_WIDGET_TEXTINPUT){
       axe = axeToNum(name[name.length()-1]);
        string s =((ofxUITextInput*)e.widget)->getTextString();
        if(s=="")return;
        ofVec3f mask(axe==0?1:0,axe==1?1:0,axe==2?1:0);
        if(name.substr(0,name.length()-1)=="min"){
            Physics::mins = ofToFloat(s)*mask + (-mask+ofVec3f(1))*Physics::mins;
        }
        if(name.substr(0,name.length()-1)=="max"){
            Physics::maxs = ofToFloat(s)*mask + (-mask+ofVec3f(1))*Physics::maxs;
        }
        scaleType[axe]->getToggles()[2]->setValue(true);
        scaleType[axe]->getToggles()[2]->triggerSelf();
        Physics::orderBy(attr[axe]->getSelected()[0]->getName()+"."+aggr[axe]->getSelected()[0]->getName(), axe, scaleType[axe]->getSelectedIndeces()[0]);
       
}
         checkMinsMaxsChanged();
    }
    
    // songs
else    if(rootName == "Songs" && parentName == "songNames"){
        Container::selectSong(name);
        
    }
else    if(rootName == "View" ){
    if(name == "alphaView"){
        Container::stateColor[0].a = ((ofxUISlider*)e.widget)->getValue();
        Physics::updateAllColors();
    }
    if(name == "linkSongs"){
        Physics::linksongs = ((ofxUIToggle*)e.widget)->getValue();
    }
    if(name == "orthoCam"){
        ofApp::setcamOrtho(((ofxUIToggle*)e.widget)->getValue());
    }
    if(name == "pointSize"){
        Container::radius = ((ofxUISlider*)e.widget)->getValue()*150.0;
        glPointSize(Container::radius);
    }
}
else    if(rootName == "Midi" ){
    if(parentName == "MidiPorts"){
        Midi::instance()->midiIn.closePort();
        Midi::instance()->midiIn.openPort(name);
    }
    if(name == "VelocityRange"){
        Midi::velScale.set(((ofxUIRangeSlider*)e.widget)->getValueLow(),((ofxUIRangeSlider*)e.widget)->getValueHigh());
    }
    if(name == "Radius"){
        Midi::radius = ((ofxUISlider*)e.widget)->getValue();
    }
    if(name=="Hold"){
        Midi::hold=((ofxUIToggle*)e.widget)->getValue();
    }
    if(name == "link2Cam"){
        Midi::link2Cam = ((ofxUIToggle*)e.widget)->getValue();
    }
    
    
}
else if (rootName=="playBack"){


    
    
}
    
    
    lastFramenum = ofGetFrameNum();
    
}

int GUI::axeToNum(char s){
    switch(s){
        case 'X':
            return 0;
        case 'Y':
            return 1;
        case 'Z':
            return 2;
        default:
            break;
    }
    return -1;
}
string GUI::numToAxe(int i){
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






void GUI::checkMinsMaxsChanged(){

    for(int i = 0 ; i < 3 ; i++){
        max[i]->setTextString(ofToString(Physics::maxs.get()[i],4));
        min[i]->setTextString(ofToString(Physics::mins.get()[i],4));
        
    }
    
}

void GUI::LogIt(string s){
//    cout << s <<"string" << endl;
    instance()->Logger->setTextString(s);

}

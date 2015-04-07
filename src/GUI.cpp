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
    ofAddListener(ofEvents().update, this, &GUI::update);
    vector<string> dumb;
    //    dumb.push_back("lol");
    
    coordinateTypeNames.push_back("cartesian");
    coordinateTypeNames.push_back("cylindrical");
    coordinateTypeNames.push_back("Toroidal");
    coordinateTypeNames.push_back("spherical");
    
    ///LOGGER///////////
    
    
    logCanvas = new ofxUISuperCanvas("Log",0,700,900,100,OFX_UI_FONT_SMALL);
    logCanvas->setName("Log");
    
    
    Logger = new ofxUITextArea("Logger","Log",900,0,0,0,OFX_UI_FONT_SMALL);
    Logger->setVisible(true);
    
    //AXES/////////////
    axes = new ofxUISuperCanvas("Axes",0,0,900,150);
    axes->setName("Axes");
    
    
    ch+=axes->getRect()->height+pad;
    
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
        min[i] =          new ofxUINumberDialer(0,1,0.0f,4,"min"+numToAxe(i),OFX_UI_FONT_SMALL);
        max[i] =          new ofxUINumberDialer(0,1,1.0f,4,"max"+numToAxe(i),OFX_UI_FONT_SMALL);
        
    }
    
    coordinateType =  new ofxUIDropDownList("coordinateType",coordinateTypeNames,150,0,0,OFX_UI_FONT_SMALL);
    coordinateType->activateToggle("cartesian");
    
    clampValues = new ofxUIToggle("clampValues",false,10,10);
    
    
    //Classes////////////////////
    
    
    classScroll = new ofxUIScrollableCanvas(0,0,scrollW,800);
    classScroll->setName("Class");
    classScroll->setScrollableDirections(false, true);
    
    classNamesDDList = new ofxUIDropDownList("ClassNames",dumb,0,0,0,OFX_UI_FONT_SMALL);
    classValueDDList = new ofxUIDropDownList("ClassValues",dumb ,0,0,0,OFX_UI_FONT_SMALL);
    
    classColor = new ofxUIToggle("ColorIt",false,10,10);
    
    classScroll->setSnapping(true);
    
    
    
    //// FITTER ///////////////
    fitterCanvas = new ofxUISuperCanvas("Fitter");
    fitterCanvas->setName("Fitter");
    samplingPct = new ofxUISlider("sampling%",.05,2,1,100,10);
    typeOfFit = new ofxUIDropDownList("typeOfFit",SliceFitter::DistanceType::types,150,0,0,OFX_UI_FONT_SMALL);
    keepResults = new ofxUIToggle("keep Results",false,10,10);
    fitIt = new ofxUIButton("FitIt",false,10,10);
    applyIt = new ofxUIButton("apply",false,10,10);
    
    ////////////////CLUSTER </////////////////////
    clusterCanvas = new ofxUISuperCanvas("Cluster");
    clusterCanvas->setName("Cluster");
    findClusters = new ofxUIButton("findClusters",false,10,10);
    clusterEps = new ofxUISlider("Epsilon",.01,.3,.05,100,10);
    clusterMinK = new ofxUISlider("MinK",2,100,10,100,10);
    minClusterSize = new ofxUISlider("minClusterSize",0,500,10,100,10);
    tSNE2D = new ofxUIToggle("tSNE2d",false,10,10);
    tSNEPerp = new ofxUISlider("tSNEPerplexity",2,65,5,100,10);
    tSNEtheta = new ofxUISlider("tSNEtheta",.0,.49,.2,100,10);
    findtSNE = new ofxUIButton("tSNE",false,10,10);
    applyTsne = new ofxUIButton("apply",false,10,10);
    
    
    
    //MIDI//////////
    midiCanvas = new ofxUISuperCanvas("Midi");
    midiCanvas->setName("Midi");
    
    
    midiPorts = new ofxUIDropDownList("MidiPorts", Midi::instance()->getPorts(),150,0,0,OFX_UI_FONT_SMALL);
    midiVel = new ofxUIRangeSlider("VelocityRange",0,1,0,1,100,10);
    
    midiRadius = new ofxUISlider("Radius",0,.5,0.05,100,10);
    midiHold = new ofxUIToggle("Hold",false,10,10);
    
    midiLink2Cam = new ofxUIToggle("link2Cam",true,10,10);
    link2x = new ofxUIToggle("link2X",false,10,10);
    
    //VIEWWWW/////////////
    
    viewCanvas = new ofxUISuperCanvas("View");
    viewCanvas->setName("View");
    
    alphaView = new ofxUISlider("alphaView",0,1,0.3f,100,10);
    selBrightest = new ofxUIToggle("SelectBrightest",false,10,10);
    linkClasses = new ofxUIToggle("linkClasses",false,10,10);
    orthoCam = new ofxUIToggle("orthoCam",true,10,10);
    pointSize = new ofxUISlider("pointSize",0,30,1,100,10);
    isClipping = new ofxUIToggle("isClipping",false,10,10);
    show2dViews = new ofxUIToggle("2dViews",false,10,10);
    fishEyeRadius = new ofxUISlider("fishEyeRadius",0.0,1.0,0.,100,10);
    fishEyeStrength = new ofxUISlider("fishEyeStrength",0.01,1,.5,100,10);
    //// PLAYBACK /////////////
    playBack =new ofxUISuperCanvas("playBack");
    playBack->setName("playBack");
    
    continuousPB = new ofxUIToggle("continuousPlayBack",true,10,10);
    holdPB = new ofxUIToggle("hold",false,10,10);
    
    
    ///PLACING//////////////
    
    
    classScroll->addWidgetDown(classNamesDDList);
    classScroll->addWidgetRight(classValueDDList);
    classScroll->addWidgetRight(classColor);
    
    viewCanvas->addWidgetDown(alphaView);
    viewCanvas->addWidgetDown(selBrightest);
    viewCanvas->addWidgetDown(linkClasses);
    viewCanvas->addWidgetDown(orthoCam);
    viewCanvas->addWidgetDown(pointSize);
    viewCanvas->addWidgetDown(isClipping);
    viewCanvas->addWidgetDown(show2dViews);
    viewCanvas->addWidgetDown(fishEyeRadius);
    viewCanvas->addWidgetDown(fishEyeStrength);
    
    midiCanvas->addWidgetDown(midiPorts);
    midiCanvas->addWidgetDown(midiVel);
    midiCanvas->addWidgetDown(midiRadius);
    midiCanvas->addWidgetDown(midiHold);
    midiCanvas->addWidgetDown(midiLink2Cam);
    midiCanvas->addWidgetDown(link2x);
    
    fitterCanvas->addWidgetDown(typeOfFit);
    fitterCanvas->addWidgetDown(samplingPct);
    fitterCanvas->addWidgetDown(keepResults);
    fitterCanvas->addWidgetDown(fitIt);
    fitterCanvas->addWidgetDown(applyIt);
    
    
    clusterCanvas->addWidgetDown(findClusters);
    
    clusterCanvas->addWidgetDown(clusterMinK);
    clusterCanvas->addWidgetDown(clusterEps);
    clusterCanvas->addWidgetDown(minClusterSize);
    clusterCanvas->addSpacer();
    clusterCanvas->addLabel("t-SNE");
    clusterCanvas->addSpacer();
    clusterCanvas->addWidgetDown(tSNEtheta);
    clusterCanvas->addWidgetDown(tSNEPerp);
    clusterCanvas->addWidgetDown(tSNE2D);
    clusterCanvas->addWidgetDown(findtSNE);
    clusterCanvas->addWidgetDown(applyTsne);
    
    clusterCanvas->setDimensions(clusterCanvas->getRect()->width, clusterCanvas->getRect()->height*1.3);
    logCanvas->addWidgetDown(Logger);
    
    
    for(int i=0;i<3;i++){
        axes->addWidgetDown(attr[i]);
        axes->addWidgetRight(aggr[i] );
        axes->addWidgetRight(scaleType[i] );
        axes->addWidgetRight(min[i] );
        axes->addWidgetRight(max[i] );
    }
    axes->addWidgetDown(coordinateType);
    axes->addWidgetRight(clampValues);
    
    
    
    playBack->addWidgetDown(continuousPB);
    playBack->addWidgetDown(holdPB);
    
    
    //GLOBAL TAB
    global = new ofxUITabBar();
    
    global->setName("Global");
    global->addCanvas(classScroll);
    global->addCanvas(axes);
    //    global->addCanvas(logCanvas);
    global->addCanvas(viewCanvas);
    global->addCanvas(fitterCanvas);
    global->addCanvas(clusterCanvas);
    global->addCanvas(midiCanvas);
    global->addCanvas(playBack);
    
    
    
    vector<ofxUIWidget*> ddls= axes->getWidgetsOfType(OFX_UI_WIDGET_DROPDOWNLIST);
    ddls.push_back(midiPorts);
    ddls.push_back(typeOfFit);
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
        
        
        coordinateType->triggerEvent(coordinateType->getToggles()[0]);
        
        
        for(int i = 0 ; i < 3 ; i++){
            attr[i]->clearToggles();
            attr[i]->addToggles(attrNames);
            aggr[i]->clearToggles();
            aggr[i]->addToggles(aggrNames);
            
            attr[i]->getToggles()[i]->setValue(true);
            aggr[i]->getToggles()[0]->setValue(true);
            scaleType[i]->getToggles()[i==0?0:1]->setValue(true);
            
            
            scaleType[i]->getToggles()[1]->triggerSelf();
            attr[i]->getToggles()[i]->triggerSelf();
            aggr[i]->getToggles()[0]->triggerSelf();
        }
        
        
        
        
        
        if(Container::getClassNames().size()>0){
            classValueDDList->setValue(true);
            classNamesDDList->clearToggles();
            vector <string> tmpName = Container::getClassNames();

            classNamesDDList->addToggles(tmpName);
            classNamesDDList->getToggles()[0]->setValue(true);
            classNamesDDList->getToggles()[0]->triggerSelf();

        }
        
        
        
        
        
        
        

        
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
        cout << root->getName() << "//" << parent->getName() << "//" << name<< endl;
    }
    
    
    
    // Axes
    if(rootName == "Axes"){
        
        int axe = axeToNum(parentName[parentName.length()-1]);
        
        
        
        
        // attributes and aggregator modification
        if(axe!=-1 && attr[axe]->getSelected().size()>0 && aggr[axe]->getSelected().size()>0 && scaleType[axe]->getSelectedIndeces().size()>0){
            cout << (parentName.find("Class", 0, 5)!=string::npos) << endl;
            if(parentName.find("Class", 0, 5)!=string::npos){
                Physics::orderByClass(name, axe);
                attr[axe]->setLabelText("Class");
                checkMinsMaxsChanged(name != "range" );
                
                
            }
            else{
                if(parentName.find("Attribute", 0, 9)!=string::npos){
                    
                    string attrtmp =attr[axe]->getSelected()[0]->getName();
                    string oldAggr =aggr[axe]->getSelected()[0]->getName();
                    aggr[axe]->clearToggles();
                    vector<string> newAggr = Container::getAggregators(attrtmp);
                    int idx = ofFind(newAggr, oldAggr);
                    if(idx == newAggr.size())idx = 0;
                    aggr[axe]->addToggles(newAggr);
                    aggr[axe]->getToggles()[idx]->setValue(true);
                    aggr[axe]->getToggles()[idx]->triggerSelf();
                    
                }
                else {
                string attrtmp =attr[axe]->getSelected()[0]->getName();
                string aggrtmp = aggr[axe]->getSelected()[0]->getName();
                    if(aggrtmp=="None"){
                        aggrtmp = "";
                    }
                int scaletmp =scaleType[axe]->getSelectedIndeces()[0];
                Physics::orderByAttributes(attrtmp+"."+aggrtmp, axe, scaletmp);
                    checkMinsMaxsChanged(name != "range"  );
                }
                
                
            }
            
            
        }
        
        
        // mins maxs modifications
        else if (kind == OFX_UI_WIDGET_NUMBERDIALER){
            axe = axeToNum(name[name.length()-1]);
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
        
    }
    
    //CLASSES ////////////////////////////////
    else    if(rootName == "Class"){
        
        if(parentName == "ClassNames"){
            updateClassValue = true;
        }
        else if(parentName == "ClassValues"){
            Container::selectClass(name =="None"?"":classNamesDDList->getSelectedNames()[0],name);
            
        }
        else if(e.widget == classColor ){

                Container::ClassValueStruct *curV = &Container::classeMap[classNamesDDList->getSelectedNames()[0]];
            int idx =0;
                for(Container::ClassValueStruct::iterator it = curV->begin() ; it != curV->end(); ++it ){
                    ofColor nC;
                    if(classColor->getValue())
                        nC = ofColor(Container::getColorForId(idx),255*alphaView->getValue());
                    else
                        nC = ofColor(ofColor::white,255*alphaView->getValue());

                    for(int i = 0 ; i < it->second.size() ; i++){
                        Physics::updateOneColor(it->second[i],nC);
                    }
                    idx++;
                }
            
        }
    }
    
    // View
    else    if(rootName == "View" ){
        
        if(e.widget == alphaView){
            Container::stateColor[0].a = pow((alphaView)->getValue(),2);
            Physics::updateAllColorsAlpha();
        }
        if(e.widget == linkClasses){
            Physics::linkClasses = linkClasses->getValue();
        }
        if(e.widget == orthoCam){
            ofApp::cam.setcamOrtho(orthoCam->getValue());
        }
        if(e.widget == pointSize){
            Container::radius = pointSize->getValue();
            //            glPointSize(Container::radius);
        }
        if(e.widget == show2dViews){
            
            Camera::mainCam->setRelativeViewPort(0, 0,show2dViews->getValue()? .75:1, 1);
            Camera::mainCam->updateViewPort();
            Camera::setSecondaryVisible(show2dViews->getValue());
            Physics::updateVScreen();
            
        }
    }
    
    // FITTER ///////////////
    
    else if(rootName == "Fitter"){
        if(e.widget == keepResults){
            SliceFitter::i()->keepResult = keepResults->getValue();
        }
        else if (e.widget == typeOfFit){
            if(typeOfFit->getSelectedIndeces().size()>0){
                SliceFitter::i()->type.idx = typeOfFit->getSelectedIndeces()[0];
                cout << SliceFitter::i()->type.idx << endl;
            }
        }
        else if(e.widget == samplingPct){
            SliceFitter::i()->samplePct = samplingPct->getValue();
        }
        else if(e.widget == fitIt && !fitIt->getValue()){
            if(SliceFitter::i()->fitThread.isThreadRunning()){
                SliceFitter::i()->fitThread.fitter->forceStop();
            }
            else
                SliceFitter::i()->fitFor();
        }
        else if (e.widget == applyIt && !applyIt->getValue()){
            Physics::applyEquation(SliceFitter::i()->fitEquation);
            for(int i = 0 ; i < 3 ; i++){
                aggr[i]->setLabelText("FitterEq");
                attr[i]->setLabelText("FitterEq");
            }
        }
        
        
    }
    else if (rootName == "Cluster"){
        if(e.widget == findClusters && !findClusters->getValue()){
            vector<int> classes(Physics::vs.size(),0);
            Physics::kNN.dbscan(&classes,  clusterMinK->getValue(), clusterEps->getValue(),minClusterSize->getValue());
            //            Physics::kNN.medoids(&classes,  clusterMinK->getValue(), clusterEps->getValue(),minClusterSize->getValue());
            

            Container::classeMap["Cluster"].clear();
            
            for(int i = 0 ; i < classes.size();i++){
                
                if(abs(classes[i])>0){
                    Physics::updateOneColor(i, ofColor(Container::getColorForId(abs(classes[i])-1),255*alphaView->getValue()));
                    
                }
                else{
                    Physics::updateOneColor(i, ofColor(ofColor::white,5));
                }
                Container::containers[i]->setClass("Cluster", ofToString(abs(classes[i])));
            }
            
            cout << Container::classeMap["Cluster"].size() << endl;
            Physics::updateVScreen();
            //       update Class DDL if needed
            
            
        }
        
        
        
        
        if(e.widget == findtSNE && !findtSNE->getValue()){
            
            cout << Container::normalizedAttributes.size() << endl;
            for(int i = 0 ; i < 3 ;i++){
            aggr[i]->setLabelText("tSNE");
            attr[i]->setLabelText("tSNE");
            }
            int dim = tSNE2D->getValue()?2:3;
            if(ofxTSNE::i()->isThreadRunning()){
            ofxTSNE::i()->stopThread();
            }
            else{
            ofxTSNE::i()->init(&Container::normalizedAttributes[0], Container::attrSize, Container::containers.size(), tSNEtheta->getValue(), tSNEPerp->getValue(),dim);
            ofxTSNE::i()->startThread();
            }
        }
        else if (e.widget == applyTsne && !applyTsne->getValue()){
            if(Physics::applyFit()){
            for(int i = 0 ; i < tSNE2D->getValue()?2:3 ;i++){
                aggr[i]->setLabelText("tSNE");
                attr[i]->setLabelText("tSNE");
            }
            }
        }
    }
    
    // Midi
    
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
        if(e.widget == link2x ){
            if(link2x->getValue())
                Midi::midiModulo = Physics::maxs->x - Physics::mins->x + 1;
            else{
                Midi::midiModulo = 12;
            }
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






void GUI::checkMinsMaxsChanged(bool updateVal){
    
    for(int i = 0 ; i < 3 ; i++){
        
        bool found = false;
        if(attr[i]->getSelected().size()*aggr[i]->getSelected().size()>0  ){
            string attrN = attr[i]->getSelected()[0]->getName()+"."+aggr[i]->getSelected()[0]->getName();
            for (vector<string> ::iterator it = Container::attributeNames.begin(); it!=Container::attributeNames.end(); ++it) {
                if(*it==attrN){
                    found = true;
                    break;
                }
                
            }
            if(!found){
                attrN = ofSplitString(attrN, ".")[0];
            }
            int idxAttr = ofFind(Container::attributeNames, attrN );
            max[i]->min = Container::mins[idxAttr] - Container::stddevs[idxAttr];
            max[i]->max = Container::maxs[idxAttr]+ Container::stddevs[idxAttr];
            min[i]->min = Container::mins[idxAttr]- Container::stddevs[idxAttr];
            min[i]->max = Container::maxs[idxAttr]+ Container::stddevs[idxAttr];
            cout << "dbg minmax : " << idxAttr << ":" << max[i]->min << "/" << max[i]->max << "up" << updateVal << Physics::maxs.get()[i]<<endl;
            if(updateVal){
                max[i]->setValue(Physics::maxs.get()[i]);
                min[i]->setValue(Physics::mins.get()[i]);
            }
        }
    }
    
}

void GUI::LogIt(string s){
    //    cout << s <<"string" << endl;
    instance()->Logger->setTextString(s);
    
}

bool GUI::isOver(int x,int y){
    bool res = global->isHit(x,y);
    
    ofxUICanvas * c = global->getActiveCanvas();
    if(c)res |= c->isHit(x, y);
    res |= logCanvas->isHit(x,y);
    return res;
    //    global->getActiveCanvas();
}

void GUI::asyncGUI(){
    if(updateClassValue){
        classValueDDList->clearToggles();
        vector<string> tmpC = Container::getClassValues(classNamesDDList->getSelected()[0]->getName());
        if(tmpC.size() < 1000){
        tmpC.insert(tmpC.begin(), "None");
        classValueDDList->addToggles(tmpC);
        
        ((ofxUIDropDownList*)classScroll->getWidgetsOfType(OFX_UI_WIDGET_DROPDOWNLIST)[0])->open();
        ofxUIRectangle * r =((ofxUIDropDownList*)classScroll->getWidgetsOfType(OFX_UI_WIDGET_DROPDOWNLIST)[0])->getRect();
        
        classScroll->setDimensions(scrollW,(MAX(classValueDDList->getToggles().size(),classNamesDDList->getToggles().size())+2)*r->height);
        }
        updateClassValue = false;
    }
}


void GUI::update(ofEventArgs &a){
    asyncGUI();
}
//
//  GUILoad.cpp
//  ViZa
//
//  Created by martin hermant on 24/04/15.
//
//

#include "GUILoad.h"

#include "AudioExtractor.h"

GUILoad::GUILoad(string name): GUICanvasBase(name){
    
    
    //VIEWWWW/////////////
    
    setName("Load");
    
    
    loadAnal = new ofxUIButton("Load",false,10,10);
    savePos = new ofxUIButton("Save Position",false,10,10);
    loadPos = new ofxUIButton("Load Position",false,10,10);
    splice = new ofxUIToggle("Slice It",false,10,10);
    onsetThreshold = new ofxUINumberDialer(0,15,6,1,"onsetThreshold",OFX_UI_FONT_SMALL);
    
    vector<string> dumb;
    dumb.push_back("Harmonic");
    dumb.push_back("Timbral");
    dumb.push_back("LowLevel");
    dumb.push_back("Mixed");
    
    types = new ofxUIDropDownList(80, "spaceType", dumb, OFX_UI_FONT_SMALL);


    // placing
    
    addWidgetDown(loadAnal);
    addWidgetRight(new ofxUISpacer(40,10));
    addWidgetRight(splice);
    addWidgetRight(new ofxUISpacer(64,10));
    addWidgetRight(savePos);
    
    addWidgetDown(types);
    addWidgetRight(onsetThreshold);
    addWidgetRight(loadPos);

    

    
//    types->setSingleSelected(0);
    types->setAutoClose(true);
    types->setShowCurrentSelected(false);

    autoSizeToFitWidgets();
    //    ofAddListener(this->newGUIEvent, this, &GUILoad::guiEvent);
    
}
void GUILoad::guiEvent(ofxUIEventArgs & e){
    if (e.widget == splice){
        SimpleEssentiaExtractor::spliceIt = e.getBool();
    }
    
    if(e.widget == onsetThreshold){
        SimpleEssentiaExtractor::onsetThresh = onsetThreshold->getValue();
    }
    
    
    if(e.widget == types){
        if(types->getSelected().size() && !e.getBool()){
            string t = types->getSelected()[0]->getName();
            if(t == "Harmonic"){
                AudioExtractor::type = AudioExtractor::HPCP;
            }
            else if (t == "Timbral") {
                AudioExtractor::type = AudioExtractor::MFCC;
            }
            else if( t == "LowLevel"){
                AudioExtractor::type = AudioExtractor::lowLevel;
            }
            else if( t == "Mixed"){
                AudioExtractor::type = AudioExtractor::MIXED;
            }
        }
    }
    // release only behaviours
    
    
    if(e.getKind()!= OFX_UI_WIDGET_BUTTON  || e.getBool()){return;}
    
    ofLogWarning("GUI") << "event from : " <<e.getKind() << " / " <<  e.getName() << " / " << e.getParentName() << " / " << e.getBool() ;
    
    if(e.widget == loadAnal){
        ofFileDialogResult f = ofSystemLoadDialog("Folder to load",true);
        if(f.bSuccess)
            FileImporter::loadAnalysisFiles(f.filePath);
    }
    if(e.widget == savePos){
        FileImporter::i()->savePosition();
    }
    if(e.widget == loadPos){
        FileImporter::i()->loadPosition();
    }

    
    
    
}


void GUILoad::setup(){

    
}
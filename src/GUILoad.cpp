//
//  GUILoad.cpp
//  ViZa
//
//  Created by martin hermant on 24/04/15.
//
//

#include "GUILoad.h"

#include "SimpleEssentiaExtractor.h"

GUILoad::GUILoad(string name): GUICanvasBase(name){
    
     
    //VIEWWWW/////////////
    
    setName("Load");
    
    
    loadAnal = new ofxUIButton("Analysis",false,10,10);
    save = new ofxUIButton("Save",false,10,10);
    splice = new ofxUIToggle("Splice",false,10,10);
    onsetThreshold = new ofxUINumberDialer(0,15,6,1,"onsetThreshold",OFX_UI_FONT_SMALL);
    
    // placing
    
    addWidgetDown(loadAnal);
    addWidgetDown(save);
    addWidgetDown(splice);
    addWidgetDown(onsetThreshold);

//    ofAddListener(this->newGUIEvent, this, &GUILoad::guiEvent);
    
}
void GUILoad::guiEvent(ofxUIEventArgs & e){
    if (e.widget == splice){
        SimpleEssentiaExtractor::spliceIt = e.getBool();
    }
    
    if(e.widget == onsetThreshold){
        SimpleEssentiaExtractor::onsetThresh = onsetThreshold->getValue();
    }
    
    // release only behaviours
    
    
    if(e.getKind()!= OFX_UI_WIDGET_BUTTON  || e.getBool()){return;}
   
    ofLogWarning("GUI") << "event from : " <<e.getKind() << " / " <<  e.getName() << " / " << e.getParentName() << " / " << e.getBool() ;
    
    if(e.widget == loadAnal){
        ofFileDialogResult f = ofSystemLoadDialog("analysisFiles",true);
        if(f.bSuccess)
            FileImporter::loadAnalysisFiles(f.filePath);
    }
    if(e.widget == save){
        FileImporter::i()->save();
    }



}


void GUILoad::setup(){
    
    
}
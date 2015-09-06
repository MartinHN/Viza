//
//  GUILoad.cpp
//  ViZa
//
//  Created by martin hermant on 24/04/15.
//
//

#include "GUILoad.h"



GUILoad::GUILoad(string name): GUICanvasBase(name){
    
     
    //VIEWWWW/////////////
    
    setName("Load");
    
    
    loadAnal = new ofxUIButton("Analysis",false,10,10);
    save = new ofxUIButton("Save",false,10,10);
    
    
    
    // placing
    
    addWidgetDown(loadAnal);
    addWidgetDown(save);


//    ofAddListener(this->newGUIEvent, this, &GUILoad::guiEvent);
    
}
void GUILoad::guiEvent(ofxUIEventArgs & e){
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
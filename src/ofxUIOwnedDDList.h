//
//  ofxUIOwnedDDList.h
//  ViZa
//
//  Created by martin hermant on 03/09/15.
//
//

#ifndef __ViZa__ofxUIOwnedDDList__
#define __ViZa__ofxUIOwnedDDList__

#include "ofxUIDropDownList.h"


class ofxUIOwnedDDList : public ofxUIDropDownList{
    
    public :
    ofxUIOwnedDDList(string _name, vector<string> items, float w = 0, float x = 0, float y = 0, int _size = OFX_UI_FONT_MEDIUM):ofxUIDropDownList(_name, items,w,x,y,_size){
        
    }
    

    
    
};
#endif /* defined(__ViZa__ofxUIOwnedDDList__) */

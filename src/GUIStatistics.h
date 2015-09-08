//
//  GUIStatistics.h
//  ViZa
//
//  Created by martin hermant on 22/04/15.
//
//

#ifndef __ViZa__GUIStatistics__
#define __ViZa__GUIStatistics__

#include "GUICanvasBase.h"

#ifdef PI
#undef PI
#include "Statistics.h"
#endif
#include "ofMain.h"

class GUIStatistics : public GUICanvasBase {
public:
    
    GUIStatistics(string name);
    ~GUIStatistics(){};
    
   
    void setup();
    
    
    void guiEvent(ofxUIEventArgs & e);
    void async(ofEventArgs & a);
    
private:
    
    //
    //    ofxUIScrollableCanvas * classNamesScroll;
    //    ofxUIScrollableCanvas * classValueScroll;
    ofxUIButton * getPCA;
    ofxUIScrollableList * statisticList;
    

    string getFormattedStat(int atId);

    int scrollW = 700;
};


#endif /* defined(__ViZa__GUIStatistics__) */

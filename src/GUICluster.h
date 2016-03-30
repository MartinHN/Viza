//
//  GUICluster.h
//  ViZa
//
//  Created by martin hermant on 24/04/15.
//
//

#ifndef __ViZa__GUICluster__
#define __ViZa__GUICluster__

#include "GUICanvasBase.h"

#include "ofxTSNE.h"


class GUICluster:  public GUICanvasBase{
    public :

    
    GUICluster(string name);
    
    ofxUIButton * findClusters;
    ofxUISlider *clusterEps;
    ofxUISlider * clusterMinK;
    ofxUISlider * minClusterSize;
    ofxUIToggle * tSNE2D;
    ofxUISlider * tSNEPerp;
    ofxUISlider * tSNEtheta;
    ofxUIButton * findtSNE;
    ofxUIToggle * tSNEfixSeed;
    ofxUIButton * applyTsne;
    
    
    ofEvent<ofMessage> * sharedGUIEvent;
    void init() override;
    void setup() override {};
    
    void guiEvent(ofxUIEventArgs & a);
    
};

#endif /* defined(__ViZa__GUICluster__) */

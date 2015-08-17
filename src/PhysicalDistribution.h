//
//  PhysicalDistribution.h
//  ViZa
//
//  Created by martin hermant on 20/05/15.
//
//

#ifndef __ViZa__PhysicalDistribution__
#define __ViZa__PhysicalDistribution__

#include "ofThread.h"
#include "Physics.h"

class PhysicalDistribution :public ofThread{
public:
    
    PhysicalDistribution(){ofAddListener(ofEvents().update, this, &PhysicalDistribution::update);kNN = new ofxNearestNeighbour3D();};
    
    static  PhysicalDistribution * i(){if(instance == NULL){instance = new PhysicalDistribution();}return instance;};
    
    float minDist = 5;
    float kSpring = 0.0002;
    
    
    
    int maxIterations = 10000;
    
    void threadedFunction();
    
    void update(ofEventArgs & e);
    
    void init();
    void updatePos();
    
    private :
    
    vector<ofVec3f> cache,origins;

    
    
    
    float minClusterSize = 1;
    float minK = 2, Eps = 0.1;
    int iterations;
    static PhysicalDistribution * instance;
    bool ended = false;
    
    ofxNearestNeighbour3D * kNN;
};
#endif /* defined(__ViZa__PhysicalDistribution__) */

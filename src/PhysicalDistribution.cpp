//
//  PhysicalDistribution.cpp
//  ViZa
//
//  Created by martin hermant on 20/05/15.
//
//

#include "PhysicalDistribution.h"


PhysicalDistribution * PhysicalDistribution::instance = NULL;


void PhysicalDistribution::threadedFunction(){
    
    while (!ended && isThreadRunning()){
        updatePos();
    }
    
}

void PhysicalDistribution::init(){
    ended = false;
    
    iterations = 0;
    cache = Physics::vs;
    origins = cache;
    //    classesCache.resize(cache.size());
    cout << minDist <<";" << kSpring <<  endl;
}
void PhysicalDistribution::updatePos(){
    
    kNN->buildIndex(cache);
    
    
    bool stable = true;
    for(int i =0 ; i< cache.size() ; i++){
        vector<pair<size_t, float> > matches;
        kNN->findPointsWithinRadius(cache[i], minDist/100.0, matches);
        ofVec3f globalForce(0);
        for(int j = 0 ; j < matches.size();j++){
            float dist =  sqrt(matches[j].second);
            if(dist>0){
                ofVec3f p2 = cache[matches[j].first];
                
                ofVec3f norm = (p2-cache[i])/dist;
                float force = (dist - minDist/100.0) * kSpring;
                globalForce+=force*norm;
                
                stable = false;
            }
        }
        for(int j = 0 ; j < 2 ; j++){
            if(cache[i][j]>0.45){globalForce[j] -= 4*kSpring*(cache[i][j]-0.45);};
            if(cache[i][j]<-0.45){globalForce[j] += 4*kSpring*(-0.45-cache[i][j]);};
        }
        
        globalForce-= (cache[i]-origins[i])*kSpring/MAX(2,2*matches.size());
        cache[i]+=globalForce;
    }
    
//    if(stable == true || iterations> maxIterations){
//        ended = true;
//    }
    
    iterations++;
   
    
}


void PhysicalDistribution::update(ofEventArgs &e){
    if(isThreadRunning()){
        Physics::setFits(cache);
    }
    

    
}





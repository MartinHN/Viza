//
//  ofxTSNE.h
//  ViZa
//
//  Created by martin hermant on 29/01/15.
//
//


#ifndef __ViZa__ofxTSNE__
#define __ViZa__ofxTSNE__

#include "tsne.h"
#include "ofMain.h"
#include <Accelerate/Accelerate.h>


class ofxTSNE{
    TSNE* tsne;
    
    int nData,dimData,outDim;
    float theta,perplexity;
    double *inVec = NULL;
    double *outVecCache = NULL;

    
    static ofxTSNE* instance;

public:
    ofxTSNE(){
        tsne = new TSNE();
    }
    
    ~ofxTSNE(){
        delete(tsne);
        free(inVec);
        free(outVecCache);
    }
    void init(float * v, int dim,int nelem,float _theta,float _perp,int _outDim);
    
    
    double* run();
    
    static ofxTSNE* i(){
        if(instance == NULL){
            instance = new ofxTSNE();
        }
        return instance;
    }
    
    
    
    
};



#endif /* defined(__ViZa__ofxTSNE__) */

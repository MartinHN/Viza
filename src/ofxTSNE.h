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
#include "DSP.h"


class ofxTSNE : public ofThread{
    
    
    TSNE* tsne;
    
    int nData,dimData,outDim;
    float theta,perplexity;
    vector<double> inVec;
    double *outVecCache = NULL;
    bool fixSeed = false;
    
    static ofxTSNE* instance;
    
    
    Matrix<float,3,Dynamic> cache;
    
    typedef void (*CallbackType)(double*);
    CallbackType cb_func;

public:
    ofxTSNE(){
        tsne = new TSNE();
        ofAddListener(ofEvents().update, this, &ofxTSNE::update);
    }
    
    ~ofxTSNE(){
        delete(tsne);

        free(outVecCache);
    }
    void init(Realv * v, int dim,int nelem,float _theta,float _perp,int _outDim,bool fixSeed);
    
    
    void threadedFunction();
    
    static ofxTSNE* i(){
        if(instance == NULL){
            instance = new ofxTSNE();
        }
        return instance;
    }
    
    
    bool hasStopped;
    
    void update(ofEventArgs & a);
    
};



#endif /* defined(__ViZa__ofxTSNE__) */

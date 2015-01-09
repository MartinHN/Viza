//
//  SliceFitter.h
//  ViZa
//
//  Created by martin hermant on 16/12/14.
//
//

#ifndef __ViZa__SliceFitter__
#define __ViZa__SliceFitter__

#include <iostream>
#include "ofMain.h"
#include "ofxNonLinearFit.h"
#include "SliceModel.h"
#include "Algorithm.h"

#include "FitEquation.h"



#define MAX_FIT_TIME 30



class FitThread : public ofThread{
public:    
    typedef ofxNonLinearFit::Models::SliceModel Model;
    typedef  std::pair<float,vector<ofVec3f> > Result;
    typedef ofxNonLinearFit::Models::SliceModel::DataPoint DataPoint;
    typedef vector<DataPoint> Dataset;
    typedef ofxNonLinearFit::Fit<Model> Fitter;

    Model *  model;
    
    Dataset  dataset;
    Fitter * fitter;
    
    
    
    bool ended = false;
    
    void init(){
        if(!fitter){
         fitter = new Fitter(ofxNonLinearFit::Algorithm(nlopt::LN_BOBYQA, ofxNonLinearFit::Algorithm::LocalGradientless),model->getParameterCount());
        }
        fitter->ignoredParams.clear();
    }
    
    void clear(){
        
        ended = false;
        delete model;
        model = NULL;
        delete fitter;
        fitter = NULL;
    }
    
    void threadedFunction()
    {
        ended = FALSE;
        //reset model parameters
//        model->initialiseParameters();
        init();
        
        double residual;

        bool success = fitter->optimise(*model, &dataset, &residual);
        
        
        ended = true;
        
    }
};

class SliceFitter {

    public:
    
    
    SliceFitter();
    ~SliceFitter();
    FitThread fitThread;

    
    void fitFor(float t = MAX_FIT_TIME);
    
    void update( ofEventArgs & a);
    
    
    void outPointsReshape();

    
    
    vector<ofVec3f> outPoints;
    vector<float> curParams;
    
    FitEquation fitEquation;
    
};



#endif /* defined(__ViZa__SliceFitter__) */

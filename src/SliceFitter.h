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





#define MAX_FIT_TIME 100.3


class FitThread : public ofThread{
public:    
    typedef ofxNonLinearFit::Models::SliceModel Model;
    typedef  std::pair<float,vector<ofVec3f> > Result;
    typedef ofxNonLinearFit::Models::SliceModel::DataPoint DataPoint;
    typedef vector<DataPoint> Dataset;
    typedef ofxNonLinearFit::Fit<Model> Fitter;

    Model model;
    
    Dataset dataset;
    Fitter * fitter;
    
    
    void threadedFunction()
    {

        //reset model parameters
        model.initialiseParameters();
        vector<ofxNonLinearFit::Algorithm> listOfAlgorithms;
        listOfAlgorithms = ofxNonLinearFit::Algorithm::ListAll();
        for(int i =0;i<listOfAlgorithms.size() ; i++){cout<<listOfAlgorithms[i].toString() <<endl;}
        fitter = new Fitter(ofxNonLinearFit::Algorithm(nlopt::LN_BOBYQA, ofxNonLinearFit::Algorithm::LocalGradientless),model.size);
        double residual;

        bool success = fitter->optimise(model, &dataset, &residual);
        
        
        delete fitter;
        
    }
};

class SliceFitter {

    public:
    
    
    SliceFitter();
    ~SliceFitter();
    vector<ofxNonLinearFit::Algorithm> listOfAlgorithms;
    float startTime,endTime,maxFitTime;
    bool startThread=false;
    FitThread fitThread;

    
    void fitFor(float t = MAX_FIT_TIME);
    
    void update( ofEventArgs & a);
    
    
    
    
};

#endif /* defined(__ViZa__SliceFitter__) */

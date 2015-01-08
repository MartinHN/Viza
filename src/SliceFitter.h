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





#define MAX_FIT_TIME 30


class FitEquation{
    
public:
    typedef map<float,int> eqStruct;
    
    eqStruct equation[3];
    vector<string> paramNames;
    
    

    
    eqStruct getTruncated(float min,int axe){
        eqStruct res ;
        for(eqStruct::iterator it = equation[axe].begin() ; it !=equation[axe].end() ; ++it){
            if(it->first>min){
                res[it->first] = it->second;
            }
        }
        return res;
    };
    eqStruct getFirsts(int num,int axe){
        eqStruct res ;
        
        if(num <=0)num = equation[axe].size();
        int i =0;
        for(eqStruct::reverse_iterator it = equation[axe].rbegin() ; i <num && it!=equation[axe].rend() ; ++it,i++){
            
            res[it->first] = it->second;
            
        }
        return res;
    }
    
    void clear(){
        for(int i = 0;i<3 ; i++){
            equation[i].clear();
        }
    }
    
    string toString(int axe,int firsts){

        string res;
        
        eqStruct curEq;
        curEq = getFirsts(firsts, axe);
        for(eqStruct::iterator it = curEq.begin() ; it != curEq.end() ; ++it){
            res += ofToString((float)it->first) + "x";
            res += paramNames[it->second];

            res+= " +\t\t";
            
        }
        if(res!="")res = res.substr(0 ,res.length() - 4);

        return res;
    }
    string toString(int firsts){
        string res ;
        for(int i = 0 ; i < 3 ; i++){
            res += toString(i,firsts);
            res+="\n";
        }
        return res;
    }
    
};


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
        if(!fitter)fitter = new Fitter(ofxNonLinearFit::Algorithm(nlopt::LN_NEWUOA, ofxNonLinearFit::Algorithm::GlobalGradient),model->getParameterCount());
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
//    float startTime,endTime,maxFitTime;
//    bool startThread=false;
    FitThread fitThread;

    
    void fitFor(float t = MAX_FIT_TIME);
    
    void update( ofEventArgs & a);
    
    
    void outPointsReshape();

    
    
    vector<ofVec3f> outPoints;
    vector<float> curParams;
    
    FitEquation fitEquation;
    
};

#endif /* defined(__ViZa__SliceFitter__) */

//
//  SimpleEssentiaExtractor.h
//  esstst
//
//  Created by martin hermant on 06/05/15.
//  Copyright (c) 2015 martin hermant. All rights reserved.
//

#ifndef __esstst__SimpleEssentiaExtractor__
#define __esstst__SimpleEssentiaExtractor__

#include "EssentiaExtractorBase.h"



//using namespace essentia ;
//
//using namespace streaming ;
//
//using namespace std;


class SimpleEssentiaExtractor : public EssentiaExtractorBase {
public:
    
    //////////////////////////////////////////
    // types
    
    typedef struct{
        string name;
        string inputName ;
        vector<string> outputs;
        ParameterMap parameters;
        pair<int, int> framecut = pair<int,int>(0,0);
    }ExtractorMap;
    
    typedef vector<ExtractorMap> ExtractorInfos ;
    
    
    ////////////////////////////////
    // constructor
    
    SimpleEssentiaExtractor( ExtractorInfos & audioF):EssentiaExtractorBase(){
        //configure Algorithms
        audioFunctions = audioF;
    };
    // TODO : Destroy everything
    ~SimpleEssentiaExtractor(){
    };
    
    
    
    void aggregate() ; 
    
    ///////////////////////////////////////////////
    // members
    
    ExtractorInfos audioFunctions;
    
protected :
    // overridable function
    // mandatory
    
    void configureIt() override ;
    void createNetwork() override ;
    

    
    
    vector<essentia::streaming::Algorithm *> audioAlgos;
    
    
    vector<essentia::streaming::Algorithm*>  FC;
    
    

    
    
    
    
};








#endif /* defined(__esstst__SimpleEssentiaExtractor__) */

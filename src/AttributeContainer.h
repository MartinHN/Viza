//
//  AttributeContainer.h
//  ViZa
//
//  Created by martin hermant on 23/01/15.
//
//

#ifndef __ViZa__AttributeContainer__
#define __ViZa__AttributeContainer__

#include "DSP.h"
#include "ofMain.h"

#include "eigen3/Eigen/Core"
#include "eigen3/Eigen/Eigen"


class Container;
using namespace Eigen;

class AttributeContainer{
    
public:
    
    static vector<string> attributeNames;
    static MatrixXd  normalizedAttributes;

    static MatrixXd attributesCache;
    
    static MatrixXd reducedAttributeCache;
    static vector<int> reducedAttributesNamesIdx;
    
    
    
    static int attrSize;
    static int totalAttr;
    
    static VectorXd mins;
    static VectorXd maxs;
    static VectorXd means;
    static VectorXd stddevs;
    static vector<int> fixAttributes;    
    

    
    

    AttributeContainer(){};
    AttributeContainer(unsigned int curI);

    virtual ~AttributeContainer(){};
    
    Realv getAttribute(int i,bool normalized=false);
    void setAttribute(const string &n,const float v);
    void setAttribute(const int idx, const float v);
    
    static void preCacheAttr(vector<string> & attr);
    
    static vector<string> getAggregators(string & s);

    static void clearAll();
    
        static void CacheNormalized(int numCount);
    static int getAttributeId(const string &s);
    
    static ofMutex staticMutex;
    
};


#endif /* defined(__ViZa__AttributeContainer__) */

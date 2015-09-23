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




class AttributeContainer{
    
public:
    
    static vector<string> attributeNames;
    static MatrixXd  normalizedAttributes;

    static MatrixXd attributesCache;
    
    static Matrix<double,Dynamic,Dynamic> reducedAttributeCache;
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
    void setAttribute(const string &n,const Realv v);
    void setAttribute(const int idx, const Realv v);
    
    static void preCacheAttr(vector<string> & attr);
    
    static vector<string> getAggregators(string & s);

    static void clearAll();
    
        static void CacheNormalized(int numCount);
    static int getAttributeId(const string &s);
    
    
    static void removeSelectedAttribute(int i);
    static void addSelectedAttribute(int i);
    static bool hasReducedAttribute();
    static void initReduced();
    static ofMutex staticMutex;
    
};


#endif /* defined(__ViZa__AttributeContainer__) */

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




class Container;






class AttributeContainer{
    
public:
    
    
    
    
    static vector<string> attributeNames;
    static Realv * normalizedAttributes;
    static vector<  int > fixAttributes;
    static Realv * attributesCache;
    
    static Realv * reducedAttributeCache;
    static vector < int> reducedAttributesNamesIdx;
    
    
    
    static int attrSize;
    static int totalSizeAttr;
    
    static vector<Realv > mins;
    static vector<Realv> maxs;
    static vector<Realv > means;
    static vector<Realv > stddevs;
    
    
    static vector<unsigned int > total;
    
    

    AttributeContainer(){};
    AttributeContainer(unsigned int curI);

    virtual ~AttributeContainer(){};
    
    Realv & getAttributes(int i,bool normalized=false);
    void setAttribute(const string &n,const Realv v);
    void setAttribute(const int idx, const Realv v);
    
    static void preCacheAttr(vector<string> & attr);
    
    static vector<string> getAggregators(string & s);

    static void clearAll();
    
        static void CacheNormalized(int numCount);
    static int getAttributeId(const string &s);
    
    static ofMutex staticMutex;
    
};


#endif /* defined(__ViZa__AttributeContainer__) */

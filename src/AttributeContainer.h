//
//  AttributeContainer.h
//  ViZa
//
//  Created by martin hermant on 23/01/15.
//
//

#ifndef __ViZa__AttributeContainer__
#define __ViZa__AttributeContainer__

#include <Accelerate/Accelerate.h>
#include "ofMain.h"
#include "AAllocator.h"



class Container;

class AttributeContainer{
    
public:
    
    static vector<string> attributeNames;
    static vector<  float,AAllocator<float, 16> > normalizedAttributes;
    static vector<  int > fixAttributes;
    static vector< float,AAllocator<float, 16> > attributesCache;
    
    static int attrSize;
    
    
    static vector<float > mins;
    static vector<float> maxs;
    static vector<float > means;
    static vector<float > stddevs;
    
    
    static vector<unsigned int > total;
    
    

    AttributeContainer(){};
    AttributeContainer(unsigned int curI);

    virtual ~AttributeContainer(){};
    
    float & getAttributes(int i,bool normalized=false);
    void setAttribute(const string &n,const float v);
    

    
    
        static void CacheNormalized(int numCount);
    static int getAttributeId(const string &s);
    
};


#endif /* defined(__ViZa__AttributeContainer__) */

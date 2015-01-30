//
//  AttributeContainer.cpp
//  ViZa
//
//  Created by martin hermant on 23/01/15.
//
//

#include "AttributeContainer.h"


#include "Container.h"


vector< float,AAllocator<float, 16> >  AttributeContainer::normalizedAttributes;
vector< int>  AttributeContainer::fixAttributes;
vector< float,AAllocator<float, 16> >  AttributeContainer::attributesCache;
vector<string> AttributeContainer::attributeNames;


vector<float > AttributeContainer::mins;
vector<float > AttributeContainer::maxs;
vector<float > AttributeContainer::means;
vector<float > AttributeContainer::stddevs;
vector<unsigned int > AttributeContainer::total;

int AttributeContainer::attrSize;





AttributeContainer::AttributeContainer(unsigned int curI){

    if(attrSize*(1+curI)>=attributesCache.size()){
        attributesCache.resize(attrSize*(curI+1));
    }
}
int AttributeContainer::getAttributeId(const string &n){
    int foundIdx = ofFind(attributeNames,n);
    
    if(foundIdx<0 || foundIdx >= attributeNames.size()) foundIdx=-1;
    return foundIdx;
    
}
void AttributeContainer::setAttribute(const string &n,const float v){
    

    
    int foundIdx = getAttributeId(n);
    if(foundIdx>=0){
    
        
        mins[foundIdx] = MIN(mins[foundIdx], v);
        maxs[foundIdx] = MAX(maxs[foundIdx], v);
        total[foundIdx]++;
        

        
    }
    
    else{
        attributeNames.push_back(n);
        int attrIdx = attributeNames.size()-1;
        
        mins.resize(attrIdx+1);
        maxs.resize(attrIdx+1);
        total.resize(attrIdx+1);
        
        mins[attrIdx]=v;
        maxs[attrIdx]=v;
        total[attrIdx]++;
        
        foundIdx = attrIdx;
        
        
        
    }
    
    
    
    attrSize = attributeNames.size();
    getAttributes(foundIdx) = v;

}

void AttributeContainer::CacheNormalized(int numCont){
    
//    int numCont = containers.size();
    int idx=0;
    
    means.resize(attrSize);
    stddevs.resize(attrSize);
    
    normalizedAttributes.resize(numCont * attrSize);
    fixAttributes.clear();
    for(int i = 0 ; i < attrSize;i++){
        
        vDSP_normalize(&attributesCache[i],attrSize, &normalizedAttributes[i], attrSize, &means[i], &stddevs[i], numCont);
        
        // fix attributes
        if(stddevs[i] ==0 || stddevs[i]!=stddevs[i]){
            fixAttributes.push_back(i);
            stddevs[i]=0;
            cout <<"Fix Attribute : " << attributeNames[i] << " = " << means[i] << endl;
            for(int j=0 ; j<numCont ; j++){
                normalizedAttributes[i+j*attrSize] = 0;
            }
        }
        
    }
    //    fixAttributes.resize(fixAttributes.size()-1);
}
float & AttributeContainer::getAttributes(int i,bool normalized){

    if(attributesCache.size()<= (((Container*)this)->index+1) * attrSize){
        attributesCache.resize((((Container*)this)->index+1)*attrSize);
    }
    return normalized?normalizedAttributes[attrSize * (((Container*)this)->index) +i]:attributesCache[attrSize * (((Container*)this)->index) +i];
};
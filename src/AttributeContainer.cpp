//
//  AttributeContainer.cpp
//  ViZa
//
//  Created by martin hermant on 23/01/15.
//
//

#include "AttributeContainer.h"


#include "Container.h"


MatrixXd AttributeContainer::normalizedAttributes ;
vector< int>  AttributeContainer::fixAttributes;
MatrixXd  AttributeContainer::attributesCache ;
vector<string> AttributeContainer::attributeNames;


MatrixXd  AttributeContainer::reducedAttributeCache ;
vector< int>  AttributeContainer::reducedAttributesNamesIdx;


VectorXd AttributeContainer::mins;
VectorXd AttributeContainer::maxs;
VectorXd AttributeContainer::means;
VectorXd AttributeContainer::stddevs;


int AttributeContainer::attrSize;
int AttributeContainer::totalAttr = 0;

ofMutex AttributeContainer::staticMutex;



AttributeContainer::AttributeContainer(unsigned int curI){

    if(attrSize*(1+curI)>totalAttr){
        ofLogError("AttributeContainer","resizing : " + ofToString(attrSize) +" : "+ ofToString(curI));
        ofScopedLock lock(staticMutex);
        totalAttr = attrSize*(curI+1)*sizeof(float);
        attributesCache.resize(attrSize,1+curI);

    }
}
int AttributeContainer::getAttributeId(const string &n){

    int foundIdx = ofFind(attributeNames,n);
    
    if(foundIdx<0 || foundIdx >= attributeNames.size()) foundIdx=-1;
    return foundIdx;
    
}
void AttributeContainer::setAttribute(const string &n,const float v){
    
    ofScopedLock lock(staticMutex);
    
    int foundIdx = ofFind(attributeNames,n);
    if(foundIdx<0 || foundIdx >= attributeNames.size()) foundIdx=-1;
    if(foundIdx>=0 ){
        mins[foundIdx] = MIN(mins[foundIdx], v);
        maxs[foundIdx] = MAX(maxs[foundIdx], v);
   
    }
    
    
    //no dynamic add of new Attribute
    else{
        
        
        ofLogError("AttributeContainer","unConsistent json files : "+ n  + "found"+ofToString(foundIdx) );
        
        for (int i = 0 ; i < attributeNames.size() ; i++){
            ofLogError("AttributeContainer") << attributeNames[i] ;
        }
        
    }
    
    
    
    
    int curIdx = ((Container*)this)->globalIdx;
    cout << n << " : ";
    for(auto f:attributeNames){cout << f<<",";}
    cout << attributesCache.rows() <<"/" << foundIdx << " : " << attributesCache.cols()<< "/" << curIdx << endl;
    attributesCache(foundIdx,curIdx ) = v;


}


void AttributeContainer::setAttribute(const int idx, const float v){
    int curIdx = ((Container*)this)->globalIdx;    
    attributesCache(idx,curIdx ) = v;
    mins[idx] = MIN(mins[idx], v);
    maxs[idx] = MAX(maxs[idx], v);

}

void AttributeContainer::preCacheAttr(vector<string> & attr){
    attributeNames = attr;
    attrSize = attributeNames.size();
    mins.resize(attrSize);
    maxs.resize(attrSize);

}

vector<string> AttributeContainer::getAggregators(string & s){
    vector<string > res;
    for(int i = 0 ;  i < attributeNames.size() ; i++){
        vector<string> split = ofSplitString(attributeNames[i], ".");
        if(split[0] == s && split.size()==2 ){
            res.push_back(split[1]);
        }
        
    }
    
    if(res.size()==0){
        res.push_back("None");
    }
    return res;
}

void AttributeContainer::CacheNormalized(int numCont){
    
//    int numCont = containers.size();
    int idx=0;
    
    means.resize(attrSize);
    stddevs.resize(attrSize);
    
    normalizedAttributes.resize(attrSize,numCont);
    fixAttributes.clear();
    
    means = attributesCache.rowwise().mean();
    MatrixXd centered = attributesCache.rowwise() - attributesCache.colwise().mean();
    MatrixXd cov = (centered.adjoint() * centered) / double(attributesCache.rows() - 1);
    stddevs = cov.diagonal();

    
    
//    for(int i = 0 ; i < attrSize;i++){
//        
//        attributesCache.,attrSize, &normalizedAttributes[i], attrSize, &means[i], &stddevs[i], numCont);
//        
//        // fix attributes
//        if(stddevs[i] ==0 || stddevs[i]!=stddevs[i]){
//            fixAttributes.push_back(i);
//            stddevs[i]=0;
//            ofLogWarning("AttributeContainer","Fix Attribute : " +ofToString(attributeNames[i]) +" = " +ofToString(means[i]));
//            for(int j=0 ; j<numCont ; j++){
//                normalizedAttributes[i+j*attrSize] = 0;
//            }
//        }
//        
//    }
    //    fixAttributes.resize(fixAttributes.size()-1);
}
Realv AttributeContainer::getAttribute(int i,bool normalized){
    return normalized?normalizedAttributes(i,((Container*)this)->globalIdx):attributesCache(i, ((Container*)this)->globalIdx );
};

void AttributeContainer::clearAll(){
    
    attributeNames.clear();
    attributesCache.setZero();
    attributesCache.setZero();
    normalizedAttributes.setZero();
    fixAttributes.clear();
    maxs.setZero();
    mins.setZero();
    means.setZero();

    attrSize = 0;
    
}
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


Matrix<double,Dynamic,Dynamic> AttributeContainer::reducedAttributeCache ;
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
void AttributeContainer::removeSelectedAttribute(int idx){
    int foundIdx = ofFind(reducedAttributesNamesIdx,idx);
    
    if(foundIdx<0 || foundIdx >=reducedAttributesNamesIdx.size()){
        ofLogError() << "not existing attribute selected" << attributeNames[idx];
        return;
    }
    else{
        reducedAttributesNamesIdx.erase(reducedAttributesNamesIdx.begin()+foundIdx);
        
        if(foundIdx!=(int)reducedAttributesNamesIdx.size()-1){
            reducedAttributeCache.block(foundIdx, 0, (int)reducedAttributesNamesIdx.size()-foundIdx, Container::numContainer) =
            reducedAttributeCache.block(foundIdx+1, 0, (int)reducedAttributesNamesIdx.size()-foundIdx-1, Container::numContainer).eval();
        }
        reducedAttributeCache.conservativeResize(reducedAttributesNamesIdx.size(), Container::numContainer);
    }
}
void AttributeContainer::addSelectedAttribute(int idx){
    int foundIdx = ofFind(reducedAttributesNamesIdx,idx);
    
    if(foundIdx>0 && foundIdx <reducedAttributesNamesIdx.size()){
        ofLogError() << "Already added Attribute" << attributeNames[idx];
        return;
    }
    else{
        reducedAttributesNamesIdx.push_back(idx);
        reducedAttributeCache.conservativeResize(reducedAttributesNamesIdx.size(), Container::numContainer);
        reducedAttributeCache.row((int)reducedAttributesNamesIdx.size()-1) = normalizedAttributes.row(idx).eval();
    }
}

bool AttributeContainer::hasReducedAttribute(){
    return reducedAttributesNamesIdx.size()>0;
}



void AttributeContainer::setAttribute(const string &n,const Realv v){
    
    ofScopedLock lock(staticMutex);
    
    int foundIdx = ofFind(attributeNames,n);
    if(foundIdx<0 || foundIdx >= attributeNames.size()) foundIdx=-1;
    if(foundIdx>=0 ){
        mins[foundIdx] = MIN(mins[foundIdx], v);
        maxs[foundIdx] = MAX(maxs[foundIdx], v);
   
    }
    
    
    //no dynamic add of new Attribute
    else{
        
        
        ofLogError("AttributeContainer","unConsistent dataset files : "+ n  + "found"+ofToString(foundIdx) );
        
        for (int i = 0 ; i < attributeNames.size() ; i++){
            ofLogError("AttributeContainer") << attributeNames[i] ;
        }
        
    }
    
    
    
    
    int curIdx = ((Container*)this)->globalIdx;

    attributesCache(foundIdx,curIdx ) = v;


}


void AttributeContainer::setAttribute(const int idx, const Realv v){
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
    MatrixXd centered = attributesCache.colwise() - attributesCache.rowwise().mean();

    stddevs = centered.rowwise().squaredNorm() / centered.cols();
    stddevs = stddevs.cwiseSqrt();
    
    
    // avoid NaNs
    VectorXd eps(stddevs.size());
    eps.setZero();
    eps = stddevs.cwiseEqual(0.0).cast<double>();
    stddevs += FLT_MIN*eps;
    
    
    normalizedAttributes = centered.array().colwise() / stddevs.array() ;//.transpose();

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
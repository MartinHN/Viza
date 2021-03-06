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
        ofLogError("AttributeContainer") << "not existing attribute selected" << attributeNames[idx];
        return;
    }
    else{
        
        if(foundIdx<(int)reducedAttributesNamesIdx.size()-1){
            reducedAttributeCache.block(foundIdx,
                                        0,
                                        (int)reducedAttributesNamesIdx.size() -(foundIdx+1),
                                        Container::numContainer
                                        ) =
            reducedAttributeCache.block(foundIdx+1,
                                        0,
                                        (int)reducedAttributesNamesIdx.size() -(foundIdx+1),
                                        Container::numContainer).eval();
        }
        reducedAttributesNamesIdx.erase(reducedAttributesNamesIdx.begin()+foundIdx);
        reducedAttributeCache.conservativeResize(reducedAttributesNamesIdx.size(), Container::numContainer);
    }
}


void AttributeContainer::addSelectedAttribute(int idx){
    int foundIdx = ofFind(reducedAttributesNamesIdx,idx);
    
    if(foundIdx>=0 && foundIdx <reducedAttributesNamesIdx.size()){
        ofLogError("AttributeContainer") << "Already added Attribute" << attributeNames[idx];
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
  Realv finiteV = v;
  if(!std::isfinite(v)){
    finiteV = 0;
    ofLogError()<<"nan or inf for "<<n;

  }
    
    int foundIdx = ofFind(attributeNames,n);
    if(foundIdx<0 || foundIdx >= attributeNames.size()) foundIdx=-1;
    if(foundIdx>=0 ){
        mins[foundIdx] = MIN(mins[foundIdx], finiteV);
        maxs[foundIdx] = MAX(maxs[foundIdx], finiteV);
      int curIdx = ((Container*)this)->globalIdx;
      attributesCache(foundIdx,curIdx ) = finiteV;
   
    }
    
    
    //no dynamic add of new Attribute
    else{
        
        
        ofLogError("AttributeContainer","unConsistent dataset files : "+ n );
        
        for (int i = 0 ; i < attributeNames.size() ; i++){
            ofLogError("AttributeContainer") << "\t" << attributeNames[i] ;
        }
        
    }
    
    
    
    





}


void AttributeContainer::setAttribute(const int idx, const Realv v){
    int curIdx = ((Container*)this)->globalIdx;
  Realv finiteV = v;
  if(!std::isfinite(v)){
    finiteV = 0;
    ofLogError()<<"nan or inf for idx"<<idx;

  }
    attributesCache(idx,curIdx ) = finiteV;
    mins[idx] = MIN(mins[idx], finiteV);
    maxs[idx] = MAX(maxs[idx], finiteV);

}

void AttributeContainer::preCacheAttr(vector<string> & attr){
    attributeNames = attr;
    attrSize = attributeNames.size();
    mins.resize(attrSize);
    maxs.resize(attrSize);
  mins.setZero();
  maxs.setZero();
	ofLogWarning() << "resizing mins "<<attrSize;

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
    ofLogNotice() << "resizing stddevs "<<attrSize;
    normalizedAttributes.resize(attrSize,numCont);
    fixAttributes.clear();
    
    means = attributesCache.rowwise().mean();
    MatrixXd centered = attributesCache.colwise() - means;

  stddevs = centered.rowwise().squaredNorm() / centered.cols();
    stddevs = stddevs.cwiseSqrt();

    
    // avoid NaNs
    VectorXd eps(stddevs.size());
    eps.setZero();
    eps = stddevs.cwiseEqual(0.0).cast<double>();
    stddevs += DBL_MIN*eps;
    
    
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
    reducedAttributeCache.resize(0,0);
    reducedAttributesNamesIdx.clear();
    maxs.setZero();
    mins.setZero();
    means.setZero();

    attrSize = 0;
    
}


void AttributeContainer::initReduced(){
    for(auto & a:attributeNames){
        if(a!="length" && a != "relativeStartTime" && a!= "startTime"){
        addSelectedAttribute(&a-&attributeNames[0]);
        }
    }
}
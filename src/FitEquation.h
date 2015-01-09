//
//  FitEquation.h
//  ViZa
//
//  Created by martin hermant on 09/01/15.
//
//

#ifndef ViZa_FitEquation_h
#define ViZa_FitEquation_h

#include "ofMain.h"

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
            res += ofToString((float)it->first,3) + " x ";
            res += paramNames[it->second];
            
            res+= " +\t";
            
        }
        if(res!="")res = res.substr(0 ,res.length() - 3);
        
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



#endif

//
//  Statistics.h
//  ViZa
//
//  Created by martin hermant on 08/09/15.
//
//

#ifndef __ViZa__Statistics__
#define __ViZa__Statistics__

#include <iostream>

#include <shogun/preprocessor/PCA.h>
#include <shogun/features/DenseFeatures.h>
#include <shogun/lib/common.h>
#include <shogun/base/init.h>
#include <map>
#include <vector>

using namespace shogun;

class Statistics{
    public :
    
    Statistics(){
       std::cout << "createStat I";

    };
    ~Statistics(){};
    
    static void init(){static bool inited = false;
        if(!inited){
            std::cout << "init shogun";
            init_shogun();
            inited = true;
        }
        };
    
    static Statistics * i(){ init();init();static Statistics * instance = new Statistics () ; return instance;}
    
    typedef float64_t Real;
    
    std::map<std::string,std::vector<Real> > stats;
    
    
    void computePCA(){

        
        try{
        
        pca.init(data);
        }
        catch(shogun::ShogunException e){
            std::cout << e.get_exception_string();
        }
        
        SGMatrix<float64_t> mat = pca.get_transformation_matrix();
        
//        mat.display_matrix();
        stats["PCARank"] = std::vector<Real> ();
        for (int i = 0 ; i < mat.num_rows ; i++){
            float sum = 0;
            for(int j = 0 ; j < mat.num_cols ; j++){
                sum+=mat(i,j)*j ;//* (j+1-mat.num_cols);
            }
            stats["PCARank"].push_back(sum/mat.num_cols);
        }
    
        
    };
    
    

    void setMatrix(float * ori,int numFeatures,int numInstances)
    {
               matData =  SGMatrix<Real>(numFeatures,numInstances);
               for(int i = 0 ;i < numFeatures ; i++){
                   for(int j = 0 ;j < numInstances ; j++){
                       matData(i,j) = ori[i*numInstances +j];
                   }
               }

        data = new CDenseFeatures<Real> (matData);//(ori, numFeatures,numInstances);
        SG_REF(data)
    };
    
    
    SGMatrix<Real> matData;

    CDenseFeatures<Real> *  data;
    CPCA pca;
    
};

#endif /* defined(__ViZa__Statistics__) */

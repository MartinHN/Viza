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
#include <shogun/base/init.h>


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
    
    typedef float32_t Real;
    
    
    
    
    void computePCA(){
//        CDenseFeatures
        
//        pca.init(&data);
//        
//        
//        SGMatrix<float64_t> mat = pca.get_transformation_matrix();
//        mat.display_matrix();
//        for (int i = 0 ; i < mat.num_rows ; i++){
//            double * c = mat.get_column_vector(i);
//            for(int j = 0 ; j < mat.num_cols ; j++);
//        }}
        
        
    };
    
    

    void setMatrix(Real * ori,int n,int m)
    {
       data =  CDenseFeatures<Real> (ori, n,m);
    };
    
    
    
    CDenseFeatures<Real> data;
    CPCA pca;
    
};

#endif /* defined(__ViZa__Statistics__) */

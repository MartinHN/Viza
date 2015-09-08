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

#include "DSP.h"

#include "Container.h"

using namespace shogun;

class Statistics{
    public :
    
    Statistics(){
        
        
    };
    ~Statistics(){
        exit_shogun();
    };
    
    static void init(){static bool inited = false;
        if(!inited){
            
            init_shogun();
            inited = true;
        }
    };
    
    static Statistics * i(){ init();static Statistics * instance = new Statistics () ; return instance;}
    
    
    
    
    
    
    void computePCA(){
        //        CDenseFeatures
        
        pca.init(data);
        
        
        
        SGMatrix<float64_t> mat = pca.get_transformation_matrix();
        for(int i = 0 ; i < mat.num_rows; i++){
            cout << Container::attributeNames[i] << " : " << mat(i,0) << endl;
        }

        //        for (int i = 0 ; i < mat.num_rows ; i++){
        //            double * c = mat.get_column_vector(i);
        //            for(int j = 0 ; j < mat.num_cols ; j++);
        //        }}
        
        
    };
    
    
    
    void setMatrix(Realv * ori,int numFeatures,int numInstances)
    {
//        matData = SGMatrix<Realv>(numFeatures,numInstances);
//        for(int i = 0 ;i < numFeatures ; i++){
//            for(int j = 0 ;j < numInstances ; j++){
//                matData(i,j) = ori[i*numInstances +j];
//            }
//        }
        data =   new CDenseFeatures<float64_t> (ori,numFeatures,numInstances);
        SG_REF(data)
//        data->set_feature_matrix(matData);
        cout << numFeatures <<","<< numInstances << endl;
    };
    
    
    SGMatrix<Realv> matData;
    CDenseFeatures<float64_t> *  data;
    CPCA pca;
    
};

#endif /* defined(__ViZa__Statistics__) */

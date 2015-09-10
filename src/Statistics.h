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
#include <shogun/converter/TDistributedStochasticNeighborEmbedding.h>
#include <shogun/features/DenseFeatures.h>
#include <shogun/lib/common.h>
#include <shogun/base/init.h>
#include <map>
#include <vector>

#include "DSP.h"

using namespace shogun;

class Statistics{
    public :
    
    Statistics(){
        data = new CDenseFeatures<Real> ();
    };
    ~Statistics(){};
    
    
    
    static void init(){static bool inited = false;
        if(!inited){
            init_shogun(&Statistics::print_message, &Statistics::print_warning,
                        &Statistics::print_error);

            inited = true;
        }
    };
    static void print_message(FILE* target, const char* str)
    {
        std::cout <<"msg shogun  " <<  str << std::endl;
    }
    
    static void print_warning(FILE* target, const char* str)
    {
        std::cout <<"warn shogun  " <<  str << std::endl;
    }
    
    static void print_error(FILE* target, const char* str)
    {
        std::cout <<"err shogun  " <<  str << std::endl;
    }
    

    static Statistics * i(){ init();init();static Statistics * instance = new Statistics () ; return instance;}
    
    typedef float64_t Real;
    
    std::map<std::string,std::vector<Real> > stats;
    
    
    void computePCA(){
        
        
        try{
            data->set_feature_matrix(matData);
            pca.io->set_loglevel(shogun::MSG_GCDEBUG);
            pca.io->enable_progress();
            pca.set_target_dim(3);
            pca.init(data);

            
            
            
            
            
            
            stats["PCARank"] =std::vector<Real> ();

            
            
            SGMatrix<float64_t> mat = pca.get_transformation_matrix();
            

//

            for (int i = 0 ; i < mat.num_rows ; i++){
                float sum = 0;
                std::cout << std::endl;
                for(int j = 0 ; j < mat.num_cols ; j++){
                    sum+=mat(i,j) *(mat.num_cols - j);

                }
                stats["PCARank"].push_back(sum);
            }
            
            
            
            
            sne.set_target_dim(3);
            sne.set_perplexity(5);
            sne.set_theta(0.6);

//
        }
        catch(shogun::ShogunException e){
            std::cout << e.get_exception_string();
        }
        
    };
    
    
    
    void setMatrix(Realv * ori,int numFeatures,int numInstances)
    {
        matData =  SGMatrix<Real>(numFeatures,numInstances);
        for(int i = 0 ;i < numFeatures ; i++){
            for(int j = 0 ;j < numInstances ; j++){
                matData(i,j) = ori[i*numInstances +j];
            }
        }

    };
    
    
    void getTransformed(float * r){
//        std::vector<float> res;
        SGMatrix<float64_t> mR = pca.apply_to_feature_matrix(data);
//        SGMatrix<float64_t> mR = sne.embed(data);
        
        std::cout << "ended";
        for(int i = 0 ; i < mR.num_rows ; i++){
        for(int j = 0 ; j < mR.num_cols ; j++){
            *(r + i*mR.num_cols + j)  = mR(i,j);
        }
        }
        
    }
    

    
    SGMatrix<Real> matData;
    
    CDenseFeatures<Real> *  data;
    CPCA pca;
    CTDistributedStochasticNeighborEmbedding sne;
    
};

#endif /* defined(__ViZa__Statistics__) */

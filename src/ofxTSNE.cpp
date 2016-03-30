//
//  ofxTSNE.cpp
//  ViZa
//
//  Created by martin hermant on 29/01/15.
//
//

#include "ofxTSNE.h"

#include "Physics.h"

ofxTSNE* ofxTSNE::instance;



void ofxTSNE::init(Realv * v, int dim,int nelem,float _theta,float _perp,int _outDim,bool _fixSeed){

    inVec = std::vector<double>(v,v+dim*nelem);

//    DSP_vspdp(v, 1, inVec, 1, nelem*dim);
    nData = nelem;
    dimData = dim;
    theta = _theta;
    perplexity = _perp;
    outDim = _outDim;
    fixSeed = _fixSeed;
    if(outVecCache)free(outVecCache);
    outVecCache = (double*)malloc(nData * outDim * sizeof(double));
    cache.resize(3,Physics::vs.size());
    
}


void ofxTSNE::threadedFunction(){
    // force z= 0 for 2d tsne
    if(outDim == 2){
        cache.row(2).setZero();
    }
    hasStopped = false;
    tsne->shouldStop  = false;
    tsne->run(&inVec[0], nData, dimData, outVecCache, outDim, perplexity, theta,fixSeed);
    
    
    hasStopped = true;
    
    
}





void ofxTSNE::update(ofEventArgs &a){
    
    if(isThreadRunning()){

        Matrix<double,Dynamic,Dynamic> origin = Map<Matrix<double,Dynamic,Dynamic> >(outVecCache,outDim,Physics::vs.size());
        cache.topLeftCorner(outDim, Physics::vs.size()) = origin.topLeftCorner(outDim, Physics::vs.size()).eval().cast<float>();
        
        VectorXf mins  = cache.topLeftCorner(outDim, Physics::vs.size()).rowwise().minCoeff();
        VectorXf diffs  = cache.topLeftCorner(outDim, Physics::vs.size()).rowwise().maxCoeff() - mins;
        
        cache.topLeftCorner(outDim, Physics::vs.size()).colwise()-= mins;
        cache.topLeftCorner(outDim, Physics::vs.size()).array().colwise() /= diffs.array();
        cache.topLeftCorner(outDim, Physics::vs.size()).array() -=0.5;


//        ofVec3f maxV;
//        ofVec3f minV;
//        float mean,dev,norm = .05;
//        float min,max;
        
//        for(int i = 0 ; i < outDim ;i++){
//            DSP_vdpsp(res +i, outDim, &cache[0][i], 3, cache.size());
//            
//            
//            // min max
//            DSP_minv(&cache[0][i], 3, &min, cache.size());
//            DSP_maxv(&cache[0][i], 3, &max, cache.size());
//            norm = 1.0/(max-min);
//            dev = -min;
//            mean = -0.5;
//            DSP_vsadd(&cache[0][i], 3, &dev, &cache[0][i], 3,cache.size());
//            DSP_vsmsa(&cache[0][i], 3, &norm, &mean,&cache[0][i], 3,cache.size());
//            
//            
//            
//        }
        Physics::setFits(cache.data(),Physics::vs.size());
        
        
        
        
    }
    
    if(hasStopped){
        hasStopped = false;
    }
    if(!tsne->shouldStop && !isThreadRunning()){
        tsne->shouldStop = true;
    }
}














//// Function that runs the Barnes-Hut implementation of t-SNE
//int main() {
//
//    // Define some variables
//	int origN, N, D, no_dims = 2, *landmarks;
//	double perc_landmarks;
//	double perplexity, theta, *data;
//    TSNE* tsne = new TSNE();
//
//    // Read the parameters and the dataset
//	if(tsne->load_data(&data, &origN, &D, &theta, &perplexity)) {
//
//		// Make dummy landmarks
//        N = origN;
//        int* landmarks = (int*) malloc(N * sizeof(int));
//        if(landmarks == NULL) { printf("Memory allocation failed!\n"); exit(1); }
//        for(int n = 0; n < N; n++) landmarks[n] = n;
//
//		// Now fire up the SNE implementation
//		double* Y = (double*) malloc(N * no_dims * sizeof(double));
//		double* costs = (double*) calloc(N, sizeof(double));
//        if(Y == NULL || costs == NULL) { printf("Memory allocation failed!\n"); exit(1); }
//		tsne->run(data, N, D, Y, no_dims, perplexity, theta);
//
//		// Save the results
//		tsne->save_data(Y, landmarks, costs, N, no_dims);
//
//        // Clean up the memory
//		free(data); data = NULL;
//		free(Y); Y = NULL;
//		free(costs); costs = NULL;
//		free(landmarks); landmarks = NULL;
//    }
//    delete(tsne);
//}
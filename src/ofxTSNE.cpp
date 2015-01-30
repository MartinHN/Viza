//
//  ofxTSNE.cpp
//  ViZa
//
//  Created by martin hermant on 29/01/15.
//
//

#include "ofxTSNE.h"



ofxTSNE* ofxTSNE::instance;



void ofxTSNE::init(float * v, int dim,int nelem,float _theta,float _perp,int _outDim){
    if(inVec!=NULL)free(inVec);
    inVec = (double*) malloc(dim*nelem*sizeof(double));
    vDSP_vspdp(v, 1, inVec, 1, nelem*dim);
    nData = nelem;
    dimData = dim;
    theta = _theta;
    perplexity = _perp;
    outDim = _outDim;
    outVecCache = (double*)malloc(nData * outDim * sizeof(double));

}


double* ofxTSNE::run(){
    tsne->run(inVec, nData, dimData, outVecCache, outDim, perplexity, theta);
    

    return outVecCache;
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
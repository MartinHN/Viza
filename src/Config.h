//
//  Config.h
//  Particles
//
//  Created by Martin Hermant on 13/01/2016.
//
//

#ifndef Config_h
#define Config_h

using namespace std;
#include "ofxPCA.h"
#define COLNUM 3

//#define DIFFERENT_SIZES
//#define FIXEDSIZE 1000//27
typedef float MatReal;
#define NEED_TO_CAST_VERT 0
//typedef  Matrix<MatReal,Dynamic,3,RowMajor|AutoAlign> MyMatrixType;
#ifdef FIXEDSIZE
#define ROWTYPE FIXEDSIZE
#else
#define ROWTYPE Dynamic
#endif


typedef  Eigen::Array<MatReal,ROWTYPE,COLNUM,RowMajor|AutoAlign> MyMatrixType;


#endif /* Config_h */

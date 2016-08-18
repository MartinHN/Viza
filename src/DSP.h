//
//  DSP.h
//  ViZa
//
//  Created by martin hermant on 31/03/15.
//
//

#ifndef ViZa_DSP_h
#define ViZa_DSP_h
#include "ofConstants.h"

static inline float
fastpow2 (float p)
{
    float offset = (p < 0) ? 1.0f : 0.0f;
    float clipp = (p < -126) ? -126.0f : p;
    int w = clipp;
    float z = clipp - w + offset;
    union { uint32_t i; float f; } v = { static_cast<uint32_t> ( (1 << 23) * (clipp + 121.2740575f + 27.7280233f / (4.84252568f - z) - 1.49012907f * z) ) };
    
    return v.f;
}

static inline float
fastexp (float p)
{
    return fastpow2 (1.442695040f * p);
}


typedef double Realv;



#undef Success
#include <eigen3/Eigen/Core>
using namespace Eigen;

#define SQRT std::sqrt


#endif

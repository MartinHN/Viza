//
//  SliceModel.h
//  ViZa
//
//  Created by martin hermant on 16/12/14.
//
//

#ifndef __ViZa__SliceModel__
#define __ViZa__SliceModel__

#include <iostream>


#include "ofxNonLinearFit.h"
#include <Accelerate/Accelerate.h>


//ç#define ANGLE_DIST

namespace ofxNonLinearFit {
	namespace Models {
        
        typedef ofVec3f Data;
		struct SliceModelPoint {
			vector<float> descriptorsDiff;
			Data angle;

		};
        
		class SliceModel : public Base<SliceModelPoint, SliceModel> {
		public:
			typedef SliceModelPoint DataPoint;
            
            vector<float>  parametersf;
            
            int size = 0;
            
			unsigned int getParameterCount() const override;
			double getResidual(DataPoint) const override;
			void evaluate(DataPoint &) const override;
            
//			/// Override this if you would like to interpret the 6 pose parameters in a non-default way
			virtual void cacheModel() override;

			Data evaluate(const vector<float> & x) const;
            Data getAngle(ofVec3f & v)const ;
		
		};
	}
}

#endif /* defined(__ViZa__SliceModel__) */

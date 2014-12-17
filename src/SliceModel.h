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

namespace ofxNonLinearFit {
	namespace Models {
        
        
		struct SliceModelPoint {
			vector<double> descriptors;
			ofVec3f xdash;

		};
        
		class SliceModel : public Base<SliceModelPoint, SliceModel> {
		public:
			typedef SliceModelPoint DataPoint;
            
            int size = 0;
            
			unsigned int getParameterCount() const override;
			double getResidual(DataPoint) const override;
			void evaluate(DataPoint &) const override;
            
//			/// Override this if you would like to interpret the 6 pose parameters in a non-default way
//			virtual void cacheModel() override;
//            
//			void setTransform(const ofVec3f & translation, const ofVec3f & rotationEuler);
//			const ofMatrix4x4 & getCachedTransform() const;
			ofVec3f evaluate(const vector<double> & x) const;
            
		protected:
			ofMatrix4x4 transform;
		};
	}
}

#endif /* defined(__ViZa__SliceModel__) */

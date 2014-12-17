//
//  SliceModel.cpp
//  ViZa
//
//  Created by martin hermant on 16/12/14.
//
//

#include "SliceModel.h"



namespace ofxNonLinearFit {
	namespace Models {
#pragma mark overrides
		//----------
		unsigned int SliceModel::getParameterCount() const {
			return 3*size;
		}
        
		//----------
		double SliceModel::getResidual(DataPoint dataPoint) const {
			return (this->evaluate(dataPoint.descriptors) - dataPoint.xdash).lengthSquared();
		}
        
		//----------
		void SliceModel::evaluate(DataPoint & dataPoint) const {
			dataPoint.xdash = this->evaluate(dataPoint.descriptors);
		}
        
//		//----------
//		void SliceModel::cacheModel() {
//			const auto & parameters = this->parameters;
//            
//			if (parameters == NULL) {
//				this->transform = ofMatrix4x4();
//			} else {
//				const auto translate = ofMatrix4x4::newTranslationMatrix(parameters[0], parameters[1], parameters[2]);
//                
//				const auto eulerAngles = ofVec3f(parameters[3], parameters[4], parameters[5]);
//				const auto quaternion = ofQuaternion(eulerAngles.x, ofVec3f(1, 0, 0), eulerAngles.y, ofVec3f(0, 1, 0), eulerAngles.z, ofVec3f(0, 0, 1));
//				const auto rotate = ofMatrix4x4::newRotationMatrix(quaternion);
//                
//				this->transform = rotate * translate;
//			}
//		}
        
#pragma mark utility functions
//		//----------
//		void SliceModel::setTransform(const ofVec3f & translation, const ofVec3f & rotationEuler) {
//			Parameter parameters[] = { translation.x, translation.y, translation.z, rotationEuler.x, rotationEuler.y, rotationEuler.z };
//			this->setParameters(parameters); // should also in turn set the transform by rebuilding it from the Parameters
//		}
//        
//		//----------
//		const ofMatrix4x4 & SliceModel::getCachedTransform() const {
//			return this->transform;
//		}
//        
		//----------
		ofVec3f SliceModel::evaluate(const vector<double> & x) const {
            double vx = 0 ,vy = 0,vz = 0;

            vDSP_dotprD(&parameters[0], 3,&x[0], 1, &vx,size);
            vDSP_dotprD(&parameters[1], 3, &x[0],1, &vy,size);
            vDSP_dotprD(&parameters[2], 3, &x[0],1, &vz,size);

            return ofVec3f(vx,vy,vz);
		}
	}
}
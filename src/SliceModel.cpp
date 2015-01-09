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
            // circular wrapped Residual (angles are positive between 0 and 180)
            Data diff = this->evaluate(dataPoint.descriptorsDiff) - dataPoint.angle;
			return diff.lengthSquared();
		}
        
		//----------
		void SliceModel::evaluate(DataPoint & dataPoint) const {
			dataPoint.angle = this->evaluate(dataPoint.descriptorsDiff);
		}
        
//		//----------
		void SliceModel::cacheModel() {
            parametersf.resize(getParameterCount());
            //  cache parameter  (float precision for speed and compatibility with vec3f (not loosing too much...?))
            vDSP_vdpsp(parameters,1,&parametersf[0],1,getParameterCount());
			
			
		}
        
#pragma mark utility functions

		//----------
		Data SliceModel::evaluate(const vector<float> & x) const {
            ofVec3f n1;
            
            // linear combination 
            vDSP_dotpr(&parametersf[0], 3, &x[0],1, &n1.x,size);
            vDSP_dotpr(&parametersf[1], 3, &x[0],1, &n1.y,size);
            vDSP_dotpr(&parametersf[2], 3, &x[0],1, &n1.z,size);
            
            
            
            return getAngle(n1);
		}
        
        Data SliceModel::getAngle(ofVec3f & v) const{
#ifdef ANGLE_DIST
            Data res(v.angleRad(ofVec3f(1,0,0)),v.angleRad(ofVec3f(0,1,0)),v.angleRad(ofVec3f(0,0,1)));
            return res;
#else
            return v;
#endif
            
        }
	}
}
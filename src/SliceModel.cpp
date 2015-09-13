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

            Data diff= this->evaluate(dataPoint.descriptorsDiff);
            diff = diff- dataPoint.angle;
//            diff.y-= dataPoint.angle.y;
//            diff.z-= dataPoint.angle.z;
            
//            double len = diff.lengthSquared();

            return diff.squaredNorm();//diff.x*diff.x + diff.y*diff.y + diff.z*diff.z;
		}
        
		//----------
		void SliceModel::evaluate(DataPoint & dataPoint) const {
			dataPoint.angle = this->evaluate(dataPoint.descriptorsDiff);
		}
        
//		//----------
		void SliceModel::cacheModel() {
//            parametersf.resize(getParameterCount());
            //  cache parameter  (float precision for speed and compatibility with vec3f (not loosing too much...?))
//            DSP_vdpsp(parameters,1,&parametersf[0],1,getParameterCount());
			
			
		}
        
#pragma mark utility functions

		//----------
		Data SliceModel::evaluate(const Matrix<double,3,Dynamic> & x) const {
            Data n1;
            
            // linear combination
            Matrix<double,3,Dynamic>  mymat = Map<Matrix<double,3,Dynamic> >(parameters,3,getParameterCount());
            n1.x() = mymat.row(0).dot(x.row(0).transpose()) ;
//            n1.y() = mymat.row(1).dot(x.row(1).transpose());
//            n1.z() = mymat.row(2).dot(x.row(2).transpose());

            
            //
            return getAngle(n1);
		}
        
        Data SliceModel::getAngle(Data & v) const{
            switch(type){
                case 0:
                    return v;
                case 1:{
                    Data res;
                    res[0] = v[0]>0?1:-1;
                    res[1] =v[1]>0?1:-1;
                    res[2] = v[2]>0?1:-1;
                    return res;
                }
                case 2:{
                    ofVec3f vv;
                    vv.x = v.x();
                    vv.y=v.y();
                    vv.z=v.z();
                    Data res;
                    res.x()=vv.angleRad(ofVec3f(1,0,0))/PI;
                    res.y() = vv.angleRad(ofVec3f(0,1,0))/PI;
                    res.z()=vv.angleRad(ofVec3f(0,0,1))/PI;
                    return res;
                }
                    
            }
            

            
        }
	}
}
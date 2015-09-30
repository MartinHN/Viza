//
//  MidiSpot.h
//  ViZa
//
//  Created by Martin Hermant on 25/09/2015.
//
//

#ifndef __ViZa__MidiSpot__
#define __ViZa__MidiSpot__

#include "ofMain.h"
#include "Camera.h"


class MidiSpot{
public:
    MidiSpot(ofVec2f v,float radius):normalizedPos(v),radius(radius){setPitch(-1);};
    
    ofVec2f normalizedPos;
    string name;
    int pitch;
    float radius;
    
    void draw(){

    }

    
    void setPitch(int p){
        pitch = p;
        name = pitchToString(p);
    }
    
    
    static string pitchToString(int p){
        if(p<0)return "-";
        int base = p%12;
        int octave = p/12 - 2;
        string res;
        cout << "base " << base <<endl;
        if (base <= 4){
            res+= (char)('C' + base/2);
            if((base%2)==1){
                res+='#';
            }
        }
        else{
            
        if ((base-5)/2 < 2){
            res+= (char)('F' + (base-5)/2);

        }
        else{
            res +=  'A' + (base - 9)/2;
        }
            if(((base-5)%2)==1){
                res+='#';
            }
        }
        
        
        res += " " + std::to_string(octave);
        return res;
    }
    
    
    void draw(bool link2cam,Camera * locCam){
        ofRectangle viewPort = locCam->viewPort;
        if(link2cam){
        ofPushMatrix();
        ofVec3f v = normalizedPos;
        v.z = locCam->toCamZ(v.z-.5);
        v*=ofVec3f(viewPort.width,viewPort.height,1);
        v=locCam->screenToWorld(v,viewPort);
        
        ofMatrix4x4 mat = locCam->getModelViewMatrix();
        ofQuaternion q = mat.getRotate();
        ofSetMatrixMode(OF_MATRIX_MODELVIEW);
        ofLoadMatrix(mat);
        ofTranslate(v);
        ofMatrix4x4 mat2;
        mat2.makeRotationMatrix(q.conj());
        ofMultMatrix(mat2);
        ofDrawBitmapString(name, 0,0);
        ofDrawCircle(ofVec3f(0),radius);
        ofPopMatrix();
        }
        else{
            ofVec3f v =normalizedPos ;
            v.x=( v.x-.5 )* viewPort.width*1.0/viewPort.height;
            v.y = (.5-v.y );
            ofDrawCircle(v,radius);
            ofDrawBitmapString(name, v + ofVec2f(0,0));
        }
        

    
    }

    

    
    
};
#endif /* defined(__ViZa__MidiSpot__) */

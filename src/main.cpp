#include "ofMain.h"
#include "ofApp.h"
//#include "ofAppGlutWindow.h"




//========================================================================
int main(int argc, char ** argv ){
//    
//    ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
//	ofSetupOpenGL( 1024,768, OF_WINDOW);


//    ofAppGlutWindow window;
	ofSetupOpenGL( 1024,768, OF_WINDOW);
    
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
 ofApp *  app = new ofApp(argc,argv);
  
  
	ofRunApp(app);

}

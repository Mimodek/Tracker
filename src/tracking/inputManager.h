#ifndef _INPUT_MANAGER
#define _INPUT_MANAGER

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxOpenCv.h"
#include "ofxControlPanel.h"

enum {

	INPUT_LIVE_VIDEO, INPUT_VIDEO

};




class inputManager {

	public:

		void setup();
		void update();
		void draw();
        void scrubVideo(int x, int y);

		int						mode;

		ofVideoGrabber			vidGrabber;
		ofVideoPlayer			vidPlayer;
		int						width,height;
		ofxCvColorImage			colorImg;
        ofxCvGrayscaleImage 	grayImage;

		bool					bIsFrameNew;			// on this update, is the frame new?


		//--- panel for movie
		ofxControlPanel panel_video;
};

#endif

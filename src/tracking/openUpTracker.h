#pragma once

#include "baseTracker.h"
#include "maskMaker.h"
#include "ofxCvBlobTracker.h"
#include "ofxCvTrackedBlob.h"
#include "ofxCvConstants_Track.h"
#include "ofxDirList.h"
#include "colorTracker.h"
#include "colorPicker.h"
#include "ofxOsc.h"
#include "tuioServer.h"

#define MAX_USERS_HARDLIMIT	25

class OpenUpTracker : public BaseTracker{

	public:

		OpenUpTracker();
		~OpenUpTracker();

		void setup(int width, int height);
		void update(ofxCvColorImage & colorImgFromCam);
		void draw(float x, float y);
	
	void learnBackground();

		// tracking funcs
		//void motionDetection();
		//void colorFinding();
		//void blobTracking();
		void generateHistory(unsigned char * input,float decreaseRate);

		void setupGui();
		void updateGui();

		void sendOsc();
		void receiveOsc(){};
	
	//--- Learn background
	int * 	pixelsScore;

		//--- masking live image
		MaskMaker					masker;

		//--- images for varoius operations
		ofxCvColorImage				colorImg;					// copy input video
		ofxCvGrayscaleImage			grayImgPreModification;		// original grayscale conversion
		ofxCvGrayscaleImage			grayBgImg;					// background capture
		ofxCvGrayscaleImage			grayBgImgMasked;					// background capture
		ofxCvGrayscaleImage			grayImg;					// grayscale image used for ops
		ofxCvGrayscaleImage			threshImg;					// thresholded binary image
		ofxCvGrayscaleImage			prevFrame;					// previous saved frame for motion detection
		ofxCvGrayscaleImage			motionImg;					//  motion detection
		ofxCvGrayscaleImage			mhiDisp;

		//--- contour finding and blob tracking
		ofxCvContourFinder			contourFinder,contourFinderMotion;
		ofxCvBlobTracker			blobTracker;


        //--- motion
		float					*mhiImageFloat;
		unsigned char			*mhiImageByte;

		//--- params
		int			minSize, maxSize;			// min and max blob size considered
		float		w, h;						// width and height of input video
		float		threshold;					// threshold for presence detection w bg subtraction
		bool        bSaveBgImage;
		float       lastTime, waitTime;
		int tBgsM;

		//--- background subtraction
		string bgCapDirectory;					// directory of saved bg images
		int tBgs;								// total bg images saved so far

		//--- color tracking
		ColorTracker				hsvColorTracker;
		ofxCvColorImage				convColor, colorMask;
		unsigned char				* colorPixels;
		ColorPicker					colorPicker;

		//--- ocs
		void setupOscSender( string host );
		vector<ofxOscSender>	oscSender;
		vector<ofxOscReceiver>	oscReceiver;
		int port;

		//--- tuio
		tuioServer          TS;




};


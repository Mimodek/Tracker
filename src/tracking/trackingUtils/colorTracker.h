#ifndef _COLOR_TRACKER
#define _COLOR_TRACKER

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "colorConverter.h"
#include "ofxCvGrayscaleAdvanced.h"

class ColorTrackerImage : public ofBaseDraws{
	
public:
	
	ColorTrackerImage(){
		img.allocate(64,64);
	}
	
	void draw(float x, float y){
		img.draw(x,y);
	}
	void draw(float x, float y, float w, float h){
		img.draw(x,y,w,h);
	};
	
	float getWidth(){
		return 64;
	}
	
	float getHeight(){
		return 64;
	}
	
	void setFromHsvPixels(unsigned char * pixels){
		img.setFromPixels(pixels, 64, 64);
		img.convertHsvToRgb();
	}
	
	ofxCvColorImage img;
	
};

class ColorTracker{

	public:

		void setup(int _w, int _h);
		void setHueSatBri(int _hue, int _sat, int _bri);
		void findBlobColor(ofxCvColorImage & colorImg, ofxCvBlob blob);
		void trackColors(unsigned char * hsvImage);
		void updateColorImage();
        ofxCvGrayscaleImage 	grayImage;
        ofxCvContourFinder		contourFinder;
		
		int w;
		int h;
		int hue, sat, val;
	
		int hueRange;
		int satRange;
		int valRange;
	
		int nDilations;
	
	
		ofPoint myColor;
	
	
		unsigned char * grayPixels;
		unsigned char * colorTrackerPix;
	
		ColorTrackerImage	colorTrackerImg;
		ofPoint				trackedPoint;
	
		ofPoint rgbColor;
	
	
		ofxCvGrayscaleAdvanced	blobMaskImg;
		ofxCvGrayscaleImage		grayMask;
		ofxCvColorImage	colorMask;
		ofxCvColorImage	colorFindImg;
	
	
	

};

#endif

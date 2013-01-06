#include "colorTracker.h"


//--------------------------------------------------------------
void ColorTracker::setup(int _w, int _h){
	
	grayPixels = new unsigned char[_w*_h];
	grayImage.allocate(_w, _h);
	w = _w;
	h = _h;
	
	hueRange = 10;
	satRange = 30;
	valRange = 25;
	
	
	colorTrackerPix = new unsigned char [64*64*3];
	
	setHueSatBri(0, 0, 0);
	
	blobMaskImg.allocate(w,h);
	colorMask.allocate(w,h);
	colorFindImg.allocate(w,h);
	grayMask.allocate(w,h);
}

//--------------------------------------------------------------
void ColorTracker::setHueSatBri(int _hue, int _sat, int _bri){
	
	hue = _hue;
	sat = _sat;
	val = _bri;
	
	myColor = convertHsvToRgb(ofPoint(hue * (255.0f/180.0f), sat, val));
	
	updateColorImage();

}

//--------------------------------------------------------------
void ColorTracker::updateColorImage(){
	// do some maths here:
	for (int i = 0;  i < 64; i++){
		for (int j = 0; j < 64; j++){
			
			float pctw = (float) i / 63.0f;
			float pcth = (float) j / 63.0f;
			
			int h = (hue - hueRange) + (float)hueRange * 2 * pctw;
			int s = (sat - satRange) + (float)satRange * 2 * pcth;
			if (h < 0) h += 180;
			if (h > 180) h -= 180;
			if (s < 0) s = 0;
			if (s > 255) s = 255;
			
			
			colorTrackerPix[ (j * 64 + i) * 3] = h;
			colorTrackerPix[ (j * 64 + i) * 3 + 1] = s;
			colorTrackerPix[ (j * 64 + i) * 3 + 2] = val;
			
		}
	}
	
	colorTrackerImg.setFromHsvPixels(colorTrackerPix);
	
	myColor = convertHsvToRgb(ofPoint(hue * (255.0f/180.0f), sat, val));

}

//--------------------------------------------------------------
void ColorTracker::trackColors(unsigned char * colorHsvPixels){

	// to do make this not suck with hues, like 0 (vs 255, for example).
	
	for (int i = 0; i < w*h; i++){
		
		
		if ((colorHsvPixels[i*3] > (hue - hueRange) && colorHsvPixels[i*3] < (hue + hueRange)) &&
			(colorHsvPixels[i*3+1] > (sat - satRange) && colorHsvPixels[i*3+1] < (sat + satRange)) &&
			(colorHsvPixels[i*3+2] > (val - valRange) && colorHsvPixels[i*3+2] < (val + valRange))){
			
			grayPixels[i] = 255;
			
			
		} else {
			
			grayPixels[i] = 0;
		}
		
	}
	
	grayImage.setFromPixels(grayPixels, w, h);
	
	for (int i = 0; i < nDilations; i++){
		grayImage.dilate();
	}
	
	// first, try nearby: 
	float xToTry1 = MAX(trackedPoint.x - 80, 0);
	float yToTry1 = MAX(trackedPoint.y - 80, 0);
	float xToTry2 = MIN(trackedPoint.x + 80, grayImage.width);
	float yToTry2 = MIN(trackedPoint.y + 80, grayImage.height);

	grayImage.setROI(ofRectangle(xToTry1,yToTry1, xToTry2-xToTry1, yToTry2-yToTry1));
	contourFinder.findContours(grayImage, 20, (340*240)/3, 1, false);	
	grayImage.resetROI();
	
	if (contourFinder.nBlobs > 0){
		trackedPoint.x = contourFinder.blobs[0].centroid.x + xToTry1;
		trackedPoint.y = contourFinder.blobs[0].centroid.y + yToTry1;
	} else {
		//try again, now with abit more: 
		
		float xToTry1 = MAX(trackedPoint.x - 160, 0);
		float yToTry1 = MAX(trackedPoint.y - 160, 0);
		float xToTry2 = MIN(trackedPoint.x + 160, grayImage.width);
		float yToTry2 = MIN(trackedPoint.y + 160, grayImage.height);
		
		
		grayImage.setROI(ofRectangle(xToTry1,yToTry1, xToTry2-xToTry1, yToTry2-yToTry1));
		contourFinder.findContours(grayImage, 20, (340*240)/3, 1, false);	
		grayImage.resetROI();
				
		if (contourFinder.nBlobs > 0){
			trackedPoint.x = contourFinder.blobs[0].centroid.x + xToTry1;
			trackedPoint.y = contourFinder.blobs[0].centroid.y + yToTry1;
		} else {
			
			contourFinder.findContours(grayImage, 20, (340*240)/3, 1, false);	
		
			if (contourFinder.nBlobs > 0){
				trackedPoint.x = contourFinder.blobs[0].centroid.x;
				trackedPoint.y = contourFinder.blobs[0].centroid.y;
			} 
				
			
		}
		
	}
	
	
	
}



void ColorTracker::findBlobColor(ofxCvColorImage & colorImg, ofxCvBlob blob)
{
	// clear mask
	blobMaskImg.set(255);
	
	// create mask image from blob
	blobMaskImg.drawBlobIntoMe( blob, 0 );
	grayMask = blobMaskImg;
	colorMask = grayMask;
	
	// set color image and subtract mask
	colorFindImg = colorImg;
	colorFindImg -= colorMask;
	
	// search through bounding box area for average color
	unsigned char * pixels = colorFindImg.getPixels();
	
	int xMin = blob.boundingRect.x;
	int xMax = blob.boundingRect.x+blob.boundingRect.width;
	int yMin = blob.boundingRect.y;
	int yMax = blob.boundingRect.y+blob.boundingRect.height;
	
	vector<ofPoint> colorPts;
	for( int x = xMin; x < xMax; x++)
	{
		for( int y = yMin; y < yMax; y++)
		{
			int pix = x + y * blob.boundingRect.width;
			if( 
			   pixels[pix*3+1] > satRange &&
			   pixels[pix*3+2] > valRange
			   ){
				float sat = pixels[pix*3+1]*1.25f;
				sat = MIN(sat,255);
				colorPts.push_back( ofPoint(pixels[pix*3],pixels[pix*3+1], sat) );
			}
			
		}
	}
	
	if( colorPts.size() > 0 )
	{
		cout << "color pts " << colorPts.size() << endl;
		ofPoint avgPoint = ofPoint(0,0,0);
		for( int i = 0; i < colorPts.size(); i++)
		{
			avgPoint.x += colorPts[i].x;
			avgPoint.y += colorPts[i].y;
			avgPoint.z += colorPts[i].z;
		}
		avgPoint.x /= colorPts.size();
		avgPoint.y /= colorPts.size();
		avgPoint.z /= colorPts.size();

		rgbColor = convertHsvToRgb(avgPoint);
		
		cout << "avgPoint " << avgPoint.x << " " << avgPoint.y << " " << avgPoint.z << endl;
		
		
	}
	
		
}

		
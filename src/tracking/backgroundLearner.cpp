/*
 *  backgroundLearner.cpp
 *  fachada_tracking
 *
 *  Created by Chris on 2/13/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#include "backgroundLearner.h"

void BackgroundLearner::setup(int _w, int _h )
{
	w = _w;
	h = _h;
	
	int totalWH = w*h;
	
	imageFloat = new float[totalWH];
	memset(imageFloat,0,totalWH*sizeof(float));
	
	imageByte = new unsigned char[totalWH];
	memset(imageByte,0,totalWH*sizeof(char));
	
	framesLearned = 0;
	
	grayImg.allocate(w, h);
}

void BackgroundLearner::learnBackground( ofxCvGrayscaleImage & graySrc, float rate )
{
	
	unsigned char * pixels = graySrc.getPixels();
	if( framesLearned == 0 ) rate = 1;
	
	float dRate = 1 - rate;
	
	
	for( int i = 0; i < w*h; i++)
	{
		imageFloat[i] = dRate * imageFloat[i] + rate * pixels[i];
		imageByte[i] = (unsigned char)(imageFloat[i]);
	}
	


	grayImg.setFromPixels(imageByte,w,h);
}

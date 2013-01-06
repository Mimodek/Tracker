/*
 *  maskMaker.h
 *  fachada_tracking
 *
 *  Created by Chris on 2/12/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */
#pragma once

#include "ofMain.h"
#include "ofxCvMain.h"
#include "polygonEditable.h"
#include "ofxCvGrayscaleAdvanced.h"
#include "ofxXmlSettings.h"

/*

*/

class MaskMaker : public PolyEditable{

	public:
		MaskMaker();
		~MaskMaker(){}
		
		void clear();
		void setup(int w, int h, int xOff = 0, int yOff = 0, float scl = 1);
		void draw();
		void makeMask(bool bInverted = true);
		
		void loadMask(string filename);
		void saveMask(string filename);
	
		ofxCvGrayscaleAdvanced	mask;
		ofPoint drawOffset;
		float scale;
		bool bSetMask;
	
};

